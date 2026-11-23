#include "data_struct.h"
#include "get_time.h"
#include "id_register.h"
#include "pots_handler.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <esp_now.h>

volatile bool shouldNotify = false;
int thresholdSoil = 3800;
int notifyID = -1;

void sendNextWake(const uint8_t id, const uint8_t *senderMac) {
  serverMsg msg;
  msg.header.type = MSG_SERVER_RES;
  msg.header.ID = id;
  msg.sleepTime = getSleepTime(id);
  Serial.print("[SERVER] Send next wake time: ");
  Serial.println(esp_now_send(senderMac, (const uint8_t *)&msg, sizeof(msg)) == ESP_OK ? "SUCCESS" : "FAIL");
}

bool isDry(const int id) {
  auto [doc, arr] = loadPotsAndArr();
  int raw, moist, dry, threshold;

  for (JsonObject obj : arr) {
    if (obj["ID"].as<int>() == id) {
      raw = obj["sensorRaw"];
      moist = obj["moist"];
      dry = obj["dry"];
      threshold = obj["threshold"];
      if (obj["moist"].isNull() || obj["dry"].isNull() || obj["threshold"].isNull())
        return false;
      break;
    }
  }

  if ((dry - raw) * 100 / (dry - moist) < threshold) {
    return true;
  }
  return false;
}

void sendRemoveID(const int id) {
  sensorRmMsg msg;
  msg.header.type = MSG_ID_REMOVE;
  msg.header.ID = id;
  char *hex;
  uint8_t mac[6];

  auto [doc, arr] = loadPotsAndArr();

  for (JsonObject obj : arr) {
    if (obj["ID"].as<int>() == id) {
      hex = obj["mac"].as<char *>();
      break;
    }
  }
  hexToMac(hex, mac);

  Serial.printf("[ID_REMOVER] pot%d: Removing...,", id);
  Serial.println(esp_now_send(mac, (const uint8_t *)&msg, sizeof(msg)) == ESP_OK ? "SUCCESS" : "FAIL");
}

void onDataReceive(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  msgHeader header;
  memcpy(&header, data, sizeof(msgHeader));

  switch (header.type) {
  case MSG_SENSOR_DUMMY:
    return;

  case MSG_ID_REMOVE_DONE: {
    removePod(header.ID);
    return;
  }
  case MSG_SENSOR_DATA: {
    time_t nowTime = getTime();
    std::string nowTimeStr = getTimeString(nowTime);
    sensorMsg msg;
    memcpy(&msg, data, sizeof(sensorMsg));
    const uint8_t *senderMac = info->src_addr;

    if (int(msg.header.ID) == ID_UNREGISTERED) {
      IDRegister(senderMac, msg.sensorRaw, nowTimeStr);
    } else {
      updatePots(msg.header.ID, msg.sensorRaw, nowTimeStr);
      Serial.printf("[RECV_INFO] %s - [%d] sensorRaw: %d\n", nowTimeStr.c_str(), msg.header.ID, msg.sensorRaw);
    }

    sendNextWake(msg.header.ID, senderMac);
    break;
  }
  }

  if (isDry(header.ID)) {
    shouldNotify = true;
    notifyID = header.ID;
  }

  Serial.println("--------------------");
}

void espNowServerCbRegister() {
  esp_err_t result = esp_now_register_recv_cb(onDataReceive);
  Serial.printf("recv_cb Register: %s\n", result == ESP_OK ? "OK" : "FAIL");
}
