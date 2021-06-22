#include "CommsWithTelemetry.hpp"
#include "TelemPathInterface.hpp"

/********************************************************************
* Variables
********************************************************************/

static Telemetry_PIGO_t userPigo[2000];
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

    /*****
    WAYPOINT TESTS
    //userPigo[index].waypointModifyFlightPathCommand = NO_FLIGHT_PATH_EDIT;
    //userPigo[index].waypointNextDirectionsCommand = REGULAR_PATH_FOLLOWING;

     /* Circling Test
    if (index == 0) {

        userPigo[index].waypointModifyFlightPathCommand = INITIALIZE_FLIGHT_PATH;

        userPigo[index].numWaypoints = 3;
        userPigo[index].waypoints[0].altitude = 50;
        userPigo[index].waypoints[0].waypointType = 0;
        userPigo[index].waypoints[0].longitude = -157.924000;
        userPigo[index].waypoints[0].latitude = 21.322000;
        userPigo[index].waypoints[0].turnRadius = 100;

        userPigo[index].waypoints[1].altitude = 50;
        userPigo[index].waypoints[1].waypointType = 0;
        userPigo[index].waypoints[1].longitude = -156.924000;
        userPigo[index].waypoints[1].latitude = 21.322000;
        userPigo[index].waypoints[1].turnRadius = 100;

        userPigo[index].waypoints[2].altitude = 50;
        userPigo[index].waypoints[2].waypointType = 0;
        userPigo[index].waypoints[2].longitude = -155.000;
        userPigo[index].waypoints[2].latitude = 21.322000;
        userPigo[index].waypoints[2].turnRadius = 100;
    }

    if (index == 600) {
        userPigo[index].waypointNextDirectionsCommand = TOGGLE_HOLDING;
        userPigo[index].holdingAltitude = 30;
        userPigo[index].holdingTurnDirection = 0;
        userPigo[index].holdingTurnRadius = 30;
    }

    if (index == 1200) {
        userPigo[index].waypointNextDirectionsCommand = TOGGLE_HOLDING;
    }

    *commands = userPigo[index];

    index++;

    return true;
*/


    // Waypoint navigation test:

    /* Appending Test
    if (index == 0) {

        userPigo[index].waypointModifyFlightPathCommand = INITIALIZE_FLIGHT_PATH;

        userPigo[index].numWaypoints = 3;

        userPigo[index].waypoints[0].altitude = 50;
        userPigo[index].waypoints[0].waypointType = 0;
        userPigo[index].waypoints[0].longitude = -157.913905;
        userPigo[index].waypoints[0].latitude = 21.327517;
        userPigo[index].waypoints[0].turnRadius = 0;

        userPigo[index].waypoints[1].altitude = 50;
        userPigo[index].waypoints[1].waypointType = 0;
        userPigo[index].waypoints[1].longitude = -157.924788;
        userPigo[index].waypoints[1].latitude = 21.312405;
        userPigo[index].waypoints[1].turnRadius = 0;

        userPigo[index].waypoints[2].altitude = 20;
        userPigo[index].waypoints[2].waypointType = 0;
        userPigo[index].waypoints[2].longitude = -157.920966;
        userPigo[index].waypoints[2].latitude = 21.327437;
        userPigo[index].waypoints[2].turnRadius = 0;
    }

    if (index == 1) {

        userPigo[index].waypointModifyFlightPathCommand = APPEND;

        userPigo[index].numWaypoints = 1;

        userPigo[index].waypoints[0].altitude = 70;
        userPigo[index].waypoints[0].waypointType = 0;
        userPigo[index].waypoints[0].longitude = -157.912549;
        userPigo[index].waypoints[0].latitude = 21.317522;
        userPigo[index].waypoints[0].turnRadius = 0;

    }

    if (index == 2) {

        userPigo[index].waypointModifyFlightPathCommand = APPEND;

        userPigo[index].numWaypoints = 1;

        userPigo[index].waypoints[0].altitude = 50;
        userPigo[index].waypoints[0].waypointType = 0;
        userPigo[index].waypoints[0].longitude = -157.966143;
        userPigo[index].waypoints[0].latitude = 21.321520;
        userPigo[index].waypoints[0].turnRadius = 0;

    }
    */
    
    /* Insertion Test
    if (index == 0) {

        userPigo[index].waypointModifyFlightPathCommand = INITIALIZE_FLIGHT_PATH;

        userPigo[index].numWaypoints = 3;

        userPigo[index].waypoints[0].altitude = 50;
        userPigo[index].waypoints[0].waypointType = 0;
        userPigo[index].waypoints[0].longitude = -157.913905;
        userPigo[index].waypoints[0].latitude = 21.327517;
        userPigo[index].waypoints[0].turnRadius = 0;

        userPigo[index].waypoints[1].altitude = 50;
        userPigo[index].waypoints[1].waypointType = 0;
        userPigo[index].waypoints[1].longitude = -157.924788;
        userPigo[index].waypoints[1].latitude = 21.312405;
        userPigo[index].waypoints[1].turnRadius = 0;

        userPigo[index].waypoints[2].altitude = 20;
        userPigo[index].waypoints[2].waypointType = 0;
        userPigo[index].waypoints[2].longitude = -157.920966;
        userPigo[index].waypoints[2].latitude = 21.327437;
        userPigo[index].waypoints[2].turnRadius = 0;
    }

    if (index == 200) {

        userPigo[index].waypointModifyFlightPathCommand = INSERT;

        userPigo[index].numWaypoints = 1;

        userPigo[index].prevId = 1;
        userPigo[index].nextId = 2;

        userPigo[index].waypoints[0].altitude = 50;
        userPigo[index].waypoints[0].waypointType = 0;
        userPigo[index].waypoints[0].longitude = -157.927143;
        userPigo[index].waypoints[0].latitude = 21.321520;
        userPigo[index].waypoints[0].turnRadius = 0;

    }
    */

   /* Updating Test 
   if (index == 0) {

        userPigo[index].waypointModifyFlightPathCommand = INITIALIZE_FLIGHT_PATH;

        userPigo[index].numWaypoints = 3;

        userPigo[index].waypoints[0].altitude = 50;
        userPigo[index].waypoints[0].waypointType = 0;
        userPigo[index].waypoints[0].longitude = -157.913905;
        userPigo[index].waypoints[0].latitude = 21.327517;
        userPigo[index].waypoints[0].turnRadius = 0;

        userPigo[index].waypoints[1].altitude = 50;
        userPigo[index].waypoints[1].waypointType = 0;
        userPigo[index].waypoints[1].longitude = -157.924788;
        userPigo[index].waypoints[1].latitude = 21.312405;
        userPigo[index].waypoints[1].turnRadius = 0;

        userPigo[index].waypoints[2].altitude = 20;
        userPigo[index].waypoints[2].waypointType = 0;
        userPigo[index].waypoints[2].longitude = -157.920788;
        userPigo[index].waypoints[2].latitude = 21.327437;
        userPigo[index].waypoints[2].turnRadius = 0;
    }

    if (index == 200) {

        userPigo[index].waypointModifyFlightPathCommand = UPDATE;

        userPigo[index].numWaypoints = 1;

        userPigo[index].modifyId = 3;

        userPigo[index].waypoints[0].altitude = 50;
        userPigo[index].waypoints[0].waypointType = 0;
        userPigo[index].waypoints[0].longitude = -157.927966;
        userPigo[index].waypoints[0].latitude = 21.318520;
        userPigo[index].waypoints[0].turnRadius = 0;

    }

    if (index == 1000) {

        userPigo[index].waypointModifyFlightPathCommand = APPEND;

        userPigo[index].numWaypoints = 1;

        userPigo[index].waypoints[0].altitude = 70;
        userPigo[index].waypoints[0].waypointType = 0;
        userPigo[index].waypoints[0].longitude = -157.912549;
        userPigo[index].waypoints[0].latitude = 21.317522;
        userPigo[index].waypoints[0].turnRadius = 0;

    }

    */

   /* Deleting test 

   if (index == 0) {

        userPigo[index].waypointModifyFlightPathCommand = INITIALIZE_FLIGHT_PATH;

        userPigo[index].numWaypoints = 3;

        userPigo[index].waypoints[0].altitude = 50;
        userPigo[index].waypoints[0].waypointType = 0;
        userPigo[index].waypoints[0].longitude = -157.913905;
        userPigo[index].waypoints[0].latitude = 21.327517;
        userPigo[index].waypoints[0].turnRadius = 0;

        userPigo[index].waypoints[1].altitude = 50;
        userPigo[index].waypoints[1].waypointType = 0;
        userPigo[index].waypoints[1].longitude = -157.924788;
        userPigo[index].waypoints[1].latitude = 21.312405;
        userPigo[index].waypoints[1].turnRadius = 0;

        userPigo[index].waypoints[2].altitude = 20;
        userPigo[index].waypoints[2].waypointType = 0;
        userPigo[index].waypoints[2].longitude = -157.920788;
        userPigo[index].waypoints[2].latitude = 21.327437;
        userPigo[index].waypoints[2].turnRadius = 0;
    }

    if (index == 200) {

        userPigo[index].waypointModifyFlightPathCommand = APPEND;

        userPigo[index].numWaypoints = 1;

        userPigo[index].waypoints[0].altitude = 50;
        userPigo[index].waypoints[0].waypointType = 0;
        userPigo[index].waypoints[0].longitude = -157.927966;
        userPigo[index].waypoints[0].latitude = 21.318520;
        userPigo[index].waypoints[0].turnRadius = 0;

    }

    if (index == 1000) {

        userPigo[index].waypointModifyFlightPathCommand = DELETE;

        userPigo[index].numWaypoints = 0;

        userPigo[index].modifyId = 3;
    }

    */

   /* Home base
   if (index == 0) {

        userPigo[index].waypointModifyFlightPathCommand = INITIALIZE_FLIGHT_PATH;

        userPigo[index].numWaypoints = 3;
        userPigo[index].initializingHomeBase = true;
        
        userPigo[index].homebase.altitude = 50;
        userPigo[index].homebase.waypointType = 0;
        userPigo[index].homebase.longitude = -157.912549;
        userPigo[index].homebase.latitude = 21.317522;
        userPigo[index].homebase.turnRadius = 0;

        userPigo[index].waypoints[0].altitude = 50;
        userPigo[index].waypoints[0].waypointType = 0;
        userPigo[index].waypoints[0].longitude = -157.913905;
        userPigo[index].waypoints[0].latitude = 21.327517;
        userPigo[index].waypoints[0].turnRadius = 0;

        userPigo[index].waypoints[1].altitude = 50;
        userPigo[index].waypoints[1].waypointType = 0;
        userPigo[index].waypoints[1].longitude = -157.924788;
        userPigo[index].waypoints[1].latitude = 21.312405;
        userPigo[index].waypoints[1].turnRadius = 0;

        userPigo[index].waypoints[2].altitude = 20;
        userPigo[index].waypoints[2].waypointType = 0;
        userPigo[index].waypoints[2].longitude = -157.920966;
        userPigo[index].waypoints[2].latitude = 21.327437;
        userPigo[index].waypoints[2].turnRadius = 0;
    }

    if (index == 1) {

        userPigo[index].waypointModifyFlightPathCommand = APPEND;

        userPigo[index].numWaypoints = 1;

        userPigo[index].waypoints[0].altitude = 70;
        userPigo[index].waypoints[0].waypointType = 0;
        userPigo[index].waypoints[0].longitude = -157.912549;
        userPigo[index].waypoints[0].latitude = 21.317522;
        userPigo[index].waypoints[0].turnRadius = 0;

    }

    if (index == 2) {

        userPigo[index].waypointModifyFlightPathCommand = APPEND;

        userPigo[index].numWaypoints = 1;

        userPigo[index].waypoints[0].altitude = 50;
        userPigo[index].waypoints[0].waypointType = 0;
        userPigo[index].waypoints[0].longitude = -157.966143;
        userPigo[index].waypoints[0].latitude = 21.321520;
        userPigo[index].waypoints[0].turnRadius = 0;

    }

    if (index == 2000) {
        userPigo[index].waypointNextDirectionsCommand = TOGGLE_HEAD_HOME;
    }

    */
    *commands = userPigo[index];

    index++;

    return true;
}

