#include "Common.h"

void constrain(int16_t* input, int16_t min, int16_t max) {
    if (*input < min) {
        *input = min;
    }
    else if (*input > max) {
        *input = max;
    }
}