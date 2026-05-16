package lib

import (
	"fmt"
)

func LogOK(label, detail string) {
	fmt.Printf("[OK]   %s: %s\n", label, detail)
}

func LogFail(label, reason string) {
	fmt.Printf("[FAIL] %s: %s\n", label, reason)
}
