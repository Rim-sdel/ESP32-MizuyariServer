#pragma once
#include <cstdint>

#ifdef SERVER_MACHINE
#define MAX_POTS 10
inline const char *POTS_FILE = "/pots.json";
typedef struct potData {
  uint8_t ID;
  uint8_t mac[6];
  char name[32];
  bool active;
  unsigned long lastSeen;
  int moisture;
} potData;
#endif

// #ifdef SENSOR_MACHINE
typedef struct struct_msg {
  uint8_t ID;
  int moisture;
} struct_msg;
// #endif
