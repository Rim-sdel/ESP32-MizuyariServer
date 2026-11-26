#include "board_config.h"
#include "data_struct.h"
#include "esp_now.h"
#include "global.h"
#include "id_handler_pot.h"
#include "now_handler_pot.h"
#include "now_setup_pot.h"
#include "secrets.h"
#include "sensor_reader.h"
#include <Arduino.h>
#include <WiFi.h>

const int sleepTimeLong = 43200; // 12h
int nextSleepTime;
bool shouldSleep = false;

RTC_DATA_ATTR float lastBattV;
RTC_DATA_ATTR int bootCount;
RTC_DATA_ATTR int currentID;

unsigned long timeOut = 300;
unsigned long sendTime;

void deepSleep(uint64_t sleepTime) {
  Serial.flush();

  esp_now_deinit();
  WiFi.mode(WIFI_OFF);

  esp_sleep_enable_timer_wakeup(sleepTime);
  esp_deep_sleep_start();
}

void bootCounter() {
  if (bootCount == 10) {
    bootCount = 1;
    return;
  }
  bootCount++;
}
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SENSOR_POWER, OUTPUT);
  digitalWrite(SENSOR_POWER, LOW);
  pinMode(SENSOR, INPUT);

  Serial.begin(115200);
  delay(500);

  Serial.println("--------------------");
  if (lastBattV < lowBattery) {
    DBG_PRINT("Battery low %3fV. Sleeping...\n", lastBattV);
    deepSleep(sleepTimeLong * uS_TO_S);
  }

  bootCounter();
  if (bootCount == 1) {
    lastBattV = getBattV();
  }

  currentID = getMyPrefID();
  espNowSetup();
  Serial.println("--------------------");

  sendData();
  sendTime = millis();
}

void loop() {
  if (shouldSleep) {
    shouldSleep = false;
    deepSleep(nextSleepTime);
  }

  if (millis() - sendTime >= timeOut) {
    deepSleep(SLEEP_TIME * uS_TO_S);
  }
}
