#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "log.hpp"
#include "config.hpp"
#include "../drivers/dht22.hpp"
#include "../drivers/mq135.hpp"
#include "../drivers/mod.hpp"

static const char* TAG = "airguard";

extern "C" void app_main(void) {
    ESP_LOGI(TAG, "=== AirGuard Hardware (C++) ===");
    log_ok("boot", "firmware started");

    drivers_init();
    log_ok("drivers", "initialized");

    while (1) {
        ESP_LOGI(TAG, "--- sensor scan ---");

        DHT22Reading dht1 = dht22_read(DHT1_PIN);
        dht22_log_reading(1, dht1);

        DHT22Reading dht2 = dht22_read(DHT2_PIN);
        dht22_log_reading(2, dht2);

        MQ135Reading mq1 = mq135_read(MQ1_AO_PIN, MQ1_DO_PIN);
        mq135_log_reading(1, mq1);

        MQ135Reading mq2 = mq135_read(MQ2_AO_PIN, MQ2_DO_PIN);
        mq135_log_reading(2, mq2);

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
