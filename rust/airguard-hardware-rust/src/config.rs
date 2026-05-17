// GPIO Pin Configuration
pub const DHT1_PIN: u32 = 4;
pub const DHT2_PIN: u32 = 15;
pub const MQ1_DO_PIN: u32 = 19;
pub const MQ1_AO_PIN: u32 = 34;
pub const MQ2_DO_PIN: u32 = 21;
pub const MQ2_AO_PIN: u32 = 35;
pub const LED_PIN: u32 = 2;

// WiFi Configuration
pub const SSID: &str = "vivo V30e";
pub const PASSWORD: &str = "1sampai8";
pub const WS_URL: &str = "ws://localhost:3000/ws/ingest";

// Device Configuration
pub const DEVICE_ID: &str = "airguard-node-01";
pub const SCAN_INTERVAL_MS: u64 = 5000;
pub const WS_RETRY_MAX: u32 = 5;
