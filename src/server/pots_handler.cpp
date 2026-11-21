#include "LittleFS.h"
#include "data_struct.h"
#include <Arduino.h>
#include <ArduinoJson.h>

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
