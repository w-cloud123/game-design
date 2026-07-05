// ============================================================
// src/HUD.cpp — HUD 实现
// ============================================================
#include "HUD.hpp"
#include "Constants.hpp"
#include <sstream>
#include <iomanip>

HUD::HUD(const sf::Font& font)
    : m_nameText(font, "", 16)
    , m_cdText(font, "", 12)
{
    // 血条：红色填充，深灰背景，白边框
    m_hpBarBg.setSize({200.f, 16.f});
    m_hpBarBg.setFillColor(sf::Color(60, 60, 60));
    m_hpBarBg.setOutlineColor(sf::Color::White);
    m_hpBarBg.setOutlineThickness(1.f);
    m_hpBarFill.setSize({200.f, 16.f});
    m_hpBarFill.setFillColor(sf::Color::Red);

    // 蓝条：蓝色填充
    m_mpBarBg.setSize({200.f, 10.f});
    m_mpBarBg.setFillColor(sf::Color(60, 60, 60));
    m_mpBarBg.setOutlineColor(sf::Color::White);
    m_mpBarBg.setOutlineThickness(1.f);
    m_mpBarFill.setSize({200.f, 10.f});
    m_mpBarFill.setFillColor(sf::Color::Blue);

    m_nameText.setFillColor(sf::Color::White);
    m_cdText.setFillColor(sf::Color(200, 200, 200));
}

// 每帧调用：从 Fighter 读取最新数据，更新所有条的宽度和文字
void HUD::update(float hp, float maxHp, float mp, float maxMp,
                 const std::string& name,
                 float atkCD, float atkCDMax,
                 float sk1CD, float sk1CDMax,
                 float sk2CD, float sk2CDMax,
                 bool isLeftSide) {
    m_isLeft = isLeftSide;
    // 计算 HUD 面板的 X 坐标：左=10，右=窗口宽度-210
    float barX = isLeftSide ? 10.f : Constants::WINDOW_WIDTH - 210.f;

    // 血条宽度 = 200 * (当前血/最大血)
    m_hpBarBg.setPosition({barX, 10.f});
    float hpRatio = (maxHp > 0.f) ? (hp / maxHp) : 0.f;
    m_hpBarFill.setSize({200.f * hpRatio, 16.f});
    m_hpBarFill.setPosition({barX, 10.f});

    // 蓝条同理
    m_mpBarBg.setPosition({barX, 30.f});
    float mpRatio = (maxMp > 0.f) ? (mp / maxMp) : 0.f;
    m_mpBarFill.setSize({200.f * mpRatio, 10.f});
    m_mpBarFill.setPosition({barX, 30.f});

    // 角色名
    m_nameText.setString(name);
    m_nameText.setPosition({barX, 44.f});

    // CD 文字：格式 "ATK:0.5 S1:2.3 S2:0.0"，保留一位小数
    // CD 值本身是从 Fighter 的倒计时读的，≤0 表示可用
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1)
        << "ATK:" << ((atkCD > 0.f) ? atkCD : 0.f)
        << " S1:" << ((sk1CD > 0.f) ? sk1CD : 0.f)
        << " S2:" << ((sk2CD > 0.f) ? sk2CD : 0.f);
    m_cdText.setString(oss.str());
    m_cdText.setPosition({barX, 62.f});
}

void HUD::draw(sf::RenderWindow& window) const {
    window.draw(m_hpBarBg); window.draw(m_hpBarFill);
    window.draw(m_mpBarBg); window.draw(m_mpBarFill);
    window.draw(m_nameText); window.draw(m_cdText);
}
