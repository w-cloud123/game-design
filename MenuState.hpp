// ============================================================
// src/MenuState.hpp
// ============================================================
#pragma once
#include "State.hpp"
#include "Map.hpp"
#include <SFML/Graphics.hpp>
#include <vector>

class MenuState : public State {
public:
    explicit MenuState(const sf::Font& font);
    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;
    void onEnter() override;
    bool isTrainingMode() const { return m_selected == 1; }
private:
    const sf::Font& m_font;
    Map m_map;
    sf::Text m_title;
    std::vector<sf::Text> m_items;
    int m_selected = 0;
};
