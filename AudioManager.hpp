// ============================================================
// src/AudioManager.hpp
// ============================================================
#pragma once
#include <string>

class AudioManager {
public:
    static AudioManager& getInstance();
    void play(const std::string& /*name*/) { /* 桩：无操作 */ }
private:
    AudioManager() = default;
};
