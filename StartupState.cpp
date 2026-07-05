// ============================================================
// src/StartupState.cpp — 启动封面实现
// ============================================================
#include "StartupState.hpp"
#include "Constants.hpp"

StartupState::StartupState(const sf::Font& font)
    : m_font(font)
    , m_title(font, "FIGHTING GAME", 48)
    , m_subtitle(font, "Press ANY KEY to start", 24)
{
    m_title.setFillColor(sf::Color::White);
    // 文字居中：获取文字实际像素大小 → 设 origin 为几何中心 → 移到屏幕中央
    sf::FloatRect b = m_title.getLocalBounds();
    m_title.setOrigin({b.size.x / 2.f, b.size.y / 2.f});
    m_title.setPosition({Constants::WINDOW_WIDTH / 2.f, 180.f});

    m_subtitle.setFillColor(sf::Color::White);
    b = m_subtitle.getLocalBounds();
    m_subtitle.setOrigin({b.size.x / 2.f, b.size.y / 2.f});
    m_subtitle.setPosition({Constants::WINDOW_WIDTH / 2.f, 380.f});
}

void StartupState::onEnter() { m_blink.restart(); m_visible = true; }

// 按任意键 → 去主菜单
void StartupState::handleEvent(const sf::Event& event) {
    if (event.is<sf::Event::KeyPressed>()) requestChange(StateID::Menu);
}

// 每 0.5 秒翻转一次字幕可见性 → 闪烁效果
void StartupState::update(float /*dt*/) {
    if (m_blink.getElapsedTime().asSeconds() >= Constants::BLINK_INTERVAL) {
        m_blink.restart();
        m_visible = !m_visible;
    }
}

void StartupState::draw(sf::RenderWindow& window) {
    m_map.drawBackground(window);
    window.draw(m_title);
    if (m_visible) window.draw(m_subtitle);  // 闪烁中的字幕
}
