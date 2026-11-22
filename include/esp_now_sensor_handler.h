#pragma once
#include "data_struct.h"
#include "esp_now.h"

extern volatile bool shouldSleep;
extern uint64_t sleepTime;

bool sendData(const int soil, enum MsgType type);
void onRecvSensor(const esp_now_recv_info_t *info, const uint8_t *data, int len);
