#include "data_struct.h"
#include "littlefs_setup.h"
#include "now_receive_cb_server.h"
#include "now_setup_server.h"
#include "time_handler.h"
#include "webhook_handler.h"
#include "webserver_handler.h"
#include <Arduino.h>

const unsigned long coolDown = 30000;
unsigned long sendTime;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("--------------------");
  if (!LFSSetup()) {
    return;
  }

  espNowSetup();
  espNowServerCBRegister();

  timeSetup();
  serverSetup();
  Serial.println("--------------------");
}

void loop() {
  serverHandle();

  if (millis() - sendTime < coolDown)
    return;

  for (int i = 0; i < MAX_POTS; i++) {
    auto &p = pot[i];
    if (!p.debugMode) {
      if (p.needWater || p.needCharge) {
        sendWebHook(i);
        sendTime = millis();
      }
    }
  }
}
