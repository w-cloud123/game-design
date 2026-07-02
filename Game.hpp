// ============================================================
// src/Game.hpp
// ============================================================
#pragma once
#include <SFML/Graphics.hpp>
#include "State.hpp"
#include "StartupState.hpp"
#include "MenuState.hpp"
#include "SelectState.hpp"
#include "FightState.hpp"
#include "TrainingState.hpp"
#include <memory>

class Game {
public:
    Game();
    void run();
    void changeState(StateID id);

private:
    sf::RenderWindow m_window;
    sf::Font m_font;
    std::unique_ptr<StartupState>  m_startup;
    std::unique_ptr<MenuState>     m_menu;
    std::unique_ptr<SelectState>   m_select;
    std::unique_ptr<FightState>    m_fight;
    std::unique_ptr<TrainingState> m_training;
    State* m_cur = nullptr;
    bool m_trainingMode = false;
    sf::Clock m_clock;
};
