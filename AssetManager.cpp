// ============================================================
// src/AssetManager.cpp — 单例纹理缓存实现
// ============================================================
#include "AssetManager.hpp"
#include <stdexcept>

// Meyer's Singleton: C++11 起 static 局部变量初始化是线程安全的。
// 第一次调用时创建实例，之后每次返回同一个实例的引用。
AssetManager& AssetManager::getInstance() {
    static AssetManager instance;
    return instance;
}

// 懒加载：key 不在 map 中才从磁盘读取并缓存。
// 如果 key 已存在，直接返回缓存的引用（O(1) 查找）。
sf::Texture& AssetManager::loadTexture(const std::string& key, const std::string& path) {
    if (auto it = m_tex.find(key); it != m_tex.end())
        return it->second;  // 已缓存，直接返回

    sf::Texture t;
    if (!t.loadFromFile(path))
        throw std::runtime_error("Failed to load texture: " + path);

    m_tex[key] = std::move(t);
    return m_tex[key];
}

// 获取已缓存的纹理。如果 key 不存在，map::at() 抛 std::out_of_range。
sf::Texture& AssetManager::getTexture(const std::string& key) {
    return m_tex.at(key);
}
