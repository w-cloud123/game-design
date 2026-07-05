// ============================================================
// src/Game.cpp — 主控制器实现
// ============================================================
#include "Game.hpp"
#include "Constants.hpp"
#include "AssetManager.hpp"
#include <stdexcept>

// ==================== 构造 ====================
// 游戏启动时只执行一次。做的事情按顺序：
//   1. 创建窗口
//   2. 加载字体（失败则抛异常退出）
//   3. 预加载所有纹理到单例缓存（全局复用）
//   4. 创建 5 个 State 实例
//   5. 为每个 State 注入背景纹理 + 状态切换回调
//   6. 切换到初始状态 Startup
Game::Game()
    : m_window(sf::VideoMode({Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT}),
               Constants::WINDOW_TITLE)
{
    // ---- 字体：所有界面文字共用这一个 sf::Font ----
    if (!m_font.openFromFile("assets/simhei.ttf"))
        throw std::runtime_error("Failed to load font: assets/simhei.ttf");

    // ---- 纹理预加载 ----
    // AssetManager 是单例。loadTexture 内部是"如果已有就跳过"的策略，
    // 所以后面如果扩展了需要加载更多纹理的地方，直接调用即可，不会重复加载。
    auto& a = AssetManager::getInstance();
    a.loadTexture("blue",   "assets/char_blue.png");
    a.loadTexture("red",    "assets/char_red.png");
    a.loadTexture("green",  "assets/char_green.png");
    a.loadTexture("yellow", "assets/char_yellow.png");
    a.loadTexture("bg_startup", "assets/bg_startup.png");
    a.loadTexture("bg_select",  "assets/bg_select.png");
    a.loadTexture("bg_battle",  "assets/bg_battle.png");

    // ---- 创建 State ----
    // 全部在构造时创建，游戏运行期间不销毁不重建。
    // 缺点：5 个 State 常驻内存。但每个只占几十~几百字节，可忽略。
    // 优点：状态切换是 O(1) 指针赋值，不需要 new/delete。
    m_startup  = std::make_unique<StartupState>(m_font);
    m_menu     = std::make_unique<MenuState>(m_font);
    m_select   = std::make_unique<SelectState>(m_font);
    m_fight    = std::make_unique<FightState>(m_font);
    m_training = std::make_unique<TrainingState>(m_font);

    // ---- 注入背景纹理 ----
    m_startup->setBackground(a.getTexture("bg_startup"));
    m_menu->setBackground(a.getTexture("bg_startup"));
    m_select->setBackground(a.getTexture("bg_select"));
    m_fight->setBackground(a.getTexture("bg_battle"));
    m_training->setBackground(a.getTexture("bg_battle"));

    // ---- 注入状态切换回调 ----
    // 核心设计：State 不持有 Game 指针，而是持有一个 std::function 回调。
    // 当 State 内部需要切换状态（如"用户按了 Enter"→"去选人界面"），
    // 它调用 requestChange(StateID)，触发此 lambda，最终到 changeState()。
    // 这种"回调注入"模式避免了 State → Game 的循环依赖。
    auto cb = [this](StateID id) { changeState(id); };
    m_startup->setChangeCallback(cb);
    m_menu->setChangeCallback(cb);
    m_select->setChangeCallback(cb);
    m_fight->setChangeCallback(cb);
    m_training->setChangeCallback(cb);

    changeState(StateID::Startup);
}

// ==================== 状态切换 ====================
// 流程：旧 State.onExit() → 从旧 State 提取数据传到新 State → 新 State.onEnter()
//
// 跨状态数据传递举例：
//   Menu → Select：传 m_trainingMode（选"训练模式"还是"1v1"）
//   Select → Fight：传 P1 和 P2 选择的 CharacterConfig
void Game::changeState(StateID id) {
    if (m_cur) m_cur->onExit();  // 通知旧状态"你要被切换了"（当前没有用到，留作扩展）

    switch (id) {
    case StateID::Startup:  m_cur = m_startup.get(); break;
    case StateID::Menu:     m_cur = m_menu.get();    break;
    case StateID::Select:
        m_trainingMode = m_menu->isTrainingMode();
        m_select->setTrainingMode(m_trainingMode);
        m_cur = m_select.get();
        break;
    case StateID::Fight:
        m_fight->setPlayers(m_select->getP1Config(), m_select->getP2Config());
        m_cur = m_fight.get();
        break;
    case StateID::Training:
        m_training->setPlayers(m_select->getP1Config(), m_select->getP2Config());
        m_cur = m_training.get();
        break;
    case StateID::Quit: m_window.close(); return;
    }

    if (m_cur) m_cur->onEnter(); // 通知新状态"你已激活，执行初始化"
}

// ==================== 主循环 ====================
// 经典的三段式游戏循环（每帧执行）：
//   1. 事件处理（键盘输入、窗口关闭等）
//   2. 逻辑更新（位置、碰撞、技能、计时器）
//   3. 渲染（先 clear 再 draw 再 display）
//
// SFML 的双缓冲机制：
//   所有 draw 调用其实是画到"后台缓冲"上。
//   display() 把后台缓冲翻转到屏幕上（同时清空后台）。
//   如果没有 display()，屏幕永远是黑的。
void Game::run() {
    m_clock.restart(); // 重置时钟，确保第一帧 dt 准确

    while (m_window.isOpen()) {
        // ---- delta time ----
        // dt = 距离上一帧过了多少秒。用 restart() 而非 getElapsedTime()
        // 是因为 restart 同时"归零"，省去再调一次 restart 的代码。
        // clamp 保护：如果调试时暂停了很久（dt 可能 >1s），
        // 直接把 dt 限制到 ~60fps 的值，防止物理计算爆炸。
        float dt = m_clock.restart().asSeconds();
        if (dt > Constants::DT_CLAMP_MAX) dt = Constants::DT_DEFAULT;

        // ---- 事件处理 ----
        // pollEvent() 返回 std::optional<sf::Event>。
        // 有事件 → 返回该事件；没事件 → 返回 nullopt。
        // while 循环一次性处理完本帧积压的所有事件。
        while (auto event = m_window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) { m_window.close(); return; }
            if (m_cur) m_cur->handleEvent(*event);
        }

        // ---- 逻辑更新 ----
        if (m_cur) m_cur->update(dt);

        // ---- 渲染 ----
        m_window.clear();           // 清空上一帧画面
        if (m_cur) m_cur->draw(m_window); // 当前 State 绘制所有元素
        m_window.display();         // 翻转缓冲区 → 屏幕可见
    }
}
