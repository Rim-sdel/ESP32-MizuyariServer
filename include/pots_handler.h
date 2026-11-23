#pragma once
#include <ArduinoJson.h>

JsonDocument loadPots();
std::pair<JsonDocument, JsonArray> loadPotsAndArr();
String loadPotsStr();
void savePots(JsonDocument &doc);
void updatePots(const int id, const int sensorRaw, std::string lastSeen);
void updatePotName(const int id, const String newName);
void updatePotCalib(const int id, const char *key, const int val);
std::pair<std::string, int> getPotData(const int id);

void removePod(const int id);
