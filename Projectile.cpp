// ============================================================
// src/Projectile.cpp — 抛射物实现
// ============================================================
#include "Projectile.hpp"
#include "Constants.hpp"
#include <cmath>

void Projectile::launch(sf::Vector2f pos, float dirX, float speed,
                         float range, float damage, int owner, float onHitMp) {
    m_position = pos;
    // 速度方向 = 纯水平（格斗游戏抛射物不追踪、不下坠）
    m_velocity = {speed * dirX, 0.f};
    m_range = range; m_damage = damage; m_onHitMp = onHitMp;
    m_traveled = 0.f; m_dead = false; m_owner = owner;
}

void Projectile::update(float dt) {
    if (m_dead) return;
    sf::Vector2f delta = m_velocity * dt;
    m_position += delta;
    m_traveled += std::abs(delta.x);
    // 超距死亡或飞出屏幕（留 50px 缓冲）
    if (m_traveled >= m_range) m_dead = true;
    if (m_position.x < -50.f || m_position.x > Constants::WINDOW_WIDTH + 50.f) m_dead = true;
}

sf::FloatRect Projectile::getBounds() const {
    return sf::FloatRect({m_position.x - 6.f, m_position.y - 6.f}, {12.f, 12.f});
}

// 视觉效果：橙色小圆（半径 5px）
void Projectile::draw(sf::RenderWindow& window) const {
    if (m_dead) return;
    sf::CircleShape shape(5.f);
    shape.setFillColor(sf::Color(255, 165, 0));
    shape.setPosition({m_position.x - 5.f, m_position.y - 5.f});
    window.draw(shape);
}
