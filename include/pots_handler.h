#pragma once
#include <ArduinoJson.h>

JsonDocument loadPots();
String loadPotsStr();
void savePots(JsonDocument &doc);
