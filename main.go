package main

import (
	"fmt"
	"machine"
	"time"

	"airguard/config"
	"airguard/drivers"
)


func main() {
	config.InitPins()
	drivers.InitDHT22()

	fmt.Println("=== AirGuard Sensor Check ===")
	fmt.Println("Waiting 3s for sensor stabilize...")
	time.Sleep(3 * time.Second)

	ledState := false
	for {
		fmt.Println("--- Scan ---")

		drivers.ReadDHT22()

		drivers.CheckMQ135("MQ135 #1", config.MQ1DO, config.MQ1AO)
		drivers.CheckMQ135("MQ135 #2", config.MQ2DO, config.MQ2AO)

		ledState = !ledState
		if ledState {
			machine.LED.High()
		} else {
			machine.LED.Low()
		}
		time.Sleep(5 * time.Second)
	}
}
