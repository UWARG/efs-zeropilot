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
    userPigo[0].waypoints[0].altitude = 1000;
    userPigo[0].waypoints[0].waypointType = 0;
    userPigo[0].waypoints[0].longitude = -157.924000;
    userPigo[0].waypoints[0].latitude = 21.322000;
    userPigo[0].waypoints[0].turnRadius = 100;

    userPigo[0].waypoints[1].altitude = 1000;
    userPigo[0].waypoints[1].waypointType = 0;
    userPigo[0].waypoints[1].longitude = -158.924000;
    userPigo[0].waypoints[1].latitude = 21.322000;
    userPigo[0].waypoints[1].turnRadius = 100;


    *commands = userPigo[index];

    index ++;

    return true;
}

