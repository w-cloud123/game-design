// ============================================================
// src/CharacterConfig.hpp — 角色参数（数据驱动）
// ============================================================
#pragma once
#include <string>

enum class CharType { Mage, Swordsman, Heavy, Trapper };

struct CharacterConfig {
    CharType    type;
    std::string name;
    std::string textureKey;

    float maxHp      = 100.f;
    float maxMp      = 100.f;
    float mpRegen    = 0.f;
    float moveSpeed  = 200.f;

    // 普攻
    float attackDamage  = 10.f;
    float attackCD      = 0.5f;
    bool  attackRanged  = false;
    float attackRange   = 0.f;
    float attackSpeed   = 0.f;
    float attackHitboxW = 30.f;

    // 技能1
    float sk1Damage    = 0.f;
    float sk1CD        = 0.f;
    float sk1Mana      = 0.f;
    float sk1Range     = 0.f;
    float sk1Speed     = 0.f;
    float sk1DashDist  = 0.f;
    float sk1AoeRadius = 0.f;
    float sk1StunDur   = 0.f;

    // 技能2
    float sk2Damage        = 0.f;
    float sk2CD            = 0.f;
    float sk2Mana          = 0.f;
    float sk2Heal          = 0.f;
    float sk2Range         = 0.f;
    float sk2Speed         = 0.f;
    float sk2Duration      = 0.f;
    float sk2DmgReduction  = 0.f;
    float sk2AtkSpeedMult  = 1.f;
    float sk2MoveSpeedMult = 1.f;
    float sk2AtkDmgMult    = 1.f;
    float sk2InvDur        = 0.f;

    // 陷阱（陷阱师技能1）
    int   maxTraps = 2;
    float trapDuration    = 0.f;
    float trapDotDmg      = 0.f;
    float trapDotInterval = 0.5f;
    float trapDotDuration = 0.f;
    float trapSlowAmount  = 0.f;
    float trapSlowDuration = 0.f;
};

// ---- 工厂函数 ----
inline CharacterConfig makeMageConfig() {
    CharacterConfig c;
    c.type = CharType::Mage;  c.name = "MAGE";  c.textureKey = "blue";
    c.maxHp = 80.f;  c.maxMp = 150.f;  c.mpRegen = 8.f;  c.moveSpeed = 200.f;
    c.attackDamage = 8.f;  c.attackCD = 0.5f;  c.attackRanged = true;
    c.attackRange  = 400.f;  c.attackSpeed = 200.f;
    c.sk1Damage = 25.f;  c.sk1CD = 3.f;  c.sk1Mana = 30.f;
    c.sk1Range  = 700.f;  c.sk1Speed = 350.f;
    c.sk2Heal   = 30.f;   c.sk2CD = 4.f;   c.sk2Mana = 25.f;
    return c;
}
inline CharacterConfig makeSwordsmanConfig() {
    CharacterConfig c;
    c.type = CharType::Swordsman;  c.name = "SWORDSMAN";  c.textureKey = "red";
    c.maxHp = 120.f;  c.maxMp = 100.f;  c.mpRegen = 5.f;  c.moveSpeed = 200.f;
    c.attackDamage = 12.f;  c.attackCD = 0.3f;  c.attackRanged = false;
    c.attackHitboxW = 30.f;
    c.sk1Damage = 20.f;  c.sk1CD = 3.f;  c.sk1Mana = 20.f;  c.sk1DashDist = 150.f;
    c.sk2Damage = 28.f;  c.sk2CD = 4.f;  c.sk2Mana = 30.f;
    c.sk2Range = 300.f;  c.sk2Speed = 400.f;
    return c;
}
inline CharacterConfig makeHeavyConfig() {
    CharacterConfig c;
    c.type = CharType::Heavy;  c.name = "HEAVY";  c.textureKey = "green";
    c.maxHp = 180.f;  c.maxMp = 60.f;  c.mpRegen = 3.f;  c.moveSpeed = 200.f;
    c.attackDamage = 18.f;  c.attackCD = 0.6f;  c.attackRanged = false;
    c.attackHitboxW = 50.f;
    c.sk1Damage = 15.f;  c.sk1CD = 5.f;  c.sk1Mana = 25.f;
    c.sk1AoeRadius = 120.f;  c.sk1StunDur = 1.f;
    c.sk2Duration = 4.f;  c.sk2DmgReduction = 0.3f;
    c.sk2AtkSpeedMult = 0.6f;  c.sk2MoveSpeedMult = 1.25f;  c.sk2AtkDmgMult = 1.2f;
    c.sk2CD = 8.f;  c.sk2Mana = 40.f;
    return c;
}
inline CharacterConfig makeTrapperConfig() {
    CharacterConfig c;
    c.type = CharType::Trapper;  c.name = "TRAPPER";  c.textureKey = "yellow";
    c.maxHp = 100.f;  c.maxMp = 110.f;  c.mpRegen = 6.f;  c.moveSpeed = 200.f;
    c.attackDamage = 10.f;  c.attackCD = 0.35f;  c.attackRanged = true;
    c.attackRange  = 300.f;  c.attackSpeed = 250.f;
    c.sk1CD = 5.f;  c.sk1Mana = 25.f;
    c.maxTraps = 2;  c.trapDuration = 20.f;
    c.trapDotDmg = 3.f;  c.trapDotInterval = 0.5f;
    c.trapDotDuration = 3.f;  c.trapSlowAmount = 0.5f;  c.trapSlowDuration = 1.f;
    c.sk2InvDur = 2.f;  c.sk2CD = 6.f;  c.sk2Mana = 30.f;
    return c;
}
