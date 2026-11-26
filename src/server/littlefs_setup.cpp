#include "data_struct.h"
#include "global.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

void initfile() {
  File file = LittleFS.open(POTS_FILE, "r");
  if (!file) {
    DBG_PRINT("File none. Creating new file...");
    File newFile = LittleFS.open(POTS_FILE, "w");
    if (newFile) {
      newFile.print("[]");
      newFile.close();
      DBG_PRINT("Created initial pots file.");
    } else {
      DBG_PRINT("Failed to create pots file.");
    }
    return;
  }
  file.close();
}

bool LFSSetup() {
  if (!LittleFS.begin(true)) {
    DBG_PRINT("Failed mount LittleFS.");
    return false;
  }
  initfile();
  return true;
}
