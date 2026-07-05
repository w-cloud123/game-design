// ============================================================
// src/SelectState.cpp — 选人界面实现
// ============================================================
#include "SelectState.hpp"
#include "Constants.hpp"

SelectState::SelectState(const sf::Font& font)
    : m_font(font)
    , m_title(font, "SELECT YOUR FIGHTER", 36)
    , m_p1Label(font, "P1 (A/D)", 20)
    , m_p2Label(font, "P2 (<-/->)", 20)
    , m_ready(font, "Press ENTER to lock in", 18)
{
    m_title.setFillColor(sf::Color::White);
    { auto b = m_title.getLocalBounds(); m_title.setOrigin({b.size.x / 2.f, b.size.y / 2.f}); }
    m_title.setPosition({Constants::WINDOW_WIDTH / 2.f, 50.f});

    m_p1Label.setFillColor(sf::Color::Cyan);
    m_p1Label.setPosition({100.f, 100.f});

    m_p2Label.setFillColor(sf::Color(255, 100, 100));
    m_p2Label.setPosition({500.f, 100.f});

    m_ready.setFillColor(sf::Color(180, 180, 180));
    m_ready.setPosition({Constants::WINDOW_WIDTH / 2.f, 500.f});

    // 构造四个角色的配置（工厂函数）
    m_configs = {makeMageConfig(), makeSwordsmanConfig(), makeHeavyConfig(), makeTrapperConfig()};
    for (auto& c : m_configs) {
        m_names.emplace_back(font, c.name, 24);
        m_names.back().setFillColor(sf::Color::White);
    }
}

void SelectState::onEnter() {
    m_p1Sel = 0; m_p2Sel = 1;  // 默认 P1 选第一个，P2 选第二个
    m_p1OK = false; m_p2OK = false;
}

void SelectState::handleEvent(const sf::Event& event) {
    const auto* k = event.getIf<sf::Event::KeyPressed>();
    if (!k) return;

    // R 键返回主菜单
    if (k->scancode == sf::Keyboard::Scancode::R) { requestChange(StateID::Menu); return; }

    // P1 选人：A=左移，D=右移（未锁定时才响应）
    if (!m_p1OK) {
        if (k->scancode == sf::Keyboard::Scancode::A) m_p1Sel = (m_p1Sel + 3) % 4;
        if (k->scancode == sf::Keyboard::Scancode::D) m_p1Sel = (m_p1Sel + 1) % 4;
    }
    // P2 选人：← 左移，→ 右移（训练模式下 P2 不参与选人）
    if (!m_training && !m_p2OK) {
        if (k->scancode == sf::Keyboard::Scancode::Left)  m_p2Sel = (m_p2Sel + 3) % 4;
        if (k->scancode == sf::Keyboard::Scancode::Right) m_p2Sel = (m_p2Sel + 1) % 4;
    }

    // Enter 锁定当前选择
    if (k->scancode == sf::Keyboard::Scancode::Enter) {
        m_p1OK = true;
        if (m_training) {
            // 训练模式：P1 锁定后即刻开始
            requestChange(StateID::Training);
        } else {
            m_p2OK = true; // P2 也锁定
            if (m_p1OK && m_p2OK) requestChange(StateID::Fight);
        }
    }
}

// 每帧更新角色名列表的显示（谁选中了、颜色区分）
void SelectState::update(float /*dt*/) {
    float cx = Constants::WINDOW_WIDTH / 2.f - 60.f;

    for (int i = 0; i < 4; ++i) {
        std::string s;
        sf::Color color = sf::Color::White;
        bool p1 = (i == m_p1Sel), p2 = (i == m_p2Sel && !m_training);

        // 两人选了同一个角色 → 黄色显示 "P1 P2"
        if (p1 && p2)      { s = "P1 P2 "; color = sf::Color::Yellow; }
        else if (p1)       { s = "> P1 ";  color = sf::Color::Cyan; }
        else if (p2)       { s = "> P2 ";  color = sf::Color(255, 100, 100); }
        else               { s = "  "; }

        s += m_configs[i].name;
        m_names[i].setString(s);
        m_names[i].setFillColor(color);
        m_names[i].setPosition({cx, 170.f + i * 50.f});
    }

    // 底部状态提示
    if (m_p1OK && (m_p2OK || m_training)) {
        m_ready.setString("READY!");
        m_ready.setFillColor(sf::Color::Green);
    } else if (m_p1OK) {
        m_ready.setString("P1 Ready - Waiting for P2...");
        m_ready.setFillColor(sf::Color::Yellow);
    } else {
        m_ready.setString("Press ENTER to lock in");
        m_ready.setFillColor(sf::Color(180, 180, 180));
    }
}

void SelectState::draw(sf::RenderWindow& window) {
    m_map.drawBackground(window);
    window.draw(m_title); window.draw(m_p1Label);
    if (!m_training) window.draw(m_p2Label); // 训练模式不显示 P2 标签
    for (auto& t : m_names) window.draw(t);
    window.draw(m_ready);
}
