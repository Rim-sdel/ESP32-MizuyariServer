#include "LittleFS.h"
#include "data_struct.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <cstdio>
#include <cstring>
#include <esp_now.h>
#include <set>

String macToString(const uint8_t *senderMac) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X", senderMac[0], senderMac[1], senderMac[2], senderMac[3], senderMac[4], senderMac[5]);
  Serial.print("Sender: ");
  Serial.println(macStr);
  return String(macStr);
}

JsonDocument loadPots() {
  JsonDocument doc;
  File file = LittleFS.open(POTS_FILE, "r");
  if (file) {
    deserializeJson(doc, file);
    file.close();
  }
  if (!doc.is<JsonArray>()) {
    doc.to<JsonArray>();
  }
  return doc;
}

void savePots(JsonDocument &doc) {
  File file = LittleFS.open(POTS_FILE, "w");
  if (file) {
    serializeJson(doc, file);
    file.close();
  }
}

bool addPeerSensor(const uint8_t addr[6]) {
  esp_now_peer_info_t sensor = {};
  memcpy(sensor.peer_addr, addr, 6);
  sensor.encrypt = false;
  if (esp_now_add_peer(&sensor) != ESP_OK) {
    Serial.println("Add peer failed. Stop registering.");
    return false;
  }
  Serial.println("Add peer Succeed.");
  return true;
}

int getNewID(JsonArray &arr) {
  std::set<int> used;
  for (JsonObject obj : arr) {
    used.insert(obj["ID"].as<int>());
  }
  int ID = 0;
  while (used.count(ID))
    ID++;
  return ID;
}

bool giveNewID(const uint8_t mac[6], const int newID) {
  if (esp_now_send(mac, (uint8_t *)&newID, sizeof(newID)) == ESP_OK) {
    Serial.println("Succeed give new ID.");
    return true;
  }
  Serial.println("Failed give new ID. Stop registering.");
  return false;
}

void IDRegister(const uint8_t mac[6]) {
  String macStr = macToString(mac);
  JsonDocument doc = loadPots();
  JsonArray arr = doc.as<JsonArray>();

  for (JsonObject obj : arr) {
    if (obj["mac"].as<String>() == macStr) {
      Serial.println("Already registered, Skipping.");
      return;
    }
  }

  int newID = getNewID(arr);

  if (!esp_now_is_peer_exist(mac)) {
    if (!addPeerSensor(mac))
      return;
  }

  if (!giveNewID(mac, newID))
    return;

  JsonObject newDevice = arr.add<JsonObject>();
  newDevice["ID"] = newID;
  newDevice["mac"] = macStr;
  savePots(doc);

  Serial.print("New device registered: ");
  Serial.printf("ID - %d, MAC - %s\n", newID, macStr.c_str());
}
