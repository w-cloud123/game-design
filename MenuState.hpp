// ============================================================
// src/MenuState.hpp — 主菜单
// ============================================================
// 三个选项：1v1 VS MODE / TRAINING MODE / EXIT
// W/S 或 ↑/↓ 移动光标，Enter 确认。
// "EXIT" → requestChange(Quit) → Game 关闭窗口。
// 其他两个 → requestChange(Select) → 选人界面。
// ============================================================
#pragma once
#include "State.hpp"
#include "Map.hpp"
#include <vector>

class MenuState : public State {
public:
    explicit MenuState(const sf::Font& font);
    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;
    void onEnter() override;
    bool isTrainingMode() const { return m_selected == 1; } // Game 读取此值
    void setBackground(const sf::Texture& tex) { m_map.setBackgroundTexture(tex); }

private:
    const sf::Font& m_font;
    Map m_map;
    sf::Text m_title;                  // "MAIN MENU"
    std::vector<sf::Text> m_items;     // 三个菜单项
    int m_selected = 0;                // 当前高亮的索引
};
