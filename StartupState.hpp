// ============================================================
// src/StartupState.hpp — 启动封面
// ============================================================
// 显示游戏标题 + 闪烁的 "Press ANY KEY to start"。
// 按任意键 → 进入主菜单。
// ============================================================
#pragma once
#include "State.hpp"
#include "Map.hpp"

class StartupState : public State {
public:
    explicit StartupState(const sf::Font& font);
    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;
    void onEnter() override;
    void setBackground(const sf::Texture& tex) { m_map.setBackgroundTexture(tex); }

private:
    const sf::Font& m_font;
    Map m_map;
    sf::Text m_title;        // "FIGHTING GAME"
    sf::Text m_subtitle;     // "Press ANY KEY to start"
    sf::Clock m_blink;       // 闪烁计时器
    bool m_visible = true;   // 字幕当前是否可见
};
