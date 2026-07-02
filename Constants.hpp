// ============================================================
// src/Constants.hpp — 游戏全局常量
// ============================================================
#pragma once

namespace Constants {

inline constexpr int   WINDOW_WIDTH   = 800;
inline constexpr int   WINDOW_HEIGHT  = 600;
inline constexpr const char* WINDOW_TITLE = "FIGHTING GAME";

inline constexpr float GROUND_Y       = 450.f;
inline constexpr float GROUND_HEIGHT  = 4.f;
inline constexpr float GRAVITY        = 980.f;
inline constexpr float HIT_STUN       = 0.12f;
inline constexpr float FIGHTER_SIZE   = 64.f;
inline constexpr float JUMP_VELOCITY  = -420.f;

inline constexpr bool  SHOW_DEBUG     = false;

} // namespace Constants
