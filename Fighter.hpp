// ============================================================
// src/Fighter.hpp — 角色类（游戏最核心的类）
// ============================================================
// 设计理念：
//   四个角色（法师/剑士/重武器/陷阱师）共用这一个 Fighter 类。
//   差异完全由 CharacterConfig 的数值驱动，技能逻辑通过 switch 分派。
//
// 状态机：
//   Idle → Walking（移动中）→ Jumping（空中）→ Attacking（攻击帧）
//   ↓                                                   ↓
//   KO ←（HP≤0）← Hit ←（受击）←─────────────────────┘
//
// 输入处理方式：
//   handleInput() 每帧读取键盘，但不直接执行攻击——
//   而是设置 m_wantsAttack / m_wantsSkill1 / m_wantsSkill2 标志。
//   update() 消费这些标志并执行攻击逻辑。
//   这样设计是因为攻击需要 opponent 引用，而 opponent 在 update() 调用链中才安全可用。
//
// SFML 3.x 注意事项：
//   sf::Sprite 没有默认构造函数，必须用纹理构造。
//   因此 Fighter 用 std::optional<sf::Sprite> 延迟到 setTexture() 时 emplace。
// ============================================================
#pragma once
#include <SFML/Graphics.hpp>
#include "CharacterConfig.hpp"
#include "Projectile.hpp"
#include "Trap.hpp"
#include <vector>
#include <string>
#include <optional>

enum class FighterState {
    Idle,      // 站立不动
    Walking,   // 水平移动（地面）
    Jumping,   // 空中（上升/下落）
    Attacking, // 攻击动作帧（瞬时，下一帧即恢复）
    Hit,       // 受击硬直中（0.12s，期间无法操作）
    KO         // 死亡（不再更新、不再渲染）
};

class Fighter {
public:
    // ---- 构造 ----
    /// @param playerIndex  1=P1, 2=P2（决定键位和初始朝向）
    /// @param config       角色参数（来自 CharacterConfig 工厂函数，按值拷贝）
    Fighter(int playerIndex, const CharacterConfig& config);

    // ---- 位置 & 精灵 ----
    void setPosition(sf::Vector2f pos);
    sf::Vector2f getPosition() const { return m_position; }
    void setTexture(const sf::Texture& texture);
    void setFacing(float dir); // dir>=0=朝右, dir<0=朝左（翻转精灵+位置补偿）

    // ---- 每帧调用 ----
    void handleInput(); // 读取键盘 → 设置移动速度 / 攻击请求标志
    /// @param dt           帧间隔（秒）
    /// @param opponent     对手引用（近战碰撞、AOE 伤害需要）
    /// @param projectiles  场景抛射物列表（本帧新生成的追加进去）
    /// @param traps        场景陷阱列表（同上）
    void update(float dt, Fighter& opponent,
                std::vector<Projectile>& projectiles, std::vector<Trap>& traps);

    // ---- 受击 & 负面效果（由 BattleState 或对手的 attack 方法调用） ----
    void takeDamage(float damage);
    void gainMp(float amount);
    void applyStun(float duration);
    void applyDot(float dmgPerTick, float interval, float duration);
    void applySlow(float amount, float duration);

    // ---- 状态查询（供 BattleState / HUD 使用） ----
    bool isKO()          const { return m_state == FighterState::KO; }
    bool isInvincible()  const { return m_invincible; }
    bool isOnGround()    const { return m_onGround; }
    float getHp()        const { return m_hp; }
    float getMp()        const { return m_mp; }
    float getMaxHp()     const { return m_config.maxHp; }
    float getMaxMp()     const { return m_config.maxMp; }
    const std::string& getName() const { return m_config.name; }

    // CD 信息（HUD 显示倒计时用）
    float getAtkCD()     const { return m_attackCD; }
    float getAtkCDMax()  const { return m_config.attackCD; }
    float getSk1CD()     const { return m_skill1CD; }
    float getSk1CDMax()  const { return m_config.sk1CD; }
    float getSk2CD()     const { return m_skill2CD; }
    float getSk2CDMax()  const { return m_config.sk2CD; }

    // 受击框：宽度窄于精灵（HURTBOX_WIDTH），水平居中。
    // 用于抛射物碰撞、近战碰撞、角色间推离。
    sf::FloatRect getHurtbox() const;

    void setDummy(bool d) { m_isDummy = d; } // Dummy 模式：不操作、不移动、不反击
    void reset();          // 重置所有状态到开局（HP/MP/技能CD/负面效果全清除）
    void draw(sf::RenderWindow& window);

private:
    // ---- 物理 ----
    void applyGravity(float dt);  // 空中 v.y += GRAVITY * dt
    void clampToGround();        // 检测脚底撞地面 → 钳制位置 + 设置 m_onGround

    // ---- 计时器 ----
    void updateTimers(float dt); // 所有倒计时统一减少 dt

    // ---- 攻击（由 update() 消费请求标志后调用） ----
    void tryNormalAttack(std::vector<Projectile>& p, Fighter& opp);
    void trySkill1(std::vector<Projectile>& p, std::vector<Trap>& t, Fighter& opp);
    void trySkill2(std::vector<Projectile>& p, Fighter& opp);

    // ---- 狂暴 buff（重武器技能2）----
    void doBerserk();  // 备份原始值 → 修改 m_config 各项 → 启动计时器
    void endBerserk(); // 恢复原始值

    // ==================== 成员变量 ====================

    int  m_playerIndex;         // 1=P1, 2=P2
    CharacterConfig m_config;   // 角色参数拷贝（狂暴时直接改这里的值）

    // SFML 3.x: Sprite 无默认构造，用 optional 延迟初始化
    std::optional<sf::Sprite> m_sprite;

    // 键位映射（构造时从 m_playerIndex 推导，之后不变）
    sf::Keyboard::Scancode m_keyLeft, m_keyRight, m_keyJump, m_keyAtk, m_keySk1, m_keySk2;

    // ---- 物理 ----
    sf::Vector2f m_position{0.f, 0.f};  // 精灵左上角
    sf::Vector2f m_velocity{0.f, 0.f};  // 当前速度
    float m_facing   = 1.f;             // 朝向（1=右, -1=左）
    bool  m_onGround = true;            // 是否着地

    FighterState m_state = FighterState::Idle;

    // ---- 资源 ----
    float m_hp = 0.f;
    float m_mp = 0.f;

    // ---- 冷却倒计时（≤0 = 可用） ----
    float m_attackCD  = 0.f;
    float m_skill1CD  = 0.f;
    float m_skill2CD  = 0.f;
    float m_hitStunTimer = 0.f;  // 受击硬直（0.12s）
    float m_stunTimer    = 0.f;  // 眩晕（重武器技能1，1s）

    // ---- DOT（持续伤害，陷阱触发） ----
    float m_dotTimer    = 0.f;   // DOT 剩余总时间
    float m_dotTickTimer = 0.f;  // 距下次扣血倒计时
    float m_dotDmg      = 0.f;   // 每次扣血量
    float m_dotInterval = 0.5f;  // 扣血间隔

    // ---- 减速（陷阱触发） ----
    float m_slowTimer  = 0.f;
    float m_slowAmount = 0.f;    // 减速比例（0.5 = 慢 50%）

    // ---- 隐匿（陷阱师技能2） ----
    float m_invisibleTimer = 0.f;
    bool  m_invincible     = false;

    // ---- 狂暴（重武器技能2） ----
    float m_berserkTimer     = 0.f;  // 剩余时间
    float m_origMoveSpeed    = 0.f;  // 以下四个备份原始值用于恢复
    float m_origAtkCD        = 0.f;
    float m_origAtkDmg       = 0.f;
    float m_origDmgReduction = 0.f;
    float m_damageReduction  = 0.f;  // 当前减伤比例

    // ---- 突进（剑士技能1） ----
    float m_dashRemaining = 0.f;  // 剩余突进距离
    float m_dashSpeed     = 0.f;  // 突进速度

    // ---- 攻击请求标志（handleInput 设 true，update 消费并清零） ----
    bool m_wantsAttack = false;
    bool m_wantsSkill1 = false;
    bool m_wantsSkill2 = false;

    bool m_isDummy = false;     // 打桩模式
    bool m_flashActive = false; // 受击闪红

    // ---- 上一帧按键（边缘检测：按下瞬间触发，不是按住连发） ----
    bool m_prevJump = false;
    bool m_prevAtk  = false;
    bool m_prevSk1  = false;
    bool m_prevSk2  = false;
};
