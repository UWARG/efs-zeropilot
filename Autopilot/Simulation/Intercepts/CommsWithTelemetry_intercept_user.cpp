#include "CommsWithTelemetry.hpp"
#include "TelemPathInterface.hpp"

/********************************************************************
* Variables
********************************************************************/

static Telemetry_PIGO_t userPigo[1000];
static int index = 0;

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
    userPigo[index].waypointModifyFlightPathCommand = INITIALIZE_FLIGHT_PATH;

    userPigo[index].numWaypoints = 3;
    userPigo[index].waypoints[0].altitude = 50;
    userPigo[index].waypoints[0].waypointType = 0;
    userPigo[index].waypoints[0].longitude = -157.924000;
    userPigo[index].waypoints[0].latitude = 21.322000;
    userPigo[index].waypoints[0].turnRadius = 100;

    userPigo[index].waypoints[1].altitude = 50;
    userPigo[index].waypoints[1].waypointType = 0;
    userPigo[index].waypoints[1].longitude = -155.924000;
    userPigo[index].waypoints[1].latitude = 21.322000;
    userPigo[index].waypoints[1].turnRadius = 100;

    userPigo[index].waypoints[2].altitude = 50;
    userPigo[index].waypoints[2].waypointType = 0;
    userPigo[index].waypoints[2].longitude = -154.000;
    userPigo[index].waypoints[2].latitude = 21.322000;
    userPigo[index].waypoints[2].turnRadius = 100;

    if(index==0)
    {
        userPigo[index].takeoffDirectionHeading = 0;
        userPigo[index].beginTakeoff = true;
    }
    if(index==300)
    {
        int i = 0;
    }

    if(index==800)
    {
        userPigo[index].beginLanding = true;
        userPigo[index].stoppingAltitude = 0;
        userPigo[index].stoppingDirectionHeading = 90;
        userPigo[index].stoppingLongitude = -157.91;
        userPigo[index].stoppingLatitude = 21.3200;
    }


    *commands = userPigo[index];

    index++;

    return true;
}

