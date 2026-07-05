// ============================================================
// src/BattleState.cpp — 对战/训练模式公共基类实现
// ============================================================
#include "BattleState.hpp"
#include "Constants.hpp"
#include "AssetManager.hpp"
#include <algorithm>

// ==================== 构造 ====================
// 注意：m_f1/m_f2 用临时配置构造——因为 setPlayers() 还没被调用。
// initFighters() 在 onEnter() 中用真实配置重新赋值覆盖。
BattleState::BattleState(const sf::Font& font)
    : m_font(font)
    , m_hudL(font), m_hudR(font)
    , m_f1(1, makeMageConfig())
    , m_f2(2, makeSwordsmanConfig())
{}

// ==================== 公开接口 ====================

void BattleState::setPlayers(const CharacterConfig& p1, const CharacterConfig& p2) {
    m_cfg1 = &p1; m_cfg2 = &p2;
}

// onEnter：每次进入对战/训练界面时调用。清空上一局的抛射物和陷阱，重建角色。
void BattleState::onEnter() {
    m_proj.clear(); m_traps.clear();
    initFighters();
}

// 所有 BattleState 子类共用 R 键返回选人
void BattleState::handleEvent(const sf::Event& event) {
    const auto* k = event.getIf<sf::Event::KeyPressed>();
    if (k && k->scancode == sf::Keyboard::Scancode::R)
        requestChange(StateID::Select);
}

// ==================== 模板方法：主更新循环 ====================
// 这个函数定义了"对战类状态"的标准更新流程，子类无法覆写（不是虚函数）。
// 子类通过 preUpdate / postFighterUpdate 等 hook 注入差异。

void BattleState::update(float dt) {
    // 钩子 1：子类可在此拦截整帧（如胜负已分，只跑倒计时不跑战斗逻辑）
    if (!preUpdate(dt)) return;

    // 输入处理 + 逻辑更新（Fighter::update 内部包含攻击、物理、碰撞）
    m_f1.handleInput();
    m_f2.handleInput();
    m_f1.update(dt, m_f2, m_proj, m_traps);
    m_f2.update(dt, m_f1, m_proj, m_traps);

    // 钩子 2：子类可在此插入 KO 判定等
    postFighterUpdate(dt);

    // 场景级碰撞检测（抛射物命中、陷阱触发）
    tickProjectiles(dt);
    tickTraps(dt);

    // 同步 UI
    refreshHUD();
}

// ==================== 渲染 ====================
// 渲染顺序决定了谁盖在谁上面：
//   背景 → 地面线 → 抛射物 → 陷阱 → 角色 → HUD → 额外元素（胜负文字）

void BattleState::draw(sf::RenderWindow& window) {
    m_map.drawBackground(window);
    m_map.drawGround(window);
    for (auto& p : m_proj) p.draw(window);
    for (auto& t : m_traps) t.draw(window);
    m_f1.draw(window); m_f2.draw(window);
    m_hudL.draw(window); m_hudR.draw(window);
    drawExtras(window);
}

// ==================== 默认 Hook 实现（什么也不做） ====================

bool        BattleState::preUpdate(float /*dt*/)        { return true; }
void        BattleState::postFighterUpdate(float /*dt*/) {}
void        BattleState::drawExtras(sf::RenderWindow&)  {}
void        BattleState::onFightersInitialized()         {}
bool        BattleState::isPlayerVulnerable(int) const   { return true; }
std::string BattleState::p2NameSuffix() const            { return ""; }

// ==================== 角色初始化 ====================
// Fighter 没有默认构造且 SFML Sprite 需要纹理才能创建，
// 所以流程是：赋值构造 Fighter（带 config）→ 绑定纹理 → 设置位置和朝向。
// 最后调用 onFightersInitialized() 让子类（TrainingState）追加 setDummy 等操作。

void BattleState::initFighters() {
    if (!m_cfg1 || !m_cfg2) return;

    m_f1 = Fighter(1, *m_cfg1);
    m_f1.setTexture(AssetManager::getInstance().getTexture(m_cfg1->textureKey));
    m_f1.setPosition({200.f, Constants::GROUND_Y - Constants::FIGHTER_SIZE});
    m_f1.setFacing(1.f);

    m_f2 = Fighter(2, *m_cfg2);
    m_f2.setTexture(AssetManager::getInstance().getTexture(m_cfg2->textureKey));
    m_f2.setPosition({600.f, Constants::GROUND_Y - Constants::FIGHTER_SIZE});
    m_f2.setFacing(-1.f);

    onFightersInitialized();
}

// ==================== 抛射物碰撞 ====================
// 对每个存活的抛射物，检测是否碰到了"可被命中"的玩家。
// 抛出者和被命中者不能是同一个人（p.getOwner() != 1/2）。
// 命中后：造成伤害 → 发射者回蓝 → 抛射物消失。

void BattleState::tickProjectiles(float dt) {
    for (auto& p : m_proj) p.update(dt);  // 先更新所有位置

    for (auto& p : m_proj) {
        if (p.isDead()) continue;
        // P1 被命中（且发射者不是 P1，且 P1 不是无敌状态）
        if (isPlayerVulnerable(1) && p.getOwner() != 1
            && p.getBounds().findIntersection(m_f1.getHurtbox())) {
            m_f1.takeDamage(p.getDamage());
            if (p.getOwner() == 2) m_f2.gainMp(p.getOnHitMp()); // P2 命中回蓝
            p.kill();
        }
        // P2 被命中
        if (isPlayerVulnerable(2) && p.getOwner() != 2
            && p.getBounds().findIntersection(m_f2.getHurtbox())) {
            m_f2.takeDamage(p.getDamage());
            if (p.getOwner() == 1) m_f1.gainMp(p.getOnHitMp()); // P1 命中回蓝
            p.kill();
        }
    }
    // 移除所有已死亡/已消失的抛射物
    // erase-remove 惯用法：remove_if 把"要删除的"移到末尾 → erase 真正删除
    m_proj.erase(std::remove_if(m_proj.begin(), m_proj.end(),
                 [](const Projectile& p) { return p.isDead(); }), m_proj.end());
}

// ==================== 陷阱碰撞 ====================
// 陷阱不移动，只检测角色的碰撞框是否和陷阱区域重叠。
// 自己的陷阱不会触发（t.getOwner() != 1/2）。
// 触发后立即标记为 triggered + expired，效果（DOT+减速）施加到角色上。

void BattleState::tickTraps(float dt) {
    for (auto& t : m_traps) t.update(dt);  // 倒计时寿命

    for (auto& t : m_traps) {
        if (t.isExpired() || t.isTriggered()) continue;
        // P1 踩中（且不是 P1 自己放的）
        if (isPlayerVulnerable(1) && t.getOwner() != 1
            && t.getBounds().findIntersection(m_f1.getHurtbox())) {
            m_f1.applyDot(t.getDotDamage(), t.getDotInterval(), t.getDotDuration());
            m_f1.applySlow(t.getSlowAmount(), t.getSlowDuration());
            t.trigger();
        }
        // P2 踩中
        if (isPlayerVulnerable(2) && t.getOwner() != 2
            && t.getBounds().findIntersection(m_f2.getHurtbox())) {
            m_f2.applyDot(t.getDotDamage(), t.getDotInterval(), t.getDotDuration());
            m_f2.applySlow(t.getSlowAmount(), t.getSlowDuration());
            t.trigger();
        }
    }
    m_traps.erase(std::remove_if(m_traps.begin(), m_traps.end(),
                  [](const Trap& t) { return t.isExpired(); }), m_traps.end());
}

// ==================== HUD 刷新 ====================
// 每帧从 Fighter 的 getter 读取数据，更新 HUD 的显示。
// p2NameSuffix() 是虚函数——TrainingState 覆写为 " [DUMMY]"。

void BattleState::refreshHUD() {
    m_hudL.update(m_f1.getHp(), m_f1.getMaxHp(), m_f1.getMp(), m_f1.getMaxMp(),
                  m_f1.getName(), m_f1.getAtkCD(), m_f1.getAtkCDMax(),
                  m_f1.getSk1CD(), m_f1.getSk1CDMax(), m_f1.getSk2CD(), m_f1.getSk2CDMax(),
                  true);   // true = 左侧 HUD
    m_hudR.update(m_f2.getHp(), m_f2.getMaxHp(), m_f2.getMp(), m_f2.getMaxMp(),
                  m_f2.getName() + p2NameSuffix(), m_f2.getAtkCD(), m_f2.getAtkCDMax(),
                  m_f2.getSk1CD(), m_f2.getSk1CDMax(), m_f2.getSk2CD(), m_f2.getSk2CDMax(),
                  false);  // false = 右侧 HUD
}
