// ============================================================
// src/State.hpp — 状态抽象基类
// ============================================================
// 设计模式：有限状态机（Finite State Machine）
//
// 整个游戏就是一个状态图：
//   Startup → Menu → Select → Fight / Training
//                              ↓（KO 3s 后）
//                            Select（循环）
//
// 每个 State 子类实现三个核心虚函数：
//   handleEvent() — 处理键盘/窗口事件
//   update(dt)    — 每帧逻辑更新
//   draw(window)  — 每帧渲染
//
// 还有两个可选的生命周期钩子：
//   onEnter() — 状态激活时调用一次（初始化界面）
//   onExit()  — 状态离开时调用一次（清理、保存数据）
//
// 状态切换机制：
//   子类不直接依赖 Game 类（解耦），而是通过 m_cb 回调函数
//   通知"我请求切换到状态 X"。Game 在构造时注入这个回调。
//   这样做的好处：State 可以单独测试，不依赖 Game 的完整环境。
// ============================================================
#pragma once
#include <SFML/Graphics.hpp>
#include <functional>

// 所有可用状态的枚举。Quit 不是真正的 State，而是通知 Game 关闭窗口。
enum class StateID { Startup, Menu, Select, Fight, Training, Quit };

class State {
public:
    virtual ~State() = default;

    // 注入状态切换回调（由 Game 在构造时调用）
    void setChangeCallback(std::function<void(StateID)> cb) { m_cb = std::move(cb); }

    // ===== 子类必须实现的纯虚函数 =====
    virtual void handleEvent(const sf::Event& e) = 0;
    virtual void update(float dt) = 0;
    virtual void draw(sf::RenderWindow& w) = 0;

    // ===== 可选的生命周期钩子 =====
    virtual void onEnter() {}
    virtual void onExit()  {}

protected:
    // 子类调用此函数请求切换状态
    void requestChange(StateID id) { if (m_cb) m_cb(id); }

    std::function<void(StateID)> m_cb; // 回调函数对象
};
