#include "get_time.h"
#include "pots_handler.h"
#include "secrets.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <string>

String buildMsg(const int notifyID) {
  auto [name, moist] = getPotData(notifyID);
  std::string moisture = std::to_string(moist);
  // std::string content = "💧💧💧水切れ！💧💧💧\n**" + name + "**\n🌱土の水分: " + moisture + "🌱\n\n検知時刻: " + getTime();
  std::string content = "💧💧💧水切れ！💧💧💧\n**" + name + "**\n🌱土の水分: " + moisture + "🌱";

  JsonDocument doc;
  doc["content"] = content;
  doc["username"] = "水やりウォッチャー";

  String jsonPayload;
  serializeJson(doc, jsonPayload);

  return jsonPayload;
}

void sendDiscordMsg(const int notifyID) {
  if (notifyID == -1)
    return;
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;
    http.begin(client, String(webhookUrl));
    // http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    // http.setTimeout(10000); // 10秒
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.POST(buildMsg(notifyID));

    if (httpCode > 0) {
      Serial.printf("Webhook Send. HTTP code: %d\n", httpCode);
    } else {
      Serial.printf("Webhook failed: %d", httpCode);
    }
    http.end();
  }
}
