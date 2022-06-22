#include "pidInterface.hpp"
#include "../../AttitudeManager/Inc/attitudeStateClasses.hpp"
#include "Controls.hpp"
void pidInterfaceInit(void) {}

void pidInterfaceExecute(void)
{
    _PidOutputG = runControlsAndGetPWM(_ControlsInstructions, SFOutputG);
}