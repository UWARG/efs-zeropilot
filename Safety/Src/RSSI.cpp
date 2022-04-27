#include "RSSI.hpp"
#include "gpio.h"

static volatile bool failed = false;
static volatile int16_t counter = 0;

//Time is in microseconds

constexpr uint8_t MIN_ON_TIME = 1200; //20% of pwm signal from 1000 to 2000 microseconds

bool CommsFailed() {
    return failed;
}

void UpdateRSSI(float onTime) {
    if (onTime < MIN_ON_TIME)
    {
        failed = true;
    }
}
