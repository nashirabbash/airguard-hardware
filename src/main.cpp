#include <Arduino.h>
#include "pins.h"
#include "dht22Sensor.h"
#include "wifiManager.h"
#include "logManagement.h"
#include "sensorTask.h"

static bool     ledState     = false;
static uint32_t lastSendMs   = 0;
static uint32_t lastLedMs    = 0;
static uint32_t lastLogDumpMs = 0;
static const uint32_t SEND_INTERVAL     = 5000;
static const uint32_t LED_INTERVAL      = 1000;
static const uint32_t LOG_DUMP_INTERVAL = 60000;

void initPins() {
    pinMode(LED_PIN, OUTPUT);
    pinMode(DHT1_PIN, INPUT_PULLUP);
    delay(100);
    pinMode(DHT2_PIN, INPUT_PULLUP);
    delay(100);
}

void setup() {
    Serial.begin(115200);
    initPins();
    initDHT22();
    initWiFi();
    connectWS();
    startSensorTask();  // sensors run async on Core 1

    Serial.println("=== AirGuard Sensor Check ===");
    Serial.println("Waiting 3s for sensor stabilize...");
    delay(3000);
}

void loop() {
    loopWS();  // keep WS alive, non-blocking

    uint32_t now = millis();

    // send sensor data every SEND_INTERVAL ms
    if (now - lastSendMs >= SEND_INTERVAL) {
        lastSendMs = now;
        SensorReading r;
        if (getSensorReading(r)) {
            sendSensorData(r.temp1, r.hum1, r.temp2, r.hum2, r.mq1, r.mq2);
        } else {
            logFail("SensorTask", "no data ready yet");
        }
    }

    // send log dump every 60s
    if (now - lastLogDumpMs >= LOG_DUMP_INTERVAL) {
        lastLogDumpMs = now;
        sendLogDump();
    }

    // blink LED independently
    if (now - lastLedMs >= LED_INTERVAL) {
        lastLedMs = now;
        ledState  = !ledState;
        digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    }
}