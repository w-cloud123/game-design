// ============================================================
// src/AssetManager.hpp — 纹理缓存（单例）
// ============================================================
// 设计模式：Meyer's Singleton + 懒加载缓存
//
// 整个游戏只有一个 AssetManager 实例（通过 getInstance() 获取）。
// 纹理加载后缓存在 unordered_map 中，key 是字符串标识符。
// loadTexture 内部：如果 key 已存在就直接返回缓存的引用，避免重复加载。
//
// 为什么用单例而不是全局变量：
//   1. 避免 static 初始化顺序问题
//   2. 保证整个程序只有一个纹理池
//   3. 懒初始化：第一次 getInstance() 时才创建
// ============================================================
#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>

class AssetManager {
public:
    static AssetManager& getInstance(); // Meyer's 单例

    // 加载纹理并缓存（已存在则跳过）
    sf::Texture& loadTexture(const std::string& key, const std::string& path);
    // 获取已缓存的纹理（key 不存在则抛 std::out_of_range）
    sf::Texture& getTexture(const std::string& key);

private:
    AssetManager() = default; // 私有构造，防止外部 new
    std::unordered_map<std::string, sf::Texture> m_tex;
};
