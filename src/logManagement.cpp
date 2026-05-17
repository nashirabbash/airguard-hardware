#include "logManagement.h"
#include <string.h>
#include <stdio.h>

static const int LOG_MAX_ENTRIES = 256;
static const int LOG_ENTRY_SIZE  = 256;

static char logBuffer[LOG_MAX_ENTRIES][LOG_ENTRY_SIZE];
static int  logIndex    = 0;
static bool logInitDone = false;

static void initLogs() {
    if (logInitDone) return;
    logInitDone = true;
    logIndex = 0;
}

void logToFlash(const char* msg) {
    initLogs();
    if (logIndex >= LOG_MAX_ENTRIES) logIndex = 0;

    strncpy(logBuffer[logIndex], msg, LOG_ENTRY_SIZE - 1);
    logBuffer[logIndex][LOG_ENTRY_SIZE - 1] = '\0';
    logIndex++;
}

static void logMessage(const char* level, const char* label, const char* message) {
    initLogs();
    char entry[LOG_ENTRY_SIZE];
    snprintf(entry, sizeof(entry), "[%s] %s: %s", level, label, message);
    logToFlash(entry);
}

void logOK(const char* label, const char* detail) {
    Serial.printf("[OK]   %s: %s\n", label, detail);
    logMessage("OK", label, detail);
}

void logFail(const char* label, const char* reason) {
    Serial.printf("[FAIL] %s: %s\n", label, reason);
    logMessage("FAIL", label, reason);
}

void sensorLog(float temp1, float hum1, float temp2, float hum2, int mq1, int mq2) {
    initLogs();
    char msg[LOG_ENTRY_SIZE];
    snprintf(msg, sizeof(msg),
        "SENSOR temp1=%.1f hum1=%.1f temp2=%.1f hum2=%.1f mq1=%d mq2=%d",
        temp1, hum1, temp2, hum2, mq1, mq2);
    logToFlash(msg);
}

void dumpLogs() {
    initLogs();
    Serial.println("\n=== Stored Logs ===");
    if (logIndex == 0) {
        Serial.println("(no logs stored)");
        return;
    }
    for (int i = 0; i < logIndex; i++) {
        Serial.printf("%d: %s\n", i + 1, logBuffer[i]);
    }
    Serial.println("=================\n");
}

void clearLogs() {
    initLogs();
    memset(logBuffer, 0, sizeof(logBuffer));
    logIndex = 0;
}

int getLogCount() {
    initLogs();
    return logIndex;
}

// builds last maxEntries log entries into buf as JSON-safe \n-separated string
int buildLogPayload(char* buf, size_t bufSize, int maxEntries) {
    initLogs();
    int start = (logIndex > maxEntries) ? logIndex - maxEntries : 0;
    size_t pos = 0;
    for (int i = start; i < logIndex; i++) {
        size_t len = strlen(logBuffer[i]);
        if (pos + len + 3 >= bufSize) break;
        memcpy(buf + pos, logBuffer[i], len);
        pos += len;
        buf[pos++] = '\\';
        buf[pos++] = 'n';
    }
    buf[pos] = '\0';
    return logIndex - start;
}
