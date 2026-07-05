// ============================================================
// src/Trap.hpp — 陷阱（陷阱师技能1）
// ============================================================
// 放置在地面的不可见区域，有存活时间限制。
// 对手的碰撞框与之重叠时触发，对触碰者施加两个效果：
//   1. DOT（持续伤害）：每 interval 秒扣 dmg 血，持续 duration 秒
//   2. 减速：移速降为原来的 (1 - slowAmount)
//
// owner 字段防止陷阱师踩到自己的陷阱。
// ============================================================
#pragma once
#include <SFML/Graphics.hpp>

class Trap {
public:
    void place(sf::Vector2f pos, float duration,
               float dotDmg, float dotInterval, float dotDuration,
               float slowAmount, float slowDuration, int owner);
    void update(float dt);            // 倒计时寿命
    bool isExpired()   const { return m_expired; }
    bool isTriggered() const { return m_triggered; }
    sf::FloatRect getBounds() const;  // 20×10 触发区域
    void trigger();                    // 标记为已触发（同时设置过期）
    int  getOwner()    const { return m_owner; }

    // DOT / 减速参数（由触发代码读取后施加到受害者 Fighter 上）
    float getDotDamage()    const { return m_dotDmg; }
    float getDotInterval()  const { return m_dotInterval; }
    float getDotDuration()  const { return m_dotDuration; }
    float getSlowAmount()   const { return m_slowAmount; }
    float getSlowDuration() const { return m_slowDuration; }

    void draw(sf::RenderWindow& w) const;

private:
    sf::Vector2f m_position;
    float m_lifetime = 0.f;
    bool  m_expired  = true, m_triggered = false;
    float m_dotDmg=0.f, m_dotInterval=0.5f, m_dotDuration=0.f;
    float m_slowAmount=0.f, m_slowDuration=0.f;
    int   m_owner = 0;
};
