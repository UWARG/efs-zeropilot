#include "CommsWithTelemetry.hpp"

/********************************************************************
* Variables
********************************************************************/

static Telemetry_PIGO_t userPigo[100];

/********************************************************************
* Code
********************************************************************/

void CommWithTelemInit()
{
}

void SendPathData(POGI *data)
{
 (void) data;
}

bool GetTelemetryCommands(Telemetry_PIGO_t *commands)
{
    static int index;

    *commands = userPigo[index];

    index ++;

    return true;
}
