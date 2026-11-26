#pragma once
#include "data_struct.h"
#include <esp_now.h>

bool sendHeader(const int ID, enum MsgType type);
void sendData();
void onPotRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len);
