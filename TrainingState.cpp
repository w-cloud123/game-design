// ============================================================
// src/TrainingState.cpp — 打桩训练模式实现
// ============================================================
#include "TrainingState.hpp"

TrainingState::TrainingState(const sf::Font& font)
    : BattleState(font)
{}

void TrainingState::onFightersInitialized() {
    m_f2.setDummy(true); // P2 不响应键盘输入
}

// P1（训练者）不受伤害，P2（Dummy）可以被攻击
bool TrainingState::isPlayerVulnerable(int playerIndex) const {
    return playerIndex == 2;
}

std::string TrainingState::p2NameSuffix() const {
    return " [DUMMY]";
}
