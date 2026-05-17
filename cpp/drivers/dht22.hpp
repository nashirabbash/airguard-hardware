#pragma once

struct DHT22Reading {
    float temperature;
    float humidity;
};

void dht22_init(void);
DHT22Reading dht22_read(int pin);
void dht22_log_reading(int sensor_num, const DHT22Reading& reading);
