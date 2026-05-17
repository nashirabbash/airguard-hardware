#include "log.hpp"
#include <esp_log.h>

static const char* TAG = "airguard";

void log_ok(const char* label, const char* detail) {
    ESP_LOGI(TAG, "[OK] %s: %s", label, detail);
}

void log_fail(const char* label, const char* detail) {
    ESP_LOGE(TAG, "[FAIL] %s: %s", label, detail);
}
