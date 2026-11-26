#include "data_struct.h"
#include "global.h"
#include "now_handler_pot.h"
#include <Arduino.h>
#include <Preferences.h>
#include <esp_now.h>

Preferences pref;
const char *myID = "myID";

int getMyPrefID() {
  pref.begin(myID, true);
  int prefID = pref.getInt(myID, -1);
  pref.end();
  return prefID;
}

void setMyNewID(const int ID) {
  pref.begin(myID, false);
  pref.putInt(myID, ID);
  pref.end();
  currentID = ID;
  DBG_PRINT("My new ID: %d", ID);
}

void removeMyID() {
  pref.begin(myID, false);
  pref.remove(myID);
  pref.end();
  DBG_PRINT("My ID is removed.");
  DBG_PRINT("Send removed report to server: %s", sendHeader(currentID, MSG_ID_REMOVE_DONE) == ESP_OK ? "Succeeded." : "Failed.");
  currentID = -1;
}
