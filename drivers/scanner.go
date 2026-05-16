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

		ReadAllDHT22()
		CheckAllMQ()

		ledState = !ledState
		if ledState {
			machine.LED.High()
		} else {
			machine.LED.Low()
		}
		time.Sleep(5 * time.Second)
	}
}
