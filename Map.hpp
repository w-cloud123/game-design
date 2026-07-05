// ============================================================
// src/Map.hpp — 背景渲染
// ============================================================
// 负责绘制场景的背景图和地面线。
// 如果设置了背景纹理（setBackgroundTexture），优先绘制纹理；
// 否则降级为纯色矩形（深灰色）作为保底。
// ============================================================
#pragma once
#include <SFML/Graphics.hpp>
#include <optional>

class Map {
public:
    Map();
    void setBackgroundTexture(const sf::Texture& tex);  // 可选背景图
    void drawBackground(sf::RenderWindow& w) const;      // 先纹理后纯色
    void drawGround(sf::RenderWindow& w) const;          // 地面线
private:
    sf::RectangleShape m_bg;                 // 无纹理时的纯色降级方案
    sf::RectangleShape m_ground;             // 地面线（4px 灰色横条）
    std::optional<sf::Sprite> m_bgSprite;    // 背景纹理（SFML 3.x，optional 延迟构造）
};
