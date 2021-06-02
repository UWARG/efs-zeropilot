
#include "attitudeManagerInterface.h"
#include "attitudeManager.hpp"

static attitudeManager attMng;

void AttitudeManagerInterfaceExecute(void)
{
    attMng.execute();
}
