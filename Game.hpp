// ============================================================
// src/Game.hpp — 游戏主控制器
// ============================================================
// Game 是整个程序的"大脑"，负责：
//   1. 创建并持有 SFML 窗口（m_window）
//   2. 加载全局字体 + 预加载所有纹理到 AssetManager 缓存
//   3. 创建 5 个 State 实例（一次性创建，避免反复 new/delete）
//   4. 用 m_cur 裸指针指向当前活跃的 State
//   5. 管理状态切换：onExit 旧状态 → 传递跨状态数据 → onEnter 新状态
//   6. run() 提供经典游戏主循环（事件 → 更新 → 渲染）
//
// 所有权关系：
//   Game 拥有 ──→ sf::RenderWindow（窗口）
//            ──→ sf::Font（字体）
//            ──→ unique_ptr<StartupState> 等 5 个 State（独占所有权）
//   m_cur 只是指向上述 State 之一的裸指针（不拥有所有权）
// ============================================================
#pragma once
#include <SFML/Graphics.hpp>
#include "State.hpp"
#include "StartupState.hpp"
#include "MenuState.hpp"
#include "SelectState.hpp"
#include "FightState.hpp"
#include "TrainingState.hpp"
#include <memory>

class Game {
public:
    Game();                   // 构造：创建窗口、加载资源、初始化 State
    void run();               // 主循环入口（阻塞到窗口关闭）
    void changeState(StateID id); // 状态切换（含数据传递）

private:
    // ===== SFML 窗口 & 字体 =====
    sf::RenderWindow m_window;  // 800×600 窗口
    sf::Font         m_font;    // 全局字体（所有文字共用，避免重复加载）

    // ===== 5 个状态（unique_ptr 管理生命周期，随 Game 销毁而销毁） =====
    std::unique_ptr<StartupState>  m_startup;   // 启动封面
    std::unique_ptr<MenuState>     m_menu;      // 主菜单
    std::unique_ptr<SelectState>   m_select;    // 选人界面
    std::unique_ptr<FightState>    m_fight;     // 1v1 对战
    std::unique_ptr<TrainingState> m_training;  // 打桩训练

    // ===== 运行时状态 =====
    State* m_cur = nullptr;    // 指向当前活跃 State（裸指针，所有权属于上面 5 个 unique_ptr）
    bool   m_trainingMode = false;  // Menu → Select 传递：是否训练模式
    sf::Clock m_clock;              // 用于计算每帧的 delta time
};
