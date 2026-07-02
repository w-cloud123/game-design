// ============================================================
// src/StartupState.cpp
// ============================================================
#include "StartupState.hpp"
#include "Constants.hpp"

StartupState::StartupState(const sf::Font& font)
    : m_font(font)
    , m_title(font, "FIGHTING GAME", 48)
    , m_subtitle(font, "Press ANY KEY to start", 24)
{
    m_title.setFillColor(sf::Color::White);
    sf::FloatRect b = m_title.getLocalBounds();
    m_title.setOrigin({b.size.x / 2.f, b.size.y / 2.f});
    m_title.setPosition({Constants::WINDOW_WIDTH / 2.f, 180.f});

    m_subtitle.setFillColor(sf::Color::White);
    b = m_subtitle.getLocalBounds();
    m_subtitle.setOrigin({b.size.x / 2.f, b.size.y / 2.f});
    m_subtitle.setPosition({Constants::WINDOW_WIDTH / 2.f, 380.f});
}

void StartupState::onEnter() { m_blink.restart(); m_visible = true; }

void StartupState::handleEvent(const sf::Event& event) {
    if (event.is<sf::Event::KeyPressed>()) requestChange(StateID::Menu);
}

void StartupState::update(float /*dt*/) {
    if (m_blink.getElapsedTime().asSeconds() >= 0.5f) {
        m_blink.restart(); m_visible = !m_visible;
    }
}

void StartupState::draw(sf::RenderWindow& window) {
    m_map.drawBackground(window);
    window.draw(m_title);
    if (m_visible) window.draw(m_subtitle);
}
