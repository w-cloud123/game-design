// ============================================================
// src/State.hpp
// ============================================================
#pragma once
#include <SFML/Graphics.hpp>
#include <functional>

enum class StateID { Startup, Menu, Select, Fight, Training, Quit };

class State {
public:
    virtual ~State() = default;
    void setChangeCallback(std::function<void(StateID)> cb) { m_changeCallback = std::move(cb); }
    virtual void handleEvent(const sf::Event& event) = 0;
    virtual void update(float dt) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual void onEnter() {}
    virtual void onExit()  {}
protected:
    void requestChange(StateID id) { if (m_changeCallback) m_changeCallback(id); }
    std::function<void(StateID)> m_changeCallback;
};
