#include "data_struct.h"
#include "global.h"
#include "time_handler.h"
#include <Arduino.h>
#include <cstdint>
#include <esp_now.h>

void sendSleepTime(const int ID, const uint8_t mac[6]) {
  msgServer msg;
  msg.header.type = MSG_SERVER_RES;
  msg.header.ID = ID;
  msg.sleepTime = (pot[ID].debugMode == true ? SLEEP_TIME * uS_TO_S : getSleepTime(ID));
  DBG_PRINT("Pot%d: Sned sleep time - %s", ID, (esp_now_send(mac, (const uint8_t *)&msg, sizeof(msg)) == ESP_OK ? "Success." : "Failed."));
}
