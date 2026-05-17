#include "mq135.hpp"
#include <esp_log.h>
#include <driver/adc.h>
#include <esp_adc/adc_oneshot.h>

static const char* TAG = "mq135";
static adc_oneshot_handle_t adc_handle = nullptr;

void mq135_init(void) {
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_handle));

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_11,
    };

    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_6, &config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_7, &config));

    ESP_LOGI(TAG, "MQ135 initialized");
}

MQ135Reading mq135_read(int ao_pin, int do_pin) {
    MQ135Reading reading = {0, AMAN};

    adc_channel_t channel = (ao_pin == 34) ? ADC_CHANNEL_6 : ADC_CHANNEL_7;

    uint32_t adc_sum = 0;
    for (int i = 0; i < 10; i++) {
        int raw = 0;
        ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, channel, &raw));
        adc_sum += raw;
    }

    reading.raw_adc = adc_sum / 10;

    reading.status = AMAN;

    return reading;
}

void mq135_log_reading(int sensor_num, const MQ135Reading& reading) {
    const char* status_str = (reading.status == AMAN) ? "AMAN" : "BAHAYA";
    ESP_LOGI(TAG, "MQ%d: raw=%u status=%s", sensor_num, reading.raw_adc, status_str);
}
