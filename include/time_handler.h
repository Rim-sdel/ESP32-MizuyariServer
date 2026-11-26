#pragma once

#include <ctime>
#include <string>

// std::string timeToString(time_t time);
void timeToString(time_t time, char *timeStting, size_t maxSize);
time_t getTime();
uint64_t getSleepTime(const int ID);
void timeSetup();
