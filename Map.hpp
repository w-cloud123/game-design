// ============================================================
// src/Map.hpp
// ============================================================
#pragma once
#include <SFML/Graphics.hpp>

class Map {
public:
    Map();
    void drawBackground(sf::RenderWindow& window) const;
    void drawGround(sf::RenderWindow& window) const;
private:
    sf::RectangleShape m_background;
    sf::RectangleShape m_ground;
};
