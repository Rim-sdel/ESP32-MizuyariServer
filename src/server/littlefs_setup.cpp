#include "data_struct.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

potData pot[MAX_POTS];

void initfile() {
  File file = LittleFS.open(POTS_FILE, "r");
  if (!file) {
    Serial.println("File none. Creating...");
    File newFile = LittleFS.open(POTS_FILE, "w");
    if (newFile) {
      newFile.print("[]");
      newFile.close();
      Serial.println("Created initial pots file.");
    } else {
      Serial.println("Failed to create pots file.");
    }
    return;
  }
  file.close();
}

#ifdef DBGG
void listRootFiles() {
  File root = LittleFS.open("/");
  if (!root || !root.isDirectory()) {
    Serial.println("Failed open root dir.");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("[DIR]  ");
      Serial.println(file.name());
    } else {
      Serial.print("[FILE] ");
      Serial.print(file.name());
      Serial.print("\t(");
      Serial.print(file.size());
      Serial.println(" bytes)");
    }
    // 次のエントリへ進む
    file = root.openNextFile();
  }
}
#endif

// bool savePots(const potData pots[]) {
//   JsonDocument doc;
//
//   doc["Id"] = "";
//   doc["mac"] = "";
//   doc["name"] = "";
//   doc["active"] = "";
//
//   String output;
//   doc.shrinkToFit();
//   serializeJson(doc, output);
//   return true;
// }

bool LFSSetup() {
  if (!LittleFS.begin(true)) {
    Serial.println("Failed mount LittleFS...");
    return false;
  }
  initfile();
  return true;
}
