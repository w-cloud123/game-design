// ============================================================
// src/HUD.cpp
// ============================================================
#include "HUD.hpp"
#include "Constants.hpp"
#include <sstream>
#include <iomanip>

HUD::HUD(const sf::Font& font)
    : m_nameText(font, "", 16)
    , m_cdText(font, "", 12)
    , m_winText(font, "", 40)
{
    m_hpBarBg.setSize({200.f, 16.f});
    m_hpBarBg.setFillColor(sf::Color(60, 60, 60));
    m_hpBarBg.setOutlineColor(sf::Color::White);
    m_hpBarBg.setOutlineThickness(1.f);
    m_hpBarFill.setSize({200.f, 16.f});
    m_hpBarFill.setFillColor(sf::Color::Red);

    m_mpBarBg.setSize({200.f, 10.f});
    m_mpBarBg.setFillColor(sf::Color(60, 60, 60));
    m_mpBarBg.setOutlineColor(sf::Color::White);
    m_mpBarBg.setOutlineThickness(1.f);
    m_mpBarFill.setSize({200.f, 10.f});
    m_mpBarFill.setFillColor(sf::Color::Blue);

    m_nameText.setFillColor(sf::Color::White);
    m_cdText.setFillColor(sf::Color(200, 200, 200));
    m_winText.setFillColor(sf::Color::Yellow);
}

void HUD::update(float hp, float maxHp, float mp, float maxMp,
                 const std::string& name,
                 float atkCD, float atkCDMax,
                 float sk1CD, float sk1CDMax,
                 float sk2CD, float sk2CDMax,
                 bool isLeftSide) {
    m_isLeft = isLeftSide;
    float barX = isLeftSide ? 10.f : Constants::WINDOW_WIDTH - 210.f;
    float barY = 10.f;

    m_hpBarBg.setPosition({barX, barY});
    float hpRatio = (maxHp > 0.f) ? (hp / maxHp) : 0.f;
    m_hpBarFill.setSize({200.f * hpRatio, 16.f});
    m_hpBarFill.setPosition({barX, barY});

    m_mpBarBg.setPosition({barX, barY + 20.f});
    float mpRatio = (maxMp > 0.f) ? (mp / maxMp) : 0.f;
    m_mpBarFill.setSize({200.f * mpRatio, 10.f});
    m_mpBarFill.setPosition({barX, barY + 20.f});

    m_nameText.setString(name);
    m_nameText.setPosition({barX, barY + 34.f});

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1)
        << "ATK:" << ((atkCD > 0.f) ? atkCD : 0.f)
        << " S1:" << ((sk1CD > 0.f) ? sk1CD : 0.f)
        << " S2:" << ((sk2CD > 0.f) ? sk2CD : 0.f);
    m_cdText.setString(oss.str());
    m_cdText.setPosition({barX, barY + 52.f});
}

void HUD::showWinText(const std::string& text) {
    m_winText.setString(text);
    sf::FloatRect b = m_winText.getLocalBounds();
    m_winText.setOrigin({b.size.x / 2.f, b.size.y / 2.f});
    m_winText.setPosition({Constants::WINDOW_WIDTH / 2.f, Constants::WINDOW_HEIGHT / 2.f});
}

void HUD::hideWinText() { m_winText.setString(""); }

void HUD::draw(sf::RenderWindow& window) const {
    window.draw(m_hpBarBg); window.draw(m_hpBarFill);
    window.draw(m_mpBarBg); window.draw(m_mpBarFill);
    window.draw(m_nameText);
    window.draw(m_cdText);
    if (!m_winText.getString().isEmpty())
        window.draw(m_winText);
}
