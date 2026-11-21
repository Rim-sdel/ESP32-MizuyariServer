#include <Arduino.h>
#include <string>
#include <time.h>

const char *ntpServer1 = "ntp.nict.jp";
const char *ntpServer2 = "time.google.com";
const long gmtOffset_sec = 32400;
const int daylightOffset_sec = 0;

std::string getTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to get local time.");
    return ""; //  if (getTime().empty()) で失敗を検知できる
  }

  char timeString[50];
  strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return timeString;
}

unsigned long getUnixTime() {
  time_t now = time(&now);

  if (now < 1000000000) {
    Serial.println("Failed to get local time.");
    return 0; // 失敗時は 0 を返す
  }
  return now;
}

void timeSetup() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);
  delay(2000);
}
