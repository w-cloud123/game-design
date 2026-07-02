// ============================================================
// src/Fighter.hpp — 角色类
// ============================================================
#pragma once
#include <SFML/Graphics.hpp>
#include "CharacterConfig.hpp"
#include "Animation.hpp"
#include "Projectile.hpp"
#include "Trap.hpp"
#include <vector>
#include <string>
#include <optional>

enum class FighterState { Idle, Walking, Jumping, Attacking, Hit, KO };

class Fighter {
public:
    Fighter(int playerIndex, const CharacterConfig& config);

    void setPosition(sf::Vector2f pos);
    sf::Vector2f getPosition() const { return m_position; }
    void setTexture(const sf::Texture& texture);
    void setFacing(float dir);

    void handleInput();
    void update(float dt, Fighter& opponent,
                std::vector<Projectile>& projectiles,
                std::vector<Trap>& traps);

    // 受击
    void takeDamage(float damage);
    void applyStun(float duration);
    void applyDot(float dmgPerTick, float interval, float duration);
    void applySlow(float amount, float duration);

    // 查询
    bool isKO()          const { return m_state == FighterState::KO; }
    bool isInvincible()  const { return m_invincible; }
    bool isOnGround()    const { return m_onGround; }
    float getHp()        const { return m_hp; }
    float getMp()        const { return m_mp; }
    float getMaxHp()     const { return m_config.maxHp; }
    float getMaxMp()     const { return m_config.maxMp; }
    const std::string& getName() const { return m_config.name; }

    float getAtkCD()     const { return m_attackCD; }
    float getAtkCDMax()  const { return m_config.attackCD; }
    float getSk1CD()     const { return m_skill1CD; }
    float getSk1CDMax()  const { return m_config.sk1CD; }
    float getSk2CD()     const { return m_skill2CD; }
    float getSk2CDMax()  const { return m_config.sk2CD; }

    sf::FloatRect getHurtbox() const;
    std::string getStateString() const;

    void setDummy(bool d) { m_isDummy = d; }
    void reset();
    void draw(sf::RenderWindow& window);

private:
    void applyGravity(float dt);
    void clampToGround();
    void updateTimers(float dt);
    void enterState(FighterState s);

    // 攻击
    void tryNormalAttack(std::vector<Projectile>& projectiles, Fighter& opponent);
    void trySkill1(std::vector<Projectile>& projectiles, std::vector<Trap>& traps, Fighter& opponent);
    void trySkill2(std::vector<Projectile>& projectiles, Fighter& opponent);

    void doBerserk();
    void endBerserk();

    int  m_playerIndex;
    CharacterConfig m_config;
    CharType m_type;

    std::optional<sf::Sprite> m_sprite;
    Animation  m_anim;

    sf::Vector2f m_position{0.f, 0.f};
    sf::Vector2f m_velocity{0.f, 0.f};
    float m_facing   = 1.f;
    bool  m_onGround = true;

    FighterState m_state = FighterState::Idle;

    float m_hp = 0.f;
    float m_mp = 0.f;

    float m_attackCD  = 0.f;
    float m_skill1CD  = 0.f;
    float m_skill2CD  = 0.f;
    float m_hitStunTimer = 0.f;
    float m_stunTimer    = 0.f;

    // DOT
    float m_dotTimer    = 0.f;
    float m_dotTickTimer = 0.f;
    float m_dotDmg      = 0.f;
    float m_dotInterval = 0.5f;

    // 减速
    float m_slowTimer  = 0.f;
    float m_slowAmount = 0.f;

    // 隐匿
    float m_invisibleTimer = 0.f;
    bool  m_invincible     = false;

    // 狂暴
    float m_berserkTimer       = 0.f;
    float m_origMoveSpeed      = 0.f;
    float m_origAtkCD          = 0.f;
    float m_origAtkDmg         = 0.f;
    float m_origDmgReduction   = 0.f;
    float m_damageReduction    = 0.f;

    // 突进
    float m_dashRemaining = 0.f;
    float m_dashSpeed     = 0.f;

    bool m_wantsAttack = false;
    bool m_wantsSkill1 = false;
    bool m_wantsSkill2 = false;
    bool m_isDummy     = false;

    bool m_prevJump = false;
    bool m_prevAtk  = false;
    bool m_prevSk1  = false;
    bool m_prevSk2  = false;
};
