#include "AttitudeManagerInterface.hpp"
#include "attitudeManager.hpp"

static attitudeManager attMng;

void AttitudeManagerInterfaceExecute(void)
{
    attMng.execute();
}
