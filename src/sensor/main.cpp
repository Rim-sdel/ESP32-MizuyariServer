#include "board_config.h"
#include "esp_now_setup.h"
#include <Arduino.h>

bool lastButtonState = HIGH;
bool stableState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

void setup() {
  pinMode(SENSOR, INPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  Serial.begin(115200);
  delay(1000);

  espNowSetup();
}

void loop() {
  bool currentButtonState = digitalRead(BUTTON);

  if (currentButtonState != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (stableState == HIGH && currentButtonState == LOW) {
      Serial.println("ぽちっ");
      int soil = analogRead(SENSOR);
      sendData(soil);
    }
    stableState = currentButtonState;
  }
  lastButtonState = currentButtonState;
}
