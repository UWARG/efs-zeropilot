
#include "attitudeManagerInterface.h"
#include "attitudeManager.hpp"
#include "PPM.hpp"
#include "PWM.hpp"

attitudeManager *attMng;

void AttitudeManagerInterfaceInit(void) {
    PPMChannel *ppm = new PPMChannel(MAX_PPM_CHANNELS);
    PWMChannel *pwm = new PWMChannel();
    HAL_Delay(1000);
    pwm->set(0,0);
    pwm->set(1,0);
    pwm->set(2,0);
    pwm->set(3,0);
    HAL_Delay(3000);
    attMng = new attitudeManager(ppm, pwm);
}

void AttitudeManagerInterfaceExecute(void)
{
    attMng->execute();
}
