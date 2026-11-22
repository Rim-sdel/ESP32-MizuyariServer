#pragma once
#include <cstdint>
#include <string>

enum MsgType : uint8_t {
  MSG_ID_REGISTER, // 0
  MSG_SENSOR_DATA,
  MSG_SENSOR_DUMMY,
  MSG_SERVER_RES,
  MSG_ID_REMOVE,
};

#ifdef SERVER_MACHINE
#define MAX_POTS 10
inline const char *POTS_FILE = "/pots.json";
typedef struct potData {
  uint8_t ID;
  uint8_t mac[6];
  char name[32];
  bool active;
  std::string lastSeen;
  int moisture;
} potData;
#endif

typedef struct __attribute__((packed)) {
  uint8_t type;
  uint8_t ID;
} msgHeader;

typedef struct __attribute__((packed)) {
  msgHeader header;
  int moisture;
} sensorMsg;

typedef struct __attribute__((packed)) {
  msgHeader header;
} registerMsg;

typedef struct __attribute__((packed)) {
  msgHeader header;
  uint64_t nextRun;
} serverMsg;
