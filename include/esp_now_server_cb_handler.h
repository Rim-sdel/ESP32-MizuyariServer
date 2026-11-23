#pragma once

extern volatile bool shouldNotify;
extern int notifyID;
void sendRemoveID(const int id);
void espNowServerCbRegister();
