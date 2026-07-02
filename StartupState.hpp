// ============================================================
// src/StartupState.hpp
// ============================================================
#pragma once
#include "State.hpp"
#include "Map.hpp"
#include <SFML/Graphics.hpp>

class StartupState : public State {
public:
    explicit StartupState(const sf::Font& font);
    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;
    void onEnter() override;
private:
    const sf::Font& m_font;
    Map m_map;
    sf::Text m_title;
    sf::Text m_subtitle;
    sf::Clock m_blink;
    bool m_visible = true;
};
