// ============================================================
// src/BattleState.hpp — 对战/训练模式的公共基类
// ============================================================
// 设计目的：消除 FightState 和 TrainingState 之间 70% 的重复代码。
//
// 这个类封装了所有"有两个角色互打"的场景的公共逻辑：
//   - 管理两个 Fighter + 抛射物列表 + 陷阱列表
//   - 主更新循环骨架（输入→更新→抛射物碰撞→陷阱触发→HUD）
//   - 渲染顺序（背景→地面→抛射物→陷阱→角色→HUD→额外元素）
//   - R 键返回选人界面
//
// 子类通过覆写 6 个 protected 虚函数来注入差异：
//   FightState：   添加 KO 判定 + 胜负文字 + 3s 倒计时
//   TrainingState：P2 设为 Dummy + P1 无敌 + 名字后缀
//
// 这种"模板方法"模式一次写完核心循环，子类只写差异部分。
// ============================================================
#pragma once
#include "State.hpp"
#include "Fighter.hpp"
#include "Map.hpp"
#include "HUD.hpp"
#include "CharacterConfig.hpp"
#include <vector>

class BattleState : public State {
public:
    explicit BattleState(const sf::Font& font);
    void setPlayers(const CharacterConfig& p1, const CharacterConfig& p2);
    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;
    void onEnter() override;
    void setBackground(const sf::Texture& tex) { m_map.setBackgroundTexture(tex); }

protected:
    // ========== 虚函数 Hooks（子类可选覆写） ==========

    /// preUpdate: 在输入/更新之前调用。返回 false 则跳过本帧（如 KO 后的倒计时期间）
    virtual bool preUpdate(float dt);
    /// postFighterUpdate: 在两个 Fighter 更新完后、碰撞检测前调用（如 KO 判定）
    virtual void postFighterUpdate(float dt);
    /// drawExtras: 在 HUD 之后绘制额外元素（如胜负文字）
    virtual void drawExtras(sf::RenderWindow& w);
    /// onFightersInitialized: Fighter 重建+绑定纹理后调用（如 setDummy）
    virtual void onFightersInitialized();
    /// isPlayerVulnerable: 此玩家是否可被抛射物/陷阱命中
    virtual bool isPlayerVulnerable(int idx) const;
    /// p2NameSuffix: P2 名字后面的额外文字（如 " [DUMMY]"）
    virtual std::string p2NameSuffix() const;

    // ========== 共享数据（子类可直接访问） ==========
    const sf::Font& m_font;
    Map    m_map;
    HUD    m_hudL, m_hudR;
    Fighter m_f1, m_f2;
    const CharacterConfig* m_cfg1 = nullptr; // 指向 SelectState 中的配置
    const CharacterConfig* m_cfg2 = nullptr;
    std::vector<Projectile> m_proj;  // 飞行中的抛射物
    std::vector<Trap>       m_traps; // 未过期的陷阱

private:
    void initFighters();             // 用 m_cfg1/m_cfg2 重建 Fighter + 绑定纹理
    void tickProjectiles(float dt);  // 更新位置 → 碰撞检测 → 造成伤害/回蓝 → 清除
    void tickTraps(float dt);        // 更新寿命 → 检测角色踩中 → 施加 DOT/减速 → 清除
    void refreshHUD();               // 从 Fighter 读取 HP/MP/CD 更新左右 HUD
};
