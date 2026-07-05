// ============================================================
// src/MenuState.cpp — 主菜单实现
// ============================================================
#include "MenuState.hpp"
#include "Constants.hpp"

namespace {
    constexpr const char* MENU_LABELS[] = {"1v1 VS MODE", "TRAINING MODE", "EXIT"};
}

MenuState::MenuState(const sf::Font& font)
    : m_font(font)
    , m_title(font, "MAIN MENU", 40)
{
    m_title.setFillColor(sf::Color::White);
    sf::FloatRect b = m_title.getLocalBounds();
    m_title.setOrigin({b.size.x / 2.f, b.size.y / 2.f});
    m_title.setPosition({Constants::WINDOW_WIDTH / 2.f, 120.f});

    // 创建三个菜单项，纵向排列，间距 60px
    for (int i = 0; i < 3; ++i) {
        sf::Text t(font, MENU_LABELS[i], 28);
        t.setFillColor(sf::Color::White);
        b = t.getLocalBounds();
        t.setOrigin({b.size.x / 2.f, b.size.y / 2.f});
        t.setPosition({Constants::WINDOW_WIDTH / 2.f, 220.f + i * 60.f});
        m_items.push_back(std::move(t));
    }
}

void MenuState::onEnter() { m_selected = 0; } // 每次进入菜单重置光标

void MenuState::handleEvent(const sf::Event& event) {
    const auto* k = event.getIf<sf::Event::KeyPressed>();
    if (!k) return;

    // W/↑ 上移，S/↓ 下移（取模实现循环）
    if (k->scancode == sf::Keyboard::Scancode::W || k->scancode == sf::Keyboard::Scancode::Up)
        m_selected = (m_selected + 2) % 3;
    if (k->scancode == sf::Keyboard::Scancode::S || k->scancode == sf::Keyboard::Scancode::Down)
        m_selected = (m_selected + 1) % 3;

    if (k->scancode == sf::Keyboard::Scancode::Enter) {
        if (m_selected == 2) requestChange(StateID::Quit);   // EXIT
        else                 requestChange(StateID::Select);  // 1v1 或 Training
    }
}

// 每帧更新：高亮选中项（黄色 + "> "前缀），其他项白色
void MenuState::update(float /*dt*/) {
    for (int i = 0; i < 3; ++i) {
        m_items[i].setFillColor(i == m_selected ? sf::Color::Yellow : sf::Color::White);
        m_items[i].setString((i == m_selected ? "> " : "  ") + std::string(MENU_LABELS[i]));
    }
}

void MenuState::draw(sf::RenderWindow& window) {
    m_map.drawBackground(window);
    window.draw(m_title);
    for (auto& t : m_items) window.draw(t);
}
