#include "telemetryManagerInterface.h"
#include "telemetryManager.hpp"

static telemetryManager telemMng;

void TelemetryManagerInterfaceExecute(void) {
    telemMng.execute();
}