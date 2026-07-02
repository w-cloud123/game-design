// ============================================================
// src/Map.cpp
// ============================================================
#include "Map.hpp"
#include "Constants.hpp"

Map::Map() {
    m_background.setSize({static_cast<float>(Constants::WINDOW_WIDTH),
                          static_cast<float>(Constants::WINDOW_HEIGHT)});
    m_background.setFillColor(sf::Color(40, 40, 40));
    m_background.setPosition({0.f, 0.f});

    m_ground.setSize({static_cast<float>(Constants::WINDOW_WIDTH), Constants::GROUND_HEIGHT});
    m_ground.setFillColor(sf::Color(60, 60, 60));
    m_ground.setPosition({0.f, Constants::GROUND_Y});
}

void Map::drawBackground(sf::RenderWindow& window) const { window.draw(m_background); }
void Map::drawGround(sf::RenderWindow& window)     const { window.draw(m_ground); }
