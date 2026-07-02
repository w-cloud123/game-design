// ============================================================
// src/AssetManager.cpp
// ============================================================
#include "AssetManager.hpp"
#include <stdexcept>

AssetManager& AssetManager::getInstance() {
    static AssetManager instance;
    return instance;
}

sf::Texture& AssetManager::loadTexture(const std::string& key, const std::string& filepath) {
    auto it = m_textures.find(key);
    if (it != m_textures.end()) return it->second;
    sf::Texture tex;
    if (!tex.loadFromFile(filepath))
        throw std::runtime_error("Failed to load texture: " + filepath);
    m_textures[key] = std::move(tex);
    return m_textures[key];
}

sf::Texture& AssetManager::getTexture(const std::string& key) {
    return m_textures.at(key);
}
