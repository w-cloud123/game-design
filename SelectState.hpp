// ============================================================
// src/SelectState.hpp — 选人界面
// ============================================================
// 两个玩家各自选择角色。P1 用 A/D 切换，P2 用 ←/→ 切换。
// 训练模式下只选 P1（P2 自动跟随 P1 的选择，因为 P2 是 Dummy）。
// 双方都按 Enter 锁定后 → 进入 FightState 或 TrainingState。
// ============================================================
#pragma once
#include "State.hpp"
#include "Map.hpp"
#include "CharacterConfig.hpp"
#include <vector>

class SelectState : public State {
public:
    explicit SelectState(const sf::Font& font);
    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;
    void onEnter() override;

    void setTrainingMode(bool t) { m_training = t; } // 从 Game::changeState 传入

    // Game 读取选人结果传给 FightState/TrainingState
    const CharacterConfig& getP1Config() const { return m_configs[m_p1Sel]; }
    const CharacterConfig& getP2Config() const { return m_configs[m_p2Sel]; }
    void setBackground(const sf::Texture& tex) { m_map.setBackgroundTexture(tex); }

private:
    const sf::Font& m_font;
    Map m_map;
    sf::Text m_title;              // "SELECT YOUR FIGHTER"
    sf::Text m_p1Label, m_p2Label; // "P1 (A/D)", "P2 (<-/->)"
    sf::Text m_ready;              // 状态提示
    std::vector<CharacterConfig> m_configs;  // 4 个角色配置
    std::vector<sf::Text> m_names;           // 4 个角色名（渲染用）
    int m_p1Sel = 0, m_p2Sel = 1;  // 当前选中的索引
    bool m_p1OK = false, m_p2OK = false; // 是否已锁定
    bool m_training = false;        // 训练模式
};
