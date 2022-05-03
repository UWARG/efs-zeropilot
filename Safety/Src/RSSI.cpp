#include "RSSI.hpp"
#include "gpio.h"

static volatile bool failed = false;
static volatile int16_t counter = 0;
constexpr uint16_t MIN_FAIL_COUNT = 10; //Number of measurements required before failure

//Time is in microseconds
constexpr uint16_t MIN_ON_TIME = 1200; //20% of pwm signal from 1000 to 2000 microseconds

bool CommsFailed() {
    return failed;
}

void UpdateRSSI(float onTime) {
    if (onTime == 0) return;

    if (onTime < MIN_ON_TIME)
    {
        counter++;
        if (counter >= MIN_FAIL_COUNT)
            failed = true;
    }
    else
    {
        counter = 0;
    }
}
