#pragma once

void initWiFi();
bool isWiFiConnected();

void connectWS();
void loopWS();
bool isWSConnected();
void sendSensorData(float temp1, float hum1, float temp2, float hum2, int mq1, int mq2);
void sendLogDump();
