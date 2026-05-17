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

ESP32 firmware written in **Rust** — uses `esp-idf-svc` + `esp-idf-hal` for GPIO/peripheral access.

## Stack

- **Runtime**: Rust (esp-idf-svc, std enabled via ESP-IDF v5.5.3)
- **Target**: `xtensa-esp32-espidf` (`cargo run` via espflash runner)
- **Sensors**: 2× DHT22 (temp + humidity), 2× MQ-135 (air quality, ADC)
- **Comms**: WebSocket → backend `/ws/ingest` (tungstenite)
- **Device ID**: `airguard-node-01` (static, hardcoded in `src/config.rs`)
- **Concurrency**: async (embassy-executor + embassy-time)

## Flash Command

```bash
cargo run
```

Monitor serial:

```bash
espflash monitor --port /dev/ttyUSB0
```

## Pin Map

| Sensor   | Pin | GPIO | Purpose          |
|----------|-----|------|------------------|
| DHT1     | -   | 4    | Temperature 1    |
| DHT2     | -   | 15   | Temperature 2    |
| MQ1      | DO  | 19   | Air quality 1    |
| MQ1      | AO  | 34   | Analog read 1    |
| MQ2      | DO  | 21   | Air quality 2    |
| MQ2      | AO  | 35   | Analog read 2    |
| LED      | -   | 2    | Status indicator |

## Data Payload

Send JSON over WebSocket to backend `/ws/ingest`:

```json
{
  "device_id": "airguard-node-01",
  "temp1": 25.3,
  "hum1": 55.1,
  "temp2": 24.8,
  "hum2": 56.2,
  "mq1_value": 420,
  "mq2_value": 415
}
```

## MQ-135 Baseline

- No calibration file — baseline computed at runtime from first 30–60s of normal readings
- Baseline NOT saved to flash/EEPROM, resets on power cycle
- Status classification is relative to baseline: aman / waspada / bahaya
- 2 MQ-135 sensors run independent baselines

## Sensor Thresholds (backend enforces, hardware just reads raw)

| Sensor    | Aman                             | Waspada        | Bahaya       |
| --------- | -------------------------------- | -------------- | ------------ |
| Suhu (°C) | 20–26                            | 26–30          | >30          |
| Kelembapan | 40–60%                          | 30–39 / 61–70% | outside that |
| MQ-135    | relative baseline (runtime only) | -              | -            |

## Rust/ESP-IDF Constraints

- No `std::process::exit` — use `esp_idf_svc::sys::esp_restart()` for resets
- No dynamic linking — all deps statically linked via ESP-IDF build system
- Use `esp_idf_hal::gpio::PinDriver` for GPIO, `esp_idf_hal::adc::AdcDriver` for ADC
- LED = GPIO2 on ESP32 WROOM 32
- DHT22 reads require `critical_section::with()` to prevent 1-wire timing corruption
- ADC pins: GPIO34 = ADC1_CH6, GPIO35 = ADC1_CH7

## WiFi Setup

Configure SSID/password as constants in `src/config.rs` (no env vars on device):

```rust
pub const SSID: &str = "YOUR_SSID";
pub const PASSWORD: &str = "YOUR_PASSWORD";
pub const WS_URL: &str = "ws://YOUR_BACKEND_IP:PORT/ws/ingest";
```

## Project Structure

```txt
rust/airguard-hardware-rust/
├── src/
│   ├── main.rs           # async entry point, WiFi init, spawn scanner
│   ├── config.rs         # pin constants, WiFi/WS config
│   ├── drivers/
│   │   ├── mod.rs
│   │   ├── dht22.rs      # DHT22 reads + critical section
│   │   └── mq135.rs      # MQ-135 DO (digital) + AO (ADC)
│   ├── scanner.rs        # async 5s loop, LED toggle
│   ├── ws.rs             # tungstenite WebSocket client
│   └── log_buffer.rs     # circular buffer 256 entries
├── Cargo.toml
└── CLAUDE.md
```
