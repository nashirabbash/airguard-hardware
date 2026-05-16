# AirGuard Hardware

ESP32 firmware written in **TinyGo** — not standard Go. Use `machine` package for GPIO/peripheral access.

## Stack

- **Runtime**: TinyGo (not `go run`, not standard Go toolchain)
- **Target**: ESP32 (`tinygo flash -target=esp32 .`)
- **Sensors**: DHT22 (temp + humidity), MQ-135 (air quality)
- **Comms**: WebSocket → backend `/ws/ingest`
- **Device ID**: `airguard-node-01` (static, hardcoded)

## Flash Command

```bash
tinygo flash -target=esp32 -port=/dev/ttyUSB0 .
```

Monitor serial:
```bash
tinygo monitor -port=/dev/ttyUSB0
```

## Data Payload

Send JSON over WebSocket to backend `/ws/ingest`:

```json
{
  "device_id": "airguard-node-01",
  "temperature": 25.3,
  "humidity": 55.1,
  "mq135_value": 420
}
```

## MQ-135 Baseline

- No calibration file — baseline computed at runtime from first 30–60s of normal readings
- Baseline NOT saved to flash/EEPROM, resets on power cycle
- Status classification is relative to baseline: aman / waspada / bahaya

## Sensor Thresholds (backend enforces, hardware just reads raw)

| Sensor      | Aman       | Waspada          | Bahaya       |
|-------------|------------|------------------|--------------|
| Suhu (°C)   | 20–26      | 26–30            | >30          |
| Kelembapan  | 40–60%     | 30–39 / 61–70%   | outside that |
| MQ-135      | relative baseline (runtime only)               |

## TinyGo Constraints

- No goroutine scheduler guarantees — keep concurrency simple
- No `net/http` — use `tinygo.org/x/drivers` or raw TCP/WebSocket
- No reflection, no `fmt.Sprintf` complex formatting in tight loops
- `machine.LED` = built-in LED pin on ESP32

## WiFi Setup

Configure SSID/password as constants in `main.go` (no env vars on device):

```go
const (
    ssid     = "YOUR_SSID"
    password = "YOUR_PASSWORD"
    wsURL    = "ws://YOUR_BACKEND_IP:PORT/ws/ingest"
)
```

## Project Structure

```
AirGuard Hardware/
├── main.go       # entry point, sensor loop, WS send
├── go.mod        # module: airguard, go 1.22.2
└── CLAUDE.md
```
