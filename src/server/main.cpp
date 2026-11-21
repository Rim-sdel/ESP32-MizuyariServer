#include "esp_now_server_cb_handler.h"
#include "esp_now_setup.h"
#include "get_time.h"
#include "pots_manage.h"
#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  delay(1000);

  timeSetup();

  if (!LFSSetup()) {
    return;
  }

  espNowSetup();
  espNowServerCbRegister();
#ifdef DBGG
  listRootFiles();
#endif
}

void loop() {}
