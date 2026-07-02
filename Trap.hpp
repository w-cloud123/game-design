// ============================================================
// src/Trap.hpp
// ============================================================
#pragma once
#include <SFML/Graphics.hpp>

class Trap {
public:
    void place(sf::Vector2f pos, float duration,
               float dotDmg, float dotInterval, float dotDuration,
               float slowAmount, float slowDuration, int owner = 0);
    void update(float dt);
    bool isExpired()   const { return m_expired; }
    bool isTriggered() const { return m_triggered; }
    sf::FloatRect getBounds() const;
    void trigger();

    float getDotDamage()    const { return m_dotDmg; }
    float getDotInterval()  const { return m_dotInterval; }
    float getDotDuration()  const { return m_dotDuration; }
    float getSlowAmount()   const { return m_slowAmount; }
    float getSlowDuration() const { return m_slowDuration; }
    int   getOwner()        const { return m_owner; }
    void draw(sf::RenderWindow& window) const;

private:
    sf::Vector2f m_position;
    float m_lifetime     = 0.f;
    float m_maxLifetime  = 0.f;
    bool  m_expired      = true;
    bool  m_triggered    = false;
    float m_dotDmg       = 0.f;
    float m_dotInterval  = 0.5f;
    float m_dotDuration  = 0.f;
    float m_slowAmount   = 0.f;
    float m_slowDuration = 0.f;
    int   m_owner        = 0;    // 1=P1, 2=P2
};
