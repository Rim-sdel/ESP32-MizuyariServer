#include "data_struct.h"
#include "get_time.h"
#include "id_register.h"
#include "pots_handler.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <esp_now.h>

volatile bool shouldNotify = false;
int thresholdSoil = 1000;
int notifyID = -1;

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
    std::string nowTime = getTime();
    sensorMsg msg;
    memcpy(&msg, data, sizeof(sensorMsg));

    if (int(msg.header.ID) == ID_UNREGISTERED) {
      const uint8_t *senderMac = info->src_addr;
      IDRegister(senderMac, msg.moisture, nowTime);
    } else {
      updatePots(msg.header.ID, msg.moisture, nowTime);
      Serial.printf("[RECV_INFO] %s - [%d] moisture: %d\n", nowTime.c_str(), msg.header.ID, msg.moisture);
    }
    // sendNextRun(msg.header.ID, nowTime); // 今からこの関数をどこかに作る
    break;
  }
  }

  // if (recvData.moisture > thresholdSoil) {
  //   shouldNotify = true;
  //   notifyID = recvData.ID;
  // }
}

void espNowServerCbRegister() {
  esp_err_t result = esp_now_register_recv_cb(onDataReceive);
  Serial.printf("recv_cb Register: %s\n", result == ESP_OK ? "OK" : "FAIL");
}
