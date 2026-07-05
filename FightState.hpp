// ============================================================
// src/FightState.hpp — 1v1 对战状态
// ============================================================
// 继承 BattleState，覆写 3 个 hook 添加 1v1 特有逻辑：
//   preUpdate：        胜负已分后跳过战斗逻辑，只跑倒计时
//   postFighterUpdate：每帧检查是否有人 KO
//   drawExtras：       在画面中央画 "XXX WINS!"
//
// 流程：
//   双方对战 → checkWin() 检测 KO →
//   m_over = true → 显示胜者文字 → 3 秒倒计时 →
//   自动返回选人界面
// ============================================================
#pragma once
#include "BattleState.hpp"

class FightState : public BattleState {
public:
    explicit FightState(const sf::Font& font);
    void onEnter() override;

protected:
    bool preUpdate(float dt) override;           // KO 后拦截本帧
    void postFighterUpdate(float dt) override;    // 每帧检查 KO
    void drawExtras(sf::RenderWindow& w) override; // 画胜负文字

private:
    void checkWin();              // 检测 KO → 设置 m_over + 胜负文字

    bool    m_over    = false;    // 战斗是否结束
    float   m_restart = 0.f;     // KO 后已过的秒数
    sf::Text m_win;              // "XXX WINS!" 文字
};
