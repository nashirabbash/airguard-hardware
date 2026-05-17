#include "mq135Sensor.h"
#include "logManagement.h"
#include "pins.h"

static void configureMQPin(int doPin, int aoPin) {
    pinMode(doPin, INPUT_PULLDOWN);
    pinMode(aoPin, INPUT);
    delay(10);
}

int checkMQ135(const char* label, int doPin, int aoPin) {
    configureMQPin(doPin, aoPin);

    int first    = digitalRead(aoPin);
    bool floating = false;
    for (int i = 0; i < 10; i++) {
        delay(100);
        if (digitalRead(aoPin) != first) {
            floating = true;
            break;
        }
    }
    if (floating) {
        logFail(label, "disconnected (AO unstable)");
        return 0;
    }

    int  doVal  = digitalRead(doPin);
    char detail[64];
    snprintf(detail, sizeof(detail), "%s (DO=%d AO=%d)", doVal ? "BAHAYA" : "AMAN", doVal, first);
    logOK(label, detail);
    return doVal ? 1 : 0;
}

void checkAllMQ() {
    checkMQ135("MQ135 #1", MQ1_DO, MQ1_AO);
    checkMQ135("MQ135 #2", MQ2_DO, MQ2_AO);
}
