#include "board_config.h"
#include <Arduino.h>

int getSoil() {
  digitalWrite(SENSOR_POWER, HIGH);
  delay(150);
  int soil = analogRead(SENSOR);
  digitalWrite(SENSOR_POWER, LOW);
  return soil;
}
