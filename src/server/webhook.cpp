#include "get_time.h"
#include "secrets.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <string>

String buildMsg() {
  std::string content = "💧💧💧水切れ！💧💧💧\n**nameをここに代入**\n🌱土の水分:（センサー値）％🌱\n\n検知時刻: " + getTime();

  JsonDocument doc;
  doc["content"] = content;
  doc["username"] = "水やりウォッチャー";

  String jsonPayload;
  serializeJson(doc, jsonPayload);

  return jsonPayload;
}

void sendDiscordMsg() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;
    http.begin(client, String(webhookUrl));
    // http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    // http.setTimeout(10000); // 10秒
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.POST(buildMsg());

    if (httpCode > 0) {
      Serial.printf("Webhook Send. HTTP code: %d\n", httpCode);
    } else {
      Serial.printf("Webhook failed: %d", httpCode);
    }
    http.end();
  }
}
