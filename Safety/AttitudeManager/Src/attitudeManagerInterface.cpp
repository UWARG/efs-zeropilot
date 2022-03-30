
#include "attitudeManagerInterface.h"
#include "attitudeManager.hpp"
#include "PPM.hpp"
#include "PWM.hpp"

attitudeManager *attMng;

void AttitudeManagerInterfaceInit(void) {
    PPMChannel *ppm = new PPMChannel(MAX_PPM_CHANNELS);
    PWMChannel *pwm = new PWMChannel();
    uint8_t grabberChannel = 5; //TODO: What channel?
    Grabber *grabber = &Grabber::getInstance(grabberChannel);
    attMng = new attitudeManager(ppm, pwm, grabber);
}

void AttitudeManagerInterfaceExecute(void)
{
    attMng->execute();
}
