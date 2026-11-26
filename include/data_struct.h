#pragma once

#include <cstdint>
#include <ctime>
#include <string>

inline const float lowBattery = 3.45;

enum MsgType : uint8_t {
  MSG_ID_REGISTER,
  MSG_POT_DATA,
  MSG_POT_DUMMY,
  MSG_SERVER_RES,
  MSG_ID_REMOVE,
  MSG_ID_REMOVE_DONE,
};

#ifdef SERVER_MACHINE
inline const uint8_t ID_UNREGISTERED = 255;
inline const char *POTS_FILE = "/pots.json";
const int MAX_POTS = 10;

typedef struct potData {
  bool active = false;
  uint8_t ID;
  // std::string name;
  char name[64];
  uint8_t mac[6];

  int sensorRaw;
  int moisture;
  int moistVal;
  int dryVal;
  int threshold;

  float battLev;
  time_t lastSeen;
  bool debugMode = false;
  bool needWater = false;
  bool needCharge = false;
} potData;

inline potData pot[MAX_POTS];
#endif

typedef struct __attribute__((packed)) {
  uint8_t type;
  uint8_t ID;
} msgHeader;

typedef struct __attribute__((packed)) {
  msgHeader header;
  uint64_t sleepTime;
} msgServer;

typedef struct __attribute__((packed)) {
  msgHeader header;
  int sensorRaw;
  float battV;
} msgPot;
