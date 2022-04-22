
#include "attitudeManagerInterface.h"
#include "attitudeManager.hpp"
#include "PPM.hpp"
#include "PWM.hpp"

#include "stm32f0xx_hal.h"

attitudeManager *attMng;

void AttitudeManagerInterfaceInit(void) {
    PPMChannel *ppm = new PPMChannel(MAX_PPM_CHANNELS);
    PWMChannel *pwm = new PWMChannel();
    attMng = new attitudeManager(ppm, pwm);

      bool CALIBRATE = true;
  if (CALIBRATE) {
for(int i = 0; i<=4; i++) {
    pwm->set(i, 0);
  }
  HAL_Delay(2000);
  for(int i = 0; i<=4; i++) {
    // pwm->set(i, 100);
    pwm->set(11, 100);
  }
  HAL_Delay(2000);
  for(int i = 0; i<=11; i++) {
    pwm->set(i, 0);
  }
  HAL_Delay(2000);
  }
}

void AttitudeManagerInterfaceExecute(void)
{
    attMng->execute();
}
