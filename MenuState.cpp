// ============================================================
// src/MenuState.cpp
// ============================================================
#include "MenuState.hpp"
#include "Constants.hpp"

MenuState::MenuState(const sf::Font& font)
    : m_font(font)
    , m_title(font, "MAIN MENU", 40)
{
    m_title.setFillColor(sf::Color::White);
    sf::FloatRect b = m_title.getLocalBounds();
    m_title.setOrigin({b.size.x / 2.f, b.size.y / 2.f});
    m_title.setPosition({Constants::WINDOW_WIDTH / 2.f, 120.f});

    const char* labels[] = {"1v1 VS MODE", "TRAINING MODE", "EXIT"};
    for (int i = 0; i < 3; ++i) {
        sf::Text t(font, labels[i], 28);
        t.setFillColor(sf::Color::White);
        b = t.getLocalBounds();
        t.setOrigin({b.size.x / 2.f, b.size.y / 2.f});
        t.setPosition({Constants::WINDOW_WIDTH / 2.f, 220.f + i * 60.f});
        m_items.push_back(std::move(t));
    }
}

void MenuState::onEnter() { m_selected = 0; }

void MenuState::handleEvent(const sf::Event& event) {
    const auto* k = event.getIf<sf::Event::KeyPressed>();
    if (!k) return;
    if (k->scancode == sf::Keyboard::Scancode::W || k->scancode == sf::Keyboard::Scancode::Up)
        m_selected = (m_selected + 2) % 3;
    if (k->scancode == sf::Keyboard::Scancode::S || k->scancode == sf::Keyboard::Scancode::Down)
        m_selected = (m_selected + 1) % 3;
    if (k->scancode == sf::Keyboard::Scancode::Enter) {
        if (m_selected == 2) requestChange(StateID::Quit);
        else requestChange(StateID::Select);
    }
}

void MenuState::update(float /*dt*/) {
    static const char* labels[] = {"1v1 VS MODE", "TRAINING MODE", "EXIT"};
    for (int i = 0; i < 3; ++i) {
        m_items[i].setFillColor(i == m_selected ? sf::Color::Yellow : sf::Color::White);
        m_items[i].setString((i == m_selected ? "> " : "  ") + std::string(labels[i]));
    }
}

void MenuState::draw(sf::RenderWindow& window) {
    m_map.drawBackground(window);
    window.draw(m_title);
    for (auto& t : m_items) window.draw(t);
}
