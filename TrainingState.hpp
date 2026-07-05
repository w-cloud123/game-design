// ============================================================
// src/TrainingState.hpp — 打桩训练模式
// ============================================================
// 继承 BattleState，覆写 3 个 hook：
//   onFightersInitialized：P2 设为 Dummy（不接受输入、不移动、不攻击）
//   isPlayerVulnerable：   只有 P2（Dummy）可被命中，P1 训练者无敌
//   p2NameSuffix：         HUD 中 P2 名字后面加 " [DUMMY]"
//
// 训练模式没有胜负判定——P2 被打到 0 血也不会触发 KO 文字或倒计时。
// P1 可以无限测试连招，按 R 键返回选人。
// ============================================================
#pragma once
#include "BattleState.hpp"

class TrainingState : public BattleState {
public:
    explicit TrainingState(const sf::Font& font);

protected:
    void onFightersInitialized()               override;
    bool isPlayerVulnerable(int playerIndex) const override;
    std::string p2NameSuffix()                 const override;
};
