// ============================================================
// src/SelectState.hpp
// ============================================================
#pragma once
#include "State.hpp"
#include "Map.hpp"
#include "CharacterConfig.hpp"
#include <SFML/Graphics.hpp>
#include <vector>

class SelectState : public State {
public:
    explicit SelectState(const sf::Font& font);
    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;
    void onEnter() override;
    void setTrainingMode(bool t) { m_training = t; }
    const CharacterConfig& getP1Config() const { return m_configs[m_p1Sel]; }
    const CharacterConfig& getP2Config() const { return m_configs[m_p2Sel]; }
private:
    const sf::Font& m_font;
    Map m_map;
    sf::Text m_title;
    sf::Text m_p1Label;
    sf::Text m_p2Label;
    sf::Text m_ready;
    std::vector<CharacterConfig> m_configs;
    std::vector<sf::Text> m_names;
    int m_p1Sel = 0, m_p2Sel = 1;
    bool m_p1OK = false, m_p2OK = false, m_training = false;
};
