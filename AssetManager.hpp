// ============================================================
// src/AssetManager.hpp
// ============================================================
#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>

class AssetManager {
public:
    static AssetManager& getInstance();

    sf::Texture& loadTexture(const std::string& key, const std::string& filepath);
    sf::Texture& getTexture(const std::string& key);

private:
    AssetManager() = default;
    std::unordered_map<std::string, sf::Texture> m_textures;
};
