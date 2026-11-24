#pragma once
#include <cstdint>
#include <string>

void hexToMac(const char *hex, uint8_t mac[6]);
bool addPeerSensor(const uint8_t mac[6]);
int IDRegister(const uint8_t mac[6], const int sensorRaw, const float battLev, std::string time);
