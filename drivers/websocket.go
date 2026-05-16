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

	lib.LogOK("WebSocket", fmt.Sprintf("Data: %s", string(data)))
}
