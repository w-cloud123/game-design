// ============================================================
// src/FightState.cpp — 1v1 对战状态实现
// ============================================================
#include "FightState.hpp"
#include "Constants.hpp"

FightState::FightState(const sf::Font& font)
    : BattleState(font)
    , m_win(font, "", 48) // SFML 3.x Text 必须在初始化列表构造（无默认构造）
{
    m_win.setFillColor(sf::Color::Yellow);
}

void FightState::onEnter() {
    BattleState::onEnter(); // 父类：清空抛射物/陷阱 + 重建角色
    m_over    = false;
    m_restart = 0.f;
}

// ==================== Hook 覆写 ====================

// preUpdate：本帧是否继续执行战斗逻辑？
//   KO 前：return true → 正常执行 Fighter 更新 + 碰撞 + HUD
//   KO 后：只累加倒计时，跳过一切（角色不再动、不再碰撞、HUD 不再刷新）
bool FightState::preUpdate(float dt) {
    if (!m_over) return true;
    m_restart += dt;
    if (m_restart > Constants::KO_RESTART_DELAY)
        requestChange(StateID::Select);
    return false;
}

void FightState::postFighterUpdate(float /*dt*/) {
    checkWin();
}

void FightState::drawExtras(sf::RenderWindow& window) {
    if (m_over) window.draw(m_win);
}

// ==================== KO 判定 ====================

void FightState::checkWin() {
    if (m_f1.isKO() || m_f2.isKO()) {
        m_over = true;
        // 如果 P1 死了 → P2 胜；否则 P1 胜
        m_win.setString((m_f1.isKO() ? m_f2.getName() : m_f1.getName()) + " WINS!");
        // 文字居中：先获取文字的实际像素大小，设 origin 为中心，再移到屏幕中央
        sf::FloatRect b = m_win.getLocalBounds();
        m_win.setOrigin({b.size.x / 2.f, b.size.y / 2.f});
        m_win.setPosition({Constants::WINDOW_WIDTH / 2.f, Constants::WINDOW_HEIGHT / 2.f});
    }
}
