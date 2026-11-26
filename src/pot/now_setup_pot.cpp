#include "data_struct.h"
#include "global.h"
#include "now_handler_pot.h"
#include "secrets.h"
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

volatile bool isSending = false;
volatile bool isChannel = false;

void updateState(const uint8_t *mac_addr, esp_now_send_status_t status) {
  isSending = false;
  isChannel = (status == ESP_NOW_SEND_SUCCESS);
}

void setChannel() {
  // ダミーを各チャネルに送信して、サーバー機とチャネルを合わせる
  wifi_second_chan_t NONE = WIFI_SECOND_CHAN_NONE;
  esp_now_register_send_cb(updateState);

  for (uint8_t i = 0; i < 14; i++) {
    esp_wifi_set_channel(i, NONE);
    isSending = true;
    isChannel = false;

    if (!sendHeader(0, MSG_POT_DUMMY)) {
      DBG_PRINT("Channel %d: Send failed.", i);
      isSending = false;
      continue;
    }

    while (isSending) {
      delay(1);
    }

    if (isChannel) {
      DBG_PRINT("Channel %d: Success!", i);
      return;
    }
    DBG_PRINT("Channel %d: Failed.", i);
  }
  DBG_PRINT("Failed find server channel. Set channel -> 0");
  esp_wifi_set_channel(0, NONE);
}

void espNowSetup() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  // WiFi.setTxPower(WIFI_POWER_8_5dBm); // C3

  if (esp_now_init() == ESP_OK) {
    DBG_PRINT("Init OK.");
  } else {
    DBG_PRINT("Internal Error.");
    return;
  }

  esp_now_peer_info_t server = {};
  memcpy(server.peer_addr, serverAddr, 6);
  server.encrypt = false;
  if (esp_now_add_peer(&server) != ESP_OK) {
    DBG_PRINT("Fail add peer...");
    return;
  }

  setChannel();

  esp_err_t result = esp_now_register_recv_cb(onPotRecv);
  DBG_PRINT("Resist receive cb: %s", result == ESP_OK ? "OK." : "failed.");
  DBG_PRINT("My MAC address: %s", WiFi.macAddress().c_str());
}
