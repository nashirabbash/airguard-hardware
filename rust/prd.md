# PRD: Rewrite AirGuard Hardware Firmware from TinyGo to Rust

## Problem Statement

The current AirGuard firmware is written in TinyGo targeting the ESP32 WROOM-32. TinyGo has no native WiFi driver for the ESP32 WROOM-32's built-in radio — the only TinyGo WiFi library (`espradio`) only supports ESP32-C3 and ESP32-S3 variants. This means the device can read sensors and log to serial, but cannot transmit data to the backend over WiFi — which is the core requirement of the system. The firmware is essentially stuck as a local serial logger with no networking capability on the target hardware.

## Solution

Rewrite the firmware in Rust using the `esp-idf-hal` + `esp-idf-svc` stack (std approach). This gives the ESP32 WROOM-32 full access to its built-in WiFi via the ESP-IDF native driver, full `std::net` support, and a mature WebSocket client. The sensor reading logic (DHT22, MQ-135), scan loop, LED blink, and serial logging are all preserved. WiFi connect and WebSocket send are added on top.

## User Stories

1. As a firmware developer, I want the build system to target ESP32 WROOM-32 natively, so that I can flash and monitor without special workarounds.
2. As a firmware developer, I want DHT22 temperature and humidity reading to work correctly, so that accurate environmental data is collected.
3. As a firmware developer, I want MQ-135 air quality reading via ADC to work correctly, so that pollution levels are measured per scan cycle.
4. As a firmware developer, I want a scan loop that reads all sensors every 5 seconds, so that data is collected at a consistent interval.
5. As a firmware developer, I want the ESP32 built-in LED to blink on each scan cycle, so that I can visually confirm the device is alive without serial.
6. As a firmware developer, I want sensor readings logged to serial in `[OK]/[FAIL]` format, so that I can debug without a backend connection.
7. As a firmware developer, I want WiFi to connect on boot using hardcoded SSID and password, so that the device joins the local network automatically.
8. As a firmware developer, I want the device to reconnect WiFi automatically on drop, so that temporary network outages do not halt data collection.
9. As a firmware developer, I want each scan cycle to send a JSON payload to the backend WebSocket endpoint, so that sensor data reaches the server in real time.
10. As a firmware developer, I want the WebSocket send to be non-blocking on failure, so that a backend outage does not halt the sensor loop.
11. As a firmware developer, I want the JSON payload to include device_id, temperature, humidity, and mq135_value fields, so that the backend can identify and process the data correctly.
12. As a firmware developer, I want temperature to be the average of two DHT22 sensors, so that readings are more robust against single-sensor noise.
13. As a firmware developer, I want humidity to be the average of two DHT22 sensors, for the same reason.
14. As a firmware developer, I want failed sensor reads to be logged as `[FAIL]` with a label and reason, so that I can distinguish sensor faults from network faults.
15. As a firmware developer, I want failed WebSocket sends to be logged as `[FAIL]` without crashing the loop, so that the device keeps running even if the backend is down.
16. As a firmware developer, I want `cargo espflash` to flash the device, so that the deployment workflow is consistent with the Rust ESP32 ecosystem.
17. As a firmware developer, I want `cargo build` to complete without warnings, so that the codebase is clean and maintainable.
18. As a backend developer, I want the payload schema to remain identical to the current TinyGo implementation, so that the backend `/ws/ingest` handler needs no changes.

## Implementation Decisions

- **Runtime**: `esp-idf-hal` + `esp-idf-svc` (std approach). Chosen over `esp-hal` (no_std) because WROOM-32 WiFi is only supported via ESP-IDF; no_std `esp-wifi` does not support original ESP32.
- **Target board**: ESP32 WROOM-32. Cargo target: `xtensa-esp32-espidf`.
- **Toolchain**: Install via `espup`. Requires Xtensa Rust fork (`esp` channel).
- **Flash tool**: `cargo-espflash`.
- **DHT22 driver**: `dht-sensor` crate (no_std compatible, works under esp-idf-hal GPIO).
- **MQ-135 driver**: No dedicated crate. Read via `esp-idf-hal` ADC directly. Digital output pin read via GPIO.
- **WiFi**: `esp-idf-svc::wifi::EspWifi`. SSID and password as compile-time constants (no env vars on device).
- **WebSocket client**: `tungstenite` crate over `std::net::TcpStream`. ESP-IDF exposes full std net so tungstenite works without modification.
- **JSON serialization**: `serde` + `serde_json`.
- **Logging**: `esp-println` for serial output. `[OK]` / `[FAIL]` prefix format preserved from TinyGo implementation.
- **Payload schema**: `{ "device_id": "airguard-node-01", "temperature": f32, "humidity": f32, "mq135_value": u16 }` — identical to current TinyGo schema.
- **Scan interval**: 5 seconds, same as TinyGo.
- **Module structure**:
  - **wifi** — init, connect, reconnect logic
  - **sensors/dht22** — read two DHT22 sensors, return averaged temp+humidity
  - **sensors/mq135** — read MQ-135 DO/AO pins, return air quality value
  - **websocket** — build payload, connect, send, handle errors
  - **scan\_loop** — orchestrate sensors + send + LED blink every 5s
  - **logger** — `log_ok` / `log_fail` helpers (serial output)

## Testing Decisions

A good test encodes *why* behavior matters, not just what it does. Tests should call the module's public interface and assert on observable outputs — not internal state or implementation details. Avoid mocking hardware in unit tests; prefer integration tests that run on real or emulated hardware when behavior depends on timing or peripheral state.

**Modules to test:**
- **logger**: `log_ok` and `log_fail` produce correct `[OK]` / `[FAIL]` prefixed strings. Pure functions, no hardware dependency — easy unit test.
- **websocket payload builder**: JSON serialization produces the correct schema with correct field names and averaged sensor values. Pure function, no network dependency.
- **sensors/dht22 averaging**: Given two raw readings, averaged output is correct. Unit testable if reading is separated from GPIO acquisition.
- **scan\_loop timing**: Integration test on device — confirm LED blinks and serial output appears every 5 seconds.

Prior art: the TinyGo project had `wifi_test.go` testing config non-emptiness and state transitions. The Rust tests should go further — test the payload builder logic, not just that constants are non-empty.

## Out of Scope

- OTA (over-the-air) firmware updates.
- Flash/EEPROM log storage (the TinyGo `LogToFlash` / `DumpLogs` system is not carried over).
- MQ-135 baseline calibration or threshold classification — backend handles thresholds.
- TLS/WSS (secure WebSocket) — plain `ws://` only, same as current.
- Multiple backend endpoints or load balancing.
- Configuration via web UI, BLE, or any runtime mechanism — all config stays as compile-time constants.
- Async runtime (Embassy) — synchronous loop is sufficient for this scan rate.

## Further Notes

- The ESP-IDF std approach requires the `IDF_PATH` env var and a one-time `idf_component install` step. Document in a `BUILDING.md`.
- `tungstenite` on ESP32 may need `native-tls` feature disabled; use `tungstenite` with default features pointing to the std TCP stream directly.
- `dht-sensor` requires careful timing — DHT22 protocol is timing-sensitive. If the crate does not work reliably under ESP-IDF scheduler, a raw GPIO bit-bang with `esp_rom_delay_us` may be needed.
- WiFi credentials stay hardcoded as constants. No `.env` file — the device has no filesystem by default.
- The `device_id` `"airguard-node-01"` stays static. If multiple nodes are deployed, this becomes a compile-time constant per device.
