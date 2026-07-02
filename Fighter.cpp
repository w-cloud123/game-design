// ============================================================
// src/Fighter.cpp
// ============================================================
#include "Fighter.hpp"
#include "Constants.hpp"
#include <cmath>
#include <algorithm>

// ==================== 构造 & 重置 ====================

Fighter::Fighter(int playerIndex, const CharacterConfig& config)
    : m_playerIndex(playerIndex), m_config(config), m_type(config.type) { reset(); }

void Fighter::reset() {
    m_hp = m_config.maxHp; m_mp = m_config.maxMp;
    m_velocity = {0.f, 0.f}; m_facing = 1.f; m_onGround = true;
    m_state = FighterState::Idle;
    m_attackCD = 0.f; m_skill1CD = 0.f; m_skill2CD = 0.f;
    m_hitStunTimer = 0.f; m_stunTimer = 0.f;
    m_dotTimer = 0.f; m_dotTickTimer = 0.f;
    m_slowTimer = 0.f; m_slowAmount = 0.f;
    m_invisibleTimer = 0.f; m_invincible = false;
    m_berserkTimer = 0.f; m_damageReduction = 0.f;
    m_dashRemaining = 0.f;
    m_wantsAttack = false; m_wantsSkill1 = false; m_wantsSkill2 = false;
    m_prevJump = false; m_prevAtk = false; m_prevSk1 = false; m_prevSk2 = false;
    m_anim.setFlash(false);
}

void Fighter::setPosition(sf::Vector2f pos) { m_position = pos; m_sprite->setPosition(pos); }

void Fighter::setTexture(const sf::Texture& texture) {
    m_sprite.emplace(texture);
    m_sprite->setTextureRect(sf::IntRect({0, 0}, {64, 64}));
    // 弃用 setOrigin：改为用 top-left 定位 + 手动计算中心
    m_anim.setSprite(&*m_sprite);
}

void Fighter::setFacing(float dir) {
    m_facing = (dir >= 0.f) ? 1.f : -1.f;
    m_sprite->setScale({m_facing, 1.f});
    // 翻转时需补偿 x 偏移（因为 scale 以左上角为基准）
    if (m_facing < 0.f)
        m_sprite->setPosition({m_position.x + 64.f, m_position.y});
    else
        m_sprite->setPosition(m_position);
}

// ==================== 输入 ====================

void Fighter::handleInput() {
    if (m_isDummy || m_state == FighterState::KO || m_stunTimer > 0.f) return;
    if (m_dashRemaining > 0.f) return;

    sf::Keyboard::Scancode keyLeft, keyRight, keyJump, keyAtk, keySk1, keySk2;
    if (m_playerIndex == 1) {
        keyLeft = sf::Keyboard::Scancode::A;  keyRight = sf::Keyboard::Scancode::D;
        keyJump = sf::Keyboard::Scancode::K;  keyAtk   = sf::Keyboard::Scancode::J;
        keySk1  = sf::Keyboard::Scancode::U;  keySk2   = sf::Keyboard::Scancode::I;
    } else {
        keyLeft = sf::Keyboard::Scancode::Left;   keyRight = sf::Keyboard::Scancode::Right;
        keyJump = sf::Keyboard::Scancode::Numpad2; keyAtk  = sf::Keyboard::Scancode::Numpad1;
        keySk1  = sf::Keyboard::Scancode::Numpad4; keySk2  = sf::Keyboard::Scancode::Numpad5;
    }

    bool left  = sf::Keyboard::isKeyPressed(keyLeft);
    bool right = sf::Keyboard::isKeyPressed(keyRight);
    bool jump  = sf::Keyboard::isKeyPressed(keyJump);
    bool atk   = sf::Keyboard::isKeyPressed(keyAtk);
    bool sk1   = sf::Keyboard::isKeyPressed(keySk1);
    bool sk2   = sf::Keyboard::isKeyPressed(keySk2);

    // 移动
    if (m_hitStunTimer <= 0.f && m_stunTimer <= 0.f && m_dashRemaining <= 0.f) {
        float speed = m_config.moveSpeed;
        if (m_slowTimer > 0.f) speed *= (1.f - m_slowAmount);
        if (left && !right)      { m_velocity.x = -speed; setFacing(-1.f); }
        else if (right && !left) { m_velocity.x =  speed; setFacing(1.f);  }
        else                     { m_velocity.x = 0.f; }
    }

    // 跳跃
    if (jump && !m_prevJump && m_onGround && m_hitStunTimer <= 0.f && m_stunTimer <= 0.f) {
        m_velocity.y = Constants::JUMP_VELOCITY;
        m_onGround = false;
        enterState(FighterState::Jumping);
    }

    // 攻击请求（边缘触发）
    if (atk && !m_prevAtk && m_hitStunTimer <= 0.f && m_stunTimer <= 0.f && m_attackCD <= 0.f)
        m_wantsAttack = true;
    if (sk1 && !m_prevSk1 && m_hitStunTimer <= 0.f && m_stunTimer <= 0.f && m_skill1CD <= 0.f && m_mp >= m_config.sk1Mana)
        m_wantsSkill1 = true;
    if (sk2 && !m_prevSk2 && m_hitStunTimer <= 0.f && m_stunTimer <= 0.f && m_skill2CD <= 0.f && m_mp >= m_config.sk2Mana)
        m_wantsSkill2 = true;

    m_prevJump = jump; m_prevAtk = atk; m_prevSk1 = sk1; m_prevSk2 = sk2;
}

// ==================== 主更新 ====================

void Fighter::update(float dt, Fighter& opponent,
                     std::vector<Projectile>& projectiles,
                     std::vector<Trap>& traps) {
    if (m_state == FighterState::KO) return;

    updateTimers(dt);
    if (m_berserkTimer > 0.f) { m_berserkTimer -= dt; if (m_berserkTimer <= 0.f) endBerserk(); }
    if (m_invisibleTimer > 0.f) { m_invisibleTimer -= dt; if (m_invisibleTimer <= 0.f) m_invincible = false; }

    // DOT
    if (m_dotTimer > 0.f) {
        m_dotTimer -= dt; m_dotTickTimer += dt;
        while (m_dotTickTimer >= m_dotInterval) {
            m_dotTickTimer -= m_dotInterval;
            m_hp -= m_dotDmg;
            if (m_hp <= 0.f) { m_hp = 0.f; enterState(FighterState::KO); m_dotTimer = 0.f; break; }
        }
    }
    // 减速计时器
    if (m_slowTimer > 0.f) { m_slowTimer -= dt; if (m_slowTimer <= 0.f) m_slowAmount = 0.f; }

    // 消费攻击请求
    if (m_wantsAttack)  { m_wantsAttack = false; tryNormalAttack(projectiles, opponent); }
    if (m_wantsSkill1)  { m_wantsSkill1 = false; trySkill1(projectiles, traps, opponent); }
    if (m_wantsSkill2)  { m_wantsSkill2 = false; trySkill2(projectiles, opponent); }

    // 物理
    if (m_stunTimer > 0.f) m_velocity.x = 0.f;
    applyGravity(dt);
    if (m_dashRemaining > 0.f) {
        float step = std::min(m_dashSpeed * dt, m_dashRemaining);
        m_position.x += step * m_facing;
        m_dashRemaining -= step;
        if (getHurtbox().findIntersection(opponent.getHurtbox())) {
            opponent.takeDamage(m_config.sk1Damage);
            m_dashRemaining = 0.f;
        }
        if (m_dashRemaining <= 0.f) m_velocity.x = 0.f;
    } else {
        // 实体碰撞预判：双方着地 + 正在靠近对手 → 阻挡；远离则不挡
        float newX = m_position.x + m_velocity.x * dt;
        sf::FloatRect newBox({newX, m_position.y}, {64.f, 64.f});
        bool overlapping = newBox.findIntersection(opponent.getHurtbox()).has_value();
        bool movingTowards = (m_velocity.x > 0.f && m_position.x < opponent.getPosition().x) ||
                             (m_velocity.x < 0.f && m_position.x > opponent.getPosition().x);
        bool shouldBlock = overlapping && m_onGround && opponent.isOnGround() && movingTowards;

        if (!shouldBlock) {
            m_position.x = newX;
        } else {
            m_velocity.x = 0.f;
        }
    }
    m_position.y += m_velocity.y * dt;
    clampToGround();

    // X 轴边界限制（角色不能走出屏幕）
    if (m_position.x < 0.f) m_position.x = 0.f;
    if (m_position.x > Constants::WINDOW_WIDTH - 64.f) m_position.x = Constants::WINDOW_WIDTH - 64.f;

    // 状态机
    if (m_hitStunTimer > 0.f)           enterState(FighterState::Hit);
    else if (!m_onGround)               enterState(FighterState::Jumping);
    else if (std::abs(m_velocity.x) > 1.f || m_dashRemaining > 0.f) enterState(FighterState::Walking);
    else                                enterState(FighterState::Idle);

    // 回蓝
    m_mp = std::min(m_config.maxMp, m_mp + m_config.mpRegen * dt);

    // 同步精灵位置（翻转时偏移 64px 补偿 scale 镜像）
    if (m_facing > 0.f)
        m_sprite->setPosition(m_position);
    else
        m_sprite->setPosition({m_position.x + 64.f, m_position.y});

    m_anim.setFlash(m_hitStunTimer > 0.f);
    m_anim.update(dt);
}

// ==================== 攻击 ====================

void Fighter::tryNormalAttack(std::vector<Projectile>& projectiles, Fighter& opponent) {
    if (m_attackCD > 0.f || m_hitStunTimer > 0.f || m_stunTimer > 0.f) return;
    if (m_invincible) { m_invincible = false; m_invisibleTimer = 0.f; }
    m_attackCD = m_config.attackCD;
    enterState(FighterState::Attacking);

    if (m_config.attackRanged) {
        Projectile p;
        float dmg = m_config.attackDamage * (m_berserkTimer > 0.f ? m_config.sk2AtkDmgMult : 1.f);
        p.launch({m_position.x + 32.f + m_facing * 32.f, m_position.y + 32.f}, m_facing,
                 m_config.attackSpeed, m_config.attackRange, dmg, m_playerIndex);
        projectiles.push_back(std::move(p));
    } else {
        // 近战：本帧检测 hitbox 是否与对手 hurtbox 相交
        float hw = m_config.attackHitboxW;
        sf::FloatRect hitbox(
            {(m_facing > 0.f) ? m_position.x + 64.f : m_position.x - hw * 2.f,
             m_position.y},
            {hw * 2.f, 64.f});
        if (hitbox.findIntersection(opponent.getHurtbox())) {
            float dmg = m_config.attackDamage * (m_berserkTimer > 0.f ? m_config.sk2AtkDmgMult : 1.f);
            opponent.takeDamage(dmg);
        }
    }
}

void Fighter::trySkill1(std::vector<Projectile>& projectiles,
                        std::vector<Trap>& traps, Fighter& opponent) {
    if (m_skill1CD > 0.f || m_mp < m_config.sk1Mana || m_hitStunTimer > 0.f || m_stunTimer > 0.f) return;
    if (m_invincible) { m_invincible = false; m_invisibleTimer = 0.f; }
    m_mp -= m_config.sk1Mana; m_skill1CD = m_config.sk1CD;

    switch (m_type) {
    case CharType::Mage: {
        Projectile p;
        p.launch({m_position.x + 32.f + m_facing * 32.f, m_position.y + 32.f}, m_facing,
                 m_config.sk1Speed, m_config.sk1Range, m_config.sk1Damage, m_playerIndex);
        projectiles.push_back(std::move(p));
        break;
    }
    case CharType::Swordsman:
        m_dashRemaining = m_config.sk1DashDist; m_dashSpeed = 400.f;
        break;
    case CharType::Heavy: {
        sf::Vector2f d = opponent.getPosition() - m_position;
        if (std::sqrt(d.x * d.x + d.y * d.y) <= m_config.sk1AoeRadius) {
            opponent.takeDamage(m_config.sk1Damage);
            opponent.applyStun(m_config.sk1StunDur);
        }
        break;
    }
    case CharType::Trapper: {
        Trap t;
        t.place({m_position.x + 32.f + m_facing * 60.f, Constants::GROUND_Y},
                m_config.trapDuration, m_config.trapDotDmg, m_config.trapDotInterval,
                m_config.trapDotDuration, m_config.trapSlowAmount, m_config.trapSlowDuration,
                m_playerIndex);
        traps.push_back(std::move(t));
        break;
    }
    }
}

void Fighter::trySkill2(std::vector<Projectile>& projectiles, Fighter& opponent) {
    (void)opponent;
    if (m_skill2CD > 0.f || m_mp < m_config.sk2Mana || m_hitStunTimer > 0.f || m_stunTimer > 0.f) return;
    if (m_invincible) { m_invincible = false; m_invisibleTimer = 0.f; }
    m_mp -= m_config.sk2Mana; m_skill2CD = m_config.sk2CD;

    switch (m_type) {
    case CharType::Mage:
        m_hp = std::min(m_config.maxHp, m_hp + m_config.sk2Heal);
        break;
    case CharType::Swordsman: {
        Projectile p;
        p.launch({m_position.x + 32.f + m_facing * 32.f, m_position.y + 32.f}, m_facing,
                 m_config.sk2Speed, m_config.sk2Range, m_config.sk2Damage, m_playerIndex);
        projectiles.push_back(std::move(p));
        break;
    }
    case CharType::Heavy:
        doBerserk();
        break;
    case CharType::Trapper:
        m_invincible = true; m_invisibleTimer = m_config.sk2InvDur;
        break;
    }
}

// ==================== 狂暴 ====================

void Fighter::doBerserk() {
    if (m_berserkTimer > 0.f) return;
    m_origMoveSpeed    = m_config.moveSpeed;
    m_origAtkCD        = m_config.attackCD;
    m_origAtkDmg       = m_config.attackDamage;
    m_origDmgReduction = m_damageReduction;
    m_config.moveSpeed    *= m_config.sk2MoveSpeedMult;
    m_config.attackCD     *= m_config.sk2AtkSpeedMult;
    m_config.attackDamage *= m_config.sk2AtkDmgMult;
    m_damageReduction      = m_config.sk2DmgReduction;
    m_berserkTimer         = m_config.sk2Duration;
}

void Fighter::endBerserk() {
    m_config.moveSpeed     = m_origMoveSpeed;
    m_config.attackCD      = m_origAtkCD;
    m_config.attackDamage  = m_origAtkDmg;
    m_damageReduction      = m_origDmgReduction;
    m_berserkTimer         = 0.f;
}

// ==================== 受击 ====================

void Fighter::takeDamage(float damage) {
    if (m_invincible || m_state == FighterState::KO) return;
    float dmg = damage * (1.f - m_damageReduction);
    if (dmg < 1.f) dmg = 1.f;
    m_hp -= dmg;
    m_hitStunTimer = Constants::HIT_STUN;
    m_velocity.x = 0.f;
    if (m_hp <= 0.f) { m_hp = 0.f; enterState(FighterState::KO); }
}

void Fighter::applyStun(float d)  { m_stunTimer = d; m_velocity.x = 0.f; }
void Fighter::applyDot(float dmg, float iv, float dur) { m_dotTimer = dur; m_dotTickTimer = 0.f; m_dotDmg = dmg; m_dotInterval = iv; }
void Fighter::applySlow(float amt, float dur) { m_slowTimer = dur; m_slowAmount = amt; }

// ==================== 物理 & 碰撞 ====================

sf::FloatRect Fighter::getHurtbox() const {
    // m_position = sprite 左上角；hurtbox 为角色碰撞体中心对齐
    return sf::FloatRect({m_position.x, m_position.y}, {64.f, 64.f});
}

void Fighter::applyGravity(float dt) { if (!m_onGround) m_velocity.y += Constants::GRAVITY * dt; }

void Fighter::clampToGround() {
    // m_position = 左上角；脚底在 y + 64
    if (m_position.y >= Constants::GROUND_Y - 64.f && m_velocity.y >= 0.f) {
        m_position.y = Constants::GROUND_Y - 64.f;
        m_velocity.y = 0.f;
        m_onGround = true;
    }
}

void Fighter::updateTimers(float dt) {
    if (m_attackCD  > 0.f) m_attackCD  = std::max(0.f, m_attackCD  - dt);
    if (m_skill1CD  > 0.f) m_skill1CD  = std::max(0.f, m_skill1CD  - dt);
    if (m_skill2CD  > 0.f) m_skill2CD  = std::max(0.f, m_skill2CD  - dt);
    if (m_hitStunTimer > 0.f) m_hitStunTimer = std::max(0.f, m_hitStunTimer - dt);
    if (m_stunTimer    > 0.f) m_stunTimer    = std::max(0.f, m_stunTimer    - dt);
}

void Fighter::enterState(FighterState s) { m_state = s; }

// ==================== 调试 & 渲染 ====================

std::string Fighter::getStateString() const {
    switch (m_state) {
    case FighterState::Idle: return "Idle"; case FighterState::Walking: return "Walking";
    case FighterState::Jumping: return "Jumping"; case FighterState::Attacking: return "Attacking";
    case FighterState::Hit: return "Hit"; case FighterState::KO: return "KO";
    }
    return "?";
}

void Fighter::draw(sf::RenderWindow& window) {
    if (m_invincible && m_type == CharType::Trapper) {
        // 半透明效果：alpha = 128（0-255）
        sf::Color c = m_sprite->getColor();
        m_sprite->setColor(sf::Color(c.r, c.g, c.b, 128));
        window.draw(*m_sprite);
        m_sprite->setColor(c); // 恢复原色
    } else {
        window.draw(*m_sprite);
    }
}
