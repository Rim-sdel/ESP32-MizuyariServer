#include "global.h"
#include <Arduino.h>
#include <string>
#include <time.h>

const char *ntpServer1 = "ntp.nict.jp";
const char *ntpServer2 = "time.google.com";
const long gmtOffset_sec = 32400;
const int daylightOffset_sec = 0;

void timeToString(time_t time, char *timeString, size_t maxSize) {
  struct tm *timeinfo = localtime(&time);
  if (timeinfo != NULL)
    strftime(timeString, maxSize, "%Y-%m-%d %H:%M:%S", timeinfo);
  else
    timeString[0] = '\0';
}

time_t getTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    DBG_PRINT("Failed to get local time.");
    return (time_t)-1;
  }
  time_t now;
  return time(&now);
}

uint64_t getSleepTime(const int ID) {
  // 長時間Sleep
  // 毎日12時00分id秒と24時00分id秒に起床
  time_t now = getTime();
  struct tm *tm = localtime(&now);
  int secNow = tm->tm_hour * 3600 + tm->tm_min * 60 + tm->tm_sec;
  int target12 = 12 * 3600 + ID; // 12:00:00 + id
  int target24 = 24 * 3600 + ID;

  int sleepSec;
  if (secNow < target12) {
    sleepSec = target12 - secNow;
  } else {
    sleepSec = target24 - secNow;
  }
  return (uint64_t)sleepSec * uS_TO_S;
}

void timeSetup() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);
  delay(2000);
}
