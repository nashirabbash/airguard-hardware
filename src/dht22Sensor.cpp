#include "dht22Sensor.h"
#include "logManagement.h"
#include "pins.h"
#include <DHTesp.h>

static DHTesp sensor1;
static DHTesp sensor2;

void initDHT22() {
    sensor1.setup(DHT1_PIN, DHTesp::DHT22);
    sensor2.setup(DHT2_PIN, DHTesp::DHT22);
}

static void readSensor(DHTesp& sensor, const char* label, float& temp, float& hum) {
    TempAndHumidity data = sensor.getTempAndHumidity();
    if (sensor.getStatus() != DHTesp::ERROR_NONE) {
        logFail(label, sensor.getStatusString());
        temp = 0;
        hum  = 0;
    } else {
        char detail[64];
        snprintf(detail, sizeof(detail), "temp=%.1f°C hum=%.1f%%", data.temperature, data.humidity);
        logOK(label, detail);
        temp = data.temperature;
        hum  = data.humidity;
    }
}

void readAllDHT22(float& temp1, float& hum1, float& temp2, float& hum2) {
    readSensor(sensor1, "DHT22 #1 (GPIO4)",  temp1, hum1);
    delay(2000);
    readSensor(sensor2, "DHT22 #2 (GPIO15)", temp2, hum2);
}
