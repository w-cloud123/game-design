// ============================================================
// src/Map.cpp — 背景渲染实现
// ============================================================
#include "Map.hpp"
#include "Constants.hpp"

Map::Map() {
    // 无背景图时的纯色降级：深灰矩形填满窗口
    m_bg.setSize({800.f, 600.f});
    m_bg.setFillColor(sf::Color(40, 40, 40));
    // 地面线：靠近窗口底部的一条灰色横线
    m_ground.setSize({800.f, Constants::GROUND_HEIGHT});
    m_ground.setFillColor(sf::Color(60, 60, 60));
    m_ground.setPosition({0.f, Constants::GROUND_Y});
}

void Map::setBackgroundTexture(const sf::Texture& tex) {
    m_bgSprite.emplace(tex); // SFML 3.x: Sprite(Texture) 就地构造
}

void Map::drawBackground(sf::RenderWindow& w) const {
    if (m_bgSprite.has_value())
        w.draw(*m_bgSprite);  // 有纹理就画纹理
    else
        w.draw(m_bg);         // 否则降级为纯色
}

void Map::drawGround(sf::RenderWindow& w) const { w.draw(m_ground); }
