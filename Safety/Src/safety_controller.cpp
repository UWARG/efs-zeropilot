#include "safety_controller.hpp"
#include "PWM.hpp"
#include "stdlib.h"
#include "interchip_S.hpp"
#include "RSSI.hpp"
#include "safetyConfig.hpp"

#include "gpio.h"

#include <cstdint>

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

// TODO: move to safetyConfig.hpp

constexpr uint8_t MANUAL_OVERRIDE_CHANNEL {4};

// temporary grabber_rx and grabber_pwm channels. To be changed later.
constexpr uint8_t GRABBER_RX_CHANNEL {0};
constexpr uint8_t GRABBER_PWM_CHANNEL {9};

constexpr uint8_t MIN_REVERSE_THRUST_PPM_TO_ACTIVATE {50};

constexpr uint8_t MIN_OVERRIDE_CHANNEL_VALUE_TO_ACTIVATE {50};

/***********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************/

static void setPWMChannel(PWMChannel &pwm, int channel, int percentage);
static uint8_t getPPM(PPMChannel &ppm, int channel);
static bool AutoPilotEngaged(PPMChannel &ppm);

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

void safety_controller_init()
{
}

void new_safety_run(PWMChannel &pwm, PPMChannel &ppm) {

    // ?get current pwm?
    volatile int16_t *AutoPilotPwmChannel = getPWM();

    // get RC PPM?
    uint8_t lrPPM = getPPM(ppm, lrPPMChannel);


}

void safety_run(PWMChannel &pwm, PPMChannel &ppm)
{

    if(CommsFailed()) {
        setPWMChannel(pwm, PWM_CHANNEL_LEFT_TAIL, 0);
        setPWMChannel(pwm, PWM_CHANNEL_RIGHT_TAIL, 100);
        setPWMChannel(pwm, PWM_CHANNEL_AILERON, 100);
        setPWMChannel(pwm, PWM_CHANNEL_THROTTLE, 0);
        setPWMChannel(pwm, PWM_CHANNEL_GIMBAL_YAW, 50);
        setPWMChannel(pwm, PWM_CHANNEL_NOSE_WHEEL, 0);

        return;
    }

    volatile int16_t *AutoPilotPwmChannel = getPWM();

    uint8_t elevatorAutoPilot = AutoPilotPwmChannel[0];
    uint8_t aileronAutoPilot = AutoPilotPwmChannel[1];
    uint8_t throttleAutoPilot =  AutoPilotPwmChannel[2];
    uint8_t rudderAutoPilot= AutoPilotPwmChannel[3];

    uint8_t elevatorPPM = getPPM(ppm, 0);
    uint8_t aileronPPM = getPPM(ppm, 1);
    uint8_t throttlePPM =  getPPM(ppm, 2);
    uint8_t rudderPPM = getPPM(ppm, 3);
    uint8_t gimbalYawPPM = getPPM(ppm, 5);
    uint8_t reverseThrustPPM = getPPM(ppm, 6);

    uint16_t leftTailMix = 0;
    uint16_t rightTailMix = 0;
    uint16_t aileronMix = 0;
    uint16_t noseWheelMix = 0;

    noseWheelMix = 100 - rudderPPM;

    if(AutoPilotEngaged(ppm))
    {
        leftTailMix = (0.7 * (100 - elevatorAutoPilot)) + (0.2 * rudderAutoPilot);
        rightTailMix = (0.7 * elevatorAutoPilot) + (0.2 * rudderAutoPilot);
        aileronMix = aileronAutoPilot;
    }

    else
    {
        leftTailMix = (0.7 * (100 - elevatorPPM)) + (0.2 * rudderPPM);
        rightTailMix = (0.7 * elevatorPPM) + (0.2 * rudderPPM);
        aileronMix = aileronPPM;
    }

    /************Reverse thrust**************/

    setPWMChannel(pwm, PWM_CHANNEL_LEFT_TAIL, leftTailMix);
    setPWMChannel(pwm, PWM_CHANNEL_RIGHT_TAIL, rightTailMix);
    setPWMChannel(pwm, PWM_CHANNEL_AILERON, aileronMix);
    setPWMChannel(pwm, PWM_CHANNEL_NOSE_WHEEL, noseWheelMix);
    setPWMChannel(pwm, PWM_CHANNEL_THROTTLE, throttlePPM);
    setPWMChannel(pwm, PWM_CHANNEL_GIMBAL_YAW, gimbalYawPPM);

    if (reverseThrustPPM > MIN_REVERSE_THRUST_PPM_TO_ACTIVATE)
    {
        HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin, GPIO_PIN_RESET); 
    }

    /************ Grabber**************/
    if (gimbalYawPPM < 40)
    {
        setPWMChannel(pwm, PWM_CHANNEL_GRABBER, 0);
        setPWMChannel(pwm, PWM_CHANNEL_GIMBAL_YAW, 50);
    }
    else if (gimbalYawPPM > 60)
    {
        setPWMChannel(pwm, PWM_CHANNEL_GRABBER, 65);
        setPWMChannel(pwm, PWM_CHANNEL_GIMBAL_YAW, 50);
    }
}

static bool AutoPilotEngaged(PPMChannel &ppm)
{

    uint8_t overrideChannelValue = getPPM(ppm, MANUAL_OVERRIDE_CHANNEL);

    if(overrideChannelValue > MIN_OVERRIDE_CHANNEL_VALUE_TO_ACTIVATE)
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

static uint8_t getPPM(PPMChannel &ppm, int channel)
{
    return ppm.get(channel);
}
