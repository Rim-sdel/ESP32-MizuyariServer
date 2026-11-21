#pragma once
#include <ArduinoJson.h>

JsonDocument loadPots();
String loadPotsStr();
void savePots(JsonDocument &doc);
void updatePots(const int id, const int moisture, std::string lastSeen);
void updatePotName(const int id, const String newName);
std::pair<std::string, int> getPotData(const int id);
