#include "data_struct.h"
#include "global.h"
#include "id_handler_pot.h"
#include "now_setup_pot.h"
#include "secrets.h"
#include "sensor_reader.h"
#include <Arduino.h>
#include <esp_now.h>

bool sendHeader(const int ID, enum MsgType type) {
  msgHeader msg;
  msg.ID = ID;
  msg.type = type;

  esp_err_t result = esp_now_send(serverAddr, (const uint8_t *)&msg, sizeof(msg));
  return (result == ESP_OK);
}

void sendData() {
  msgPot msg;
  msg.header.type = MSG_POT_DATA;
  msg.header.ID = currentID;
  msg.sensorRaw = getMoisture();
  msg.battV = lastBattV;

  esp_err_t result = esp_now_send(serverAddr, (const uint8_t *)&msg, sizeof(msg));
  if (result == ESP_ERR_ESPNOW_CHAN) {
    setChannel();
    esp_now_send(serverAddr, (const uint8_t *)&msg, sizeof(msg));
  }
}

void onPotRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  if ((size_t)len < sizeof(msgHeader))
    return;

  msgHeader header;
  memcpy(&header, data, sizeof(msgHeader));

  switch (header.type) {
  case MSG_ID_REGISTER: {
    setMyNewID(header.ID);
    break;
  }
  case MSG_ID_REMOVE: {
    removeMyID();
    return;
  }
  case MSG_SERVER_RES: {
    msgServer msg;
    memcpy(&msg, data, sizeof(msg));
    nextSleepTime = msg.sleepTime;
    shouldSleep = true;
    return;
  }
  }
}
