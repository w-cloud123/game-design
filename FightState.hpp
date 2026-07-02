// ============================================================
// src/FightState.hpp
// ============================================================
#pragma once
#include "State.hpp"
#include "Fighter.hpp"
#include "Map.hpp"
#include "HUD.hpp"
#include "CharacterConfig.hpp"
#include <vector>

class FightState : public State {
public:
    explicit FightState(const sf::Font& font);
    void setPlayers(const CharacterConfig& p1, const CharacterConfig& p2);
    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;
    void onEnter() override;

protected:
    const sf::Font& m_font;
    Map m_map;
    HUD m_hudL, m_hudR;
    Fighter m_f1, m_f2;
    const CharacterConfig* m_cfg1 = nullptr;
    const CharacterConfig* m_cfg2 = nullptr;
    std::vector<Projectile> m_proj;
    std::vector<Trap> m_traps;
    bool   m_over = false;
    float  m_restart = 0.f;
    sf::Text m_win;

    void initFighters();
    void tickProjectiles(float dt);
    void tickTraps(float dt);
    void checkWin();
    void refreshHUD();
};
