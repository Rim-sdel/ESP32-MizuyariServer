#include "data_struct.h"
#include "pots_handler.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <esp_now.h>
#include <set>
#include <string>

#define uS_TO_S_FACTOR 1000000ULL
#define TIME_TO_SLEEP 30 // sec

String macToString(const uint8_t *mac) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.print("Sender: ");
  Serial.println(macStr);
  return String(macStr);
}

void macToHex(const uint8_t mac[6], char *hex) {
  for (int i = 0; i < 6; i++)
    sprintf(hex + i * 2, "%02X", mac[i]);
}

void hexToMac(const char *hex, uint8_t mac[6]) {
  for (int i = 0; i < 6; i++)
    sscanf(hex + i * 2, "%2hhx", &mac[i]);
}

bool addPeerSensor(const uint8_t mac[6]) {
  esp_now_peer_info_t sensor = {};
  memcpy(sensor.peer_addr, mac, 6);
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
  registerMsg msg;
  msg.header.type = MSG_ID_REGISTER;
  msg.header.ID = newID;
  if (esp_now_send(mac, (uint8_t *)&msg, sizeof(msg)) == ESP_OK) {
    Serial.println("[ID_REGISTER] Succeed give new ID.");
    return true;
  }
  Serial.println("[ID_REGISTER] Failed give new ID. Stop registering.");
  return false;
}

int IDRegister(const uint8_t mac[6], const int sensorRaw, const std::string time) {
  char macHex[13];
  macToHex(mac, macHex);

  JsonDocument doc = loadPots();
  JsonArray arr = doc.as<JsonArray>();

  for (JsonObject obj : arr) {
    if (strcmp(obj["mac"].as<const char *>(), macHex) == 0) {
      Serial.println("[ID_REGISTER] Already registered, Skipping.");
      return obj["ID"].as<int>();
    }
  }

  int newID = getNewID(arr);

  if (!esp_now_is_peer_exist(mac)) {
    if (!addPeerSensor(mac))
      return -1;
  }

  if (!giveNewID(mac, newID))
    return -1;

  JsonObject newDevice = arr.add<JsonObject>();
  newDevice["ID"] = newID;
  // newDevice["name"] = "";
  newDevice["sensorRaw"] = sensorRaw;
  // newDevice["moist"] = "";
  // newDevice["dry"] = "";
  // newDevice["threshold"] = "";
  newDevice["lastSeen"] = time;
  // newDevice["nextRun"] = "";
  newDevice["mac"] = macHex;
  newDevice["debugMode"] = 1;
  savePots(doc);

  String macStr = macToString(mac);
  Serial.print("[ID_REGISTER] New device registered: ");
  Serial.printf("ID - %d, MAC - %s\n", newID, macStr.c_str());
  return newID;
}
