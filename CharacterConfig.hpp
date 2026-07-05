// ============================================================
// src/CharacterConfig.hpp — 角色参数结构体 + 四个工厂函数
// ============================================================
// 设计理念：数据驱动（Data-Driven Design）
//   四个角色共用同一个 Fighter 类，差异完全由此结构体的数值决定。
//   加新角色 = 加一个新工厂函数，不碰 Fighter 代码（除了 switch 分支）。
//
// 字段命名规则：
//   sk1/sk2 = 技能1/技能2
//   每个技能可能有完全不同的效果（火球/突进/AOE/陷阱），
//   所以字段按"可能用到的所有参数"设计——某个角色不用的字段设 0 即可。
// ============================================================
#pragma once
#include <string>

enum class CharType { Mage, Swordsman, Heavy, Trapper };

struct CharacterConfig {
    CharType    type;
    std::string name;          // 显示名
    std::string textureKey;    // AssetManager 中的 key

    // ---- 基础属性 ----
    float maxHp       = 100.f;
    float maxMp       = 100.f;
    float mpRegen     = 0.f;    // 每秒被动回蓝
    float onHitMpGain = 5.f;    // 命中回蓝
    float moveSpeed   = 200.f;  // 水平移速（px/s）

    // ---- 普攻 ----
    float attackDamage  = 10.f;
    float attackCD      = 0.5f;  // 冷却（秒）
    bool  attackRanged  = false; // true=抛射物, false=近战
    float attackRange   = 0.f;   // 远程射程
    float attackSpeed   = 0.f;   // 远程弹速
    float attackHitboxW = 30.f;  // 近战半宽

    // ---- 技能1 ----
    float sk1Damage    = 0.f;
    float sk1CD        = 0.f;
    float sk1Mana      = 0.f;    // 蓝耗
    float sk1Range     = 0.f;    // 法师火球射程
    float sk1Speed     = 0.f;    // 法师火球速度
    float sk1DashDist  = 0.f;    // 剑士突进距离
    float sk1AoeRadius = 0.f;    // 重武器 AOE 半径
    float sk1StunDur   = 0.f;    // 重武器眩晕时长

    // ---- 技能2 ----
    float sk2Damage        = 0.f; // 剑士剑气伤害
    float sk2CD            = 0.f;
    float sk2Mana          = 0.f;
    float sk2Heal          = 0.f; // 法师治疗量
    float sk2Range         = 0.f; // 剑士剑气射程
    float sk2Speed         = 0.f; // 剑士剑气速度
    float sk2Duration      = 0.f; // 狂暴/隐匿持续时长
    float sk2DmgReduction  = 0.f; // 狂暴减伤比例
    float sk2AtkSpeedMult  = 1.f; // 狂暴攻速倍率（<1 更快）
    float sk2MoveSpeedMult = 1.f; // 狂暴移速倍率
    float sk2AtkDmgMult    = 1.f; // 狂暴攻击力倍率
    float sk2InvDur        = 0.f; // 隐匿时长

    // ---- 陷阱 ----
    int   maxTraps         = 2;   // 最多同时存在
    float trapDuration     = 0.f; // 存活时间
    float trapDotDmg       = 0.f; // DOT 每跳伤害
    float trapDotInterval  = 0.5f;// DOT 间隔
    float trapDotDuration  = 0.f; // DOT 总时长
    float trapSlowAmount   = 0.f; // 减速比例
    float trapSlowDuration = 0.f; // 减速时长
};

// ==================== 工厂函数 ====================
// 每个函数返回一个"配好数值"的配置。inline 避免链接时重复符号。

inline CharacterConfig makeMageConfig() {
    CharacterConfig c;
    c.type = CharType::Mage;  c.name = "MAGE";  c.textureKey = "blue";
    c.maxHp = 80.f;  c.maxMp = 150.f;  c.mpRegen = 8.f;  c.onHitMpGain = 8.f;
    c.attackDamage = 8.f;  c.attackCD = 0.5f;  c.attackRanged = true;
    c.attackRange = 400.f;  c.attackSpeed = 200.f;
    c.sk1Damage = 25.f;  c.sk1CD = 3.f;  c.sk1Mana = 30.f;
    c.sk1Range = 700.f;  c.sk1Speed = 350.f;
    c.sk2Heal = 30.f;    c.sk2CD = 4.f;   c.sk2Mana = 25.f;
    return c;
}

inline CharacterConfig makeSwordsmanConfig() {
    CharacterConfig c;
    c.type = CharType::Swordsman;  c.name = "SWORDSMAN";  c.textureKey = "red";
    c.maxHp = 120.f;  c.maxMp = 100.f;  c.mpRegen = 5.f;  c.onHitMpGain = 5.f;
    c.attackDamage = 12.f;  c.attackCD = 0.3f;  c.attackRanged = false;
    c.attackHitboxW = 30.f;
    c.sk1Damage = 20.f;  c.sk1CD = 3.f;  c.sk1Mana = 20.f;
    c.sk1DashDist = 150.f;
    c.sk2Damage = 28.f;  c.sk2CD = 4.f;  c.sk2Mana = 30.f;
    c.sk2Range = 300.f;  c.sk2Speed = 400.f;
    return c;
}

inline CharacterConfig makeHeavyConfig() {
    CharacterConfig c;
    c.type = CharType::Heavy;  c.name = "HEAVY";  c.textureKey = "green";
    c.maxHp = 180.f;  c.maxMp = 60.f;  c.mpRegen = 3.f;  c.onHitMpGain = 10.f;
    c.attackDamage = 18.f;  c.attackCD = 0.6f;  c.attackRanged = false;
    c.attackHitboxW = 50.f;
    c.sk1Damage = 15.f;  c.sk1CD = 5.f;  c.sk1Mana = 25.f;
    c.sk1AoeRadius = 120.f;  c.sk1StunDur = 1.f;
    c.sk2Duration = 4.f;  c.sk2DmgReduction = 0.3f;
    c.sk2AtkSpeedMult = 0.6f;  c.sk2MoveSpeedMult = 1.25f;
    c.sk2AtkDmgMult = 1.2f;
    c.sk2CD = 8.f;  c.sk2Mana = 40.f;
    return c;
}

inline CharacterConfig makeTrapperConfig() {
    CharacterConfig c;
    c.type = CharType::Trapper;  c.name = "TRAPPER";  c.textureKey = "yellow";
    c.maxHp = 100.f;  c.maxMp = 110.f;  c.mpRegen = 6.f;  c.onHitMpGain = 6.f;
    c.attackDamage = 10.f;  c.attackCD = 0.35f;  c.attackRanged = true;
    c.attackRange = 300.f;  c.attackSpeed = 250.f;
    c.sk1CD = 5.f;  c.sk1Mana = 25.f;
    c.maxTraps = 2;  c.trapDuration = 20.f;
    c.trapDotDmg = 3.f;  c.trapDotInterval = 0.5f;
    c.trapDotDuration = 3.f;  c.trapSlowAmount = 0.5f;
    c.trapSlowDuration = 1.f;
    c.sk2InvDur = 2.f;  c.sk2CD = 6.f;  c.sk2Mana = 30.f;
    return c;
}
