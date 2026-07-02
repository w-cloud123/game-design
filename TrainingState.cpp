// ============================================================
// src/TrainingState.cpp
// ============================================================
#include "TrainingState.hpp"
#include "Constants.hpp"
#include "AssetManager.hpp"
#include <algorithm>

TrainingState::TrainingState(const sf::Font& font)
    : m_font(font)
    , m_hudL(font)
    , m_hudR(font)
    , m_f1(1, makeMageConfig())
    , m_f2(2, makeSwordsmanConfig()) {}

void TrainingState::setPlayers(const CharacterConfig& p1, const CharacterConfig& p2) {
    m_cfg1 = &p1; m_cfg2 = &p2;
}

void TrainingState::onEnter() {
    m_proj.clear(); m_traps.clear();
    initFighters();
}

void TrainingState::initFighters() {
    if (!m_cfg1 || !m_cfg2) return;
    m_f1 = Fighter(1, *m_cfg1);
    m_f1.setTexture(AssetManager::getInstance().getTexture(m_cfg1->textureKey));
    m_f1.setPosition({200.f, Constants::GROUND_Y - 64.f});
    m_f1.setFacing(1.f);

    m_f2 = Fighter(2, *m_cfg2);
    m_f2.setTexture(AssetManager::getInstance().getTexture(m_cfg2->textureKey));
    m_f2.setPosition({550.f, Constants::GROUND_Y - 64.f});
    m_f2.setFacing(-1.f);
    m_f2.setDummy(true);
}

void TrainingState::handleEvent(const sf::Event& event) {
    const auto* k = event.getIf<sf::Event::KeyPressed>();
    if (k && k->scancode == sf::Keyboard::Scancode::R) requestChange(StateID::Select);
}

void TrainingState::update(float dt) {
    m_f1.handleInput();
    m_f1.update(dt, m_f2, m_proj, m_traps);
    m_f2.update(dt, m_f1, m_proj, m_traps);

    tickProjectiles(dt);
    tickTraps(dt);
    refreshHUD();
}

void TrainingState::tickProjectiles(float dt) {
    for (auto& p : m_proj) p.update(dt);
    for (auto& p : m_proj) {
        if (p.isDead()) continue;
        // 只检测 Dummy（P2）受击
        if (p.getBounds().findIntersection(m_f2.getHurtbox())) { m_f2.takeDamage(p.getDamage()); p.kill(); }
    }
    m_proj.erase(std::remove_if(m_proj.begin(), m_proj.end(),
                 [](const Projectile& p){ return p.isDead(); }), m_proj.end());
}

void TrainingState::tickTraps(float dt) {
    for (auto& t : m_traps) t.update(dt);
    for (auto& t : m_traps) {
        if (t.isExpired() || t.isTriggered()) continue;
        if (t.getOwner() != 2 && t.getBounds().findIntersection(m_f2.getHurtbox())) {
            m_f2.applyDot(t.getDotDamage(), t.getDotInterval(), t.getDotDuration());
            m_f2.applySlow(t.getSlowAmount(), t.getSlowDuration());
            t.trigger();
        }
    }
    m_traps.erase(std::remove_if(m_traps.begin(), m_traps.end(),
                  [](const Trap& t){ return t.isExpired(); }), m_traps.end());
}

void TrainingState::refreshHUD() {
    m_hudL.update(m_f1.getHp(), m_f1.getMaxHp(), m_f1.getMp(), m_f1.getMaxMp(),
                  m_f1.getName(), m_f1.getAtkCD(), m_f1.getAtkCDMax(),
                  m_f1.getSk1CD(), m_f1.getSk1CDMax(), m_f1.getSk2CD(), m_f1.getSk2CDMax(), true);
    m_hudR.update(m_f2.getHp(), m_f2.getMaxHp(), m_f2.getMp(), m_f2.getMaxMp(),
                  m_f2.getName() + " [DUMMY]", m_f2.getAtkCD(), m_f2.getAtkCDMax(),
                  m_f2.getSk1CD(), m_f2.getSk1CDMax(), m_f2.getSk2CD(), m_f2.getSk2CDMax(), false);
}

void TrainingState::draw(sf::RenderWindow& window) {
    m_map.drawBackground(window);
    m_map.drawGround(window);
    for (auto& p : m_proj) p.draw(window);
    for (auto& t : m_traps) t.draw(window);
    m_f1.draw(window); m_f2.draw(window);
    m_hudL.draw(window); m_hudR.draw(window);
}
