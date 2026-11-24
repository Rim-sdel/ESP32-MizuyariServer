#pragma once

#include "esp_attr.h"
extern RTC_DATA_ATTR float battLev;
int getSoil();
void updateBattLv();
