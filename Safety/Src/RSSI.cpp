#include "RSSI.hpp"
#include "gpio.h"

static volatile bool failed = false;
static volatile int16_t counter = 0;

bool CommsFailed() {
    return failed;
}
void RSSI_Check() {
    GPIO_PinState state = HAL_GPIO_ReadPin(RSSI_GPIO_Port, RSSI_Pin);
    if (state == GPIO_PIN_SET) {
        // Low
        counter = 0;
        failed = false;
    } else {
        // High
        counter++;
        if (counter > 1000) {
            counter = 0;
            failed = true;
        }
    }

}
