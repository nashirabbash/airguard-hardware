package drivers

import (
	"fmt"
	"machine"
	"time"
)

func RunScanLoop() {
	fmt.Println("=== AirGuard Sensor Check ===")
	fmt.Println("Waiting 3s for sensor stabilize...")
	time.Sleep(3 * time.Second)

	ledState := false
	for {
		fmt.Println("--- Scan ---")

		temp1, hum1, temp2, hum2 := ReadAllDHT22()
		mq135 := CheckAllMQ()

		SendSensorData(temp1, hum1, temp2, hum2, mq135)

		ledState = !ledState
		if ledState {
			machine.LED.High()
		} else {
			machine.LED.Low()
		}
		time.Sleep(5 * time.Second)
	}
}
