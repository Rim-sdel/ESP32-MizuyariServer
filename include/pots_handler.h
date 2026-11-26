#pragma once

#include "WString.h"
#include "data_struct.h"
#include <ArduinoJson.h>
#include <ctime>

JsonDocument loadPotsJson();
String loadPotsStr();
void savePotsJson(JsonDocument &doc);
void updatePots(const int ID, msgPot msg, const time_t now);
void updateMoisture(const int ID);
void updateNotifyState(potData &p);
void updatePotName(const int ID, const char *newName);
void updatePotCalib(const int ID, const char *key, const int val);
void removePot(const int ID);
void toggleDbg(const int ID, const bool newState);
void loadPots();
void resetAllPots();
