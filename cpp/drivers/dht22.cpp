#include "dht22.hpp"
#include <esp_log.h>
#include <dht.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char* TAG = "dht22";

void dht22_init(void) {
    ESP_LOGI(TAG, "DHT22 initialized");
}

DHT22Reading dht22_read(int pin) {
    DHT22Reading reading = {0.0f, 0.0f};

    float humidity = 0.0f;
    float temperature = 0.0f;

    esp_err_t res = dht_read_float_data(DHT_TYPE_DHT22, pin, &humidity, &temperature);

    if (res == ESP_OK) {
        reading.temperature = temperature;
        reading.humidity = humidity;
    } else {
        ESP_LOGE(TAG, "DHT read failed on pin %d: %s", pin, esp_err_to_name(res));
    }

    return reading;
}

void dht22_log_reading(int sensor_num, const DHT22Reading& reading) {
    ESP_LOGI(TAG, "DHT%d: temp=%0.1f°C hum=%0.1f%%",
             sensor_num, reading.temperature, reading.humidity);
}
