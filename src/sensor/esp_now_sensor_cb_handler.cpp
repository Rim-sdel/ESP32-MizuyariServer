#include "data_struct.h"
#include "secrets.h"
#include <Arduino.h>
#include <Preferences.h>
#include <WiFi.h>
#include <cstdint>
#include <esp_now.h>
#include <esp_wifi.h>

Preferences pref;
const char *myID = "myID";

volatile bool shouldSleep = false;
uint64_t sleepTime;

int getMyId() {
  pref.begin(myID, true);
  int savedID = pref.getInt(myID, -1);
  pref.end();
  return savedID;
}

bool sendData(const int soil, enum MsgType type) {
  if (type == MSG_ID_REMOVE_DONE) {
    sensorRmMsg sendMsg;
    sendMsg = {};
    sendMsg.header.type = type;
    sendMsg.header.ID = -1;
    return esp_now_send(serverAdrr, (const uint8_t *)&sendMsg, sizeof(sendMsg)) == ESP_OK;
  }

  sensorMsg sendMsg;
  sendMsg = {};
  sendMsg.header.type = type;
  sendMsg.header.ID = getMyId();
  sendMsg.moisture = soil;
  return esp_now_send(serverAdrr, (const uint8_t *)&sendMsg, sizeof(sendMsg)) == ESP_OK;
}

void setMyNewID(const int ID) {
  pref.begin(myID, false);
  pref.putInt(myID, ID);
  pref.end();
  Serial.printf("[ID_REGISTRER] My new ID: %d\n", ID);
}

void removeMyID() {
  pref.begin(myID, false);
  pref.remove(myID);
  pref.end();
  Serial.printf("[ID_REGISTER] My ID is removed.");
  Serial.println(sendData(-1, MSG_ID_REMOVE_DONE) ? "[ID_REGISTER] Removed report has been sent to the server." : "[ID_REGISTER] Failed send removed report to the server.");
}

void onRecvSensor(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
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
    serverMsg msg;
    memcpy(&msg, data, sizeof(msg));
    sleepTime = msg.sleepTime;
    shouldSleep = true;
    return;
  }
  }
}
