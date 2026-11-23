#pragma once
#include <cstdint>
#include <ctime>
#include <string>

std::string getNextWake(time_t now, time_t sleepTime);
time_t getTime();
std::string getTimeString(time_t time);
uint64_t getSleepTime(const int id);
void timeSetup();
