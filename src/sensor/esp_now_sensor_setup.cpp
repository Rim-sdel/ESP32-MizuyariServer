#include "board_config.h"
#include "data_struct.h"
#include "esp_now_sensor_cb_handler.h"
#include "secrets.h"
#include <Arduino.h>
#include <Preferences.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

volatile bool isSending = false;
volatile bool isChannel = false;

void dummySent(const wifi_tx_info_t *info, esp_now_send_status_t status) {
  isSending = false;
  isChannel = (status == ESP_NOW_SEND_SUCCESS ? true : false);
}
void setChannel() {
  // ダミーを各チャネルに送信して、サーバー機とチャネルを合わせる
  // IDが割り振られていればそれを使って名乗る

  wifi_second_chan_t NONE = WIFI_SECOND_CHAN_NONE;
  esp_now_register_send_cb(dummySent);

  for (uint8_t i = 0; i < 14; i++) {
    esp_wifi_set_channel(i, NONE);
    isSending = true;
    isChannel = false;

    if (!sendIDData(0, MSG_SENSOR_DUMMY)) {
      Serial.printf("Channel %d: Send failed.\n", i);
      isSending = false;
      continue;
    }

    while (isSending) {
      delay(1);
    }

    if (isChannel) {
      Serial.printf("Channel %d: Success!\n", i);
      return;
    }
    Serial.printf("Channel %d: Failed.\n", i);
  }
  Serial.println("Failed find server channel. Set channel: 0");
  esp_wifi_set_channel(0, NONE);
}

bool espOk() {
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESP-NOW: OK");
    return true;
  } else {
    Serial.println("ESP-NOW: ERROR INTERNAL");
    return false;
  }
}

void espNowSetup() {
  WiFi.mode(WIFI_STA);

#ifdef SENSOR_MACHINE
#ifdef BOARD_C3
  // ESP32-C3のWiFiは'WIFI_POWER_8_5dBm'がないと安定しづらい
  WiFi.setTxPower(WIFI_POWER_8_5dBm);
#endif
  WiFi.disconnect();

  if (!espOk()) {
    return;
  }

  esp_now_peer_info_t server = {};
  memcpy(server.peer_addr, serverAdrr, 6);
  server.encrypt = false;
  if (esp_now_add_peer(&server) != ESP_OK) {
    Serial.println("Fail add peer...");
    return;
  }
  setChannel();

  esp_err_t result = esp_now_register_recv_cb(onRecvSensor);
  Serial.printf("recv_cb Register: %s\n", result == ESP_OK ? "OK" : "FAIL");

#endif

  Serial.printf("STA MAC Address: %s\n", WiFi.macAddress().c_str());
}
