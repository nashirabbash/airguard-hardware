#include "sensorTask.h"
#include "pins.h"
#include "dht22Sensor.h"
#include "mq135Sensor.h"
#include "logManagement.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

static SensorReading  sharedData  = {};
static SemaphoreHandle_t dataMutex = nullptr;

static void sensorTaskFn(void* /*param*/) {
    for (;;) {
        float temp1, hum1, temp2, hum2;
        readAllDHT22(temp1, hum1, temp2, hum2);  // 2s delay inside — yields to scheduler
        int mq1 = checkMQ135("MQ135 #1", MQ1_DO, MQ1_AO);
        int mq2 = checkMQ135("MQ135 #2", MQ2_DO, MQ2_AO);

        if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            sharedData = {temp1, hum1, temp2, hum2, mq1, mq2, true};
            xSemaphoreGive(dataMutex);
        }

        logOK("SensorTask", "cycle done");
        vTaskDelay(pdMS_TO_TICKS(3000));  // rest before next cycle
    }
}

void startSensorTask() {
    dataMutex = xSemaphoreCreateMutex();
    xTaskCreatePinnedToCore(
        sensorTaskFn,
        "SensorTask",
        4096,
        nullptr,
        1,          // priority
        nullptr,
        1           // Core 1
    );
}

bool getSensorReading(SensorReading& out) {
    if (!dataMutex) return false;
    if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(50)) != pdTRUE) return false;
    out = sharedData;
    xSemaphoreGive(dataMutex);
    return out.ready;
}
