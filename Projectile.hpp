// ============================================================
// src/Projectile.hpp — 抛射物（火球/飞刀/剑气通用）
// ============================================================
// 一个沿 X 轴匀速飞行的"子弹"，有最大射程和伤害值。
// 碰撞体是 12×12 的小方块，居中于抛射物位置。
//
// 所有权：由 BattleState::m_proj 列表持有。
// 生命周期：发射(launch) → 飞行(update) → 命中/超距/出界(kill) → 清除(erase)
//
// owner 字段用于防止"自己打自己"——P1 的抛射物不会伤到 P1。
// onHitMp 字段存储命中后给发射者的回蓝量。
// ============================================================
#pragma once
#include <SFML/Graphics.hpp>

class Projectile {
public:
    /// @param pos     起始位置（像素坐标）
    /// @param dirX    方向（1=右, -1=左）
    /// @param speed   飞行速度（px/s）
    /// @param range   最大飞行距离（px）
    /// @param damage  命中伤害
    /// @param owner   发射者编号（1=P1, 2=P2）
    /// @param onHitMp 命中回蓝量
    void launch(sf::Vector2f pos, float dirX, float speed,
                float range, float damage, int owner, float onHitMp = 0.f);

    void update(float dt);          // 移动 + 超距/出界检测
    bool isDead() const { return m_dead; }
    sf::FloatRect getBounds() const; // 12×12 碰撞盒
    float getDamage()   const { return m_damage; }
    int   getOwner()    const { return m_owner; }
    float getOnHitMp()  const { return m_onHitMp; }
    void draw(sf::RenderWindow& w) const;
    void kill() { m_dead = true; }

private:
    sf::Vector2f m_position, m_velocity;  // 位置 + 速度向量
    float m_range=0.f, m_traveled=0.f;    // 射程上限 + 已飞行距离
    float m_damage=0.f, m_onHitMp=0.f;   // 伤害 + 回蓝量
    bool  m_dead = true;                  // true=待清除
    int   m_owner = 0;                    // 发射者编号
};
