#include <stdint.h>
#define PWM_NUM_CHANNELS (12)
#define PWM_MAX (3200) // 500 us pulse
#define PWM_MIN (-3200) // 2500 us pulse
#define HALF_PWM_RANGE (PWM_MAX - PWM_MIN)/2

#define SAFETY_CHANNEL 4

void constrain(int16_t* input, int16_t min, int16_t max);