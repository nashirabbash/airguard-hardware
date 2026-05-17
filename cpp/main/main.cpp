#include <esp_log.h>
#include "log.hpp"

static const char* TAG = "airguard";

extern "C" void app_main(void) {
    ESP_LOGI(TAG, "=== AirGuard Hardware (C++) ===");
    log_ok("boot", "firmware started");

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
