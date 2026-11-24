#include "board_config.h"
#include "data_struct.h"
#include "esp_now_sensor_handler.h"
#include "esp_now_sensor_setup.h"
#include "sensor_handler.h"
#include <Arduino.h>
#include <WiFi.h>
#include <cstdint>

#define uS_TO_S 1000000ULL
#define SLEEP_TIME 30

// bool lastButtonState = HIGH;
// bool stableState = HIGH;
// unsigned long lastDebounceTime = 0;
// unsigned long debounceDelay = 50;

unsigned long timeOut = 300;
unsigned long sendTime;

void deepSleep(uint64_t sleepTime) {
  // Serial.printf("sleepTime: %llu us\n", sleepTime);
  Serial.println("Data reveived. Going to deep sleep...");
  Serial.flush();

  esp_now_deinit();
  WiFi.mode(WIFI_OFF);

  esp_sleep_enable_timer_wakeup(sleepTime);
  esp_deep_sleep_start();
}

void setup() {
  pinMode(SENSOR_POWER, OUTPUT);
  digitalWrite(SENSOR_POWER, LOW);
  pinMode(SENSOR, INPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  Serial.begin(115200);
  delay(1000);

  espNowSetup();
  sendData(getSoil(), MSG_SENSOR_DATA);
  sendTime = millis();
}

void loop() {
  if (shouldSleep) {
    shouldSleep = false;
    deepSleep(sleepTime);
  }

  if (millis() - sendTime >= timeOut) {
    deepSleep(SLEEP_TIME * uS_TO_S);
  }

  // bool currentButtonState = digitalRead(BUTTON);
  //
  // if (currentButtonState != lastButtonState) {
  //   lastDebounceTime = millis();
  // }
  //
  // if ((millis() - lastDebounceTime) > debounceDelay) {
  //   if (stableState == HIGH && currentButtonState == LOW) {
  //     Serial.println("ぽちっ");
  //     sendData(getSoil(), MSG_SENSOR_DATA);
  //   }
  //   stableState = currentButtonState;
  // }
  // lastButtonState = currentButtonState;
}
