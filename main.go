package main

import (
	"fmt"
	"machine"
	"time"

	"airguard/config"
	"airguard/lib"
	"tinygo.org/x/drivers/dht"
)

func checkMQ(label string, doPin machine.Pin, aoPin machine.Pin) {
	config.ConfigureMQPin(doPin, aoPin)

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
		lib.LogFail(label, "disconnected (AO unstable)")
		return
	}

	do := doPin.Get()
	status := "AMAN"
	if do {
		status = "BAHAYA"
	}
	lib.LogOK(label, fmt.Sprintf("%s (DO=%v AO=%v)", status, do, first))
}

func main() {
	config.InitPins()

	sensor1 := dht.New(config.DHT1Pin, dht.DHT22)
	sensor2 := dht.New(config.DHT2Pin, dht.DHT22)

	fmt.Println("=== AirGuard Sensor Check ===")
	fmt.Println("Waiting 3s for sensor stabilize...")
	time.Sleep(3 * time.Second)

	ledState := false
	for {
		fmt.Println("--- Scan ---")

		temp1, hum1, err1 := sensor1.Measurements()
		if err1 != nil {
			lib.LogFail("DHT22 #1 (GPIO4)", err1.Error())
		} else {
			lib.LogOK("DHT22 #1 (GPIO4)", fmt.Sprintf("temp=%.1f°C hum=%.1f%%", float32(temp1)/10, float32(hum1)/10))
		}

		time.Sleep(2 * time.Second)

		temp2, hum2, err2 := sensor2.Measurements()
		if err2 != nil {
			lib.LogFail("DHT22 #2 (GPIO15)", err2.Error())
		} else {
			lib.LogOK("DHT22 #2 (GPIO15)", fmt.Sprintf("temp=%.1f°C hum=%.1f%%", float32(temp2)/10, float32(hum2)/10))
		}

		checkMQ("MQ135 #1", config.MQ1DO, config.MQ1AO)
		checkMQ("MQ135 #2", config.MQ2DO, config.MQ2AO)

		ledState = !ledState
		if ledState {
			machine.LED.High()
		} else {
			machine.LED.Low()
		}
		time.Sleep(5 * time.Second)
	}
}
