#pragma once
#include "data_struct.h"
#include "esp_now.h"

extern volatile bool shouldSleep;
extern uint64_t sleepTime;

int getMyID();
bool sendIDData(const int ID, enum MsgType type);
bool sendData(const int soil, const float battLev, enum MsgType type);
void onRecvSensor(const esp_now_recv_info_t *info, const uint8_t *data, int len);
