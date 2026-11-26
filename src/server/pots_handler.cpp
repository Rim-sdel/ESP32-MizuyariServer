#include "LittleFS.h"
#include "data_struct.h"
#include "global.h"
#include "time_handler.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ctime>

JsonDocument loadPotsJson() {
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
  String str;
  serializeJson(loadPotsJson(), str);
  return str;
}

void savePotsJson(JsonDocument &doc) {
  File file = LittleFS.open(POTS_FILE, "w");
  if (file) {
    serializeJson(doc, file);
    file.close();
  }
}

struct PotsJson {
  JsonDocument doc;
  JsonArray arr;
};

PotsJson loadPotsArr() {
  PotsJson data;
  data.doc = loadPotsJson();
  data.arr = data.doc.as<JsonArray>();
  return data;
}

void updatePots(const int ID, msgPot msg, const time_t now) {
  auto &p = pot[ID];
  p.sensorRaw = msg.sensorRaw;
  p.battLev = msg.battV;
  p.lastSeen = now;

  auto [doc, arr] = loadPotsArr();

  for (JsonObject obj : arr) {
    if (obj["ID"].as<int>() == ID) {
      obj["sensorRaw"] = msg.sensorRaw;
      obj["battLev"] = msg.battV;
      obj["lastSeen"] = timeToString(now);
      break;
    }
  }
  savePotsJson(doc);
  DBG_PRINT("Pot%d: Sensor data updated.", ID);
}

void updateMoisture(const int ID) {
  auto &p = pot[ID];

  if (!p.moistVal || !p.dryVal || !p.threshold)
    return;

  p.moisture = (p.dryVal - p.sensorRaw) * 100 / (p.dryVal - p.moistVal);
  if (p.moisture > 100)
    p.moisture = 100;
  if (p.moisture < 0)
    p.moisture = 0;

  auto [doc, arr] = loadPotsArr();

  for (JsonObject obj : arr) {
    if (obj["ID"].as<int>() == ID) {
      obj["moisture"] = p.moisture;
      break;
    }
  }
  savePotsJson(doc);
  DBG_PRINT("Pot%d: Moisture updated.", ID);
}

void updateNotifyState(potData &p) {
  if (p.battLev) {
    if (p.battLev <= lowBattery)
      p.needCharge = true;
    else
      p.needCharge = false;
  }

  if (!p.moistVal || !p.dryVal || !p.threshold)
    return;

  if (p.moisture < p.threshold)
    p.needWater = true;
  else
    p.needWater = false;
}

void updatePotName(const int ID, const char *newName) {
  auto &p = pot[ID];
  strlcpy(p.name, newName, sizeof(p.name));

  auto [doc, arr] = loadPotsArr();
  for (JsonObject obj : arr) {
    if (obj["ID"].as<int>() == ID) {
      obj["name"] = newName;
      break;
    }
  }
  savePotsJson(doc);
  DBG_PRINT("Pot%d: Name changed.", ID);
}

void updatePotCalib(const int ID, const char *key, const int val) {
  auto &p = pot[ID];
  auto [doc, arr] = loadPotsArr();

  if (strcmp(key, "moistVal") == 0)
    p.moistVal = val;
  else if (strcmp(key, "dryVal") == 0)
    p.dryVal = val;
  else if (strcmp(key, "threshold") == 0)
    p.threshold = val;
  else
    return;

  for (JsonObject obj : arr) {
    if (obj["ID"].as<int>() == ID) {
      obj[key] = val;
      break;
    }
  }
  savePotsJson(doc);
  DBG_PRINT("Pot%d: %s changed.", ID, key);
}

void removePot(const int ID) {
  auto &p = pot[ID];
  auto [doc, arr] = loadPotsArr();

  for (size_t i = 0; i < arr.size(); i++) {
    if (arr[i]["ID"].as<int>() == ID) {
      arr.remove(i);
      break;
    }
  }
  savePotsJson(doc);
  p = {};
  DBG_PRINT("Pot%d: Removed.", ID);
}

void toggleDbg(const int ID, const bool newState) {
  auto &p = pot[ID];
  auto [doc, arr] = loadPotsArr();

  p.debugMode = newState;

  for (JsonObject obj : arr) {
    if (obj["ID"].as<int>() == ID) {
      obj["debugMode"] = newState;
      break;
    }
  }
  savePotsJson(doc);
  DBG_PRINT("Pot%d : Set debug mode %b", ID, p.debugMode);
}

void loadPots() {
  auto [doc, arr] = loadPotsArr();

  int i = 0;
  for (JsonObject obj : arr) {
    if (i >= MAX_POTS) {
      DBG_PRINT("Too many pots. Stoped load pots.");
      break;
    }

    int id = obj["ID"];

    auto &p = pot[id];
    p.active = false;
    p.ID = id;
    strlcpy(p.name, obj["name"] | "未設定", sizeof(p.name));

    const char *macStr = doc["mac"];
    sscanf(macStr, "%hhX:%hhX:%hhX:%hhX:%hhX:%hhX", &p.mac[0], &p.mac[1], &p.mac[2], &p.mac[3], &p.mac[4], &p.mac[5]);
    p.sensorRaw = obj["sensorRaw"];
    p.moisture = obj["moisture"];
    p.moistVal = obj["moistVal"];
    p.dryVal = obj["dryVal"];
    p.threshold = obj["threshold"];
    p.battLev = obj["battLev"];
    p.lastSeen = (time_t)(obj["lastSeen"].as<long long>());
    p.debugMode = obj["debugMode"];

    updateNotifyState(pot[id]);

    i++;
  }
}
