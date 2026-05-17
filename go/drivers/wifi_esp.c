// WiFi not supported on TinyGo ESP32 targets (no ESP-IDF runtime).
// Implement in Rust project using esp-idf-svc instead.

void airguard_wifi_init(const char *ssid, const char *pass) {
    (void)ssid;
    (void)pass;
}

int airguard_wifi_connected(void) {
    return 0;
}
