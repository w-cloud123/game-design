// ============================================================
// src/Animation.hpp
// ============================================================
#pragma once
#include <SFML/Graphics.hpp>

class Animation {
public:
    void setSprite(sf::Sprite* sprite) { m_sprite = sprite; }
    void update(float /*dt*/) {}
    void setFrame(int x, int y, int w, int h) {
        if (m_sprite)
            m_sprite->setTextureRect(sf::IntRect({x, y}, {w, h}));
    }
    void setFlash(bool active) {
        if (m_sprite)
            m_sprite->setColor(active ? sf::Color::Red : sf::Color::White);
    }
private:
    sf::Sprite* m_sprite = nullptr;
};
