#include "wifiManager.h"
#include "logManagement.h"
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <time.h>

static const char* SSID        = "vivo V30e";
static const char* PASSWORD    = "1sampai8";
static const char* WS_HOST     = "10.140.237.1";
static const uint16_t WS_PORT  = 3000;
static const char* WS_PATH     = "/ws/ingest";

// Fill these after registering device via POST /api/device
static const char* DEVICE_ID    = "device-001";
static const char* DEVICE_TOKEN = "device-001";  // plaintext; backend hashes this to compare

static WebSocketsClient wsClient;
static bool wsConnected = false;

static void onWSEvent(WStype_t type, uint8_t* payload, size_t length) {
    switch (type) {
        case WStype_CONNECTED:
            wsConnected = true;
            logOK("WebSocket", "connected to backend");
            break;
        case WStype_DISCONNECTED:
            wsConnected = false;
            logFail("WebSocket", "disconnected from backend");
            break;
        case WStype_ERROR:
            logFail("WebSocket", "error");
            break;
        case WStype_TEXT:
            Serial.printf("[WS←]  %.*s\n", (int)length, payload);
            break;
        default:
            break;
    }
}

void initWiFi() {
    WiFi.begin(SSID, PASSWORD);
    Serial.printf("Connecting to %s", SSID);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.printf("\nWiFi connected. IP: %s\n", WiFi.localIP().toString().c_str());

    // sync time via NTP for proper ISO timestamps
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    Serial.print("NTP sync");
    time_t now = 0;
    struct tm ti = {};
    int retry = 0;
    while (!getLocalTime(&ti) && retry++ < 20) {
        delay(500);
        Serial.print(".");
    }
    Serial.println(retry < 20 ? " OK" : " FAILED (timestamps may be wrong)");
}

bool isWiFiConnected() {
    return WiFi.status() == WL_CONNECTED;
}

void connectWS() {
    wsClient.begin(WS_HOST, WS_PORT, WS_PATH);
    wsClient.onEvent(onWSEvent);
    wsClient.setReconnectInterval(5000);
    Serial.printf("WS connecting → ws://%s:%d%s\n", WS_HOST, WS_PORT, WS_PATH);
}

void loopWS() {
    wsClient.loop();
}

bool isWSConnected() {
    return wsConnected;
}

void sendSensorData(float temp1, float hum1, float temp2, float hum2, int mq1, int mq2) {
    if (!wsConnected) {
        logFail("WS Transfer", "not connected — data not sent");
        return;
    }

    // backend expects single temperature/humidity — average both sensors
    float temperature = (temp1 + temp2) / 2.0f;
    float humidity    = (hum1  + hum2)  / 2.0f;
    int   mq135_value = (mq1 > mq2) ? mq1 : mq2;  // worst reading

    // ISO 8601 timestamp from NTP-synced clock
    struct tm ti = {};
    char timestamp[32] = "1970-01-01T00:00:00Z";
    if (getLocalTime(&ti)) {
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%SZ", &ti);
    }

    char payload[384];
    snprintf(payload, sizeof(payload),
        "{\"type\":\"sensor_reading\","
        "\"device_id\":\"%s\","
        "\"token\":\"%s\","
        "\"timestamp\":\"%s\","
        "\"temperature\":%.1f,"
        "\"humidity\":%.1f,"
        "\"mq135_value\":%d}",
        DEVICE_ID, DEVICE_TOKEN, timestamp, temperature, humidity, mq135_value);

    bool ok = wsClient.sendTXT(payload);
    if (ok) {
        char detail[64];
        snprintf(detail, sizeof(detail), "temp=%.1f hum=%.1f mq=%d", temperature, humidity, mq135_value);
        logOK("WS Transfer", detail);
    } else {
        logFail("WS Transfer", "send failed");
    }
}

void sendLogDump() {
    if (!wsConnected) return;

    const size_t LOGS_SIZE = 13000;
    char* logsBuf = (char*)malloc(LOGS_SIZE);
    if (!logsBuf) { Serial.println("[LogDump] malloc failed"); return; }

    int count = buildLogPayload(logsBuf, LOGS_SIZE, 50);

    const size_t JSON_SIZE = LOGS_SIZE + 128;
    char* payload = (char*)malloc(JSON_SIZE);
    if (!payload) { free(logsBuf); return; }

    snprintf(payload, JSON_SIZE,
        "{\"type\":\"log_dump\",\"device_id\":\"%s\",\"count\":%d,\"logs\":\"%s\"}",
        DEVICE_ID, count, logsBuf);

    free(logsBuf);
    wsClient.sendTXT(payload);
    free(payload);
    Serial.printf("[LogDump] sent %d entries\n", count);
}
