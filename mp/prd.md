# PRD: Rewrite AirGuard Hardware Firmware from TinyGo to MicroPython

## Problem Statement

The current AirGuard firmware is written in TinyGo targeting the ESP32 WROOM-32. TinyGo has no native WiFi driver for this chip — the only available WiFi library (`espradio`) only supports ESP32-C3 and ESP32-S3 variants. The device can read sensors and log to serial, but cannot transmit data to the backend over WiFi, which is the core requirement of the system.

## Solution

Rewrite the firmware in MicroPython. MicroPython ships with a native `network.WLAN()` driver for the ESP32 WROOM-32 that works out of the box — no external library, no precompiled blobs, no toolchain configuration required. The `dht` module (DHT22), `machine.ADC` (MQ-135), and `ujson` are all built-in. WebSocket send is handled by the `uwebsockets` third-party library installable via `mip`. The sensor reading logic, scan loop, LED blink, and serial logging from the TinyGo firmware are all preserved. WiFi connect and WebSocket send are added on top.

## User Stories

1. As a firmware developer, I want the build system to target ESP32 WROOM-32 natively with no cross-compilation step, so that I can iterate and flash quickly without a heavy toolchain.
2. As a firmware developer, I want to flash MicroPython firmware via `esptool`, so that device setup follows the standard MicroPython ESP32 workflow.
3. As a firmware developer, I want to deploy application files via `mpremote`, so that code changes reach the device without reflashing the entire firmware.
4. As a firmware developer, I want DHT22 temperature and humidity reading to work correctly using the built-in `dht` module, so that accurate environmental data is collected.
5. As a firmware developer, I want two DHT22 sensors to be read per scan cycle, so that redundancy is built into the hardware layer.
6. As a firmware developer, I want temperature to be the average of two DHT22 sensor readings, so that readings are more robust against single-sensor noise.
7. As a firmware developer, I want humidity to be the average of two DHT22 sensor readings, for the same reason.
8. As a firmware developer, I want failed DHT22 reads to be logged as `[FAIL]` with a label and reason, so that I can distinguish sensor faults from network faults during debugging.
9. As a firmware developer, I want MQ-135 air quality reading via `machine.ADC` to work correctly, so that pollution levels are measured per scan cycle.
10. As a firmware developer, I want the MQ-135 digital output pin to also be read via `machine.Pin`, so that a threshold-exceeded signal is available alongside the raw ADC value.
11. As a firmware developer, I want a scan loop that reads all sensors every 5 seconds, so that data is collected at a consistent interval matching the existing backend expectations.
12. As a firmware developer, I want the ESP32 built-in LED to blink on each scan cycle, so that I can visually confirm the device is alive without a serial connection.
13. As a firmware developer, I want sensor readings logged to serial in `[OK]/[FAIL]` format, so that I can debug without a backend connection.
14. As a firmware developer, I want WiFi to connect on boot using hardcoded SSID and password, so that the device joins the local network automatically without any runtime configuration mechanism.
15. As a firmware developer, I want the device to reconnect WiFi automatically when the connection drops, so that temporary network outages do not halt data collection.
16. As a firmware developer, I want each scan cycle to send a JSON payload to the backend WebSocket endpoint, so that sensor data reaches the server in real time.
17. As a firmware developer, I want the WebSocket send to be non-blocking on failure, so that a backend outage does not halt the sensor loop.
18. As a firmware developer, I want failed WebSocket sends to be logged as `[FAIL]` without crashing the loop, so that the device keeps running even if the backend is down.
19. As a firmware developer, I want the JSON payload to include `device_id`, `temperature`, `humidity`, and `mq135_value` fields, so that the backend can identify and process the data correctly without schema changes.
20. As a firmware developer, I want `device_id` to be a hardcoded constant `"airguard-node-01"`, so that the device is identifiable without a runtime configuration mechanism.
21. As a firmware developer, I want third-party packages installed via `mip`, so that the dependency workflow is consistent with the MicroPython ecosystem.
22. As a backend developer, I want the payload schema to remain identical to the current TinyGo implementation, so that the backend `/ws/ingest` handler needs no changes.

## Implementation Decisions

- **Runtime**: MicroPython (ESP32 port). Chosen over TinyGo because MicroPython ships with native `network.WLAN()` for ESP32 WROOM-32; no external WiFi driver needed.
- **Target board**: ESP32 WROOM-32. Flash MicroPython generic ESP32 firmware via `esptool`.
- **Flash tool**: `esptool.py erase_flash` + `esptool.py write_flash` for MicroPython firmware. `mpremote cp` for application files.
- **DHT22 driver**: Built-in `dht.DHT22(machine.Pin(n))`. No third-party dependency.
- **MQ-135 driver**: `machine.ADC(machine.Pin(n))` for analog read. `machine.Pin(n, machine.Pin.IN)` for digital output.
- **WiFi**: `network.WLAN(network.STA_IF)`. SSID and password as module-level constants. Reconnect by checking `wlan.isconnected()` before each send.
- **WebSocket client**: `uwebsockets` installed via `mip.install("uwebsockets")`. Connects to `ws://` endpoint; no TLS required.
- **JSON serialization**: `ujson.dumps()`. Built-in, no dependency.
- **Logging**: `print()` to serial. `[OK]` / `[FAIL]` prefix format preserved from TinyGo implementation.
- **Payload schema**: `{"device_id": "airguard-node-01", "temperature": float, "humidity": float, "mq135_value": int}` — identical to current schema.
- **Scan interval**: 5 seconds via `time.sleep(5)`.
- **Module structure**:
  - **wifi** — init, connect, reconnect logic
  - **sensors/dht22** — read two DHT22 sensors, return averaged temp+humidity
  - **sensors/mq135** — read MQ-135 ADC + DO pins, return air quality value
  - **websocket** — build payload dict, connect, send, handle errors
  - **scanner** — orchestrate sensors + send + LED blink every 5s
  - **logger** — `log_ok(label)` / `log_fail(label, reason)` helpers
  - **main** — entry point: init pins, connect WiFi, start scan loop
- **Config**: Single `config.py` with SSID, password, WS URL, device ID, pin numbers as module-level constants. Imported by all modules that need config.

## Testing Decisions

A good test encodes *why* behavior matters, not just what it does. Tests call the module's public interface and assert on observable outputs — not internal state or implementation details. Avoid mocking hardware in unit tests; prefer integration tests that run on real or emulated hardware when behavior depends on timing or peripheral state.

**Modules to test:**

- **logger**: `log_ok` and `log_fail` produce correct `[OK]` / `[FAIL]` prefixed strings. Pure functions, no hardware dependency — easy unit test runnable on desktop CPython.
- **websocket payload builder**: `ujson.dumps()` of the payload dict produces correct schema with correct field names, types, and averaged sensor values. Pure function, no network dependency — unit testable on desktop CPython.
- **sensors/dht22 averaging**: Given two raw `(temp, humidity)` tuples, averaged output is numerically correct. Unit testable if averaging is extracted from the GPIO acquisition path.
- **wifi reconnect logic**: Given a mock `wlan` object that reports `isconnected() == False`, the reconnect path is triggered. Testable by dependency-injecting the wlan object.
- **scan_loop timing**: Integration test on device — confirm LED blinks and serial output appears every 5 seconds.

Prior art: the TinyGo project had `wifi_test.go` testing config non-emptiness and state transitions. The MicroPython tests should go further — test the payload builder logic and averaging math, not just that constants are non-empty.

## Out of Scope

- OTA (over-the-air) firmware updates.
- Flash/EEPROM log storage.
- MQ-135 baseline calibration or threshold classification — backend handles thresholds.
- TLS/WSS (secure WebSocket) — plain `ws://` only, same as current.
- Multiple backend endpoints or load balancing.
- Configuration via web UI, BLE, or any runtime mechanism — all config stays as compile-time constants in `config.py`.
- Async runtime (`asyncio`) — synchronous loop is sufficient for this scan rate.
- Filesystem persistence — no files written to flash at runtime.

## Further Notes

- MicroPython must be flashed to the device before deploying application files. Use official ESP32 generic build from micropython.org.
- `uwebsockets` must be installed to the device via `mip.install("uwebsockets")` once after MicroPython is flashed. Requires WiFi to be connected on the device during install.
- If `uwebsockets` proves unstable, fall back to plain HTTP POST via `urequests` — simpler, but requires a REST endpoint on the backend instead of WebSocket.
- `dht.DHT22` requires a pull-up resistor on the data line (4.7kΩ). If readings fail intermittently, check hardware before blaming the driver.
- MicroPython's `machine.ADC` on ESP32 has non-linear response and limited range (0–3.3V, 12-bit). Raw values are sent to backend without calibration — backend handles interpretation.
- WiFi credentials stay hardcoded in `config.py`. No `.env` file — MicroPython has a filesystem but env var convention does not apply to embedded targets.
- `device_id` `"airguard-node-01"` stays static. If multiple nodes are deployed, `config.py` becomes the per-device config file.
- Document flash + deploy workflow in `BUILDING.md`.
