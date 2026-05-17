#pragma once

#include <cstdint>

enum MQ135Status {
    AMAN = 0,
    BAHAYA = 1
};

struct MQ135Reading {
    uint32_t raw_adc;
    MQ135Status status;
};

void mq135_init(void);
MQ135Reading mq135_read(int ao_pin, int do_pin);
void mq135_log_reading(int sensor_num, const MQ135Reading& reading);
