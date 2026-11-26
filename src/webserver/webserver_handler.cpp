#include "WString.h"
#include "id_register.h"
#include "pots_handler.h"
#include "webpage.h"
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
  int ID = doc["id"];

  if (!doc["moist"].isNull()) {
    updatePotCalib(ID, "moist", doc["moist"].as<int>());
  } else if (!doc["dry"].isNull()) {
    updatePotCalib(ID, "dry", doc["dry"].as<int>());
  } else if (!doc["threshold"].isNull()) {
    updatePotCalib(ID, "threshold", doc["threshold"].as<int>());
  }

  server.send(200, "text/plain", "OK");
}

void handleRemove() {
  JsonDocument doc;
  deserializeJson(doc, server.arg("plain"));
  int ID = doc["id"];
  sendIDRemoveMsg(ID);

  server.send(200, "text/plain", "OK");
}

void handleDbg() {
  JsonDocument doc;
  deserializeJson(doc, server.arg("plain"));
  int ID = doc["id"];
  bool dbgMode = doc["debugMode"].as<bool>();
  toggleDbg(ID, dbgMode);

  server.send(200, "text/plain", "OK");
}
void serverSetup() {
  server.on("/", handleRoot);
  server.on("/pots", getPotsData);
  server.on("/rename", HTTP_POST, handleRename);
  server.on("/calibrate", HTTP_POST, handleCalib);
  server.on("/remove", HTTP_POST, handleRemove);
  server.on("/debug", HTTP_POST, handleDbg);
  server.begin();
  Serial.println("Web server start!!");
}

void serverHandle() { server.handleClient(); }
