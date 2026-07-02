// ============================================================
// src/SelectState.cpp
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

    m_configs = {makeMageConfig(), makeSwordsmanConfig(), makeHeavyConfig(), makeTrapperConfig()};
    for (auto& c : m_configs) {
        m_names.emplace_back(font, c.name, 24);
        m_names.back().setFillColor(sf::Color::White);
    }
}

void SelectState::onEnter() { m_p1Sel = 0; m_p2Sel = 1; m_p1OK = false; m_p2OK = false; }

void SelectState::handleEvent(const sf::Event& event) {
    const auto* k = event.getIf<sf::Event::KeyPressed>();
    if (!k) return;
    if (k->scancode == sf::Keyboard::Scancode::R) { requestChange(StateID::Menu); return; }
    if (!m_p1OK) {
        if (k->scancode == sf::Keyboard::Scancode::A) m_p1Sel = (m_p1Sel + 3) % 4;
        if (k->scancode == sf::Keyboard::Scancode::D) m_p1Sel = (m_p1Sel + 1) % 4;
    }
    if (!m_training && !m_p2OK) {
        if (k->scancode == sf::Keyboard::Scancode::Left)  m_p2Sel = (m_p2Sel + 3) % 4;
        if (k->scancode == sf::Keyboard::Scancode::Right) m_p2Sel = (m_p2Sel + 1) % 4;
    }
    if (k->scancode == sf::Keyboard::Scancode::Enter) {
        m_p1OK = true;
        if (m_training) requestChange(StateID::Training);
        else { m_p2OK = true; if (m_p1OK && m_p2OK) requestChange(StateID::Fight); }
    }
}

void SelectState::update(float /*dt*/) {
    float cx = Constants::WINDOW_WIDTH / 2.f - 60.f;
    for (int i = 0; i < 4; ++i) {
        std::string s;
        sf::Color color = sf::Color::White;
        bool p1 = (i == m_p1Sel), p2 = (i == m_p2Sel && !m_training);
        if (p1 && p2)      { s = "P1 P2 "; color = sf::Color::Yellow; }
        else if (p1)       { s = "> P1 ";  color = sf::Color::Cyan; }
        else if (p2)       { s = "> P2 ";  color = sf::Color(255, 100, 100); }
        else               { s = "  "; }
        s += m_configs[i].name;
        m_names[i].setString(s);
        m_names[i].setFillColor(color);
        m_names[i].setPosition({cx, 170.f + i * 50.f});
    }
    if (m_p1OK && (m_p2OK || m_training)) { m_ready.setString("READY!"); m_ready.setFillColor(sf::Color::Green); }
    else if (m_p1OK) { m_ready.setString("P1 Ready - Waiting for P2..."); m_ready.setFillColor(sf::Color::Yellow); }
    else { m_ready.setString("Press ENTER to lock in"); m_ready.setFillColor(sf::Color(180, 180, 180)); }
}

void SelectState::draw(sf::RenderWindow& window) {
    m_map.drawBackground(window);
    window.draw(m_title); window.draw(m_p1Label);
    if (!m_training) window.draw(m_p2Label);
    for (auto& t : m_names) window.draw(t);
    window.draw(m_ready);
}
