package lib

import (
	"fmt"
	"time"
)

// Flash storage configuration for ESP32
const (
	logStorageStart  = 0x3D0000 // Reserve ~64KB for logs (adjust as needed)
	logStorageSize   = 0x10000  // 64KB storage area
	logMaxEntries    = 256      // Max log entries
	logEntrySize     = 256      // Bytes per log entry
)

var (
	logIndex    = 0    // Current write position
	logInitDone = false
	logBuffer   [logMaxEntries][logEntrySize]byte
)

func initLogs() {
	if logInitDone {
		return
	}
	logInitDone = true
	logIndex = 0
}

func LogOK(label, detail string) {
	fmt.Printf("[OK]   %s: %s\n", label, detail)
	logMessage("OK", label, detail)
}

func LogFail(label, reason string) {
	fmt.Printf("[FAIL] %s: %s\n", label, reason)
	logMessage("FAIL", label, reason)
}

func logMessage(level, label, message string) {
	initLogs()
	timestamp := time.Now().Format("2006-01-02 15:04:05")
	entry := fmt.Sprintf("[%s] %s %s: %s", timestamp, level, label, message)
	LogToFlash(entry)
}

func LogToFlash(msg string) {
	initLogs()

	if logIndex >= logMaxEntries {
		logIndex = 0 // Circular buffer: wrap around
	}

	// Truncate message if too long
	if len(msg) > logEntrySize-1 {
		msg = msg[:logEntrySize-1]
	}

	// Write to in-memory buffer
	copy(logBuffer[logIndex][:], msg)
	logBuffer[logIndex][len(msg)] = 0 // Null terminate

	logIndex++
}

func SensorLog(temp1, hum1, temp2, hum2, mq1, mq2 float32) {
	initLogs()
	msg := fmt.Sprintf("SENSOR temp1=%.1f hum1=%.1f temp2=%.1f hum2=%.1f mq1=%d mq2=%d", temp1, hum1, temp2, hum2, int(mq1), int(mq2))
	LogToFlash(msg)
}

func DumpLogs() {
	initLogs()

	fmt.Println("\n=== Stored Logs ===")
	if logIndex == 0 {
		fmt.Println("(no logs stored)")
		return
	}

	for i := 0; i < logIndex; i++ {
		msg := string(logBuffer[i][:])
		// Stop at null terminator
		for j := 0; j < len(msg); j++ {
			if msg[j] == 0 {
				msg = msg[:j]
				break
			}
		}
		if len(msg) > 0 {
			fmt.Printf("%d: %s\n", i+1, msg)
		}
	}
	fmt.Println("=================\n")
}

func ClearLogs() {
	initLogs()
	for i := 0; i < logMaxEntries; i++ {
		for j := 0; j < logEntrySize; j++ {
			logBuffer[i][j] = 0
		}
	}
	logIndex = 0
}

func GetLogCount() int {
	initLogs()
	return logIndex
}

