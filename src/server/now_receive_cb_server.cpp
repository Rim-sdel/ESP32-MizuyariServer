#include "data_struct.h"
#include "global.h"
#include "id_register.h"
#include "now_send_handler_server.h"
#include "pots_handler.h"
#include "time_handler.h"
#include <Arduino.h>
#include <ctime>
#include <esp_now.h>

void onDataReceive(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  msgHeader header;
  memcpy(&header, data, sizeof(msgHeader));
  int ID = header.ID;

  switch (header.type) {
  case MSG_POT_DUMMY:
    return;
  case MSG_ID_REMOVE_DONE: {
    removePot(header.ID);
    return;
  }

  case MSG_POT_DATA:
    time_t nowTime = getTime();
    msgPot msg;
    memcpy(&msg, data, sizeof(msg));
    const uint8_t *senderMac = info->src_addr;

    if (ID == ID_UNREGISTERED) {
      ID = IDRegister(senderMac, msg.sensorRaw, msg.battV, nowTime);
      if (ID == -1) {
        DBG_PRINT("Failed regist new device.");
        return;
      }
    } else {
      updatePots(ID, msg, nowTime);
      DBG_PRINT("Pot%d: sensor raw: %d, battery level: %.3f", ID, msg.sensorRaw, msg.battV);
    }

    sendSleepTime(ID, senderMac);
    updateMoisture(ID);
    updateNotifyState(pot[ID]);
    break;
  }

  Serial.println("--------------------");
}

void espNowServerCBRegister() {
  esp_err_t result = esp_now_register_recv_cb(onDataReceive);
  DBG_PRINT("Resist: %s", result == ESP_OK ? "Succeeded." : "Failed.");
}
