#include "safety_controller.hpp"
#include "PWM.hpp"
#include "stdlib.h"
#include "interchip_S.hpp"

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

constexpr int MANUAL_OVERRIDE_CHANNEL = 4; 

/***********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************/

static void setPWMChannel(PWMChannel &pwm, int channel, int percentage);
static int getPPM(PPMChannel &ppm, int channel);

static bool AutoPilotEngaged(PPMChannel &ppm);

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

void safety_controller_init()
{
}

void safety_run(PWMChannel &pwm, PPMChannel &ppm)
{

    // For the second flight test, AutoPilot only controls elevator and aileron.
    if(AutoPilotEngaged(ppm))
    {
         volatile int16_t *AutoPilotPwmChannel = getPWM();

        for(int channel = 0; channel < 2; channel++)
        {
            setPWMChannel(pwm, channel, static_cast<uint32_t> (AutoPilotPwmChannel[channel]));
        }
        
        for(int channel = 2; channel < 8; channel++)
        {
            setPWMChannel(pwm, channel, (uint32_t) getPPM(ppm, channel));
        }
    }
    else
    {
        for(int channel = 0; channel < 8; channel++)
        {
            setPWMChannel(pwm, channel, (uint32_t) getPPM(ppm, channel));
        }
    }

}

static bool AutoPilotEngaged(PPMChannel &ppm)
{

    uint8_t overrideChannelValue = getPPM(ppm, MANUAL_OVERRIDE_CHANNEL);

    if(overrideChannelValue > 50)
    {
        return true;
    }

    return false;
}

/*
* Sets an individual PWM channel to the desired output.
*/
static void setPWMChannel(PWMChannel &pwm, int channel, int percentage)
{
    //Input protection
    if(percentage > 100) {percentage = 100;}
    if(percentage < 0) {percentage = 0;}
    pwm.set(channel, percentage);
}

static int getPPM(PPMChannel &ppm, int channel)
{
    return ppm.get(channel);
}
