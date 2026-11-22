#include "esp_now_server_cb_handler.h"
#include "esp_now_server_setup.h"
#include "get_time.h"
#include "littlefs_setup.h"
#include "webhook.h"
#include "webserver_page.h"
#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("--------------------");
  if (!LFSSetup()) {
    return;
  }

  espNowSetup();
  espNowServerCbRegister();

  timeSetup();
  serverSetup();
#ifdef DBGG
  listRootFiles();
#endif
  Serial.println("--------------------");
}

void loop() {
  serverHandle();

  if (shouldNotify) {
    shouldNotify = false;
    sendDiscordMsg(notifyID);
    notifyID = -1;
  }
}
