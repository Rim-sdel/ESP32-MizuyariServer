#pragma once

extern volatile bool shouldNotify;
extern int notifyID;
extern int notifyMoisture;
void sendRemoveID(const int id);
void espNowServerCbRegister();
