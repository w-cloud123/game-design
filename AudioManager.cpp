// ============================================================
// src/AudioManager.cpp
// ============================================================
#include "AudioManager.hpp"

AudioManager& AudioManager::getInstance() {
    static AudioManager instance;
    return instance;
}
