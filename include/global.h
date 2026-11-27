#pragma once
#include <Arduino.h>
#include <cstdint>

#define DBG_PRINT(fmt, ...) Serial.printf("[%s] " fmt "\n", __func__, ##__VA_ARGS__)

#define uS_TO_S 1000000ULL
inline const int SLEEP_TIME = 30;

#ifdef POT_MACHINE
extern float lastBattV;
extern int currentID;
extern uint64_t nextSleepTime;
extern bool shouldSleep;
#endif
