// ============================================================
// src/HUD.hpp
// ============================================================
#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class HUD {
public:
    explicit HUD(const sf::Font& font);
    void update(float hp, float maxHp, float mp, float maxMp,
                const std::string& name,
                float atkCD, float atkCDMax,
                float sk1CD, float sk1CDMax,
                float sk2CD, float sk2CDMax,
                bool  isLeftSide);
    void showWinText(const std::string& text);
    void hideWinText();
    void draw(sf::RenderWindow& window) const;

private:
    sf::RectangleShape m_hpBarBg, m_hpBarFill;
    sf::RectangleShape m_mpBarBg, m_mpBarFill;
    sf::Text m_nameText;
    sf::Text m_cdText;
    sf::Text m_winText;
    bool m_isLeft = true;
};
