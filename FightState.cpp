// ============================================================
// src/FightState.cpp
// ============================================================
#include "FightState.hpp"
#include "Constants.hpp"
#include "AssetManager.hpp"
#include <algorithm>

FightState::FightState(const sf::Font& font)
    : m_font(font)
    , m_hudL(font)
    , m_hudR(font)
    , m_f1(1, makeMageConfig())
    , m_f2(2, makeSwordsmanConfig())
    , m_win(font, "", 48)
{
    m_win.setFillColor(sf::Color::Yellow);
}

void FightState::setPlayers(const CharacterConfig& p1, const CharacterConfig& p2) {
    m_cfg1 = &p1; m_cfg2 = &p2;
}

void FightState::onEnter() {
    m_proj.clear(); m_traps.clear(); m_over = false; m_restart = 0.f;
    initFighters();
}

void FightState::initFighters() {
    if (!m_cfg1 || !m_cfg2) return;
    m_f1 = Fighter(1, *m_cfg1);
    m_f1.setTexture(AssetManager::getInstance().getTexture(m_cfg1->textureKey));
    m_f1.setPosition({200.f, Constants::GROUND_Y - 64.f});
    m_f1.setFacing(1.f);

    m_f2 = Fighter(2, *m_cfg2);
    m_f2.setTexture(AssetManager::getInstance().getTexture(m_cfg2->textureKey));
    m_f2.setPosition({600.f, Constants::GROUND_Y - 64.f});
    m_f2.setFacing(-1.f);
}

void FightState::handleEvent(const sf::Event& event) {
    const auto* k = event.getIf<sf::Event::KeyPressed>();
    if (k && k->scancode == sf::Keyboard::Scancode::R) requestChange(StateID::Select);
}

void FightState::update(float dt) {
    if (m_over) { m_restart += dt; if (m_restart > 3.f) requestChange(StateID::Select); return; }

    m_f1.handleInput();
    m_f2.handleInput();
    m_f1.update(dt, m_f2, m_proj, m_traps);
    m_f2.update(dt, m_f1, m_proj, m_traps);

    // === 落地推离：双方着地时不能有任何重叠 ===
    if (m_f1.isOnGround() && m_f2.isOnGround()) {
        auto ov = m_f1.getHurtbox().findIntersection(m_f2.getHurtbox());
        if (ov) {
            float dx = ov->size.x * 0.5f;
            sf::Vector2f p1 = m_f1.getPosition();
            sf::Vector2f p2 = m_f2.getPosition();
            if (p1.x < p2.x) {
                p1.x -= dx; p2.x += dx;
            } else {
                p1.x += dx; p2.x -= dx;
            }
            p1.x = std::clamp(p1.x, 0.f, Constants::WINDOW_WIDTH - 64.f);
            p2.x = std::clamp(p2.x, 0.f, Constants::WINDOW_WIDTH - 64.f);
            m_f1.setPosition(p1);
            m_f2.setPosition(p2);
        }
    }

    tickProjectiles(dt);
    tickTraps(dt);
    checkWin();
    refreshHUD();
}

void FightState::tickProjectiles(float dt) {
    for (auto& p : m_proj) p.update(dt);
    for (auto& p : m_proj) {
        if (p.isDead()) continue;
        // 只检测与发射者之外的玩家碰撞
        if (p.getOwner() != 1 && p.getBounds().findIntersection(m_f1.getHurtbox()))
            { m_f1.takeDamage(p.getDamage()); p.kill(); }
        if (p.getOwner() != 2 && p.getBounds().findIntersection(m_f2.getHurtbox()))
            { m_f2.takeDamage(p.getDamage()); p.kill(); }
    }
    m_proj.erase(std::remove_if(m_proj.begin(), m_proj.end(),
                 [](const Projectile& p){ return p.isDead(); }), m_proj.end());
}

void FightState::tickTraps(float dt) {
    for (auto& t : m_traps) t.update(dt);
    for (auto& t : m_traps) {
        if (t.isExpired() || t.isTriggered()) continue;
        if (t.getOwner() != 1 && t.getBounds().findIntersection(m_f1.getHurtbox())) {
            m_f1.applyDot(t.getDotDamage(), t.getDotInterval(), t.getDotDuration());
            m_f1.applySlow(t.getSlowAmount(), t.getSlowDuration());
            t.trigger();
        }
        if (t.getOwner() != 2 && t.getBounds().findIntersection(m_f2.getHurtbox())) {
            m_f2.applyDot(t.getDotDamage(), t.getDotInterval(), t.getDotDuration());
            m_f2.applySlow(t.getSlowAmount(), t.getSlowDuration());
            t.trigger();
        }
    }
    m_traps.erase(std::remove_if(m_traps.begin(), m_traps.end(),
                  [](const Trap& t){ return t.isExpired(); }), m_traps.end());
}

void FightState::checkWin() {
    if (m_f1.isKO() || m_f2.isKO()) {
        m_over = true;
        m_win.setString((m_f1.isKO() ? m_f2.getName() : m_f1.getName()) + " WINS!");
        sf::FloatRect b = m_win.getLocalBounds();
        m_win.setOrigin({b.size.x / 2.f, b.size.y / 2.f});
        m_win.setPosition({Constants::WINDOW_WIDTH / 2.f, Constants::WINDOW_HEIGHT / 2.f});
    }
}

void FightState::refreshHUD() {
    m_hudL.update(m_f1.getHp(), m_f1.getMaxHp(), m_f1.getMp(), m_f1.getMaxMp(),
                  m_f1.getName(), m_f1.getAtkCD(), m_f1.getAtkCDMax(),
                  m_f1.getSk1CD(), m_f1.getSk1CDMax(), m_f1.getSk2CD(), m_f1.getSk2CDMax(), true);
    m_hudR.update(m_f2.getHp(), m_f2.getMaxHp(), m_f2.getMp(), m_f2.getMaxMp(),
                  m_f2.getName(), m_f2.getAtkCD(), m_f2.getAtkCDMax(),
                  m_f2.getSk1CD(), m_f2.getSk1CDMax(), m_f2.getSk2CD(), m_f2.getSk2CDMax(), false);
}

void FightState::draw(sf::RenderWindow& window) {
    m_map.drawBackground(window);
    m_map.drawGround(window);
    for (auto& p : m_proj) p.draw(window);
    for (auto& t : m_traps) t.draw(window);
    m_f1.draw(window); m_f2.draw(window);
    m_hudL.draw(window); m_hudR.draw(window);
    if (m_over) window.draw(m_win);
}
