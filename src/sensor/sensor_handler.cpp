#include "board_config.h"
#include <Arduino.h>

RTC_DATA_ATTR float battLev;

int getSoil() {
  int soilAdd = 0;
  digitalWrite(SENSOR_POWER, HIGH);
  delay(150);
  for (int i = 0; i < 10; i++) {
    soilAdd += analogRead(SENSOR);
  }
  digitalWrite(SENSOR_POWER, LOW);
  return soilAdd / 10;
}

void updateBattLv() {
  uint32_t Vbatt = 0;
  for (int i = 0; i < 16; i++) {
    Vbatt += analogReadMilliVolts(BATT);
  }
  float Vbattf = 2.0 * Vbatt / 16.0 / 1000.0;
  battLev = (Vbattf - 3.30) / (4.20 - 3.30) * 100;
  if (battLev > 100.0)
    battLev = 100.0;
  if (battLev < 0.0)
    battLev = 0.0;
}
