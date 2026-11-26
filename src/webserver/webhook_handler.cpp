#include "data_struct.h"
#include "secrets.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

void buildPayload(char *buf, size_t len, const potData &p) {
  const char *body;
  char content[192];

  if (p.needWater && p.needCharge) {
    snprintf(content, sizeof(content),
             "🚨🚨🚨 水も電池もない！🚨🚨🚨\\n"
             "**%s**\\n"
             "💧土の水分: %d％ (raw: %d)\\n"
             "🔋バッテリー残量: %f",
             p.name, p.moisture, p.sensorRaw, p.battLev);
  } else if (p.needWater) {
    snprintf(content, sizeof(content),
             "💧💧💧水切れ！💧💧💧\\n"
             "**%s**\\n"
             "🌱土の水分: %d％ (raw: %d)🌱",
             p.name, p.moisture, p.sensorRaw);
  } else if (p.needCharge) {
    snprintf(content, sizeof(content),
             "⚡️⚡️⚡️充電切れ！⚡️⚡️⚡️\\n"
             "**%s**\\n"
             "🔋バッテリー残量: %f🔋",
             p.name, p.battLev);
  } else {
    content[0] = '\0';
  }

  snprintf(buf, len, "{\"content\":\"%s\",\"username\":\"水やりウォッチャー\"}", content);
}

void sendWebHook(const int ID) {
  if (ID == -1)
    return;
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;
    http.begin(client, String(webhookUrl));
    http.addHeader("Content-Type", "application/json");

    char payload[384];
    buildPayload(payload, sizeof(payload), pot[ID]);
    int httpCode = http.POST(payload);

    if (httpCode > 0) {
      Serial.printf("Webhook Send. HTTP code: %d\n", httpCode);
      pot[ID].needWater = false;
      pot[ID].needCharge = false;
    } else {
      Serial.printf("Webhook failed: %d", httpCode);
    }
    http.end();
  }
}
