
#include "attitudeManagerInterface.h"
#include "attitudeManager.hpp"
#include "PPM.hpp"
#include "PWM.hpp"

attitudeManager *attMng;

void AttitudeManagerInterfaceInit(void) {
    PPMChannel *ppm = new PPMChannel(MAX_PPM_CHANNELS);
    PWMChannel *pwm = new PWMChannel();
    attMng = new attitudeManager(ppm, pwm);
}

void AttitudeManagerInterfaceExecute(void)
{
    attMng->execute();
}
