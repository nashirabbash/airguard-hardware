package main

import (
	"fmt"
	"machine"
	"time"

	"tinygo.org/x/drivers/dht"
)

const (
	dht1Pin  = machine.GPIO4
	dht2Pin  = machine.GPIO15
	mq1AO   = machine.GPIO34
	mq2AO   = machine.GPIO35
	mq1DO   = machine.GPIO19
	mq2DO   = machine.GPIO21
)

func checkMQ(label string, doPin machine.Pin, aoPin machine.Pin) {
	doPin.Configure(machine.PinConfig{Mode: machine.PinInputPulldown})
	aoPin.Configure(machine.PinConfig{Mode: machine.PinInput})
	time.Sleep(10 * time.Millisecond)

	first := aoPin.Get()
	floating := false
	for i := 0; i < 10; i++ {
		time.Sleep(100 * time.Millisecond)
		if aoPin.Get() != first {
			floating = true
			break
		}
	}
	if floating {
		fmt.Printf("[FAIL] %s: disconnected (AO unstable)\n", label)
		return
	}

	do := doPin.Get()
	status := "AMAN"
	if do {
		status = "BAHAYA"
	}
	fmt.Printf("[OK]   %s: %s (DO=%v AO=%v)\n", label, status, do, first)
}

func main() {
	led := machine.LED
	led.Configure(machine.PinConfig{Mode: machine.PinOutput})

	machine.GPIO4.Configure(machine.PinConfig{Mode: machine.PinInputPullup})
	time.Sleep(100 * time.Millisecond)
	machine.GPIO15.Configure(machine.PinConfig{Mode: machine.PinInputPullup})
	time.Sleep(100 * time.Millisecond)
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
			fmt.Printf("[FAIL] DHT22 #2 (GPIO15): %s\n", err2.Error())
		} else {
			fmt.Printf("[OK]   DHT22 #2 (GPIO15): temp=%.1f°C hum=%.1f%%\n", float32(temp2)/10, float32(hum2)/10)
		}

		checkMQ("MQ135 #1", mq1DO, mq1AO)
		checkMQ("MQ135 #2", mq2DO, mq2AO)

		ledState = !ledState
		if ledState {
			led.High()
		} else {
			led.Low()
		}
		time.Sleep(5 * time.Second)
	}
}
