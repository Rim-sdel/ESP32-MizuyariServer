#include "data_struct.h"
#include "get_time.h"
#include "id_register.h"
#include "webhook.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <esp_now.h>

volatile bool shouldNotify = false;
// #if ESP_ARDUINO_VERSION_MAJOR >= 3
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

  shouldNotify = true;
  // 時間を計算して返すやつ
}
// #else
// void onDataReceive(const uint8_t *mac_addr, const uint8_t *data, int len) {
//   if (len == sizeof(int)) {
//     int senderID;
//     memcpy(&senderID, data, sizeof(senderID));
//     if (senderID == -1) {
//       IDRegister(mac_addr);
//       return;
//     }
//     return;
//   }
//
//   struct_msg recvData;
//   memcpy(&recvData, data, sizeof(recvData));
//   Serial.printf("%s - [%d] moisture: %d\n", getTime().c_str(), recvData.ID, recvData.moisture);
//
//   sendDiscordMsg();
//   // 時間を計算して返すやつ
// }
// // #endif
void espNowServerCbRegister() {
  esp_err_t result = esp_now_register_recv_cb(onDataReceive);
  Serial.printf("recv_cb Register: %s\n", result == ESP_OK ? "OK" : "FAIL");
}
