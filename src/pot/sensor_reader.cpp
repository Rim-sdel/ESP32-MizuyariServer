#include "board_config.h"
#include <Arduino.h>

int getMoisture() {
  digitalWrite(SENSOR_POWER, HIGH);
  delay(150);
  int rawMoist = 0;
  for (int i = 0; i < 10; i++) {
    rawMoist += analogRead(SENSOR);
  }
  digitalWrite(SENSOR_POWER, LOW);
  return rawMoist / 10;
}

float getBattV() {
  uint32_t rawBatt = 0;
  for (int i = 0; i < 16; i++) {
    rawBatt += analogReadMilliVolts(BATT);
  }
  return (2.0 * rawBatt / 16.0 / 1000.0);
}
