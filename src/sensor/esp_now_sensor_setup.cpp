#include "board_config.h"
#include "data_struct.h"
#include "secrets.h"
#include "sensor_handler.h"
#include <Arduino.h>
#include <Preferences.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

Preferences pref;
const char *myID = "myID";
volatile bool isSending = false;
volatile bool isChannel = false;
bool firstConnect = false;

void dummySent(const wifi_tx_info_t *info, esp_now_send_status_t status) {
  isSending = false;
  isChannel = (status == ESP_NOW_SEND_SUCCESS ? true : false);
}

int getMyId() {
  pref.begin(myID, true);
  int savedID = pref.getInt(myID, -1);
  pref.end();
  if (savedID == -1) {
    firstConnect = true;
    return savedID;
  }
  firstConnect = false;
  return savedID;
}

bool sendData(const int soil, enum MsgType type) {
  sensorMsg sendMsg;
  sendMsg = {};
  sendMsg.header.type = type;
  sendMsg.header.ID = getMyId();
  sendMsg.moisture = soil;
  return esp_now_send(serverAdrr, (const uint8_t *)&sendMsg, sizeof(sendMsg)) == ESP_OK;
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

    if (!sendData(getSoil(), MSG_SENSOR_DUMMY)) {
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

void onRecvSensor(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  if (len == sizeof(int)) {
    int recvID;
    memcpy(&recvID, data, sizeof(recvID));
    pref.begin(myID, false);
    if (firstConnect) {
      pref.putInt(myID, recvID);
      pref.end();
      Serial.printf("My new ID: %d\n", recvID);
    } else if (recvID == 9999) {
      const char *msg = "rmDone.";
      pref.remove(myID);
      pref.end();
      Serial.println("My ID is removed.");
      esp_now_send(serverAdrr, (uint8_t *)msg, strlen(msg) + 1);
      return;
      // server側の削除処理を今度書く！！！
    }
  }
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
