#include "pathStateClasses.hpp"



/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

bool isError; 
Telemetry_PIGO_t commsWithTelemetry::_incomingData;

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

void commsWithAttitude::execute(pathManager* pathMgr)
{
    //initial mode
    pathMgr->setState(commsWithTelemetry::getInstance());
}

pathManagerState& commsWithAttitude::getInstance()
{
    static commsWithAttitude singleton;
    return singleton;
}

void commsWithTelemetry::execute(pathManager* pathMgr)
{
    //send data to telemetry

    // Get data from telemetry

    // Do any processing required (update struct that contains telemetry data and process it)

    // Store data inside of the Telemetry_PIGO_t struct that is a parameter of this child class

    if(isError)
    {
        pathMgr->setState(fatalFailureMode::getInstance());
    }
    else
    {
        pathMgr->setState(getSensorData::getInstance());
    }
}

pathManagerState& commsWithTelemetry::getInstance()
{
    static commsWithTelemetry singleton;
    return singleton;
}

void getSensorData::execute(pathManager* pathMgr)
{
    //obtain sensor data
    if(isError)
    {
        pathMgr->setState(fatalFailureMode::getInstance());
    }
    else
    {
        pathMgr->setState(sensorFusion::getInstance());
    }
}

pathManagerState& getSensorData::getInstance()
{
    static getSensorData singleton;
    return singleton;
}

void sensorFusion::execute(pathManager* pathMgr)
{
    //fuse sensor data
    if(isError)
    {
        pathMgr->setState(fatalFailureMode::getInstance());
    }
    else
    {
        pathMgr->setState(cruisingState::getInstance());
    }
}

pathManagerState& sensorFusion::getInstance()
{
    static sensorFusion singleton;
    return singleton;
}

void cruisingState::execute(pathManager* pathMgr)
{

    Telemetry_PIGO_t * telemetryData = commsWithTelemetry::GetTelemetryIncomingData(); // Get struct from telemetry state with all of the commands.

    _WaypointOutputType outputType;
    _WaypointStatus operationStatus;

    /* Editing the flight path */

    if (telemetryData->numWaypoints == 1 && (telemetryData->waypointModifyFlightPathCommand >= 2 && telemetryData->waypointModifyFlightPathCommand <= 5)) { // Inserting, Appending, Updating, Deleting
        if (telemetryData->waypoints[0]->outputType == 0) {
            outputType = PATH_FOLLOW;
        } else if (telemetryData->waypoints[0]->outputType == 1) {
            outputType = ORBIT_FOLLOW;
        } else {
            outputType = HOLD_WAYPOINT;
        }

        // outputType = getOutputType(telemetryData->waypoints[0]->outputType); // Get the enum value of the output type of the new waypoint
        _PathData * modifyWaypoint = cruisingStateManager.initialize_waypoint(telemetryData->waypoints[0]->longitude, telemetryData->waypoints[0]->latitude, telemetryData->waypoints[0]->altitude, outputType); // Create a _PathData object
        
        // Update flight path by calling the appropriate method
        if (telemetryData->waypointModifyFlightPathCommand == 2) { // Append
            operationStatus = cruisingStateManager.update_path_nodes(modifyWaypoint, APPEND_WAYPOINT, 0, 0, 0);
        } else if (telemetryData->waypointModifyFlightPathCommand == 3) { // Insert
            operationStatus = cruisingStateManager.update_path_nodes(modifyWaypoint, INSERT_WAYPOINT, 0, telemetryData->prevId, telemetryData->nextId);
        } else if (telemetryData->waypointModifyFlightPathCommand == 4) { // Update
            operationStatus = cruisingStateManager.update_path_nodes(modifyWaypoint, UPDATE_WAYPOINT, telemetryData->modifyId, 0, 0);
        } else { // Delete
            operationStatus = cruisingStateManager.update_path_nodes(modifyWaypoint, DELETE_WAYPOINT, telemetryData->modifyId, 0, 0);
        }
    } else if (telemetryData->numWaypoints > 1 && telemetryData->waypointModifyFlightPathCommand == 1) { // Initialize flight path array

        _PathData * newFlightPath[PATH_BUFFER_SIZE];

        for(int i = 0; i < telemetryData->numWaypoints; i++) {
            if (telemetryData->waypoints[i]->outputType == 0) {
                outputType = PATH_FOLLOW;
            } else if (telemetryData->waypoints[i]->outputType == 1) {
                outputType = ORBIT_FOLLOW;
            } else {
                outputType = HOLD_WAYPOINT;
            }
            // outputType = getOutputType(telemetryData->waypoints[i]->outputType);
            newFlightPath[i] = cruisingStateManager.initialize_waypoint(telemetryData->waypoints[i]->longitude, telemetryData->waypoints[i]->latitude, telemetryData->waypoints[i]->altitude, outputType); 
        }

        // Nukes current flight path (ensures there are no conflicts)
        cruisingStateManager.clear_path_nodes();

        if (telemetryData->initializingHomeBase) { // Updating home base too
            // Remove current home base
            cruisingStateManager.clear_home_base();

            // Create _PathData object for the homebase
            if (telemetryData->homebase.outputType == 0) {
                outputType = PATH_FOLLOW;
            } else if (telemetryData->homebase.outputType == 1) {
                outputType = ORBIT_FOLLOW;
            } else {
                outputType = HOLD_WAYPOINT;
            }
            // outputType = getOutputType(telemetryData->waypoints[0]->outputType); // Get the enum value of the output type of the new waypoint
            _PathData * newHomeBase = cruisingStateManager.initialize_waypoint(telemetryData->homebase.longitude, telemetryData->homebase.latitude, telemetryData->homebase.altitude, outputType); 
        
            // Call method
            operationStatus = cruisingStateManager.initialize_flight_path(newFlightPath, telemetryData->numWaypoints, newHomeBase);

        } else { // Only initializing the flight path
            operationStatus = cruisingStateManager.initialize_flight_path(newFlightPath, telemetryData->numWaypoints);
        }

    } else if (telemetryData->numWaypoints == 0 && telemetryData->waypointModifyFlightPathCommand == 6) {  // Nuke flight path
        cruisingStateManager.clear_path_nodes();
    } else if (telemetryData->numWaypoints != 0) { // Set error code saying invalid command

    }
    
    /* Path following data */
    
    _WaypointManager_Data_In input;
    _WaypointManager_Data_Out output;

    /*
    
     Set input struct values (variables are not in it rn)

    */

    if (telemetryData->waypointNextDirectionsCommand == 0) { // Regular path following
        operationStatus = cruisingStateManager.get_next_directions(input, &output);
    } else if (telemetryData->waypointNextDirectionsCommand == 1) { // Start holding
        if (inHold) {
            inHold = false;
        } else {
            inHold = true;
        }

        operationStatus = cruisingStateManager.start_circling(input, telemetryData->holdingTurnRadius, telemetryData->holdingTurnDirection, telemetryData->holdingAltitude, inHold);
    } else if (telemetryData->waypointNextDirectionsCommand == 2) { // Heading home
        if (goingHome) {
            goingHome = false;
        } else {
            goingHome = true;
        }
        _HeadHomeStatus goingHomeStatus;
        goingHomeStatus = cruisingStateManager.head_home(goingHome);

        if (goingHomeStatus == HOME_UNDEFINED_PARAMETER) {
            // Set error code
        }
    } else { // Return error code

    }

    // Update the id array that is a parameter of this child class

    if(isError)
    {
        pathMgr->setState(fatalFailureMode::getInstance());
    }
    else
    {
        pathMgr->setState(coordinateTurnElevation::getInstance());
    }
}

pathManagerState& cruisingState::getInstance()
{
    static cruisingState singleton;
    return singleton;
}

void coordinateTurnElevation::execute(pathManager* pathMgr)
{
    //get elevation and turning data
    if(isError)
    {
        pathMgr->setState(fatalFailureMode::getInstance());
    }
    else
    {
        pathMgr->setState(commsWithAttitude::getInstance());
    }
}

pathManagerState& coordinateTurnElevation::getInstance()
{
    static coordinateTurnElevation singleton;
    return singleton;
}

void fatalFailureMode::execute(pathManager* pathMgr)
{
    pathMgr->setState(fatalFailureMode::getInstance());
}

pathManagerState& fatalFailureMode::getInstance()
{
    static fatalFailureMode singleton;
    return singleton;
}


