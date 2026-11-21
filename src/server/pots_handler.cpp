#include "LittleFS.h"
#include "data_struct.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <string>

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

String loadPotsStr() {
  String jstr;
  serializeJson(loadPots(), jstr);
  return jstr;
}

void savePots(JsonDocument &doc) {
  File file = LittleFS.open(POTS_FILE, "w");
  if (file) {
    serializeJson(doc, file);
    file.close();
  }
}

void updatePots(const int id, const int moisture, std::string lastSeen) {
  JsonDocument doc = loadPots();
  JsonArray arr = doc.as<JsonArray>();

  for (JsonObject obj : arr) {
    if (obj["ID"].as<int>() == id) {
      obj["moisture"] = moisture;
      obj["lastSeen"] = lastSeen;
      break;
    }
  }
  savePots(doc);
  Serial.printf("[POT_HANDLER] Pot%d: Updated.\n", id);
}

void updatePotName(const int id, const String newName) {
  JsonDocument doc = loadPots();
  JsonArray arr = doc.as<JsonArray>();

  for (JsonObject obj : arr) {
    if (obj["ID"].as<int>() == id) {
      obj["name"] = newName;
      break;
    }
  }
  savePots(doc);
  Serial.printf("[POT_HANDLER] Pot%d: Name changed.\n", id);
}

std::pair<std::string, int> getPotData(const int id) {
  JsonDocument doc = loadPots();
  JsonArray arr = doc.as<JsonArray>();

  for (JsonObject obj : arr) {
    if (obj["ID"].as<int>() == id) {
      std::string name = obj["name"].as<std::string>();
      int moisture = obj["moisture"].as<int>();
      return { name, moisture };
    }
  }
  return { "null", 0 };
}
