#include "convert_str.h"
#include "data_struct.h"
#include "global.h"
#include "pots_handler.h"
#include "time_handler.h"
#include <Arduino.h>
#include <esp_now.h>

bool addPeerPot(const uint8_t mac[6]) {
  esp_now_peer_info_t pot = {};
  memcpy(pot.peer_addr, mac, 6);
  pot.encrypt = false;
  if (esp_now_add_peer(&pot) != ESP_OK) {
    DBG_PRINT("Failed add peer. Skip registering.");
    return false;
  }
  DBG_PRINT("Succeed add peer.");
  return true;
}

bool giveNewID(const int newID, const uint8_t mac[6]) {
  if (!esp_now_is_peer_exist(mac)) {
    if (!addPeerPot(mac))
      return false;
  }

  msgHeader msg;
  msg.type = MSG_ID_REGISTER;
  msg.ID = newID;
  if (esp_now_send(mac, (uint8_t *)&msg, sizeof(msg)) == ESP_OK) {
    DBG_PRINT("Succeed give newID!");
    return true;
  }
  DBG_PRINT("Failed give newID. Stop registering.");
  return false;
}

int IDRegister(const uint8_t senderMac[6], const int sensorRaw, const float battLev, const time_t now) {
  DBG_PRINT("Start ID register.");
  for (int i = 0; i < MAX_POTS; i++) {
    if (memcmp(pot[i].mac, senderMac, sizeof(pot[i].mac)) == 0) {
      DBG_PRINT("Already registerd, Skipping.");
      return i;
    }
  }

  int newID = -1;
  for (int i = 0; i < MAX_POTS; i++) {
    if (pot[i].active == false) {
      newID = i;
      break;
    }
  }
  if (newID == -1)
    return -1;

  if (!giveNewID(newID, senderMac)) {
    return -1;
  }

  pot[newID].active = true;
  pot[newID].ID = newID;
  for (int i = 0; i < 6; i++) {
    pot[newID].mac[i] = senderMac[i];
  }
  pot[newID].sensorRaw = sensorRaw;
  pot[newID].battLev = battLev;
  pot[newID].lastSeen = now;
  pot[newID].debugMode = true;

  JsonDocument doc = loadPotsJson();
  JsonArray arr = doc.as<JsonArray>();
  JsonObject newPot = arr.add<JsonObject>();

  newPot["ID"] = newID;
  newPot["name"] = nullptr;

  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X", senderMac[0], senderMac[1], senderMac[2], senderMac[3], senderMac[4], senderMac[5]);
  newPot["mac"] = macStr;

  newPot["sensorRaw"] = sensorRaw;
  newPot["moisture"] = nullptr;
  newPot["moistVal"] = nullptr;
  newPot["dryVal"] = nullptr;
  newPot["threshold"] = nullptr;
  newPot["battLev"] = battLev;
  newPot["lastSeen"] = (long long)now;
  newPot["debugMode"] = true;

  savePotsJson(doc);
  macToString(senderMac, macStr, sizeof(macStr));
  DBG_PRINT("New device registered: ID - %d, MAC - %s\n", newID, macStr);
  return newID;
}

void sendIDRemoveMsg(const int ID) {
  DBG_PRINT("Start ID remover.");
  msgHeader msg;
  msg.ID = ID;
  msg.type = MSG_ID_REMOVE;
  esp_now_send(pot[ID].mac, (const uint8_t *)&msg, sizeof(msg));
}
