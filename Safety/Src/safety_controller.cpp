#include "safety_controller.hpp"
#include "PWM.hpp"
#include "stdlib.h"
#include "interchip_S.hpp"
#include "RSSI.hpp"

#include "gpio.h"

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

constexpr int MANUAL_OVERRIDE_CHANNEL = 4;
// temporary grabber_rx and grabber_pwm channels. To be changed later.
constexpr int grabber_rx_channel = 0;
constexpr int grabber_pwm_channel = 9;


        // pwm order: left tail, right tail, aileron, nose wheel, throttle, gimbal x, gimbal y, reverse thrust.
#define PWM_CHANNEL_LEFT_TAIL 0
#define PWM_CHANNEL_RIGHT_TAIL 1
#define PWM_CHANNEL_AILERON 2
#define PWM_CHANNEL_NOSE_WHEEL 3
#define PWM_CHANNEL_THROTTLE 4
#define PWM_CHANNEL_GIMBAL_YAW 5
#define PWM_CHANNEL_GRABBER 6

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

    int elevatorAutoPilot = AutoPilotPwmChannel[0];
    int aileronAutoPilot = AutoPilotPwmChannel[1];
    int throttleAutoPilot =  AutoPilotPwmChannel[2];
    int rudderAutoPilot= AutoPilotPwmChannel[3];

    int elevatorPPM = getPPM(ppm, 0);
    int aileronPPM = getPPM(ppm, 1);
    int throttlePPM =  getPPM(ppm, 2);
    int rudderPPM = getPPM(ppm, 3);
    int gimbalYawPPM = getPPM(ppm, 5);
    int reverseThrustPPM = getPPM(ppm, 6);

    int leftTailMix = 0;
    int rightTailMix = 0;
    int aileronMix = 0;
    int noseWheelMix = 0;

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

    if (reverseThrustPPM > 50)
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
