package config

import (
	"machine"
	"time"
)

// DHT22 sensor pins
const (
	DHT1Pin = machine.GPIO4
	DHT2Pin = machine.GPIO15
)

// MQ-135 air quality sensor pins
const (
	MQ1AO = machine.GPIO34
	MQ2AO = machine.GPIO35
	MQ1DO = machine.GPIO19
	MQ2DO = machine.GPIO21
)

func InitPins() {
	// Initialize LED
	machine.GPIO2.Configure(machine.PinConfig{Mode: machine.PinOutput})

	// Initialize DHT22 sensor pins
	DHT1Pin.Configure(machine.PinConfig{Mode: machine.PinInputPullup})
	time.Sleep(100 * time.Millisecond)
	DHT2Pin.Configure(machine.PinConfig{Mode: machine.PinInputPullup})
	time.Sleep(100 * time.Millisecond)
}

// ConfigureMQPin sets up MQ sensor pins for reading
func ConfigureMQPin(doPin, aoPin machine.Pin) {
	doPin.Configure(machine.PinConfig{Mode: machine.PinInputPulldown})
	aoPin.Configure(machine.PinConfig{Mode: machine.PinInput})
	time.Sleep(10 * time.Millisecond)
}
