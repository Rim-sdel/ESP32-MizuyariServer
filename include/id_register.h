#pragma once
#include <cstdint>
#include <string>

void hexToMac(const char *hex, uint8_t mac[6]);
bool addPeerSensor(const uint8_t mac[6]);
void IDRegister(const uint8_t mac[6], const int sensorRaw, std::string time);
