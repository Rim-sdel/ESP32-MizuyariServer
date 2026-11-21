#include "data_struct.h"
#include "get_time.h"
#include "id_register.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <esp_now.h>

void onDataReceive(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  if (len == sizeof(int)) {
    int senderID;
    memcpy(&senderID, data, sizeof(senderID));
    const uint8_t *senderMac = info->src_addr;
    if (senderID == -1) {
      IDRegister(senderMac);
      return;
    }
    return;
  }

  struct_msg recvData;
  memcpy(&recvData, data, sizeof(recvData));
  Serial.printf("%s - [%d] moisture: %d\n", getTime().c_str(), recvData.ID, recvData.moisture);

  // 時間を計算して返すやつ
}

void espNowServerCbRegister() {
  esp_err_t result = esp_now_register_recv_cb(onDataReceive);
  Serial.printf("recv_cb Register: %s\n", result == ESP_OK ? "OK" : "FAIL");
}
