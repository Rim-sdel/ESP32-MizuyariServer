#include "id_register.h"
#include "secrets.h"
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <pots_handler.h>

void peerSetup() {
  auto [doc, arr] = loadPotsAndArr();

  for (JsonObject obj : arr) {
    const char *hex = obj["mac"].as<const char *>();
    uint8_t mac[6];
    hexToMac(hex, mac);
    addPeerSensor(mac);
  }

  Serial.println("[ESP-NOW] Peer setup done.");
}

bool espOk() {
  if (esp_now_init() == ESP_OK) {
    Serial.println("[ESP-NOW] init: OK");
    return true;
  } else {
    Serial.println("[ESP-NOW] init: ERROR INTERNAL");
    return false;
  }
}

void espNowSetup() {
  WiFi.mode(WIFI_STA);
  WiFi.setHostname("esp32-Mizuyazi");
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

  peerSetup();
  Serial.printf("STA MAC Address: %s\n", WiFi.macAddress().c_str());
}
