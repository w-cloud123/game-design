// ============================================================
// src/Trap.cpp — 陷阱实现
// ============================================================
#include "Trap.hpp"

void Trap::place(sf::Vector2f pos, float duration,
                 float dotDmg, float dotInterval, float dotDuration,
                 float slowAmount, float slowDuration, int owner) {
    m_position = pos; m_lifetime = duration;
    m_expired = false; m_triggered = false;
    m_dotDmg = dotDmg; m_dotInterval = dotInterval; m_dotDuration = dotDuration;
    m_slowAmount = slowAmount; m_slowDuration = slowDuration;
    m_owner = owner;
}

void Trap::update(float dt) {
    if (m_expired) return;
    m_lifetime -= dt;
    if (m_lifetime <= 0.f) m_expired = true;
}

// 碰撞区域：20×10 像素，居中于放置位置
sf::FloatRect Trap::getBounds() const {
    return sf::FloatRect({m_position.x - 10.f, m_position.y - 5.f}, {20.f, 10.f});
}

// 触发时立即标记为已触发+已过期（一次性陷阱）
void Trap::trigger() { m_triggered = true; m_expired = true; }

// 视觉效果：暗红色小横条
void Trap::draw(sf::RenderWindow& window) const {
    if (m_expired) return;
    sf::RectangleShape shape({20.f, 6.f});
    shape.setFillColor(sf::Color(139, 0, 0));
    shape.setPosition({m_position.x - 10.f, m_position.y - 3.f});
    window.draw(shape);
}
