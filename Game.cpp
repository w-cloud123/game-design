// ============================================================
// src/Game.cpp
// ============================================================
#include "Game.hpp"
#include "Constants.hpp"
#include "AssetManager.hpp"
#include "StartupState.hpp"
#include "MenuState.hpp"
#include "SelectState.hpp"
#include "FightState.hpp"
#include "TrainingState.hpp"
#include <stdexcept>

Game::Game()
    : m_window(sf::VideoMode({Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT}),
               Constants::WINDOW_TITLE)
{
    if (!m_font.openFromFile("assets/simhei.ttf"))
        throw std::runtime_error("Failed to load font: assets/simhei.ttf");

    auto& a = AssetManager::getInstance();
    a.loadTexture("blue",   "assets/char_blue.png");
    a.loadTexture("red",    "assets/char_red.png");
    a.loadTexture("green",  "assets/char_green.png");
    a.loadTexture("yellow", "assets/char_yellow.png");

    m_startup  = std::make_unique<StartupState>(m_font);
    m_menu     = std::make_unique<MenuState>(m_font);
    m_select   = std::make_unique<SelectState>(m_font);
    m_fight    = std::make_unique<FightState>(m_font);
    m_training = std::make_unique<TrainingState>(m_font);

    auto cb = [this](StateID id) { changeState(id); };
    m_startup->setChangeCallback(cb);
    m_menu->setChangeCallback(cb);
    m_select->setChangeCallback(cb);
    m_fight->setChangeCallback(cb);
    m_training->setChangeCallback(cb);

    changeState(StateID::Startup);
}

void Game::changeState(StateID id) {
    if (m_cur) m_cur->onExit();

    switch (id) {
    case StateID::Startup:  m_cur = m_startup.get(); break;
    case StateID::Menu:     m_cur = m_menu.get();    break;
    case StateID::Select:
        m_trainingMode = m_menu->isTrainingMode();
        m_select->setTrainingMode(m_trainingMode);
        m_cur = m_select.get();
        break;
    case StateID::Fight:
        m_fight->setPlayers(m_select->getP1Config(), m_select->getP2Config());
        m_cur = m_fight.get();
        break;
    case StateID::Training:
        m_training->setPlayers(m_select->getP1Config(), m_select->getP2Config());
        m_cur = m_training.get();
        break;
    case StateID::Quit: m_window.close(); return;
    }

    if (m_cur) m_cur->onEnter();
}

void Game::run() {
    m_clock.restart();
    while (m_window.isOpen()) {
        float dt = m_clock.restart().asSeconds();
        if (dt > 0.1f) dt = 0.016f;

        while (auto event = m_window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) { m_window.close(); return; }
            if (m_cur) m_cur->handleEvent(*event);
        }

        if (m_cur) m_cur->update(dt);

        m_window.clear();
        if (m_cur) m_cur->draw(m_window);
        m_window.display();
    }
}
