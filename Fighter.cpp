// ============================================================
// src/Fighter.cpp — 角色类实现（约 420 行）
// ============================================================
// 阅读指南：
//   1. 构造 & reset()        — 开局状态初始化
//   2. handleInput()         — 键盘→移动/攻击请求
//   3. update(dt, ...)       — 主更新（10 个步骤，按顺序执行）
//   4. tryNormalAttack()     — 普攻（近战 hitbox / 远程抛射物）
//   5. trySkill1()           — 技能1（4 个角色完全不同）
//   6. trySkill2()           — 技能2（同上）
//   7. takeDamage/gainMp/... — 外部可调用的效果接口
//   8. 物理 & 碰撞体         — 重力、落地、受击框
// ============================================================
#include "Fighter.hpp"
#include "Constants.hpp"
#include <cmath>
#include <algorithm>

// ==================== 构造 & 重置 ====================

Fighter::Fighter(int playerIndex, const CharacterConfig& config)
    : m_playerIndex(playerIndex), m_config(config)
{
    // 根据玩家编号初始化键位映射（构造后不再改变）
    if (m_playerIndex == 1) {
        // P1：左手 A/D 移动，右手 J/K/U/I 攻击/技能
        m_keyLeft = sf::Keyboard::Scancode::A;  m_keyRight = sf::Keyboard::Scancode::D;
        m_keyJump = sf::Keyboard::Scancode::K;  m_keyAtk   = sf::Keyboard::Scancode::J;
        m_keySk1  = sf::Keyboard::Scancode::U;  m_keySk2   = sf::Keyboard::Scancode::I;
    } else {
        // P2：方向键移动，小键盘攻击/技能
        m_keyLeft = sf::Keyboard::Scancode::Left;   m_keyRight = sf::Keyboard::Scancode::Right;
        m_keyJump = sf::Keyboard::Scancode::Numpad2; m_keyAtk  = sf::Keyboard::Scancode::Numpad1;
        m_keySk1  = sf::Keyboard::Scancode::Numpad4; m_keySk2  = sf::Keyboard::Scancode::Numpad5;
    }
    reset();
}

// 把所有状态复位到"刚开局"。
// 蓝量从 0 开始（鼓励主动进攻积累资源），血量回满。
void Fighter::reset() {
    m_hp = m_config.maxHp; m_mp = 0.f;
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
    m_flashActive = false;
}

// ==================== 精灵管理 ====================

void Fighter::setPosition(sf::Vector2f pos) { m_position = pos; m_sprite->setPosition(pos); }

// SFML 3.x 的 sf::Sprite 必须用纹理构造。我们用 optional::emplace 延迟初始化。
// textureRect 裁切整个 96×96 纹理——因为我们的精灵是单帧的，整张纹理就是角色。
void Fighter::setTexture(const sf::Texture& texture) {
    m_sprite.emplace(texture);
    m_sprite->setTextureRect(sf::IntRect({0, 0}, {96, 96}));
}

// 朝向处理：scale(-1, 1) 水平翻转精灵。
// 翻转以左上角为轴，所以翻转后位置会偏移一个精灵宽度。
// 需要手动补偿：flip(←) 时把 sprite 位置右移 FIGHTER_SIZE 像素。
void Fighter::setFacing(float dir) {
    m_facing = (dir >= 0.f) ? 1.f : -1.f;
    m_sprite->setScale({m_facing, 1.f});
    if (m_facing < 0.f)
        m_sprite->setPosition({m_position.x + Constants::FIGHTER_SIZE, m_position.y});
    else
        m_sprite->setPosition(m_position);
}

// ==================== 输入处理 ====================
// 设计要点：
//   1. 移动是"持续按下检测"：按住就持续移动
//   2. 跳跃和攻击是"边缘触发"：只在按下瞬间触发一次，防止按住连跳/连发
//   3. 边缘触发通过 m_prevXxx 记录上帧状态来实现：本帧为 true + 上帧为 false = 刚按下
//   4. 攻击不在此处执行，只设标志——因为近战碰撞需要 opponent，在 update 里才能拿到

void Fighter::handleInput() {
    // 这些状态下不接受任何输入
    if (m_isDummy || m_state == FighterState::KO || m_stunTimer > 0.f) return;
    if (m_dashRemaining > 0.f) return; // 突进中不响应方向输入（保持冲刺方向）

    // 读取当前帧按键
    bool left  = sf::Keyboard::isKeyPressed(m_keyLeft);
    bool right = sf::Keyboard::isKeyPressed(m_keyRight);
    bool jump  = sf::Keyboard::isKeyPressed(m_keyJump);
    bool atk   = sf::Keyboard::isKeyPressed(m_keyAtk);
    bool sk1   = sf::Keyboard::isKeyPressed(m_keySk1);
    bool sk2   = sf::Keyboard::isKeyPressed(m_keySk2);

    // ---- 水平移动 ----
    // 左右同时按 → 不动；只按一个 → 向该方向移动
    // 处于减速状态时移速按比例降低
    if (m_hitStunTimer <= 0.f && m_stunTimer <= 0.f && m_dashRemaining <= 0.f) {
        float speed = m_config.moveSpeed;
        if (m_slowTimer > 0.f) speed *= (1.f - m_slowAmount);
        if (left && !right)      { m_velocity.x = -speed; setFacing(-1.f); }
        else if (right && !left) { m_velocity.x =  speed; setFacing(1.f);  }
        else                     { m_velocity.x = 0.f; }
    }

    // ---- 跳跃（边缘触发） ----
    if (jump && !m_prevJump && m_onGround && m_hitStunTimer <= 0.f && m_stunTimer <= 0.f) {
        m_velocity.y = Constants::JUMP_VELOCITY; // 瞬间赋予向上速度
        m_onGround = false;
        m_state = FighterState::Jumping;
    }

    // ---- 攻击请求（边缘触发 + CD/蓝量/硬直检查） ----
    if (atk && !m_prevAtk && m_hitStunTimer <= 0.f && m_stunTimer <= 0.f && m_attackCD <= 0.f)
        m_wantsAttack = true;
    if (sk1 && !m_prevSk1 && m_hitStunTimer <= 0.f && m_stunTimer <= 0.f && m_skill1CD <= 0.f && m_mp >= m_config.sk1Mana)
        m_wantsSkill1 = true;
    if (sk2 && !m_prevSk2 && m_hitStunTimer <= 0.f && m_stunTimer <= 0.f && m_skill2CD <= 0.f && m_mp >= m_config.sk2Mana)
        m_wantsSkill2 = true;

    // 保存本帧按键状态 → 下一帧用来检测"刚按下"
    m_prevJump = jump; m_prevAtk = atk; m_prevSk1 = sk1; m_prevSk2 = sk2;
}

// ==================== 主更新 ====================
// 每帧按以下顺序执行 10 个步骤。顺序很重要——
// 比如必须先更新计时器再消费攻击请求（确保 CD 倒计时准确），
// 先移动再检查边界限制（确保撞墙后位置合法）。

void Fighter::update(float dt, Fighter& opponent,
                     std::vector<Projectile>& projectiles,
                     std::vector<Trap>& traps) {
    if (m_state == FighterState::KO) return;

    // 1. 所有计时器倒计时 + 特殊 buff 到期处理
    updateTimers(dt);
    if (m_berserkTimer > 0.f)   { m_berserkTimer -= dt;   if (m_berserkTimer <= 0.f)   endBerserk(); }
    if (m_invisibleTimer > 0.f) { m_invisibleTimer -= dt; if (m_invisibleTimer <= 0.f) m_invincible = false; }

    // 2. DOT（持续伤害）
    //    用 while 而非 if——如果 dt 异常大（断点调试），可能一次跳过好几跳
    if (m_dotTimer > 0.f) {
        m_dotTimer -= dt;
        m_dotTickTimer += dt;
        while (m_dotTickTimer >= m_dotInterval) {
            m_dotTickTimer -= m_dotInterval;
            m_hp -= m_dotDmg;
            if (m_hp <= 0.f) { m_hp = 0.f; m_state = FighterState::KO; m_dotTimer = 0.f; break; }
        }
    }

    // 3. 减速倒计时
    if (m_slowTimer > 0.f) { m_slowTimer -= dt; if (m_slowTimer <= 0.f) m_slowAmount = 0.f; }

    // 4. 消费攻击请求（handleInput 设的标志）
    if (m_wantsAttack) { m_wantsAttack = false; tryNormalAttack(projectiles, opponent); }
    if (m_wantsSkill1) { m_wantsSkill1 = false; trySkill1(projectiles, traps, opponent); }
    if (m_wantsSkill2) { m_wantsSkill2 = false; trySkill2(projectiles, opponent); }

    // 5. 物理移动
    //    分支 A：突进中（剑士技能1）——高速向 facing 方向移动
    //    分支 B：正常移动——水平位移 + 角色间碰撞推离
    if (m_stunTimer > 0.f) m_velocity.x = 0.f;
    applyGravity(dt);

    if (m_dashRemaining > 0.f) {
        // ----- 突进 ----
        float step = std::min(m_dashSpeed * dt, m_dashRemaining);
        m_position.x += step * m_facing;
        m_dashRemaining -= step;
        if (getHurtbox().findIntersection(opponent.getHurtbox())) {
            opponent.takeDamage(m_config.sk1Damage);
            gainMp(m_config.onHitMpGain);
            m_dashRemaining = 0.f;
        }
        if (m_dashRemaining <= 0.f) m_velocity.x = 0.f;
    } else {
        // ----- 正常移动 + 角色间推离碰撞 -----
        // 碰撞逻辑：计算"如果正常移动，我的碰撞框会在哪里"，
        // 如果和对手的碰撞框重叠，则把我推到刚好不重叠的位置。
        // 推离距离 = 两个框的重叠量（overlap->size.x）。
        float newX = m_position.x + m_velocity.x * dt;
        bool blocked = false;

        // 仅在双方都着地时阻挡（允许空中跳越对手）
        if (m_onGround && opponent.isOnGround()) {
            float margin = (Constants::FIGHTER_SIZE - Constants::HURTBOX_WIDTH) / 2.f;
            sf::FloatRect myBox({newX + margin, m_position.y},
                                {Constants::HURTBOX_WIDTH, Constants::FIGHTER_SIZE});
            if (auto overlap = myBox.findIntersection(opponent.getHurtbox())) {
                if (m_velocity.x > 0.f)
                    m_position.x = newX - overlap->size.x;  // 向右走 → 被向左推
                else if (m_velocity.x < 0.f)
                    m_position.x = newX + overlap->size.x;  // 向左走 → 被向右推
                m_velocity.x = 0.f;
                blocked = true;
            }
        }
        if (!blocked)
            m_position.x = newX;
    }
    m_position.y += m_velocity.y * dt;  // Y 轴不受碰撞阻挡（重力自己处理落地）
    clampToGround();                     // 检测并钳制到地面

    // 6. X 轴边界（不能走出屏幕）
    if (m_position.x < 0.f) m_position.x = 0.f;
    if (m_position.x > Constants::WINDOW_WIDTH - Constants::FIGHTER_SIZE)
        m_position.x = Constants::WINDOW_WIDTH - Constants::FIGHTER_SIZE;

    // 7. 状态机：优先级从高到低判定
    //    硬直 → Hit；不在地面 → Jumping；水平速度非零 → Walking；否则 → Idle
    if (m_hitStunTimer > 0.f)
        m_state = FighterState::Hit;
    else if (!m_onGround)
        m_state = FighterState::Jumping;
    else if (std::abs(m_velocity.x) > 1.f || m_dashRemaining > 0.f)
        m_state = FighterState::Walking;
    else
        m_state = FighterState::Idle;

    // 8. 被动回蓝（每秒 mpRegen 点）
    m_mp = std::min(m_config.maxMp, m_mp + m_config.mpRegen * dt);

    // 9. 同步精灵位置（翻转时需要偏移补偿，同 setFacing 的原理）
    if (m_facing > 0.f)
        m_sprite->setPosition(m_position);
    else
        m_sprite->setPosition({m_position.x + Constants::FIGHTER_SIZE, m_position.y});

    // 10. 受击闪红标记（下一帧 draw() 中生效）
    m_flashActive = (m_hitStunTimer > 0.f);
}

// ==================== 普攻 ====================

void Fighter::tryNormalAttack(std::vector<Projectile>& projectiles, Fighter& opponent) {
    if (m_attackCD > 0.f || m_hitStunTimer > 0.f || m_stunTimer > 0.f) return;

    // 隐匿中攻击 → 自动解除隐匿
    if (m_invincible) { m_invincible = false; m_invisibleTimer = 0.f; }

    m_attackCD = m_config.attackCD;  // 进入冷却
    m_state = FighterState::Attacking;

    if (m_config.attackRanged) {
        // ---- 远程：创建抛射物 ----
        // 生成位置：角色中心 + facing 方向偏移半宽 = 角色正前方
        // 狂暴时攻击力乘以狂暴倍率
        Projectile p;
        float dmg = m_config.attackDamage * (m_berserkTimer > 0.f ? m_config.sk2AtkDmgMult : 1.f);
        p.launch(
            {m_position.x + Constants::FIGHTER_SIZE / 2.f + m_facing * Constants::FIGHTER_SIZE / 2.f,
             m_position.y + Constants::FIGHTER_SIZE / 2.f},
            m_facing, m_config.attackSpeed, m_config.attackRange, dmg, m_playerIndex,
            m_config.onHitMpGain);
        projectiles.push_back(std::move(p));
    } else {
        // ---- 近战：生成 hitbox 并立即检测碰撞 ----
        // hitbox 放在角色前方：x = 角色右边缘（facing 右）或 hitbox 左边缘（facing 左）
        float hw = m_config.attackHitboxW;  // 半宽
        sf::FloatRect hitbox(
            {(m_facing > 0.f) ? m_position.x + Constants::FIGHTER_SIZE
                              : m_position.x - hw * 2.f,
             m_position.y},
            {hw * 2.f, Constants::FIGHTER_SIZE});
        if (hitbox.findIntersection(opponent.getHurtbox())) {
            float dmg = m_config.attackDamage * (m_berserkTimer > 0.f ? m_config.sk2AtkDmgMult : 1.f);
            opponent.takeDamage(dmg);
            gainMp(m_config.onHitMpGain);  // 命中回蓝
        }
    }
}

// ==================== 技能1 ====================

void Fighter::trySkill1(std::vector<Projectile>& projectiles,
                        std::vector<Trap>& traps, Fighter& opponent) {
    // 条件检查：冷却中 / 蓝量不够 / 硬直中 / 眩晕中 → 不能放
    if (m_skill1CD > 0.f || m_mp < m_config.sk1Mana || m_hitStunTimer > 0.f || m_stunTimer > 0.f) return;
    if (m_invincible) { m_invincible = false; m_invisibleTimer = 0.f; }
    m_mp -= m_config.sk1Mana;    // 扣蓝
    m_skill1CD = m_config.sk1CD; // 进入冷却

    switch (m_config.type) {
    case CharType::Mage:
        // 火球术：和普攻一样的抛射物，但数值更大（更远、更快、更高伤害）
        {
            Projectile p;
            p.launch({m_position.x + Constants::FIGHTER_SIZE / 2.f + m_facing * Constants::FIGHTER_SIZE / 2.f,
                      m_position.y + Constants::FIGHTER_SIZE / 2.f},
                     m_facing, m_config.sk1Speed, m_config.sk1Range, m_config.sk1Damage,
                     m_playerIndex, m_config.onHitMpGain);
            projectiles.push_back(std::move(p));
        }
        break;

    case CharType::Swordsman:
        // 突进斩：设置 dash 参数，后续帧在 update() 的 dash 分支中逐帧移动
        // 碰到对手或跑完距离自动停止
        m_dashRemaining = m_config.sk1DashDist;
        m_dashSpeed     = Constants::DASH_SPEED;
        break;

    case CharType::Heavy:
        // 大地震击：以自身为中心检查圆形 AOE，范围内敌人眩晕并受伤
        // 用两点间距离公式 sqrt(dx²+dy²) ≤ 半径
        {
            sf::Vector2f d = opponent.getPosition() - m_position;
            if (std::sqrt(d.x * d.x + d.y * d.y) <= m_config.sk1AoeRadius) {
                opponent.takeDamage(m_config.sk1Damage);
                opponent.applyStun(m_config.sk1StunDur);
                gainMp(m_config.onHitMpGain);
            }
        }
        break;

    case CharType::Trapper:
        // 布置陷阱：在地面角色前方放置，持续 20s，被对手踩到触发 DOT+减速
        {
            Trap t;
            t.place({m_position.x + Constants::FIGHTER_SIZE / 2.f + m_facing * Constants::TRAP_OFFSET_X,
                     Constants::GROUND_Y},
                    m_config.trapDuration, m_config.trapDotDmg, m_config.trapDotInterval,
                    m_config.trapDotDuration, m_config.trapSlowAmount, m_config.trapSlowDuration,
                    m_playerIndex);
            traps.push_back(std::move(t));
        }
        break;
    }
}

// ==================== 技能2 ====================

void Fighter::trySkill2(std::vector<Projectile>& projectiles, Fighter& opponent) {
    (void)opponent; // 部分角色（Mage/Heavy/Trapper）不需要 opponent 引用
    if (m_skill2CD > 0.f || m_mp < m_config.sk2Mana || m_hitStunTimer > 0.f || m_stunTimer > 0.f) return;
    if (m_invincible) { m_invincible = false; m_invisibleTimer = 0.f; }
    m_mp -= m_config.sk2Mana;
    m_skill2CD = m_config.sk2CD;

    switch (m_config.type) {
    case CharType::Mage:
        // 治愈术：瞬间回血（不受减伤影响，直接加回 m_hp）
        m_hp = std::min(m_config.maxHp, m_hp + m_config.sk2Heal);
        break;

    case CharType::Swordsman:
        // 剑气斩：远程直线抛射物，数值比普攻高
        {
            Projectile p;
            p.launch({m_position.x + Constants::FIGHTER_SIZE / 2.f + m_facing * Constants::FIGHTER_SIZE / 2.f,
                      m_position.y + Constants::FIGHTER_SIZE / 2.f},
                     m_facing, m_config.sk2Speed, m_config.sk2Range, m_config.sk2Damage,
                     m_playerIndex, m_config.onHitMpGain);
            projectiles.push_back(std::move(p));
        }
        break;

    case CharType::Heavy:
        // 狂暴：直接修改 m_config（移动速度/攻击力/CD），到期后恢复
        // ⚠ 如果狂暴期间调用 reset()，m_config 中的值是修改后的，会永久错误
        doBerserk();
        break;

    case CharType::Trapper:
        // 隐匿：2s 无敌 + 不渲染（draw() 中透明度降低）
        // 主动攻击或放技能会自动解除隐匿
        m_invincible     = true;
        m_invisibleTimer = m_config.sk2InvDur;
        break;
    }
}

// ==================== 狂暴 ====================

void Fighter::doBerserk() {
    if (m_berserkTimer > 0.f) return; // 已在狂暴中，忽略重复激活

    // 备份原始值——必须备份，否则到期后无法恢复
    m_origMoveSpeed    = m_config.moveSpeed;
    m_origAtkCD        = m_config.attackCD;
    m_origAtkDmg       = m_config.attackDamage;
    m_origDmgReduction = m_damageReduction;

    // 应用增益（直接改 m_config，所有读取 m_config 的代码自动生效）
    m_config.moveSpeed    *= m_config.sk2MoveSpeedMult; // 1.25 倍移速
    m_config.attackCD     *= m_config.sk2AtkSpeedMult;  // 0.6 倍 CD（更快）
    m_config.attackDamage *= m_config.sk2AtkDmgMult;    // 1.2 倍攻击
    m_damageReduction      = m_config.sk2DmgReduction;  // 30% 减伤
    m_berserkTimer         = m_config.sk2Duration;
}

void Fighter::endBerserk() {
    m_config.moveSpeed     = m_origMoveSpeed;
    m_config.attackCD      = m_origAtkCD;
    m_config.attackDamage  = m_origAtkDmg;
    m_damageReduction      = m_origDmgReduction;
    m_berserkTimer         = 0.f;
}

// ==================== 受击 & 负面效果 ====================

void Fighter::takeDamage(float damage) {
    if (m_invincible || m_state == FighterState::KO) return;

    // 减伤计算：如果有狂暴 buff（30% 减伤），实际伤害*0.7
    float dmg = damage * (1.f - m_damageReduction);
    if (dmg < 1.f) dmg = 1.f; // 保底 1 点伤害，防止减伤叠到 100% 后永远不掉血
    m_hp -= dmg;

    // 受击硬直：0.12s 内无法操作（天然防连招保护）
    m_hitStunTimer = Constants::HIT_STUN;
    m_velocity.x = 0.f;

    if (m_hp <= 0.f) { m_hp = 0.f; m_state = FighterState::KO; }
}

void Fighter::gainMp(float amount)   { m_mp = std::min(m_config.maxMp, m_mp + amount); }
void Fighter::applyStun(float d)    { m_stunTimer = d; m_velocity.x = 0.f; }
void Fighter::applyDot(float dmg, float iv, float dur) {
    m_dotTimer = dur; m_dotTickTimer = 0.f; m_dotDmg = dmg; m_dotInterval = iv;
}
void Fighter::applySlow(float amt, float dur) { m_slowTimer = dur; m_slowAmount = amt; }

// ==================== 物理 & 碰撞体 ====================

// 返回碰撞检测用的矩形。
// 宽度使用 HURTBOX_WIDTH（44px），水平居中于精灵画布内，
// 这样角色边缘的透明留白不会被算进碰撞。
sf::FloatRect Fighter::getHurtbox() const {
    float margin = (Constants::FIGHTER_SIZE - Constants::HURTBOX_WIDTH) / 2.f;
    return sf::FloatRect({m_position.x + margin, m_position.y},
                         {Constants::HURTBOX_WIDTH, Constants::FIGHTER_SIZE});
}

void Fighter::applyGravity(float dt) {
    if (!m_onGround) m_velocity.y += Constants::GRAVITY * dt;
}

// 落地条件：脚底 (m_position.y + FIGHTER_SIZE) 摸到或超过地面线，
//           且速度方向向下（v.y >= 0，防止上升阶段就被钳制）
void Fighter::clampToGround() {
    if (m_position.y >= Constants::GROUND_Y - Constants::FIGHTER_SIZE && m_velocity.y >= 0.f) {
        m_position.y = Constants::GROUND_Y - Constants::FIGHTER_SIZE;
        m_velocity.y = 0.f;
        m_onGround = true;
    }
}

// 统一的倒计时管理：所有冷却都用 std::max(0, ...) 防止减到负数
void Fighter::updateTimers(float dt) {
    if (m_attackCD     > 0.f) m_attackCD     = std::max(0.f, m_attackCD     - dt);
    if (m_skill1CD     > 0.f) m_skill1CD     = std::max(0.f, m_skill1CD     - dt);
    if (m_skill2CD     > 0.f) m_skill2CD     = std::max(0.f, m_skill2CD     - dt);
    if (m_hitStunTimer > 0.f) m_hitStunTimer = std::max(0.f, m_hitStunTimer - dt);
    if (m_stunTimer    > 0.f) m_stunTimer    = std::max(0.f, m_stunTimer    - dt);
}

// ==================== 渲染 ====================

// 渲染逻辑很简单：
//   1. 基础颜色：受击闪红 / 正常白色
//   2. 如果当前是陷阱师隐匿状态 → alpha 设为 128（半透明）
//   3. setColor 会叠加到纹理上（白色=原色，红色=染红，半透明=可见但淡）
void Fighter::draw(sf::RenderWindow& window) {
    sf::Color color = m_flashActive ? sf::Color::Red : sf::Color::White;
    if (m_invincible && m_config.type == CharType::Trapper)
        color.a = 128;
    m_sprite->setColor(color);
    window.draw(*m_sprite);
}
