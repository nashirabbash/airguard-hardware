package drivers

/*
#include <stdlib.h>

void airguard_wifi_init(const char *ssid, const char *pass);
int  airguard_wifi_connected(void);
*/
import "C"
import "unsafe"

const (
	ssid       = "vivo V30e"
	password   = "1sampai8"
	BackendURL = "ws://localhost:3000/ws/ingest"
)

func InitWiFi() {
	cSSID := C.CString(ssid)
	cPass := C.CString(password)
	defer C.free(unsafe.Pointer(cSSID))
	defer C.free(unsafe.Pointer(cPass))
	C.airguard_wifi_init(cSSID, cPass)
}

func IsWiFiConnected() bool {
	return C.airguard_wifi_connected() == 1
}
