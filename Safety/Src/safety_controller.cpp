#include "safety_controller.hpp"
#include "PWM.hpp"
#include "stdlib.h"
#include "interchip_S.hpp"

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

constexpr int MANUAL_OVERRIDE_CHANNEL = 4;
// temporary grabber_rx and grabber_pwm channels. To be changed later.
constexpr int grabber_rx_channel = 0;
constexpr int grabber_pwm_channel = 9;

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

    // note that we don't need to set the servo to max opening or closing if we don't need to.
    // this can help prevent motor overheating if it constantly tries to hit a position which it physically can't.
    if (getPPM(ppm, grabber_rx_channel)>50){
        setPWMChannel(pwm, grabber_pwm_channel, 75);
    } else {
        setPWMChannel(pwm, grabber_pwm_channel, 25);
    }

    // For the second flight test, AutoPilot only controls elevator and aileron.
    if(AutoPilotEngaged(ppm))
    {
         volatile int16_t *AutoPilotPwmChannel = getPWM();

        // These are Autopilot controlled pwm channels.
        setPWMChannel(pwm, 0, (uint32_t) getPPM(ppm, 0));
        setPWMChannel(pwm, 1, static_cast<uint32_t> (AutoPilotPwmChannel[1]));
        setPWMChannel(pwm, 3, static_cast<uint32_t> (AutoPilotPwmChannel[3]));
        setPWMChannel(pwm, 7, static_cast<uint32_t> (AutoPilotPwmChannel[7]));

        // these are ppm controls (not affected by autopilot). Ask Anthony or comment above if for more info.
        setPWMChannel(pwm, 2, (uint32_t) getPPM(ppm, 2));
        setPWMChannel(pwm, 4, (uint32_t) getPPM(ppm, 4));
        setPWMChannel(pwm, 5, (uint32_t) getPPM(ppm, 5));
        setPWMChannel(pwm, 6, (uint32_t) getPPM(ppm, 6));
    }
    else
    {
        // we maintain pwm control for our pwm channels for gimbal (ch 3 and 7).
        setPWMChannel(pwm, 3, 0);
        setPWMChannel(pwm, 7, 50);

        // we maintain ppm control for the rest of the servos.
        for(int channel = 0; channel < 3; channel++)
        {
            setPWMChannel(pwm, channel, (uint32_t) getPPM(ppm, channel));
        }
        for(int channel = 4; channel < 7; channel++)
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
