#pragma once
#include <Arduino.h>

void logOK(const char* label, const char* detail);
void logFail(const char* label, const char* reason);
void logToFlash(const char* msg);
void sensorLog(float temp1, float hum1, float temp2, float hum2, int mq1, int mq2);
void dumpLogs();
void clearLogs();
int  getLogCount();
int  buildLogPayload(char* buf, size_t bufSize, int maxEntries);
