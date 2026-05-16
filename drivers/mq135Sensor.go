package drivers

import (
	"fmt"
	"time"

	"airguard/config"
	"airguard/lib"
	"machine"
)

func CheckMQ135(label string, doPin machine.Pin, aoPin machine.Pin) {
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
