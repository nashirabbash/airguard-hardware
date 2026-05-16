# CLAUDE.md

use rtk while every use command
These rules apply to every task in this project unless explicitly overridden.
Bias: caution over speed on non-trivial work. Use judgment on trivial tasks.

## Rule 1 — Think Before Coding

State assumptions explicitly. If uncertain, ask rather than guess.
Present multiple interpretations when ambiguity exists.
Push back when a simpler approach exists.
Stop when confused. Name what's unclear.

## Rule 2 — Simplicity First

Minimum code that solves the problem. Nothing speculative.
No features beyond what was asked. No abstractions for single-use code.
Test: would a senior engineer say this is overcomplicated? If yes, simplify.

## Rule 3 — Surgical Changes

Touch only what you must. Clean up only your own mess.
Don't "improve" adjacent code, comments, or formatting.
Don't refactor what isn't broken. Match existing style.

## Rule 4 — Goal-Driven Execution

Define success criteria. Loop until verified.
Don't follow steps. Define success and iterate.
Strong success criteria let you loop independently.

## Rule 5 — Use the model only for judgment calls

Use me for: classification, drafting, summarization, extraction.
Do NOT use me for: routing, retries, deterministic transforms.
If code can answer, code answers.

## Rule 6 — Token budgets are not advisory

Per-task: 4,000 tokens. Per-session: 30,000 tokens.
If approaching budget, summarize and start fresh.
Surface the breach. Do not silently overrun.

## Rule 7 — Surface conflicts, don't average them

If two patterns contradict, pick one (more recent / more tested).
Explain why. Flag the other for cleanup.
Don't blend conflicting patterns.

## Rule 8 — Read before you write

Before adding code, read exports, immediate callers, shared utilities.
"Looks orthogonal" is dangerous. If unsure why code is structured a way, ask.

## Rule 9 — Tests verify intent, not just behavior

Tests must encode WHY behavior matters, not just WHAT it does.
A test that can't fail when business logic changes is wrong.

## Rule 10 — Checkpoint after every significant step

Summarize what was done, what's verified, what's left.
Don't continue from a state you can't describe back.
If you lose track, stop and restate.

## Rule 11 — Match the codebase's conventions, even if you disagree

Conformance > taste inside the codebase.
If you genuinely think a convention is harmful, surface it. Don't fork silently.

## Rule 12 — Fail loud

"Completed" is wrong if anything was skipped silently.
"Tests pass" is wrong if any were skipped.
Default to surfacing uncertainty, not hiding it.

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

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

| Sensor     | Aman                             | Waspada        | Bahaya       |
| ---------- | -------------------------------- | -------------- | ------------ |
| Suhu (°C)  | 20–26                            | 26–30          | >30          |
| Kelembapan | 40–60%                           | 30–39 / 61–70% | outside that |
| MQ-135     | relative baseline (runtime only) |

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
