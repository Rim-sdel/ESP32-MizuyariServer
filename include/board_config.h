#pragma once

#ifdef SENSOR_MACHINE
// === ボードに合わせて切り替え ===
#define BOARD_C3
// #define BOARD_XIAO_C6

// === ピン設定 ===
#ifdef BOARD_C3
#define SENSOR 0
#define SENSOR_POWER 1
#define BUTTON 3
#endif

#ifdef BOARD_XIAO_C6
#define SENSOR D0
#define BUTTON D1
#endif

#endif
