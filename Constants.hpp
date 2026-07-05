// ============================================================
// src/Constants.hpp — 游戏全局常量
// ============================================================
// 设计理念：
//   所有"调了就能影响游戏手感/平衡"的参数集中在这个命名空间中。
//   改数值不需要翻 Fighter、Game、Map 等业务代码，
//   避免"找一个参数要搜 5 个文件"的情况。
//
// inline constexpr 的作用：
//   C++17 起，inline 变量可在头文件中定义而不会产生重复符号。
//   constexpr 确保编译器在编译期计算该值，零运行时开销。
// ============================================================
#pragma once

namespace Constants {

// ==================== 窗口 ====================
inline constexpr int   WINDOW_WIDTH   = 800;   // 窗口宽度（像素）
inline constexpr int   WINDOW_HEIGHT  = 600;   // 窗口高度（像素）
inline constexpr const char* WINDOW_TITLE = "FIGHTING GAME";

// ==================== 场景 ====================
// GROUND_Y：角色脚底 y 坐标 = 地面线位置。
// 角色位置（m_position）是精灵左上角，所以实际脚底 = m_position.y + FIGHTER_SIZE。
// 落地检测就是 m_position.y + FIGHTER_SIZE >= GROUND_Y。
inline constexpr float GROUND_Y       = 450.f;
inline constexpr float GROUND_HEIGHT  = 4.f;   // 地面线渲染厚度（仅视觉效果）

// ==================== 物理 ====================
// GRAVITY：模拟 980 px/s² 重力加速度。每帧 dt 秒，v.y += GRAVITY * dt。
//          跳跃初速度 JUMP_VELOCITY = -420（负=向上），大约 0.86 秒后落地。
// HIT_STUN：受击后 0.12s 内无法操作。作用有二：
//           1. 让受击有"被打中的感觉"
//           2. 天然防止同一帧被连击多次（硬直期间不能再次受击——实际上可以，
//              但受击会重置硬直计时器，所以连续攻击会持续锁住对手）
inline constexpr float GRAVITY        = 980.f;
inline constexpr float HIT_STUN       = 0.12f;
inline constexpr float JUMP_VELOCITY  = -420.f;

// ==================== 精灵 & 碰撞 ====================
// FIGHTER_SIZE：精灵纹理的宽=高（px），也是渲染尺寸。
// HURTBOX_WIDTH：碰撞/受击框的宽度。刻意比精灵窄很多——
//                精灵边缘有大量透明留白，用全宽会让角色"隔空气碰撞"。
//                44px 约是 96px 的 46%，左右各留 26px 透明边距。
inline constexpr float FIGHTER_SIZE   = 96.f;
inline constexpr float HURTBOX_WIDTH  = 44.f;

// ==================== 技能 ====================
inline constexpr float DASH_SPEED       = 400.f; // 剑士突进速度（px/s）
inline constexpr float TRAP_OFFSET_X    = 60.f;  // 陷阱放置在角色前方多少 px

// ==================== 时间 ====================
// DT_CLAMP_MAX：如果一帧耗时超过 0.1s（比如断点调试暂停了很久），
//               直接把 dt 钳制到 DT_DEFAULT（≈60fps 的帧间隔）。
//               防止 delta time 过大导致角色瞬移穿墙。
inline constexpr float DT_CLAMP_MAX     = 0.1f;
inline constexpr float DT_DEFAULT       = 0.016f; // 1/60 ≈ 16.67ms
inline constexpr float KO_RESTART_DELAY = 3.f;    // KO 后几秒自动回选人界面
inline constexpr float BLINK_INTERVAL   = 0.5f;   // 启动画面文字闪烁间隔

} // namespace Constants
