#include "WString.h"
#include "global.h"
#include "id_register.h"
#include "pots_handler.h"
#include "webpage_string.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <WebServer.h>

WebServer server(80);

void handleRoot() { server.send(200, "text/html", page); }

void getPotsData() {
  String res = loadPotsStr();
  server.send(200, "text/plain", res);
}

void handleRename() {
  JsonDocument doc;
  deserializeJson(doc, server.arg("plain"));
  int ID = doc["ID"];
  const char *newName = doc["name"].as<const char *>();

  updatePotName(ID, newName);
  server.send(200, "text/plain", "OK");
}

void handleCalib() {
  JsonDocument doc;
  deserializeJson(doc, server.arg("plain"));
  int ID = doc["ID"];

  if (!doc["moistVal"].isNull()) {
    updatePotCalib(ID, "moistVal", doc["moistVal"].as<int>());
  } else if (!doc["dryVal"].isNull()) {
    updatePotCalib(ID, "dryVal", doc["dryVal"].as<int>());
  } else if (!doc["threshold"].isNull()) {
    updatePotCalib(ID, "threshold", doc["threshold"].as<int>());
  }

  server.send(200, "text/plain", "OK");
}

void handleRemove() {
  JsonDocument doc;
  deserializeJson(doc, server.arg("plain"));
  int ID = doc["ID"];
  removePot(ID);

  server.send(200, "text/plain", "OK");
}

void handleDbg() {
  JsonDocument doc;
  deserializeJson(doc, server.arg("plain"));
  int ID = doc["ID"];
  bool dbgMode = doc["debugMode"].as<bool>();
  toggleDbg(ID, dbgMode);

  server.send(200, "text/plain", "OK");
}
void handleReset() {
  resetAllPots();
  server.send(200, "text/plain", "OK");
}

void serverSetup() {
  server.on("/", handleRoot);
  server.on("/pots", getPotsData);
  server.on("/rename", HTTP_POST, handleRename);
  server.on("/calibrate", HTTP_POST, handleCalib);
  server.on("/remove", HTTP_POST, handleRemove);
  server.on("/debug", HTTP_POST, handleDbg);
  server.on("/reset", HTTP_POST, handleReset);
  server.begin();
  DBG_PRINT("Web server start!!");
}

void serverHandle() { server.handleClient(); }
