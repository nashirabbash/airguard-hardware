package drivers

import (
	"encoding/json"
	"fmt"

	"airguard/lib"
)

const deviceID = "airguard-node-01"

type SensorPayload struct {
	DeviceID    string  `json:"device_id"`
	Temperature float32 `json:"temperature"`
	Humidity    float32 `json:"humidity"`
	MQ135Value  uint16  `json:"mq135_value"`
}

func SendSensorData(temp1, hum1, temp2, hum2 float32, mq135 uint16) {
	if !IsWiFiConnected() {
		lib.LogFail("WebSocket", "WiFi not connected")
		return
	}

	payload := SensorPayload{
		DeviceID:    deviceID,
		Temperature: (temp1 + temp2) / 2,
		Humidity:    (hum1 + hum2) / 2,
		MQ135Value:  mq135,
	}

	data, err := json.Marshal(payload)
	if err != nil {
		lib.LogFail("WebSocket", "JSON marshal failed")
		return
	}

	err = sendWebSocket(data)
	if err != nil {
		lib.LogFail("WebSocket", fmt.Sprintf("send failed: %v", err))
		return
	}

	lib.LogOK("WebSocket", "sent")
}

func sendWebSocket(payload []byte) error {
	// TODO: Implement actual WebSocket client when platform-specific networking libs available
	// For now, stub returns success. Scanner loop continues even if send fails (non-blocking).
	// When WiFi backend is ready, implement:
	// 1. TCP connection to backend (requires tinygo.org/x/drivers or platform SDK)
	// 2. WebSocket handshake (HTTP upgrade)
	// 3. Frame marshaling and transmission
	return nil
}
