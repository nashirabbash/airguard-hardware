package drivers

import (
	"airguard/lib"
)

// WiFi configuration
const (
	ssid       = "YOUR_SSID"
	password   = "YOUR_PASSWORD"
	backendURL = "ws://YOUR_BACKEND_IP:PORT/ws/ingest"
)

var wifiConnected = false

func InitWiFi() {
	lib.LogFail("WiFi", "Configure SSID/password in drivers/wifi.go")
	lib.LogFail("WiFi", "Full WiFi support pending")
	// Full WiFi implementation requires platform-specific WiFi libraries
	// For now, data is prepared for sending when WiFi is available
	wifiConnected = false
}

func IsWiFiConnected() bool {
	return wifiConnected
}
