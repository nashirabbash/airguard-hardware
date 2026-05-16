package main

import (
	"airguard/config"
	"airguard/drivers"
)

func main() {
	config.InitPins()
	drivers.InitDHT22()
	drivers.InitWiFi()
	drivers.RunScanLoop()
}
