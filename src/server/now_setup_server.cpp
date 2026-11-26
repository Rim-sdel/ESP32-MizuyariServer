#include "data_struct.h"
#include "global.h"
#include "id_register.h"
#include "pots_handler.h"
#include "secrets.h"
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

void peerSetup() {
  for (int i = 0; i < MAX_POTS; i++) {
    if (!pot[i].active)
      continue;
    addPeerPot(pot[i].mac);
  }
}

void initPots() {
  for (int i = 0; i < MAX_POTS; i++) {
    pot[i] = {};
    pot[i].active = false;
    pot[i].ID = -1;
  }
}

void espNowSetup() {
  WiFi.setHostname("esp32-Mizuyazi");
  WiFi.setSleep(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nConnected.\nChannel: %d\n", (int)WiFi.channel());
  Serial.print("localIP: ");
  Serial.print(WiFi.localIP());
  Serial.println("");

  if (esp_now_init() == ESP_OK) {
    DBG_PRINT("Init OK.");
  } else {
    DBG_PRINT("Internal Error.");
    return;
  }

  initPots();
  loadPots();
  peerSetup();
  Serial.printf("STA MAC Address: %s\n", WiFi.macAddress().c_str());
}
