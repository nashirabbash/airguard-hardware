package drivers

import (
	"fmt"
	"time"

	"airguard/config"
	"airguard/lib"
	"machine"
)

func CheckMQ135(label string, doPin machine.Pin, aoPin machine.Pin) uint16 {
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
		return 0
	}

	do := doPin.Get()
	status := "AMAN"
	if do {
		status = "BAHAYA"
	}
	lib.LogOK(label, fmt.Sprintf("%s (DO=%v AO=%v)", status, do, first))
	if do {
		return 1
	}
	return 0
}

func CheckAllMQ() uint16 {
	mq1 := CheckMQ135("MQ135 #1", config.MQ1DO, config.MQ1AO)
	CheckMQ135("MQ135 #2", config.MQ2DO, config.MQ2AO)
	return mq1
}
