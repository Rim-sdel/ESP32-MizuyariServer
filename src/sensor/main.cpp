#include "board_config.h"
#include "data_struct.h"
#include "esp_now_sensor_handler.h"
#include "esp_now_sensor_setup.h"
#include "sensor_handler.h"
#include <Arduino.h>
#include <WiFi.h>

bool lastButtonState = HIGH;
bool stableState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

void setup() {
  pinMode(SENSOR_POWER, OUTPUT);
  digitalWrite(SENSOR_POWER, LOW);
  pinMode(SENSOR, INPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  Serial.begin(115200);
  delay(1000);

  espNowSetup();
  sendData(getSoil(), MSG_SENSOR_DATA);
}

void deepSleep() {
  // Serial.printf("sleepTime: %llu us\n", sleepTime);
  Serial.println("Data reveived. Going to deep sleep...");
  Serial.flush();

  esp_now_deinit();
  WiFi.mode(WIFI_OFF);

  esp_sleep_enable_timer_wakeup(sleepTime);
  esp_deep_sleep_start();
}

void loop() {
  if (shouldSleep) {
    shouldSleep = false;
    deepSleep();
  }

  bool currentButtonState = digitalRead(BUTTON);

  if (currentButtonState != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (stableState == HIGH && currentButtonState == LOW) {
      Serial.println("ぽちっ");
      sendData(getSoil(), MSG_SENSOR_DATA);
    }
    stableState = currentButtonState;
  }
  lastButtonState = currentButtonState;
}
