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

void updatePots(const int id, const int sensorRaw, std::string lastSeen) {
  JsonDocument doc = loadPots();
  JsonArray arr = doc.as<JsonArray>();

  for (JsonObject obj : arr) {
    if (obj["ID"].as<int>() == id) {
      obj["sensorRaw"] = sensorRaw;
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

void updatePotCalib(const int id, const char *key, const int val) {
  JsonDocument doc = loadPots();
  JsonArray arr = doc.as<JsonArray>();

  for (JsonObject obj : arr) {
    if (obj["ID"].as<int>() == id) {
      obj[key] = val;
      break;
    }
  }
  savePots(doc);
  Serial.printf("[POT_HANDLER] Pot%d: %s changed.\n", id, key);
}

std::pair<std::string, int> getPotData(const int id) {
  JsonDocument doc = loadPots();
  JsonArray arr = doc.as<JsonArray>();

  for (JsonObject obj : arr) {
    if (obj["ID"].as<int>() == id) {
      std::string name = obj["name"].as<std::string>();
      int sensorRaw = obj["sensorRaw"].as<int>();
      return { name, sensorRaw };
    }
  }
  return { "null", 0 };
}

void toggleDbg(const int id, const int dbgMode) {
  JsonDocument doc = loadPots();
  JsonArray arr = doc.as<JsonArray>();

  for (JsonObject obj : arr) {
    if (obj["ID"].as<int>() == id) {
      obj["debugMode"] = dbgMode;
      break;
    }
  }
  savePots(doc);
  Serial.printf("[POT_HANDLER] Pot%d: debugMode %s", id, dbgMode == 1 ? "ON" : "OFF");
}

bool isDbgMode(const int id) {
  JsonDocument doc = loadPots();
  JsonArray arr = doc.as<JsonArray>();

  for (JsonObject obj : arr) {
    if (obj["ID"].as<int>() == id) {
      return (obj["debugMode"] == 1);
    }
  }
  return 1;
}

void removePod(const int id) {
  JsonDocument doc = loadPots();
  JsonArray arr = doc.as<JsonArray>();

  for (size_t i = 0; i < arr.size(); i++) {
    if (arr[i]["ID"].as<int>() == id) {
      arr.remove(i);
      break;
    }
  }
  savePots(doc);
  Serial.printf("[POT_HANDLER] Pot%d: Removed.\n", id);
}
