#include "board_config.h"
#include "data_struct.h"
#include "secrets.h"
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

#ifdef SENSOR_MACHINE
#include <Preferences.h>
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

void setChannel() {
  // ダミーを各チャネルに送信して、サーバー機とチャネルを合わせる
  // IDが割り振られていればそれを使って名乗る
  int myID;
  myID = getMyId();

  wifi_second_chan_t NONE = WIFI_SECOND_CHAN_NONE;
  esp_now_register_send_cb(dummySent);

  for (uint8_t i = 0; i < 14; i++) {
    esp_wifi_set_channel(i, NONE);
    isSending = true;
    isChannel = false;

    if (esp_now_send(serverAdrr, (uint8_t *)&myID, sizeof(myID)) != ESP_OK) {
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

  // typedef struct msg {
  //   int ID; // 登録時以外の通信にIDっているの？
  //   unsigned long nextWakeTime;
  // } msg;
  // msg newMsg;
  // memcpy(&newMsg, data, sizeof(newMsg));

  // nextWakeTimeをタイマーとしてセットしてDeepSleepを始める処理を書く
}

void sendData(const int soil) {
  struct_msg sendData;
  sendData = {};
  sendData.ID = getMyId();
  sendData.moisture = soil;
  Serial.println(esp_now_send(serverAdrr, (const uint8_t *)&sendData, sizeof(sendData)) == ESP_OK ? "Send success" : "Send fail");
}
#endif

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

#ifdef SERVER_MACHINE
  WiFi.setHostname("esp32-moisture");
  WiFi.begin(ssid, pass);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  WiFi.setSleep(false);
  Serial.printf("\nConnected.\nChannel: %d\n", (int)WiFi.channel());
  Serial.print("localIP: ");
  Serial.print(WiFi.localIP());
  Serial.println("");

  if (!espOk()) {
    return;
  }
#endif

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
