#pragma once

// === ボードに合わせて切り替え ===
#define BOARD_C3
// #define BOARD_XIAO_C6

// === ピン設定 ===
#ifdef BOARD_C3
#define SENSOR 4
#define BUTTON 3
#endif

#ifdef BOARD_XIAO_C6
#define SENSOR D0
#define BUTTON D1
#endif
