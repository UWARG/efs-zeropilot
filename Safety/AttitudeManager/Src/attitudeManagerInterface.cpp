
#include "attitudeManagerInterface.h"
#include "attitudeManager.hpp"
#include "PPM.hpp"
#include "PWM.hpp"
#include "grabber.hpp"

attitudeManager *attMng;

void AttitudeManagerInterfaceInit(void) {
    PPMChannel *ppm = new PPMChannel(MAX_PPM_CHANNELS);
    PWMChannel *pwm = new PWMChannel();
    Grabber *grabber = &Grabber::getInstance(4,5,6,7,pwm); //TODO: What output channel?
    attMng = new attitudeManager(ppm, pwm, grabber);
}

void AttitudeManagerInterfaceExecute(void)
{
    attMng->execute();
}
