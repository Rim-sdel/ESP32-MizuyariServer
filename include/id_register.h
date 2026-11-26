#pragma once

#include <cstdint>
#include <ctime>
bool addPeerPot(const uint8_t mac[6]);
int IDRegister(const uint8_t senderMac[6], const int sensorRaw, const float battLev, const time_t now);
void sendIDRemoveMsg(const int ID);
