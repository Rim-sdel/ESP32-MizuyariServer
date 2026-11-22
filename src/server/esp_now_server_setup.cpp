#include "secrets.h"
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <pots_handler.h>

void peerSetup() { JsonDocument doc = loadPots(); }

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
  Serial.printf("STA MAC Address: %s\n", WiFi.macAddress().c_str());
}
