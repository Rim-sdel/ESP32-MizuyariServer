#include <Arduino.h>
#include <cstdio>

void macToString(const uint8_t mac[6], char *macStr, size_t size) { snprintf(macStr, size, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]); }
