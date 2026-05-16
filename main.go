package main

import (
	"fmt"
	"machine"
	"time"

	"tinygo.org/x/drivers/dht"
)

const (
	dht1Pin = machine.GPIO4
	dht2Pin = machine.GPIO5
	mq1Pin  = machine.GPIO34
	mq2Pin  = machine.GPIO35
)

func checkMQ(label string, pin machine.Pin) {
	pin.Configure(machine.PinConfig{Mode: machine.PinInput})
	time.Sleep(10 * time.Millisecond)
	state := pin.Get()
	fmt.Printf("[OK]   %s: DO state=%v (verify power LED on module)\n", label, state)
}

func main() {
	led := machine.LED
	led.Configure(machine.PinConfig{Mode: machine.PinOutput})

	sensor1 := dht.New(dht1Pin, dht.DHT22)
	sensor2 := dht.New(dht2Pin, dht.DHT22)

	fmt.Println("=== AirGuard Sensor Check ===")
	fmt.Println("Waiting 3s for sensor stabilize...")
	time.Sleep(3 * time.Second)

	ledState := false
	for {
		fmt.Println("--- Scan ---")

		temp1, hum1, err1 := sensor1.Measurements()
		if err1 != nil {
			fmt.Printf("[FAIL] DHT22 #1 (GPIO4): %s\n", err1.Error())
		} else {
			fmt.Printf("[OK]   DHT22 #1 (GPIO4): temp=%.1f°C hum=%.1f%%\n", float32(temp1)/10, float32(hum1)/10)
		}

		time.Sleep(2 * time.Second)

		temp2, hum2, err2 := sensor2.Measurements()
		if err2 != nil {
			fmt.Printf("[FAIL] DHT22 #2 (GPIO5): %s\n", err2.Error())
		} else {
			fmt.Printf("[OK]   DHT22 #2 (GPIO5): temp=%.1f°C hum=%.1f%%\n", float32(temp2)/10, float32(hum2)/10)
		}

		checkMQ("MQ135 #1 (GPIO34)", mq1Pin)
		checkMQ("MQ135 #2 (GPIO35)", mq2Pin)

		ledState = !ledState
		if ledState {
			led.High()
		} else {
			led.Low()
		}
		time.Sleep(5 * time.Second)
	}
}
