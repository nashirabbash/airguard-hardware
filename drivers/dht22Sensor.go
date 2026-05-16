package drivers

import (
	"fmt"
	"time"

	"airguard/config"
	"airguard/lib"
	"tinygo.org/x/drivers/dht"
)

var (
	sensor1 dht.Device
	sensor2 dht.Device
)

func InitDHT22() {
	sensor1 = dht.New(config.DHT1Pin, dht.DHT22)
	sensor2 = dht.New(config.DHT2Pin, dht.DHT22)
}

func ReadDHT22() (temp1, hum1, temp2, hum2 float32) {
	t1, h1, err1 := sensor1.Measurements()
	if err1 != nil {
		lib.LogFail("DHT22 #1 (GPIO4)", err1.Error())
	} else {
		lib.LogOK("DHT22 #1 (GPIO4)", fmt.Sprintf("temp=%.1f°C hum=%.1f%%", float32(t1)/10, float32(h1)/10))
	}

	time.Sleep(2 * time.Second)

	t2, h2, err2 := sensor2.Measurements()
	if err2 != nil {
		lib.LogFail("DHT22 #2 (GPIO15)", err2.Error())
	} else {
		lib.LogOK("DHT22 #2 (GPIO15)", fmt.Sprintf("temp=%.1f°C hum=%.1f%%", float32(t2)/10, float32(h2)/10))
	}

	return float32(t1) / 10, float32(h1) / 10, float32(t2) / 10, float32(h2) / 10
}
