#pragma once
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

struct SensorReading {
    float temp1, hum1, temp2, hum2;
    int   mq1, mq2;
    bool  ready;
};

void startSensorTask();
bool getSensorReading(SensorReading& out);
