# AirGuard Hardware — C++ ESP-IDF Implementation

ESP32 firmware in C++ with DHT22 and MQ-135 sensors.

## Build & Flash

Set up ESP-IDF environment, then build:

```bash
source ~/.espressif/python_env/idf5.5/bin/activate  # or local ESP-IDF v5.5.3
export IDF_PATH=path/to/esp-idf
idf.py build
idf.py flash --port /dev/ttyUSB0
idf.py monitor --port /dev/ttyUSB0
```

## Configuration

Edit `main/config.hpp`:

```cpp
#define WIFI_SSID "YOUR_SSID"
#define WIFI_PASSWORD "YOUR_PASSWORD"
#define WS_URL "ws://YOUR_BACKEND_IP:PORT/ws/ingest"
```

Edit `sdkconfig.defaults` for WiFi settings if needed.

## Project Structure

- `CMakeLists.txt` — ESP-IDF root config
- `main/CMakeLists.txt` — component build config
- `main/main.cpp` — entry point (app_main)
- `main/config.hpp` — pin constants & WiFi/WS config
- `main/log.hpp/cpp` — ESP_LOGI/ESP_LOGE wrappers
- `sdkconfig.defaults` — WiFi + WebSocket build flags

## Pins

| Sensor   | Pin  | Purpose          |
|----------|------|------------------|
| DHT1     | 4    | Temperature 1    |
| DHT2     | 15   | Temperature 2    |
| MQ1 DO   | 19   | Air quality 1    |
| MQ1 AO   | 34   | Analog read 1    |
| MQ2 DO   | 21   | Air quality 2    |
| MQ2 AO   | 35   | Analog read 2    |
| LED      | 2    | Status indicator |
