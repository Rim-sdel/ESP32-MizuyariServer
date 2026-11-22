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

uint64_t getSleepTime() {
  // これは仮のやつ
  uint64_t time = SLEEP_TIME * uS_TO_S;
  return time;
}

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

void timeSetup() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);
  delay(2000);
}
