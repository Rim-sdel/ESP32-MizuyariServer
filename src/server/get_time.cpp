#include "pots_handler.h"
#include <Arduino.h>
#include <cstdint>
#include <ctime>
#include <string>
#include <time.h>

const char *ntpServer1 = "ntp.nict.jp";
const char *ntpServer2 = "time.google.com";
const long gmtOffset_sec = 32400;
const int daylightOffset_sec = 0;

#define uS_TO_S 1000000ULL
#define SLEEP_TIME 30

// uint64_t initSleepTime() {
// ブラウザでセンサー取得時刻を設定したあとの初回DeepSleep用
// 目標となる時刻と現在時刻の差分を求めて返す
// }

std::string getNextWake(time_t now, time_t sleepTime) {
  time_t wakeTime = now + sleepTime;
  struct tm timeinfo;
  localtime_r(&wakeTime, &timeinfo);

  char timeString[50];
  strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return timeString;
}

time_t getTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to get local time.");
    return (time_t)-1; //  if (getTime().empty()) で失敗を検知できる
  }

  time_t now;
  return time(&now);
}

std::string getTimeString(time_t time) {
  struct tm *timeinfo = localtime(&time);

  char timeString[50];
  strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", timeinfo);
  return timeString;
}

uint64_t getSleepTime(const int id) {
  if (isDbgMode(id)) {
    // 短時間Sleep
    return SLEEP_TIME * uS_TO_S;
  }

  // 長時間Sleep
  // 毎日12時00分id秒と24時00分id秒に起床
  time_t now = getTime();
  struct tm *tm = localtime(&now);
  int secNow = tm->tm_hour * 3600 + tm->tm_min * 60 + tm->tm_sec;

  int target12 = 12 * 3600 + id; // 12:00:00 + id
  int target24 = 24 * 3600 + id;

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
