
#include "attitudeManagerInterface.h"
#include "attitudeManager.hpp"
#include "PPM.hpp"
#include "PWM.hpp"



static PPMChannel *ppm = new PPMChannel(MAX_PPM_CHANNELS);
static PWMChannel *pwm = new PWMChannel();
static attitudeManager attMng(ppm, pwm);


void AttitudeManagerInterfaceExecute(void)
{
    attMng.execute();
}
