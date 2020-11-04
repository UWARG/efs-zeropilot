#include "SendInstructionsToSafety.hpp"
#include "interchip_A.h"
#define PWM_CHANNELS 12

int16_t pwmPercentages[PWM_CHANNELS] = {0};
int16_t initialPWMPercentages[PWM_CHANNELS] = {0}; //TODO: put in initial PWM states in here. 


void SendToSafety_Init(void)
{
    for(int i = 0; i < PWM_CHANNELS; i++)
    {
        pwmPercentages[i] = initialPWMPercentages[i];
    }
}

SendToSafety_error_t SendToSafety_Execute(int channel, int percent)
{
    SendToSafety_error_t error;
    error.errorCode = 0;
    pwmPercentages[channel] = percent;

    Interchip_SetPWM(pwmPercentages);
    return error;
}