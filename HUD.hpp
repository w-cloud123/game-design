// ============================================================
// src/HUD.hpp — 抬头显示（血条 / 蓝条 / CD 文字）
// ============================================================
// 每个 Fighter 对应一个 HUD 实例，显示在屏幕左右两侧。
// P1 的 HUD 在左侧（x=10），P2 的在右侧（x=W-210）。
//
// 显示内容：
//   血条：红色填充 + 灰色背景，宽度 200px
//   蓝条：蓝色填充 + 灰色背景，宽度 200px
//   角色名 + CD 倒计时文字
// ============================================================
#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class HUD {
public:
    explicit HUD(const sf::Font& font); // SFML 3.x Text 必须用 font 构造

    /// @param isLeftSide true=左侧(P1), false=右侧(P2)
    void update(float hp, float maxHp, float mp, float maxMp,
                const std::string& name,
                float atkCD, float atkCDMax,
                float sk1CD, float sk1CDMax,
                float sk2CD, float sk2CDMax,
                bool isLeftSide);
    void draw(sf::RenderWindow& window) const;

private:
    sf::RectangleShape m_hpBarBg, m_hpBarFill;  // 血条背景 + 填充
    sf::RectangleShape m_mpBarBg, m_mpBarFill;  // 蓝条背景 + 填充
    sf::Text m_nameText;   // 角色名
    sf::Text m_cdText;     // CD 文字
    bool m_isLeft = true;  // 是否左侧
};
