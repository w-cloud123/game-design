// ============================================================
// src/Projectile.hpp
// ============================================================
#pragma once
#include <SFML/Graphics.hpp>

class Projectile {
public:
    void launch(sf::Vector2f pos, float dirX, float speed, float range, float damage, int owner = 0);
    void update(float dt);
    bool isDead() const { return m_dead; }
    sf::FloatRect getBounds() const;
    float getDamage() const { return m_damage; }
    int   getOwner()  const { return m_owner; }
    void draw(sf::RenderWindow& window) const;
    void kill() { m_dead = true; }

private:
    sf::Vector2f m_position;
    sf::Vector2f m_velocity;
    float m_speed    = 0.f;
    float m_range    = 0.f;
    float m_traveled = 0.f;
    float m_damage   = 0.f;
    bool  m_dead     = true;
    int   m_owner    = 0;     // 1=P1, 2=P2
};
