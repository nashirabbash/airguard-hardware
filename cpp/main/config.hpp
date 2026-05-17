#pragma once

// Pin constants
#define DHT1_PIN 4
#define DHT2_PIN 15
#define MQ1_AO_PIN 34
#define MQ2_AO_PIN 35
#define MQ1_DO_PIN 19
#define MQ2_DO_PIN 21
#define LED_PIN 2

// WiFi credentials
#define WIFI_SSID "YOUR_SSID"
#define WIFI_PASSWORD "YOUR_PASSWORD"

// WebSocket config
#define WS_URL "ws://YOUR_BACKEND_IP:PORT/ws/ingest"
#define DEVICE_ID "airguard-node-01"

// Device configuration
#define WS_RECONNECT_INTERVAL_MS 5000
#define SENSOR_READ_INTERVAL_MS 5000
