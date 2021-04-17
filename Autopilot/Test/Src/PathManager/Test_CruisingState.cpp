/*
* Author: Dhruv Rawat
*/

#include <gtest/gtest.h>

#include "pathManager.hpp"
#include "pathStateClasses.hpp"
#include "waypointManager.hpp"
#include "cruisingState.hpp"
#include "pathDatatypes.hpp"

using namespace std; 
using ::testing::Test;

/*

    Cases we want to test:

        - Incorrect telemetry commands     
        
        // Editing
        - Initialzing the flight path      
        - Nuking the flight path
        - Appending (also test failing)    
        - Inserting (also test failing)    
        - Updating (also test failing)     
        - Deleting (also test failing)   

        // Flight
        - Get next direction
        - Start circling (also cancel)
        - Start going home (also cancel)

*/

Telemetry_PIGO_t TelemetryTestData;
_CruisingState_Telemetry_Return TestReturnToGround;
_WaypointManager_Data_In TestInputData;
_WaypointManager_Data_Out TestOutputData; 


/*** HELPER FUNCTIONS ***/


enum _ArrayStatus {ARRAY_SUCCESS = 0, ARRAY_DIFFERENT};
enum _ReturnToGroundStatus {RETURN_SUCCESS = 0, RETURN_DIFFERENT};
enum _OutputStatus {OUTPUT_CORRECT = 0, OUTPUT_INCORRECT};
enum _WaypointSt {WAYPOINT_CORRECT = 0, WAYPOINT_INCORRECT};

static _ArrayStatus compare_id_arrays (int * toCheck, int * ans) {
    for (int i = 0; i < PATH_BUFFER_SIZE; i++) {
        if (toCheck[i] != ans[i]) {
            // cout << i << " | " << toCheck[i] << " ; ans: " << ans[i] << endl;
            return ARRAY_DIFFERENT;
        }
    }

    return ARRAY_SUCCESS;
}

static _OutputStatus compare_output_data(_WaypointManager_Data_Out *ans, _WaypointManager_Data_Out *test) {
    if(ans->desiredAltitude == test->desiredAltitude && ans->desiredTrack == test->desiredTrack && ans->distanceToNextWaypoint == round(test->distanceToNextWaypoint) && ans->radius == test->radius && ans->turnDirection == test->turnDirection && ans->out_type == test->out_type) {
        return OUTPUT_CORRECT;
    } else {
        // cout << "Comparing Output Data: Alt " << ans->desiredAltitude << " " << test->desiredAltitude << " | Heading " << ans->desiredTrack << " " << test->desiredTrack << " | Distance " << ans->distanceToNextWaypoint << " " << test->distanceToNextWaypoint << " | Radius " << ans->radius << " " << test->radius << " | Direction " << ans->turnDirection << " " << test->turnDirection << " | OutType " << ans->out_type << " " << test->out_type << endl;
        return OUTPUT_INCORRECT;
    }
}

static _ArrayStatus compare_arrays(_PathData ** ans, _PathData ** testArray, int numElements) {
    // Checks if entire array is empty if there are no elements
    if (numElements == 0) {
        for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
            if (testArray[i] || ans[i]) {
                return ARRAY_DIFFERENT;
            }
        }
        return ARRAY_SUCCESS;
    }

    _PathData * nextWaypoint = testArray[0];

    // Checks if nexts are linked properly
    for(int i = 0; i < numElements; i++) {
        if(ans[i]->waypointId == nextWaypoint->waypointId && ans[i]->longitude == nextWaypoint->longitude && ans[i]->latitude == nextWaypoint->latitude && ans[i]->altitude == nextWaypoint->altitude && ans[i]->waypointType == nextWaypoint->waypointType && ans[i]->turnRadius == nextWaypoint->turnRadius) {
            nextWaypoint = nextWaypoint->next;
        } else {
            // cout << "Next Check Index: " << i << " | " << ans[i]->waypointId << " " << nextWaypoint->waypointId << " | " << ans[i]->longitude << " " << nextWaypoint->longitude << " | " << ans[i]->latitude << " " << nextWaypoint->latitude << " | " << ans[i]->altitude << " " << nextWaypoint->altitude << " | " << ans[i]->waypointType << " " << nextWaypoint->waypointType << " | " << ans[i]->turnRadius << " " << nextWaypoint->turnRadius << endl;
            return ARRAY_DIFFERENT;
        }
    }

    nextWaypoint = testArray[numElements - 1];

    // Checks if previous are linked properly
    for(int i = numElements-1; i >= 0; i--) {
        if(ans[i]->waypointId == nextWaypoint->waypointId && ans[i]->longitude == nextWaypoint->longitude && ans[i]->latitude == nextWaypoint->latitude && ans[i]->altitude == nextWaypoint->altitude && ans[i]->waypointType == nextWaypoint->waypointType && ans[i]->turnRadius == nextWaypoint->turnRadius) {
            nextWaypoint = nextWaypoint->previous;
        } else {
            // // cout << "Previous Check Index: " << i << " | " << ans[i]->waypointId << " " << nextWaypoint->waypointId << " | " << ans[i]->longitude << " " << nextWaypoint->longitude << " | " << ans[i]->latitude << " " << nextWaypoint->latitude << " | " << ans[i]->altitude << " " << nextWaypoint->altitude << " | " << ans[i]->waypointType << " " << nextWaypoint->waypointType << " | " << ans[i]->turnRadius << " " << nextWaypoint->turnRadius << endl;
            return ARRAY_DIFFERENT;
        }
    }

    // Checks if indexes are the same
    for(int i = 0; i < numElements; i++) {
        nextWaypoint = testArray[i];
        if(ans[i]->waypointId != nextWaypoint->waypointId && ans[i]->longitude != nextWaypoint->longitude && ans[i]->latitude != nextWaypoint->latitude && ans[i]->altitude != nextWaypoint->altitude && ans[i]->waypointType != nextWaypoint->waypointType && ans[i]->turnRadius != nextWaypoint->turnRadius) {
            // // cout << "Array Equality Check Index: " << i << " | " << ans[i]->waypointId << " " << nextWaypoint->waypointId << " | " << ans[i]->longitude << " " << nextWaypoint->longitude << " | " << ans[i]->latitude << " " << nextWaypoint->latitude << " | " << ans[i]->altitude << " " << nextWaypoint->altitude << " | " << ans[i]->waypointType << " " << nextWaypoint->waypointType << " | " << ans[i]->turnRadius << " " << nextWaypoint->turnRadius << endl;
            return ARRAY_DIFFERENT;
        }
    }

    return ARRAY_SUCCESS;
}

static _ArrayStatus compare_buffer_status(_WaypointBufferStatus * ans, WaypointManager& waypointManagerInstance) {
    // // cout << "Here" << endl;
    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        if(ans[i] != waypointManagerInstance.get_status_of_index(i)) {
            // // cout << "Buffer Compare Index: " << i << " | " << ans[i] << " " << waypointManagerInstance.get_status_of_index(i) << endl;
            return ARRAY_DIFFERENT;
        }
    }

    return ARRAY_SUCCESS;
} 

static _WaypointSt compare_waypoint(_PathData * ans, _PathData * test) {
    if(ans->waypointId != test->waypointId && ans->longitude != test->longitude && ans->latitude != test->latitude && ans->altitude != test->altitude && ans->waypointType != test->waypointType && ans->turnRadius != test->turnRadius) {
        // // cout << "Waypoint Equality Check: " << ans->waypointId << " " << test->waypointId << " | " << ans->longitude << " " << test->longitude << " | " << ans->latitude << " " << test->latitude << " | " << ans->altitude << " " << test->altitude << " | " << ans->waypointType << " " << test->waypointType << " | " << ans->turnRadius << " " << test->turnRadius << endl;
        return WAYPOINT_INCORRECT;
    }

    return WAYPOINT_CORRECT;
}

static Telemetry_Waypoint_Data_t * createTelemetryWaypoint(long double lon, long double lat, int alt, float turnRadius, int type) {
    Telemetry_Waypoint_Data_t * waypoint = new Telemetry_Waypoint_Data_t {};

    waypoint->longitude = lon;
    waypoint->latitude = lat;
    waypoint->altitude = alt;
    waypoint->turnRadius = turnRadius;
    waypoint->waypointType = type;

    return waypoint;
}


/*** TESTS ***/


TEST (CruisingState, IncorrectTelemetryCommandsReturnErrorCode) {
    /***********************SETUP***********************/

	TelemetryTestData.numWaypoints = 1;
    TelemetryTestData.waypointModifyFlightPathCommand = (_ModifyFlightPathCommand) 7;
    TelemetryTestData.initializingHomeBase = 0;
    TelemetryTestData.waypointNextDirectionsCommand = (_GetNextDirectionsCommand) 3;
    TelemetryTestData.holdingAltitude = 0;
    TelemetryTestData.holdingTurnRadius = 0;
    TelemetryTestData.holdingTurnDirection = 0;
    TelemetryTestData.nextId = 0;
    TelemetryTestData.prevId = 0;
    TelemetryTestData.modifyId = 0;
    TelemetryTestData.waypoints[0] = createTelemetryWaypoint(0.0, 0.0, 6, 0.0, 0);
    TelemetryTestData.homebase = *(createTelemetryWaypoint(0.0, 0.0, 100, 0.0, 0));

	/********************DEPENDENCIES*******************/	
	/********************STEPTHROUGH********************/

    int idArray[PATH_BUFFER_SIZE];
    WaypointManager cruisingStateManager;
    bool goingHome = false, inHold = false; 

    _ModifyFlightPathErrorCode editError = editFlightPath(&TelemetryTestData, cruisingStateManager, idArray);
    _GetNextDirectionsErrorCode pathError = pathFollow(&TelemetryTestData, cruisingStateManager, TestInputData, &TestOutputData, goingHome, inHold);
    setReturnValues(&TestReturnToGround, cruisingStateManager, editError, pathError);

    // Remove heap-allocated memory
    cruisingStateManager.clear_path_nodes();
    cruisingStateManager.clear_home_base();
    delete TelemetryTestData.waypoints[0];

	/**********************ASSERTS**********************/

    EXPECT_EQ(TestReturnToGround.editingFlightPathErrorCode, 2);
    EXPECT_EQ(TestReturnToGround.pathFollowingErrorCode, 2);
    EXPECT_EQ(TestReturnToGround.currentWaypointId, 0);
    EXPECT_EQ(TestReturnToGround.currentWaypointIndex, 0);
    EXPECT_EQ(TestReturnToGround.homeBaseInitialized, 0);
}

TEST (CruisingState, InitializeFlightPathSuccess) {
    /***********************SETUP***********************/

    WaypointManager cruisingStateManager;

    // Set up input telemetry data
    TelemetryTestData.waypoints[0] = createTelemetryWaypoint(0.0, 0.0, 6, 0.0, 0);
    TelemetryTestData.waypoints[1] = createTelemetryWaypoint(0.0, 0.0, 7, 0.0, 0);
    TelemetryTestData.waypoints[2] = createTelemetryWaypoint(0.0, 0.0, 8, 0.0, 0);
    TelemetryTestData.waypoints[3] = createTelemetryWaypoint(0.0, 0.0, 9, 0.0, 0);

	TelemetryTestData.numWaypoints = 4;
    TelemetryTestData.waypointModifyFlightPathCommand = INITIALIZE_FLIGHT_PATH;
    TelemetryTestData.initializingHomeBase = 1;
    TelemetryTestData.waypointNextDirectionsCommand = REGULAR_PATH_FOLLOWING;
    TelemetryTestData.holdingAltitude = 0;
    TelemetryTestData.holdingTurnRadius = 0;
    TelemetryTestData.holdingTurnDirection = 0;
    TelemetryTestData.nextId = 0;
    TelemetryTestData.prevId = 0;
    TelemetryTestData.modifyId = 0;
    TelemetryTestData.homebase = *(createTelemetryWaypoint(0.0, 0.0, 100, 0.0, 0));

    // Set up id test array (desired result)
    int idTestArray[PATH_BUFFER_SIZE];
    idTestArray[0] = 1;
    idTestArray[1] = 2;
    idTestArray[2] = 3;
    idTestArray[3] = 4;
    for (int i = 4; i < PATH_BUFFER_SIZE; i++) {
        idTestArray[i] = 0;
    }

    // Set up flight path array (desired result)

    WaypointManager testWaypointManager; // Use separate waypoint manager object for comparisions. This ensures that the autoincrements of waypoint ids don't make the tests fail

    _PathData * testFlightPath[PATH_BUFFER_SIZE];
    testFlightPath[0] = testWaypointManager.initialize_waypoint(0.0, 0.0, 6, PATH_FOLLOW);
    testFlightPath[1] = testWaypointManager.initialize_waypoint(0.0, 0.0, 7, PATH_FOLLOW);
    testFlightPath[2] = testWaypointManager.initialize_waypoint(0.0, 0.0, 8, PATH_FOLLOW);
    testFlightPath[3] = testWaypointManager.initialize_waypoint(0.0, 0.0, 9, PATH_FOLLOW);

    // Set up flight path buffer (desired result)
    _WaypointBufferStatus testStatus[PATH_BUFFER_SIZE];
    testStatus[0] = FULL;
    testStatus[1] = FULL;
    testStatus[2] = FULL;
    testStatus[3] = FULL;
    for (int i = 4; i < PATH_BUFFER_SIZE; i++) {
        testStatus[i] = FREE;
    }

    // Set up home base (desired result)
    _PathData * testHomeBase = testWaypointManager.initialize_waypoint(0.0, 0.0, 100, PATH_FOLLOW);
    
    // Variables that will be modified as we go
    int idArray[PATH_BUFFER_SIZE] = { 0 };
    bool goingHome = false, inHold = false; 

	/********************DEPENDENCIES*******************/	
	/********************STEPTHROUGH********************/

    _ModifyFlightPathErrorCode editError = editFlightPath(&TelemetryTestData, cruisingStateManager, idArray);
    _GetNextDirectionsErrorCode pathError = pathFollow(&TelemetryTestData, cruisingStateManager, TestInputData, &TestOutputData, goingHome, inHold);
    setReturnValues(&TestReturnToGround, cruisingStateManager, editError, pathError);

    _ArrayStatus idArrayComparision = compare_id_arrays(idArray, idTestArray);    
    
    _PathData ** flightPath = cruisingStateManager.get_waypoint_buffer();
    _ArrayStatus flightPathComparision = compare_arrays(testFlightPath, flightPath, 4);
    _ArrayStatus flightPathStatusComparision = compare_buffer_status(testStatus, cruisingStateManager);

    _PathData * home = cruisingStateManager.get_home_base();
    _WaypointSt homeBaseComparision = compare_waypoint(testHomeBase, home);
    
    // Remove heap-allocated memory 
    cruisingStateManager.clear_path_nodes();
    delete testHomeBase;
    delete testFlightPath[0]; delete testFlightPath[1]; delete testFlightPath[2]; delete testFlightPath[3];
    delete TelemetryTestData.waypoints[0]; delete TelemetryTestData.waypoints[1]; delete TelemetryTestData.waypoints[2]; delete TelemetryTestData.waypoints[3];

	/**********************ASSERTS**********************/

    EXPECT_EQ(TestReturnToGround.editingFlightPathErrorCode, 0);
    EXPECT_EQ(TestReturnToGround.pathFollowingErrorCode, 0);
    EXPECT_EQ(TestReturnToGround.currentWaypointId, 3);
    EXPECT_EQ(TestReturnToGround.currentWaypointIndex, 2); // My dumbass when testing the waypoint manager made the currentWaypointIndex = 2 when calling this method
    EXPECT_EQ(TestReturnToGround.homeBaseInitialized, true);

    EXPECT_EQ(idArrayComparision, ARRAY_SUCCESS); // Need to figure out how to pass arrays by reference
    EXPECT_EQ(flightPathComparision, ARRAY_SUCCESS);
    EXPECT_EQ(flightPathStatusComparision, ARRAY_SUCCESS);
    EXPECT_EQ(homeBaseComparision, WAYPOINT_CORRECT);
}

TEST (CruisingState, NukeFlightPathSuccess) {
    /***********************SETUP***********************/

    WaypointManager cruisingStateManager;

    // Set up input telemetry data
    TelemetryTestData.waypoints[0] = createTelemetryWaypoint(0.0, 0.0, 6, 0.0, 0);
    TelemetryTestData.waypoints[1] = createTelemetryWaypoint(0.0, 0.0, 7, 0.0, 0);
    TelemetryTestData.waypoints[2] = createTelemetryWaypoint(0.0, 0.0, 8, 0.0, 0);
    TelemetryTestData.waypoints[3] = createTelemetryWaypoint(0.0, 0.0, 9, 0.0, 0);

	TelemetryTestData.numWaypoints = 4;
    TelemetryTestData.waypointModifyFlightPathCommand = INITIALIZE_FLIGHT_PATH;
    TelemetryTestData.initializingHomeBase = 0;
    TelemetryTestData.waypointNextDirectionsCommand = (_GetNextDirectionsCommand) 10;
    TelemetryTestData.holdingAltitude = 0;
    TelemetryTestData.holdingTurnRadius = 0;
    TelemetryTestData.holdingTurnDirection = 0;
    TelemetryTestData.nextId = 0;
    TelemetryTestData.prevId = 0;
    TelemetryTestData.modifyId = 0;
    TelemetryTestData.homebase = *(createTelemetryWaypoint(0.0, 0.0, 100, 0.0, 0));

    // Set up id test array (desired result)
    int idTestArray[PATH_BUFFER_SIZE];

    // Set up flight path array (desired result)

    _PathData * testFlightPath[PATH_BUFFER_SIZE];
    // Set up flight path buffer (desired result)
    _WaypointBufferStatus testStatus[PATH_BUFFER_SIZE];
    
    for (int i = 0; i < PATH_BUFFER_SIZE; i++) {
        idTestArray[i] = 0;
        testFlightPath[i] = nullptr;
        testStatus[i] = FREE;
    }
    
    // Variables that will be modified as we go
    int idArray[PATH_BUFFER_SIZE] = { 0 };
    bool goingHome = false, inHold = false; 

	/********************DEPENDENCIES*******************/	
	/********************STEPTHROUGH********************/

    // Initialize waypoint manager for cruisingState
    _ModifyFlightPathErrorCode editError = editFlightPath(&TelemetryTestData, cruisingStateManager, idArray);

    TelemetryTestData.numWaypoints = 0;
    TelemetryTestData.waypointModifyFlightPathCommand = NUKE;

    editError = editFlightPath(&TelemetryTestData, cruisingStateManager, idArray);
    _GetNextDirectionsErrorCode pathError = pathFollow(&TelemetryTestData, cruisingStateManager, TestInputData, &TestOutputData, goingHome, inHold);
    setReturnValues(&TestReturnToGround, cruisingStateManager, editError, pathError);

    _ArrayStatus idArrayComparision = compare_id_arrays(idArray, idTestArray);    
    
    _PathData ** flightPath = cruisingStateManager.get_waypoint_buffer();
    _ArrayStatus flightPathComparision = compare_arrays(testFlightPath, flightPath, 0);
    _ArrayStatus flightPathStatusComparision = compare_buffer_status(testStatus, cruisingStateManager);

    delete TelemetryTestData.waypoints[0]; delete TelemetryTestData.waypoints[1]; delete TelemetryTestData.waypoints[2]; delete TelemetryTestData.waypoints[3];

	/**********************ASSERTS**********************/

    EXPECT_EQ(TestReturnToGround.editingFlightPathErrorCode, 0);
    EXPECT_EQ(TestReturnToGround.pathFollowingErrorCode, 2);
    EXPECT_EQ(TestReturnToGround.currentWaypointId, 0);
    EXPECT_EQ(TestReturnToGround.currentWaypointIndex, 0); // My dumbass when testing the waypoint manager made the currentWaypointIndex = 2 when calling this method
    EXPECT_EQ(TestReturnToGround.homeBaseInitialized, false);

    EXPECT_EQ(idArrayComparision, ARRAY_SUCCESS); // Need to figure out how to pass arrays by reference
    EXPECT_EQ(flightPathComparision, ARRAY_SUCCESS);
    EXPECT_EQ(flightPathStatusComparision, ARRAY_SUCCESS);
}

TEST (CruisingState, AppendWaypointSuccess) {
    /***********************SETUP***********************/

    WaypointManager cruisingStateManager;

    // Set up input telemetry data
    TelemetryTestData.waypoints[0] = createTelemetryWaypoint(0.0, 0.0, 6, 0.0, 0);
    TelemetryTestData.waypoints[1] = createTelemetryWaypoint(0.0, 0.0, 7, 0.0, 0);
    TelemetryTestData.waypoints[2] = createTelemetryWaypoint(0.0, 0.0, 8, 0.0, 0);
    TelemetryTestData.waypoints[3] = createTelemetryWaypoint(0.0, 0.0, 9, 0.0, 0);

	TelemetryTestData.numWaypoints = 4;
    TelemetryTestData.waypointModifyFlightPathCommand = INITIALIZE_FLIGHT_PATH;
    TelemetryTestData.initializingHomeBase = 0;
    TelemetryTestData.waypointNextDirectionsCommand = (_GetNextDirectionsCommand) 10; // Don't want to get next directions, so set it to return error
    TelemetryTestData.holdingAltitude = 0;
    TelemetryTestData.holdingTurnRadius = 0;
    TelemetryTestData.holdingTurnDirection = 0;
    TelemetryTestData.nextId = 0;
    TelemetryTestData.prevId = 0;
    TelemetryTestData.modifyId = 0;
    TelemetryTestData.homebase = *(createTelemetryWaypoint(0.0, 0.0, 100, 0.0, 0));

    // Set up id test array (desired result)
    int idTestArray[PATH_BUFFER_SIZE] = { 0 };
    idTestArray[0] = 1;
    idTestArray[1] = 2;
    idTestArray[2] = 3;
    idTestArray[3] = 4;
    idTestArray[4] = 5;
    
    // Set up flight path array (desired result)

    WaypointManager testWaypointManager; // Use separate waypoint manager object for comparisions. This ensures that the autoincrements of waypoint ids don't make the tests fail

    _PathData * testFlightPath[PATH_BUFFER_SIZE];
    testFlightPath[0] = testWaypointManager.initialize_waypoint(0.0, 0.0, 6, PATH_FOLLOW);
    testFlightPath[1] = testWaypointManager.initialize_waypoint(0.0, 0.0, 7, PATH_FOLLOW);
    testFlightPath[2] = testWaypointManager.initialize_waypoint(0.0, 0.0, 8, PATH_FOLLOW);
    testFlightPath[3] = testWaypointManager.initialize_waypoint(0.0, 0.0, 9, PATH_FOLLOW);
    testFlightPath[4] = testWaypointManager.initialize_waypoint(1.0, 0.0, 10, PATH_FOLLOW);

    // Set up flight path buffer (desired result)
    _WaypointBufferStatus testStatus[PATH_BUFFER_SIZE];
    testStatus[0] = FULL;
    testStatus[1] = FULL;
    testStatus[2] = FULL;
    testStatus[3] = FULL;
    testStatus[4] = FULL;
    for (int i = 5; i < PATH_BUFFER_SIZE; i++) {
        testStatus[i] = FREE;
        testFlightPath[i] = nullptr;
    }
    
    // Variables that will be modified as we go
    int idArray[PATH_BUFFER_SIZE] = {0};
    bool goingHome = false, inHold = false; 

	/********************DEPENDENCIES*******************/	
	/********************STEPTHROUGH********************/

    // Initialize waypoint manager for cruisingState
    _ModifyFlightPathErrorCode editError = editFlightPath(&TelemetryTestData, cruisingStateManager, idArray);

    TelemetryTestData.numWaypoints = 1;
    TelemetryTestData.waypointModifyFlightPathCommand = APPEND; // Appending

    delete TelemetryTestData.waypoints[0]; delete TelemetryTestData.waypoints[1]; delete TelemetryTestData.waypoints[2]; delete TelemetryTestData.waypoints[3];
    TelemetryTestData.waypoints[0] = createTelemetryWaypoint(1.0, 0.0, 10, 0.0, 0);  

    editError = editFlightPath(&TelemetryTestData, cruisingStateManager, idArray);
    _GetNextDirectionsErrorCode pathError = pathFollow(&TelemetryTestData, cruisingStateManager, TestInputData, &TestOutputData, goingHome, inHold);
    setReturnValues(&TestReturnToGround, cruisingStateManager, editError, pathError);

    _ArrayStatus idArrayComparision = compare_id_arrays(idArray, idTestArray);    
    
    _PathData ** flightPath = cruisingStateManager.get_waypoint_buffer();
    _ArrayStatus flightPathComparision = compare_arrays(testFlightPath, flightPath, 5);
    _ArrayStatus flightPathStatusComparision = compare_buffer_status(testStatus, cruisingStateManager);

    // Remove heap-allocated stuff
    cruisingStateManager.clear_path_nodes();
    delete testFlightPath[0]; delete testFlightPath[1]; delete testFlightPath[2]; delete testFlightPath[3]; delete testFlightPath[4];
    delete TelemetryTestData.waypoints[0];

	/**********************ASSERTS**********************/

    EXPECT_EQ(TestReturnToGround.editingFlightPathErrorCode, 0);
    EXPECT_EQ(TestReturnToGround.pathFollowingErrorCode, 2);
    EXPECT_EQ(TestReturnToGround.currentWaypointId, 3);
    EXPECT_EQ(TestReturnToGround.currentWaypointIndex, 2); // My dumbass when testing the waypoint manager made the currentWaypointIndex = 2 when calling this method
    EXPECT_EQ(TestReturnToGround.homeBaseInitialized, false);

    EXPECT_EQ(idArrayComparision, ARRAY_SUCCESS); // Need to figure out how to pass arrays by reference
    EXPECT_EQ(flightPathComparision, ARRAY_SUCCESS);
    EXPECT_EQ(flightPathStatusComparision, ARRAY_SUCCESS);
}

TEST (CruisingState, AppendWaypointFail) {
    /***********************SETUP***********************/

    WaypointManager cruisingStateManager;

    // Set up input telemetry data
    TelemetryTestData.waypoints[0] = createTelemetryWaypoint(0.0, 0.0, 6, 0.0, 0);
    TelemetryTestData.waypoints[1] = createTelemetryWaypoint(0.0, 0.0, 7, 0.0, 0);
    TelemetryTestData.waypoints[2] = createTelemetryWaypoint(0.0, 0.0, 8, 0.0, 0);
    TelemetryTestData.waypoints[3] = createTelemetryWaypoint(0.0, 0.0, 9, 0.0, 0);

	TelemetryTestData.numWaypoints = 4;
    TelemetryTestData.waypointModifyFlightPathCommand = INITIALIZE_FLIGHT_PATH;
    TelemetryTestData.initializingHomeBase = 0;
    TelemetryTestData.waypointNextDirectionsCommand = (_GetNextDirectionsCommand) 10; // Don't want to get next directions, so set it to return error
    TelemetryTestData.holdingAltitude = 0;
    TelemetryTestData.holdingTurnRadius = 0;
    TelemetryTestData.holdingTurnDirection = 0;
    TelemetryTestData.nextId = 0;
    TelemetryTestData.prevId = 0;
    TelemetryTestData.modifyId = 0;
    TelemetryTestData.homebase = *(createTelemetryWaypoint(0.0, 0.0, 100, 0.0, 0));

    // Set up id test array (desired result)
    int idTestArray[PATH_BUFFER_SIZE] = { 0 };
    idTestArray[0] = 1;
    idTestArray[1] = 2;
    idTestArray[2] = 3;
    idTestArray[3] = 4;
    
    // Set up flight path array (desired result)

    WaypointManager testWaypointManager; // Use separate waypoint manager object for comparisions. This ensures that the autoincrements of waypoint ids don't make the tests fail

    _PathData * testFlightPath[PATH_BUFFER_SIZE];
    testFlightPath[0] = testWaypointManager.initialize_waypoint(0.0, 0.0, 6, PATH_FOLLOW);
    testFlightPath[1] = testWaypointManager.initialize_waypoint(0.0, 0.0, 7, PATH_FOLLOW);
    testFlightPath[2] = testWaypointManager.initialize_waypoint(0.0, 0.0, 8, PATH_FOLLOW);
    testFlightPath[3] = testWaypointManager.initialize_waypoint(0.0, 0.0, 9, PATH_FOLLOW);

    // Set up flight path buffer (desired result)
    _WaypointBufferStatus testStatus[PATH_BUFFER_SIZE];
    testStatus[0] = FULL;
    testStatus[1] = FULL;
    testStatus[2] = FULL;
    testStatus[3] = FULL;
    for (int i = 4; i < PATH_BUFFER_SIZE; i++) {
        testStatus[i] = FREE;
        testFlightPath[i] = nullptr;
    }
    
    // Variables that will be modified as we go
    int idArray[PATH_BUFFER_SIZE] = { 0 };
    bool goingHome = false, inHold = false; 

	/********************DEPENDENCIES*******************/	
	/********************STEPTHROUGH********************/

    // Initialize waypoint manager for cruisingState
    _ModifyFlightPathErrorCode editError = editFlightPath(&TelemetryTestData, cruisingStateManager, idArray);

    TelemetryTestData.numWaypoints = 1;
    TelemetryTestData.waypointModifyFlightPathCommand = APPEND; // Appending

    delete TelemetryTestData.waypoints[0]; delete TelemetryTestData.waypoints[1]; delete TelemetryTestData.waypoints[2]; delete TelemetryTestData.waypoints[3];
    TelemetryTestData.waypoints[0] = createTelemetryWaypoint(0.0, 0.0, 10, 0.0, 0); // Duplicate, so it will not append

    editError = editFlightPath(&TelemetryTestData, cruisingStateManager, idArray);
    _GetNextDirectionsErrorCode pathError = pathFollow(&TelemetryTestData, cruisingStateManager, TestInputData, &TestOutputData, goingHome, inHold);
    setReturnValues(&TestReturnToGround, cruisingStateManager, editError, pathError);

    _ArrayStatus idArrayComparision = compare_id_arrays(idArray, idTestArray);    
    
    _PathData ** flightPath = cruisingStateManager.get_waypoint_buffer();
    _ArrayStatus flightPathComparision = compare_arrays(testFlightPath, flightPath, 4);
    _ArrayStatus flightPathStatusComparision = compare_buffer_status(testStatus, cruisingStateManager);

    // Remove heap-allocated stuff
    cruisingStateManager.clear_path_nodes();
    delete testFlightPath[0]; delete testFlightPath[1]; delete testFlightPath[2]; delete testFlightPath[3];
    delete TelemetryTestData.waypoints[0];

	/**********************ASSERTS**********************/

    EXPECT_EQ(TestReturnToGround.editingFlightPathErrorCode, 1);
    EXPECT_EQ(TestReturnToGround.pathFollowingErrorCode, 2);
    EXPECT_EQ(TestReturnToGround.currentWaypointId, 3);
    EXPECT_EQ(TestReturnToGround.currentWaypointIndex, 2); 
    EXPECT_EQ(TestReturnToGround.homeBaseInitialized, false);

    EXPECT_EQ(idArrayComparision, ARRAY_SUCCESS); // Need to figure out how to pass arrays by reference
    EXPECT_EQ(flightPathComparision, ARRAY_SUCCESS);
    EXPECT_EQ(flightPathStatusComparision, ARRAY_SUCCESS);
}

TEST (CruisingState, InsertWaypointSuccess) {
    /***********************SETUP***********************/

    WaypointManager cruisingStateManager;

    // Set up input telemetry data
    TelemetryTestData.waypoints[0] = createTelemetryWaypoint(0.0, 0.0, 6, 0.0, 0);
    TelemetryTestData.waypoints[1] = createTelemetryWaypoint(0.0, 0.0, 7, 0.0, 0);
    TelemetryTestData.waypoints[2] = createTelemetryWaypoint(0.0, 0.0, 8, 0.0, 0);
    TelemetryTestData.waypoints[3] = createTelemetryWaypoint(0.0, 0.0, 9, 0.0, 0);

	TelemetryTestData.numWaypoints = 4;
    TelemetryTestData.waypointModifyFlightPathCommand = INITIALIZE_FLIGHT_PATH;
    TelemetryTestData.initializingHomeBase = 0;
    TelemetryTestData.waypointNextDirectionsCommand = (_GetNextDirectionsCommand) 10; // Don't want to get next directions, so set it to return error
    TelemetryTestData.holdingAltitude = 0;
    TelemetryTestData.holdingTurnRadius = 0;
    TelemetryTestData.holdingTurnDirection = 0;
    TelemetryTestData.nextId = 4;
    TelemetryTestData.prevId = 3;
    TelemetryTestData.modifyId = 0;
    TelemetryTestData.homebase = *(createTelemetryWaypoint(0.0, 0.0, 100, 0.0, 0));

    // Set up id test array (desired result)
    int idTestArray[PATH_BUFFER_SIZE] = { 0 };
    idTestArray[0] = 1;
    idTestArray[1] = 2;
    idTestArray[2] = 3;
    idTestArray[3] = 5;
    idTestArray[4] = 4;
    
    // Set up flight path array (desired result)

    WaypointManager testWaypointManager; // Use separate waypoint manager object for comparisions. This ensures that the autoincrements of waypoint ids don't make the tests fail

    _PathData * testFlightPath[PATH_BUFFER_SIZE];
    testFlightPath[0] = testWaypointManager.initialize_waypoint(0.0, 0.0, 6, PATH_FOLLOW);
    testFlightPath[1] = testWaypointManager.initialize_waypoint(0.0, 0.0, 7, PATH_FOLLOW);
    testFlightPath[2] = testWaypointManager.initialize_waypoint(0.0, 0.0, 8, PATH_FOLLOW);
    testFlightPath[4] = testWaypointManager.initialize_waypoint(0.0, 0.0, 9, PATH_FOLLOW);
    testFlightPath[3] = testWaypointManager.initialize_waypoint(1.0, 0.0, 10, PATH_FOLLOW);

    // Set up flight path buffer (desired result)
    _WaypointBufferStatus testStatus[PATH_BUFFER_SIZE];
    testStatus[0] = FULL;
    testStatus[1] = FULL;
    testStatus[2] = FULL;
    testStatus[3] = FULL;
    testStatus[4] = FULL;
    for (int i = 5; i < PATH_BUFFER_SIZE; i++) {
        testStatus[i] = FREE;
        testFlightPath[i] = nullptr;
    }
    
    // Variables that will be modified as we go
    int idArray[PATH_BUFFER_SIZE] = { 0 };
    bool goingHome = false, inHold = false; 

	/********************DEPENDENCIES*******************/	
	/********************STEPTHROUGH********************/

    // Initialize waypoint manager for cruisingState
    _ModifyFlightPathErrorCode editError = editFlightPath(&TelemetryTestData, cruisingStateManager, idArray);

    TelemetryTestData.numWaypoints = 1;
    TelemetryTestData.waypointModifyFlightPathCommand = INSERT; // Inserting

    delete TelemetryTestData.waypoints[0]; delete TelemetryTestData.waypoints[1]; delete TelemetryTestData.waypoints[2]; delete TelemetryTestData.waypoints[3];
    TelemetryTestData.waypoints[0] = createTelemetryWaypoint(1.0, 0.0, 10, 0.0, 0);  

    editError = editFlightPath(&TelemetryTestData, cruisingStateManager, idArray);
    _GetNextDirectionsErrorCode pathError = pathFollow(&TelemetryTestData, cruisingStateManager, TestInputData, &TestOutputData, goingHome, inHold);
    setReturnValues(&TestReturnToGround, cruisingStateManager, editError, pathError);

    _ArrayStatus idArrayComparision = compare_id_arrays(idArray, idTestArray);    
    
    _PathData ** flightPath = cruisingStateManager.get_waypoint_buffer();
    _ArrayStatus flightPathComparision = compare_arrays(testFlightPath, flightPath, 5);
    _ArrayStatus flightPathStatusComparision = compare_buffer_status(testStatus, cruisingStateManager);

    // Remove heap-allocated stuff
    cruisingStateManager.clear_path_nodes();
    delete testFlightPath[0]; delete testFlightPath[1]; delete testFlightPath[2]; delete testFlightPath[3]; delete testFlightPath[4];
    delete TelemetryTestData.waypoints[0];

	/**********************ASSERTS**********************/

    EXPECT_EQ(TestReturnToGround.editingFlightPathErrorCode, 0);
    EXPECT_EQ(TestReturnToGround.pathFollowingErrorCode, 2);
    EXPECT_EQ(TestReturnToGround.currentWaypointId, 3);
    EXPECT_EQ(TestReturnToGround.currentWaypointIndex, 2); // My dumbass when testing the waypoint manager made the currentWaypointIndex = 2 when calling this method
    EXPECT_EQ(TestReturnToGround.homeBaseInitialized, false);

    EXPECT_EQ(idArrayComparision, ARRAY_SUCCESS); // Need to figure out how to pass arrays by reference
    EXPECT_EQ(flightPathComparision, ARRAY_SUCCESS);
    EXPECT_EQ(flightPathStatusComparision, ARRAY_SUCCESS);
}

TEST (CruisingState, InsertWaypointFail) {
    /***********************SETUP***********************/

    WaypointManager cruisingStateManager;

    // Set up input telemetry data
    TelemetryTestData.waypoints[0] = createTelemetryWaypoint(0.0, 0.0, 6, 0.0, 0);
    TelemetryTestData.waypoints[1] = createTelemetryWaypoint(0.0, 0.0, 7, 0.0, 0);
    TelemetryTestData.waypoints[2] = createTelemetryWaypoint(0.0, 0.0, 8, 0.0, 0);
    TelemetryTestData.waypoints[3] = createTelemetryWaypoint(0.0, 0.0, 9, 0.0, 0);

	TelemetryTestData.numWaypoints = 4;
    TelemetryTestData.waypointModifyFlightPathCommand = INITIALIZE_FLIGHT_PATH;
    TelemetryTestData.initializingHomeBase = 0;
    TelemetryTestData.waypointNextDirectionsCommand = (_GetNextDirectionsCommand) 10; // Don't want to get next directions, so set it to return error
    TelemetryTestData.holdingAltitude = 0;
    TelemetryTestData.holdingTurnRadius = 0;
    TelemetryTestData.holdingTurnDirection = 0;
    TelemetryTestData.nextId = 2;
    TelemetryTestData.prevId = 1;
    TelemetryTestData.modifyId = 0;
    TelemetryTestData.homebase = *(createTelemetryWaypoint(0.0, 0.0, 100, 0.0, 0));

    // Set up id test array (desired result)
    int idTestArray[PATH_BUFFER_SIZE] = { 0 };
    idTestArray[0] = 1;
    idTestArray[1] = 2;
    idTestArray[2] = 3;
    idTestArray[3] = 4;
    
    // Set up flight path array (desired result)

    WaypointManager testWaypointManager; // Use separate waypoint manager object for comparisions. This ensures that the autoincrements of waypoint ids don't make the tests fail

    _PathData * testFlightPath[PATH_BUFFER_SIZE];
    testFlightPath[0] = testWaypointManager.initialize_waypoint(0.0, 0.0, 6, PATH_FOLLOW);
    testFlightPath[1] = testWaypointManager.initialize_waypoint(0.0, 0.0, 7, PATH_FOLLOW);
    testFlightPath[2] = testWaypointManager.initialize_waypoint(0.0, 0.0, 8, PATH_FOLLOW);
    testFlightPath[3] = testWaypointManager.initialize_waypoint(0.0, 0.0, 9, PATH_FOLLOW);

    // Set up flight path buffer (desired result)
    _WaypointBufferStatus testStatus[PATH_BUFFER_SIZE];
    testStatus[0] = FULL;
    testStatus[1] = FULL;
    testStatus[2] = FULL;
    testStatus[3] = FULL;
    for (int i = 4; i < PATH_BUFFER_SIZE; i++) {
        testStatus[i] = FREE;
        testFlightPath[i] = nullptr;
    }
    
    // Variables that will be modified as we go
    int idArray[PATH_BUFFER_SIZE] = { 0 };
    bool goingHome = false, inHold = false; 

	/********************DEPENDENCIES*******************/	
	/********************STEPTHROUGH********************/

    // Initialize waypoint manager for cruisingState
    _ModifyFlightPathErrorCode editError = editFlightPath(&TelemetryTestData, cruisingStateManager, idArray);

    TelemetryTestData.numWaypoints = 1;
    TelemetryTestData.waypointModifyFlightPathCommand = INSERT; // Inserting

    delete TelemetryTestData.waypoints[0]; delete TelemetryTestData.waypoints[1]; delete TelemetryTestData.waypoints[2]; delete TelemetryTestData.waypoints[3];
    TelemetryTestData.waypoints[0] = createTelemetryWaypoint(1.0, 0.0, 10, 0.0, 0);  

    editError = editFlightPath(&TelemetryTestData, cruisingStateManager, idArray);
    _GetNextDirectionsErrorCode pathError = pathFollow(&TelemetryTestData, cruisingStateManager, TestInputData, &TestOutputData, goingHome, inHold);
    setReturnValues(&TestReturnToGround, cruisingStateManager, editError, pathError);

    _ArrayStatus idArrayComparision = compare_id_arrays(idArray, idTestArray);    
    
    _PathData ** flightPath = cruisingStateManager.get_waypoint_buffer();
    _ArrayStatus flightPathComparision = compare_arrays(testFlightPath, flightPath, 4);
    _ArrayStatus flightPathStatusComparision = compare_buffer_status(testStatus, cruisingStateManager);

    // Remove heap-allocated stuff
    cruisingStateManager.clear_path_nodes();
    delete testFlightPath[0]; delete testFlightPath[1]; delete testFlightPath[2]; delete testFlightPath[3]; 
    delete TelemetryTestData.waypoints[0];

	/**********************ASSERTS**********************/

    EXPECT_EQ(TestReturnToGround.editingFlightPathErrorCode, 1);
    EXPECT_EQ(TestReturnToGround.pathFollowingErrorCode, 2);
    EXPECT_EQ(TestReturnToGround.currentWaypointId, 3);
    EXPECT_EQ(TestReturnToGround.currentWaypointIndex, 2); // My dumbass when testing the waypoint manager made the currentWaypointIndex = 2 when calling this method
    EXPECT_EQ(TestReturnToGround.homeBaseInitialized, false);

    EXPECT_EQ(idArrayComparision, ARRAY_SUCCESS); // Need to figure out how to pass arrays by reference
    EXPECT_EQ(flightPathComparision, ARRAY_SUCCESS);
    EXPECT_EQ(flightPathStatusComparision, ARRAY_SUCCESS);
}

TEST (CruisingState, UpdateWaypointSuccess) {
    /***********************SETUP***********************/

    WaypointManager cruisingStateManager;

    // Set up input telemetry data
    TelemetryTestData.waypoints[0] = createTelemetryWaypoint(0.0, 0.0, 6, 0.0, 0);
    TelemetryTestData.waypoints[1] = createTelemetryWaypoint(0.0, 0.0, 7, 0.0, 0);
    TelemetryTestData.waypoints[2] = createTelemetryWaypoint(0.0, 0.0, 8, 0.0, 0);
    TelemetryTestData.waypoints[3] = createTelemetryWaypoint(0.0, 0.0, 9, 0.0, 0);

	TelemetryTestData.numWaypoints = 4;
    TelemetryTestData.waypointModifyFlightPathCommand = INITIALIZE_FLIGHT_PATH;
    TelemetryTestData.initializingHomeBase = 0;
    TelemetryTestData.waypointNextDirectionsCommand = (_GetNextDirectionsCommand) 10; // Don't want to get next directions, so set it to return error
    TelemetryTestData.holdingAltitude = 0;
    TelemetryTestData.holdingTurnRadius = 0;
    TelemetryTestData.holdingTurnDirection = 0;
    TelemetryTestData.nextId = 0;
    TelemetryTestData.prevId = 0;
    TelemetryTestData.modifyId = 3;
    TelemetryTestData.homebase = *(createTelemetryWaypoint(0.0, 0.0, 100, 0.0, 0));

    // Set up id test array (desired result)
    int idTestArray[PATH_BUFFER_SIZE] = { 0 };
    idTestArray[0] = 1;
    idTestArray[1] = 2;
    idTestArray[2] = 5;
    idTestArray[3] = 4;
    
    // Set up flight path array (desired result)

    WaypointManager testWaypointManager; // Use separate waypoint manager object for comparisions. This ensures that the autoincrements of waypoint ids don't make the tests fail

    _PathData * testFlightPath[PATH_BUFFER_SIZE];
    testFlightPath[0] = testWaypointManager.initialize_waypoint(0.0, 0.0, 6, PATH_FOLLOW);
    testFlightPath[1] = testWaypointManager.initialize_waypoint(0.0, 0.0, 7, PATH_FOLLOW);
    testFlightPath[2] = testWaypointManager.initialize_waypoint(0.0, 0.0, 8, PATH_FOLLOW);
    testFlightPath[3] = testWaypointManager.initialize_waypoint(0.0, 0.0, 9, PATH_FOLLOW);
    testFlightPath[2] = testWaypointManager.initialize_waypoint(1.0, 0.0, 10, PATH_FOLLOW);

    // Set up flight path buffer (desired result)
    _WaypointBufferStatus testStatus[PATH_BUFFER_SIZE];
    testStatus[0] = FULL;
    testStatus[1] = FULL;
    testStatus[2] = FULL;
    testStatus[3] = FULL;
    for (int i = 4; i < PATH_BUFFER_SIZE; i++) {
        testStatus[i] = FREE;
        testFlightPath[i] = nullptr;
    }
    
    // Variables that will be modified as we go
    int idArray[PATH_BUFFER_SIZE] = { 0 };
    bool goingHome = false, inHold = false; 

	/********************DEPENDENCIES*******************/	
	/********************STEPTHROUGH********************/

    // Initialize waypoint manager for cruisingState
    _ModifyFlightPathErrorCode editError = editFlightPath(&TelemetryTestData, cruisingStateManager, idArray);

    TelemetryTestData.numWaypoints = 1;
    TelemetryTestData.waypointModifyFlightPathCommand = UPDATE; // Updating

    delete TelemetryTestData.waypoints[0]; delete TelemetryTestData.waypoints[1]; delete TelemetryTestData.waypoints[2]; delete TelemetryTestData.waypoints[3];
    TelemetryTestData.waypoints[0] = createTelemetryWaypoint(1.0, 0.0, 10, 0.0, 0);  

    editError = editFlightPath(&TelemetryTestData, cruisingStateManager, idArray);
    _GetNextDirectionsErrorCode pathError = pathFollow(&TelemetryTestData, cruisingStateManager, TestInputData, &TestOutputData, goingHome, inHold);
    setReturnValues(&TestReturnToGround, cruisingStateManager, editError, pathError);

    _ArrayStatus idArrayComparision = compare_id_arrays(idArray, idTestArray);    
    
    _PathData ** flightPath = cruisingStateManager.get_waypoint_buffer();
    _ArrayStatus flightPathComparision = compare_arrays(testFlightPath, flightPath, 4);
    _ArrayStatus flightPathStatusComparision = compare_buffer_status(testStatus, cruisingStateManager);

    // Remove heap-allocated stuff
    cruisingStateManager.clear_path_nodes();
    delete testFlightPath[0]; delete testFlightPath[1]; delete testFlightPath[2]; delete testFlightPath[3];
    delete TelemetryTestData.waypoints[0];

	/**********************ASSERTS**********************/

    EXPECT_EQ(TestReturnToGround.editingFlightPathErrorCode, 0);
    EXPECT_EQ(TestReturnToGround.pathFollowingErrorCode, 2);
    EXPECT_EQ(TestReturnToGround.currentWaypointId, 5);
    EXPECT_EQ(TestReturnToGround.currentWaypointIndex, 2); // My dumbass when testing the waypoint manager made the currentWaypointIndex = 2 when calling this method
    EXPECT_EQ(TestReturnToGround.homeBaseInitialized, false);

    EXPECT_EQ(idArrayComparision, ARRAY_SUCCESS); // Need to figure out how to pass arrays by reference
    EXPECT_EQ(flightPathComparision, ARRAY_SUCCESS);
    EXPECT_EQ(flightPathStatusComparision, ARRAY_SUCCESS);
}

TEST (CruisingState, UpdateWaypointFail) {
    /***********************SETUP***********************/

    WaypointManager cruisingStateManager;

    // Set up input telemetry data
    TelemetryTestData.waypoints[0] = createTelemetryWaypoint(0.0, 0.0, 6, 0.0, 0);
    TelemetryTestData.waypoints[1] = createTelemetryWaypoint(0.0, 0.0, 7, 0.0, 0);
    TelemetryTestData.waypoints[2] = createTelemetryWaypoint(0.0, 0.0, 8, 0.0, 0);
    TelemetryTestData.waypoints[3] = createTelemetryWaypoint(0.0, 0.0, 9, 0.0, 0);

	TelemetryTestData.numWaypoints = 4;
    TelemetryTestData.waypointModifyFlightPathCommand = INITIALIZE_FLIGHT_PATH;
    TelemetryTestData.initializingHomeBase = 0;
    TelemetryTestData.waypointNextDirectionsCommand = (_GetNextDirectionsCommand) 10; // Don't want to get next directions, so set it to return error
    TelemetryTestData.holdingAltitude = 0;
    TelemetryTestData.holdingTurnRadius = 0;
    TelemetryTestData.holdingTurnDirection = 0;
    TelemetryTestData.nextId = 0;
    TelemetryTestData.prevId = 0;
    TelemetryTestData.modifyId = 10; // Not a valid index
    TelemetryTestData.homebase = *(createTelemetryWaypoint(0.0, 0.0, 100, 0.0, 0));

    // Set up id test array (desired result)
    int idTestArray[PATH_BUFFER_SIZE] = { 0 };
    idTestArray[0] = 1;
    idTestArray[1] = 2;
    idTestArray[2] = 3;
    idTestArray[3] = 4;
    
    // Set up flight path array (desired result)

    WaypointManager testWaypointManager; // Use separate waypoint manager object for comparisions. This ensures that the autoincrements of waypoint ids don't make the tests fail

    _PathData * testFlightPath[PATH_BUFFER_SIZE];
    testFlightPath[0] = testWaypointManager.initialize_waypoint(0.0, 0.0, 6, PATH_FOLLOW);
    testFlightPath[1] = testWaypointManager.initialize_waypoint(0.0, 0.0, 7, PATH_FOLLOW);
    testFlightPath[2] = testWaypointManager.initialize_waypoint(0.0, 0.0, 8, PATH_FOLLOW);
    testFlightPath[3] = testWaypointManager.initialize_waypoint(0.0, 0.0, 9, PATH_FOLLOW);

    // Set up flight path buffer (desired result)
    _WaypointBufferStatus testStatus[PATH_BUFFER_SIZE];
    testStatus[0] = FULL;
    testStatus[1] = FULL;
    testStatus[2] = FULL;
    testStatus[3] = FULL;
    for (int i = 4; i < PATH_BUFFER_SIZE; i++) {
        testStatus[i] = FREE;
        testFlightPath[i] = nullptr;
    }
    
    // Variables that will be modified as we go
    int idArray[PATH_BUFFER_SIZE] = { 0 };
    bool goingHome = false, inHold = false; 

	/********************DEPENDENCIES*******************/	
	/********************STEPTHROUGH********************/

    // Initialize waypoint manager for cruisingState
    _ModifyFlightPathErrorCode editError = editFlightPath(&TelemetryTestData, cruisingStateManager, idArray);

    TelemetryTestData.numWaypoints = 1;
    TelemetryTestData.waypointModifyFlightPathCommand = UPDATE; // Updating

    delete TelemetryTestData.waypoints[0]; delete TelemetryTestData.waypoints[1]; delete TelemetryTestData.waypoints[2]; delete TelemetryTestData.waypoints[3];
    TelemetryTestData.waypoints[0] = createTelemetryWaypoint(1.0, 0.0, 10, 0.0, 0);  

    editError = editFlightPath(&TelemetryTestData, cruisingStateManager, idArray);
    _GetNextDirectionsErrorCode pathError = pathFollow(&TelemetryTestData, cruisingStateManager, TestInputData, &TestOutputData, goingHome, inHold);
    setReturnValues(&TestReturnToGround, cruisingStateManager, editError, pathError);

    _ArrayStatus idArrayComparision = compare_id_arrays(idArray, idTestArray);    
    
    _PathData ** flightPath = cruisingStateManager.get_waypoint_buffer();
    _ArrayStatus flightPathComparision = compare_arrays(testFlightPath, flightPath, 4);
    _ArrayStatus flightPathStatusComparision = compare_buffer_status(testStatus, cruisingStateManager);

    // Remove heap-allocated stuff
    cruisingStateManager.clear_path_nodes();
    delete testFlightPath[0]; delete testFlightPath[1]; delete testFlightPath[2]; delete testFlightPath[3];
    delete TelemetryTestData.waypoints[0];

	/**********************ASSERTS**********************/

    EXPECT_EQ(TestReturnToGround.editingFlightPathErrorCode, 1);
    EXPECT_EQ(TestReturnToGround.pathFollowingErrorCode, 2);
    EXPECT_EQ(TestReturnToGround.currentWaypointId, 3);
    EXPECT_EQ(TestReturnToGround.currentWaypointIndex, 2); // My dumbass when testing the waypoint manager made the currentWaypointIndex = 2 when calling this method
    EXPECT_EQ(TestReturnToGround.homeBaseInitialized, false);

    EXPECT_EQ(idArrayComparision, ARRAY_SUCCESS); // Need to figure out how to pass arrays by reference
    EXPECT_EQ(flightPathComparision, ARRAY_SUCCESS);
    EXPECT_EQ(flightPathStatusComparision, ARRAY_SUCCESS);
}

TEST (CruisingState, DeleteWaypointSuccess) {
    /***********************SETUP***********************/

    WaypointManager cruisingStateManager;

    // Set up input telemetry data
    TelemetryTestData.waypoints[0] = createTelemetryWaypoint(0.0, 0.0, 6, 0.0, 0);
    TelemetryTestData.waypoints[1] = createTelemetryWaypoint(0.0, 0.0, 7, 0.0, 0);
    TelemetryTestData.waypoints[2] = createTelemetryWaypoint(0.0, 0.0, 8, 0.0, 0);
    TelemetryTestData.waypoints[3] = createTelemetryWaypoint(0.0, 0.0, 9, 0.0, 0);

	TelemetryTestData.numWaypoints = 4;
    TelemetryTestData.waypointModifyFlightPathCommand = INITIALIZE_FLIGHT_PATH;
    TelemetryTestData.initializingHomeBase = 0;
    TelemetryTestData.waypointNextDirectionsCommand = (_GetNextDirectionsCommand) 10; // Don't want to get next directions, so set it to return error
    TelemetryTestData.holdingAltitude = 0;
    TelemetryTestData.holdingTurnRadius = 0;
    TelemetryTestData.holdingTurnDirection = 0;
    TelemetryTestData.nextId = 0;
    TelemetryTestData.prevId = 0;
    TelemetryTestData.modifyId = 3;
    TelemetryTestData.homebase = *(createTelemetryWaypoint(0.0, 0.0, 100, 0.0, 0));

    // Set up id test array (desired result)
    int idTestArray[PATH_BUFFER_SIZE] = { 0 };
    idTestArray[0] = 1;
    idTestArray[1] = 2;
    idTestArray[2] = 4;
    
    // Set up flight path array (desired result)

    WaypointManager testWaypointManager; // Use separate waypoint manager object for comparisions. This ensures that the autoincrements of waypoint ids don't make the tests fail

    _PathData * testFlightPath[PATH_BUFFER_SIZE];
    testFlightPath[0] = testWaypointManager.initialize_waypoint(0.0, 0.0, 6, PATH_FOLLOW);
    testFlightPath[1] = testWaypointManager.initialize_waypoint(0.0, 0.0, 7, PATH_FOLLOW);
    testFlightPath[2] = testWaypointManager.initialize_waypoint(0.0, 0.0, 8, PATH_FOLLOW); // Deleting id = 3
    delete testFlightPath[2]; // remove from heap before deleting
    testFlightPath[2] = testWaypointManager.initialize_waypoint(0.0, 0.0, 9, PATH_FOLLOW);
    testFlightPath[3] = testWaypointManager.initialize_waypoint(1.0, 0.0, 10, PATH_FOLLOW);

    // Set up flight path buffer (desired result)
    _WaypointBufferStatus testStatus[PATH_BUFFER_SIZE];
    testStatus[0] = FULL;
    testStatus[1] = FULL;
    testStatus[2] = FULL;
    for (int i = 3; i < PATH_BUFFER_SIZE; i++) {
        testStatus[i] = FREE;
        testFlightPath[i] = nullptr;
    }
    
    // Variables that will be modified as we go
    int idArray[PATH_BUFFER_SIZE] = { 0 };
    bool goingHome = false, inHold = false; 

	/********************DEPENDENCIES*******************/	
	/********************STEPTHROUGH********************/

    // Initialize waypoint manager for cruisingState
    _ModifyFlightPathErrorCode editError = editFlightPath(&TelemetryTestData, cruisingStateManager, idArray);

    TelemetryTestData.numWaypoints = 0;
    TelemetryTestData.waypointModifyFlightPathCommand = DELETE; // Deleting

    delete TelemetryTestData.waypoints[0]; delete TelemetryTestData.waypoints[1]; delete TelemetryTestData.waypoints[2]; delete TelemetryTestData.waypoints[3];

    editError = editFlightPath(&TelemetryTestData, cruisingStateManager, idArray);
    _GetNextDirectionsErrorCode pathError = pathFollow(&TelemetryTestData, cruisingStateManager, TestInputData, &TestOutputData, goingHome, inHold);
    setReturnValues(&TestReturnToGround, cruisingStateManager, editError, pathError);

    _ArrayStatus idArrayComparision = compare_id_arrays(idArray, idTestArray);    
    
    _PathData ** flightPath = cruisingStateManager.get_waypoint_buffer();
    _ArrayStatus flightPathComparision = compare_arrays(testFlightPath, flightPath, 3);
    _ArrayStatus flightPathStatusComparision = compare_buffer_status(testStatus, cruisingStateManager);

    // Remove heap-allocated stuff
    cruisingStateManager.clear_path_nodes();
    delete testFlightPath[0]; delete testFlightPath[1]; delete testFlightPath[2]; delete testFlightPath[3];

	/**********************ASSERTS**********************/

    EXPECT_EQ(TestReturnToGround.editingFlightPathErrorCode, 0);
    EXPECT_EQ(TestReturnToGround.pathFollowingErrorCode, 2);
    EXPECT_EQ(TestReturnToGround.currentWaypointId, 4);
    EXPECT_EQ(TestReturnToGround.currentWaypointIndex, 2); // My dumbass when testing the waypoint manager made the currentWaypointIndex = 2 when calling this method
    EXPECT_EQ(TestReturnToGround.homeBaseInitialized, false);

    EXPECT_EQ(idArrayComparision, ARRAY_SUCCESS); // Need to figure out how to pass arrays by reference
    EXPECT_EQ(flightPathComparision, ARRAY_SUCCESS);
    EXPECT_EQ(flightPathStatusComparision, ARRAY_SUCCESS);
}

TEST (CruisingState, DeleteWaypointFail) {
    /***********************SETUP***********************/

    WaypointManager cruisingStateManager;

    // Set up input telemetry data
    TelemetryTestData.waypoints[0] = createTelemetryWaypoint(0.0, 0.0, 6, 0.0, 0);
    TelemetryTestData.waypoints[1] = createTelemetryWaypoint(0.0, 0.0, 7, 0.0, 0);
    TelemetryTestData.waypoints[2] = createTelemetryWaypoint(0.0, 0.0, 8, 0.0, 0);
    TelemetryTestData.waypoints[3] = createTelemetryWaypoint(0.0, 0.0, 9, 0.0, 0);

	TelemetryTestData.numWaypoints = 4;
    TelemetryTestData.waypointModifyFlightPathCommand = INITIALIZE_FLIGHT_PATH;
    TelemetryTestData.initializingHomeBase = 0;
    TelemetryTestData.waypointNextDirectionsCommand = (_GetNextDirectionsCommand) 10; // Don't want to get next directions, so set it to return error
    TelemetryTestData.holdingAltitude = 0;
    TelemetryTestData.holdingTurnRadius = 0;
    TelemetryTestData.holdingTurnDirection = 0;
    TelemetryTestData.nextId = 0;
    TelemetryTestData.prevId = 0;
    TelemetryTestData.modifyId = 10;
    TelemetryTestData.homebase = *(createTelemetryWaypoint(0.0, 0.0, 100, 0.0, 0));

    // Set up id test array (desired result)
    int idTestArray[PATH_BUFFER_SIZE] = { 0 };
    idTestArray[0] = 1;
    idTestArray[1] = 2;
    idTestArray[2] = 3;
    idTestArray[3] = 4;
    
    // Set up flight path array (desired result)

    WaypointManager testWaypointManager; // Use separate waypoint manager object for comparisions. This ensures that the autoincrements of waypoint ids don't make the tests fail

    _PathData * testFlightPath[PATH_BUFFER_SIZE];
    testFlightPath[0] = testWaypointManager.initialize_waypoint(0.0, 0.0, 6, PATH_FOLLOW);
    testFlightPath[1] = testWaypointManager.initialize_waypoint(0.0, 0.0, 7, PATH_FOLLOW);
    testFlightPath[2] = testWaypointManager.initialize_waypoint(0.0, 0.0, 8, PATH_FOLLOW); // Deleting id = 3
    testFlightPath[3] = testWaypointManager.initialize_waypoint(0.0, 0.0, 9, PATH_FOLLOW);

    // Set up flight path buffer (desired result)
    _WaypointBufferStatus testStatus[PATH_BUFFER_SIZE];
    testStatus[0] = FULL;
    testStatus[1] = FULL;
    testStatus[2] = FULL;
    testStatus[3] = FULL;
    for (int i = 4; i < PATH_BUFFER_SIZE; i++) {
        testStatus[i] = FREE;
        testFlightPath[i] = nullptr;
    }
    
    // Variables that will be modified as we go
    int idArray[PATH_BUFFER_SIZE] = { 0 };
    bool goingHome = false, inHold = false; 

	/********************DEPENDENCIES*******************/	
	/********************STEPTHROUGH********************/

    // Initialize waypoint manager for cruisingState
    _ModifyFlightPathErrorCode editError = editFlightPath(&TelemetryTestData, cruisingStateManager, idArray);

    TelemetryTestData.numWaypoints = 0;
    TelemetryTestData.waypointModifyFlightPathCommand = DELETE; // Deleting

    delete TelemetryTestData.waypoints[0]; delete TelemetryTestData.waypoints[1]; delete TelemetryTestData.waypoints[2]; delete TelemetryTestData.waypoints[3];

    editError = editFlightPath(&TelemetryTestData, cruisingStateManager, idArray);
    _GetNextDirectionsErrorCode pathError = pathFollow(&TelemetryTestData, cruisingStateManager, TestInputData, &TestOutputData, goingHome, inHold);
    setReturnValues(&TestReturnToGround, cruisingStateManager, editError, pathError);

    _ArrayStatus idArrayComparision = compare_id_arrays(idArray, idTestArray);    
    
    _PathData ** flightPath = cruisingStateManager.get_waypoint_buffer();
    _ArrayStatus flightPathComparision = compare_arrays(testFlightPath, flightPath, 4);
    _ArrayStatus flightPathStatusComparision = compare_buffer_status(testStatus, cruisingStateManager);

    // Remove heap-allocated stuff
    cruisingStateManager.clear_path_nodes();
    delete testFlightPath[0]; delete testFlightPath[1]; delete testFlightPath[2]; delete testFlightPath[3];

	/**********************ASSERTS**********************/

    EXPECT_EQ(TestReturnToGround.editingFlightPathErrorCode, 1);
    EXPECT_EQ(TestReturnToGround.pathFollowingErrorCode, 2);
    EXPECT_EQ(TestReturnToGround.currentWaypointId, 3);
    EXPECT_EQ(TestReturnToGround.currentWaypointIndex, 2); // My dumbass when testing the waypoint manager made the currentWaypointIndex = 2 when calling this method
    EXPECT_EQ(TestReturnToGround.homeBaseInitialized, false);

    EXPECT_EQ(idArrayComparision, ARRAY_SUCCESS); // Need to figure out how to pass arrays by reference
    EXPECT_EQ(flightPathComparision, ARRAY_SUCCESS);
    EXPECT_EQ(flightPathStatusComparision, ARRAY_SUCCESS);
}

TEST (CruisingState, NextDirectionsRegularCorrect) {
    /***********************SETUP***********************/

    WaypointManager cruisingStateManager;

    // Set up input telemetry data
    TelemetryTestData.waypoints[0] = createTelemetryWaypoint(-80.5479053969044, 43.47075830402289, 10, 0.0, 0);
    TelemetryTestData.waypoints[1] = createTelemetryWaypoint(-80.55044911526599, 43.469649460242174, 20, 0.0, 0);
    TelemetryTestData.waypoints[2] = createTelemetryWaypoint(-80.54172626568685, 43.46764349709017, 30, 0.0, 0);
    TelemetryTestData.waypoints[3] = createTelemetryWaypoint(-80.54806720987989, 43.46430420301871, 33, 0.0, 0);

	TelemetryTestData.numWaypoints = 4;
    TelemetryTestData.waypointModifyFlightPathCommand = INITIALIZE_FLIGHT_PATH;
    TelemetryTestData.initializingHomeBase = 0;
    TelemetryTestData.waypointNextDirectionsCommand = (_GetNextDirectionsCommand) 10; // Don't want to get next directions, so set it to return error
    TelemetryTestData.holdingAltitude = 0;
    TelemetryTestData.holdingTurnRadius = 0;
    TelemetryTestData.holdingTurnDirection = 0;
    TelemetryTestData.nextId = 0;
    TelemetryTestData.prevId = 0;
    TelemetryTestData.modifyId = 10;
    TelemetryTestData.homebase = *(createTelemetryWaypoint(0.0, 0.0, 100, 0.0, 0));    
      
    // Variables that will be modified as we go
    int idArray[PATH_BUFFER_SIZE] = { 0 };
    bool goingHome = false, inHold = false; 

    // Set up input data for waypoint manager:
    TestInputData = {43.467998128, -80.537331184, 11, 100};  // latitude, longitude, altitude, heading

    // Set up answer for output data
    _WaypointManager_Data_Out * ans1 = new _WaypointManager_Data_Out;
    ans1->desiredTrack = 294;         
    ans1->desiredAltitude = 33;
    ans1->distanceToNextWaypoint = 960;
    ans1->radius = 0;
    ans1->turnDirection = 0;
    ans1->errorCode = WAYPOINT_SUCCESS;
    ans1->isDataNew = true;
    ans1->timeOfData = 0;
    ans1->out_type = PATH_FOLLOW;

	/********************DEPENDENCIES*******************/	
	/********************STEPTHROUGH********************/

    // Initialize waypoint manager for cruisingState
    _ModifyFlightPathErrorCode editError = editFlightPath(&TelemetryTestData, cruisingStateManager, idArray);

    TelemetryTestData.numWaypoints = 0;
    TelemetryTestData.waypointModifyFlightPathCommand = NO_FLIGHT_PATH_EDIT; // Nothing
    TelemetryTestData.waypointNextDirectionsCommand = REGULAR_PATH_FOLLOWING; // Get next directions

    editError = editFlightPath(&TelemetryTestData, cruisingStateManager, idArray);
    _GetNextDirectionsErrorCode pathError = pathFollow(&TelemetryTestData, cruisingStateManager, TestInputData, &TestOutputData, goingHome, inHold);
    setReturnValues(&TestReturnToGround, cruisingStateManager, editError, pathError);
  
    _OutputStatus returnedValuesSuccess = compare_output_data(ans1, &TestOutputData);

    // Remove heap-allocated stuff
    cruisingStateManager.clear_path_nodes();
    delete ans1;
    delete TelemetryTestData.waypoints[0]; delete TelemetryTestData.waypoints[1]; delete TelemetryTestData.waypoints[2]; delete TelemetryTestData.waypoints[3];

	/**********************ASSERTS**********************/

    EXPECT_EQ(TestReturnToGround.editingFlightPathErrorCode, 0);
    EXPECT_EQ(TestReturnToGround.pathFollowingErrorCode, 0);
    EXPECT_EQ(TestReturnToGround.currentWaypointId, 3);
    EXPECT_EQ(TestReturnToGround.currentWaypointIndex, 2); // My dumbass when testing the waypoint manager made the currentWaypointIndex = 2 when calling this method
    EXPECT_EQ(TestReturnToGround.homeBaseInitialized, false);

    EXPECT_EQ(returnedValuesSuccess, OUTPUT_CORRECT);
}

TEST (CruisingState, NextDirectionsGoingHomeCorrect) {
    /***********************SETUP***********************/

    WaypointManager cruisingStateManager;

    // Set up input telemetry data
    TelemetryTestData.waypoints[0] = createTelemetryWaypoint(-80.5479053969044, 43.47075830402289, 10, 0.0, 0);
    TelemetryTestData.waypoints[1] = createTelemetryWaypoint(-80.55044911526599, 43.469649460242174, 20, 0.0, 0);
    TelemetryTestData.waypoints[2] = createTelemetryWaypoint(-80.54172626568685, 43.46764349709017, 30, 0.0, 0);
    TelemetryTestData.waypoints[3] = createTelemetryWaypoint(-80.54806720987989, 43.46430420301871, 33, 0.0, 0);

	TelemetryTestData.numWaypoints = 4;
    TelemetryTestData.waypointModifyFlightPathCommand = INITIALIZE_FLIGHT_PATH;
    TelemetryTestData.initializingHomeBase = 1;
    TelemetryTestData.waypointNextDirectionsCommand = (_GetNextDirectionsCommand) 10; // Don't want to get next directions, so set it to return error
    TelemetryTestData.holdingAltitude = 0;
    TelemetryTestData.holdingTurnRadius = 0;
    TelemetryTestData.holdingTurnDirection = 0;
    TelemetryTestData.nextId = 0;
    TelemetryTestData.prevId = 0;
    TelemetryTestData.modifyId = 10;
    TelemetryTestData.homebase = *(createTelemetryWaypoint(-80.537331184, 43.467998128, 45, 0.0, 2));
    
    // Variables that will be modified as we go
    int idArray[PATH_BUFFER_SIZE] = { 0 };
    bool goingHome = false, inHold = false; 

    // Set up input data for waypoint manager:
    TestInputData = {43.567998128, -80.437331184, 11, 100};  // latitude, longitude, altitude, heading

    // Set up answer for output data
    _WaypointManager_Data_Out * ans1 = new _WaypointManager_Data_Out;
    ans1->desiredTrack = 215;        
    ans1->desiredAltitude = 45;
    ans1->distanceToNextWaypoint = 13755;
    ans1->radius = 0;
    ans1->turnDirection = 0;
    ans1->errorCode = WAYPOINT_SUCCESS;
    ans1->isDataNew = true;
    ans1->timeOfData = 0;
    ans1->out_type = PATH_FOLLOW;

	/********************DEPENDENCIES*******************/	
	/********************STEPTHROUGH********************/

    // Initialize waypoint manager for cruisingState
    _ModifyFlightPathErrorCode editError = editFlightPath(&TelemetryTestData, cruisingStateManager, idArray);

    TelemetryTestData.numWaypoints = 0;
    TelemetryTestData.waypointModifyFlightPathCommand = NO_FLIGHT_PATH_EDIT; // Nothing
    TelemetryTestData.waypointNextDirectionsCommand = TOGGLE_HEAD_HOME; // Head Home
    TelemetryTestData.initializingHomeBase = 0; // Get next directions

    editError = editFlightPath(&TelemetryTestData, cruisingStateManager, idArray);    
    _GetNextDirectionsErrorCode pathError = pathFollow(&TelemetryTestData, cruisingStateManager, TestInputData, &TestOutputData, goingHome, inHold); // Set going home to true
    
    TelemetryTestData.waypointNextDirectionsCommand = REGULAR_PATH_FOLLOWING; // Regular path following
    
    pathError = pathFollow(&TelemetryTestData, cruisingStateManager, TestInputData, &TestOutputData, goingHome, inHold); // Get next directions (output)

    bool firstRunGoingHome = goingHome; // Store current going home value

    TelemetryTestData.waypointNextDirectionsCommand = TOGGLE_HEAD_HOME; // Head home

    pathError = pathFollow(&TelemetryTestData, cruisingStateManager, TestInputData, &TestOutputData, goingHome, inHold); // Set going home to false
    setReturnValues(&TestReturnToGround, cruisingStateManager, editError, pathError);
  
    _OutputStatus returnedValuesSuccess = compare_output_data(ans1, &TestOutputData);

    // Remove heap-allocated stuff
    cruisingStateManager.clear_path_nodes();
    delete ans1;
    delete TelemetryTestData.waypoints[0]; delete TelemetryTestData.waypoints[1]; delete TelemetryTestData.waypoints[2]; delete TelemetryTestData.waypoints[3];

	/**********************ASSERTS**********************/

    EXPECT_EQ(TestReturnToGround.editingFlightPathErrorCode, 0);
    EXPECT_EQ(TestReturnToGround.pathFollowingErrorCode, 0);
    // Fligh path should be cleared.
    EXPECT_EQ(TestReturnToGround.currentWaypointId, 0);
    EXPECT_EQ(TestReturnToGround.currentWaypointIndex, 0); 
    EXPECT_EQ(TestReturnToGround.homeBaseInitialized, true);

    EXPECT_EQ(firstRunGoingHome, true);
    EXPECT_EQ(goingHome, false);

    EXPECT_EQ(returnedValuesSuccess, OUTPUT_CORRECT);
}

TEST (CruisingState, GoingHomeHomeBaseUndefined) {
    /***********************SETUP***********************/

    WaypointManager cruisingStateManager;

    // Set up input telemetry data
    TelemetryTestData.waypoints[0] = createTelemetryWaypoint(-80.5479053969044, 43.47075830402289, 10, 0.0, 0);
    TelemetryTestData.waypoints[1] = createTelemetryWaypoint(-80.55044911526599, 43.469649460242174, 20, 0.0, 0);
    TelemetryTestData.waypoints[2] = createTelemetryWaypoint(-80.54172626568685, 43.46764349709017, 30, 0.0, 0);
    TelemetryTestData.waypoints[3] = createTelemetryWaypoint(-80.54806720987989, 43.46430420301871, 33, 0.0, 0);

	TelemetryTestData.numWaypoints = 4;
    TelemetryTestData.waypointModifyFlightPathCommand = INITIALIZE_FLIGHT_PATH;
    TelemetryTestData.initializingHomeBase = 0;
    TelemetryTestData.waypointNextDirectionsCommand = (_GetNextDirectionsCommand) 10; // Don't want to get next directions, so set it to return error
    TelemetryTestData.holdingAltitude = 0;
    TelemetryTestData.holdingTurnRadius = 0;
    TelemetryTestData.holdingTurnDirection = 0;
    TelemetryTestData.nextId = 0;
    TelemetryTestData.prevId = 0;
    TelemetryTestData.modifyId = 10;
    TelemetryTestData.homebase = *(createTelemetryWaypoint(-80.537331184, 43.467998128, 45, 0.0, 2));
    
    // Variables that will be modified as we go
    int idArray[PATH_BUFFER_SIZE] = { 0 };
    bool goingHome = false, inHold = false; 

    // Set up input data for waypoint manager:
    TestInputData = {43.567998128, -80.437331184, 11, 100};  // latitude, longitude, altitude, heading

    // Set up answer for output data
    _WaypointManager_Data_Out * ans1 = new _WaypointManager_Data_Out;
    ans1->desiredTrack = 215;        
    ans1->desiredAltitude = 45;
    ans1->distanceToNextWaypoint = 13755;
    ans1->radius = 0;
    ans1->turnDirection = 0;
    ans1->errorCode = WAYPOINT_SUCCESS;
    ans1->isDataNew = true;
    ans1->timeOfData = 0;
    ans1->out_type = PATH_FOLLOW;

	/********************DEPENDENCIES*******************/	
	/********************STEPTHROUGH********************/

    // Initialize waypoint manager for cruisingState
    _ModifyFlightPathErrorCode editError = editFlightPath(&TelemetryTestData, cruisingStateManager, idArray);

    TelemetryTestData.numWaypoints = 0;
    TelemetryTestData.waypointModifyFlightPathCommand = NO_FLIGHT_PATH_EDIT; // Nothing
    TelemetryTestData.waypointNextDirectionsCommand = TOGGLE_HEAD_HOME; // Head Home
    TelemetryTestData.initializingHomeBase = 0; // Get next directions

    editError = editFlightPath(&TelemetryTestData, cruisingStateManager, idArray);    
    _GetNextDirectionsErrorCode pathError = pathFollow(&TelemetryTestData, cruisingStateManager, TestInputData, &TestOutputData, goingHome, inHold); // Set going home to true
    setReturnValues(&TestReturnToGround, cruisingStateManager, editError, pathError);

    // Remove heap-allocated stuff
    cruisingStateManager.clear_path_nodes();
    delete ans1;
    delete TelemetryTestData.waypoints[0]; delete TelemetryTestData.waypoints[1]; delete TelemetryTestData.waypoints[2]; delete TelemetryTestData.waypoints[3];

	/**********************ASSERTS**********************/

    EXPECT_EQ(TestReturnToGround.editingFlightPathErrorCode, 0);
    EXPECT_EQ(TestReturnToGround.pathFollowingErrorCode, 3); // Should return special error code :))
    // Fligh path should be cleared.
    EXPECT_EQ(TestReturnToGround.currentWaypointId, 3);
    EXPECT_EQ(TestReturnToGround.currentWaypointIndex, 2); 
    EXPECT_EQ(TestReturnToGround.homeBaseInitialized, false);

    EXPECT_EQ(goingHome, false);
}

TEST (CruisingState, NextDirectionsStartHoldingCorrect) {
    /***********************SETUP***********************/

    WaypointManager cruisingStateManager;

    // Set up input telemetry data
    TelemetryTestData.waypoints[0] = createTelemetryWaypoint(-80.5479053969044, 43.47075830402289, 10, 0.0, 0);
    TelemetryTestData.waypoints[1] = createTelemetryWaypoint(-80.55044911526599, 43.469649460242174, 20, 0.0, 0);
    TelemetryTestData.waypoints[2] = createTelemetryWaypoint(-80.54172626568685, 43.46764349709017, 30, 0.0, 0);
    TelemetryTestData.waypoints[3] = createTelemetryWaypoint(-80.54806720987989, 43.46430420301871, 33, 0.0, 0);

	TelemetryTestData.numWaypoints = 4;
    TelemetryTestData.waypointModifyFlightPathCommand = INITIALIZE_FLIGHT_PATH;
    TelemetryTestData.initializingHomeBase = 1;
    TelemetryTestData.waypointNextDirectionsCommand = (_GetNextDirectionsCommand) 10; // Don't want to get next directions, so set it to return error
    TelemetryTestData.holdingAltitude = 78;
    TelemetryTestData.holdingTurnRadius = 100;
    TelemetryTestData.holdingTurnDirection = 0;
    TelemetryTestData.nextId = 0;
    TelemetryTestData.prevId = 0;
    TelemetryTestData.modifyId = 10;
    TelemetryTestData.homebase = *(createTelemetryWaypoint(-80.537331184, 43.467998128, 45, 0.0, 2));
    
    // Variables that will be modified as we go
    int idArray[PATH_BUFFER_SIZE] = { 0 };
    bool goingHome = false, inHold = false; 

    // Set up input data for waypoint manager:
    _WaypointManager_Data_In setup1 = {43.467998128, -80.537331184, 100, 100};  // latitude, longitude, altitude, heading
    TestInputData = {43.467998128, -80.537331184, 100, 100};  // latitude, longitude, altitude, heading

    // Set up answer for output data
    _WaypointManager_Data_Out * ans1 = new _WaypointManager_Data_Out;
    ans1->desiredTrack = 100;        
    ans1->desiredAltitude = 78;
    ans1->distanceToNextWaypoint = 0;
    ans1->radius = 100;
    ans1->turnDirection = -1;
    ans1->errorCode = WAYPOINT_SUCCESS;
    ans1->isDataNew = true;
    ans1->timeOfData = 0;
    ans1->out_type = ORBIT_FOLLOW;

	/********************DEPENDENCIES*******************/	
	/********************STEPTHROUGH********************/

    // Initialize waypoint manager for cruisingState
    _ModifyFlightPathErrorCode editError = editFlightPath(&TelemetryTestData, cruisingStateManager, idArray);

    TelemetryTestData.numWaypoints = 0;
    TelemetryTestData.waypointModifyFlightPathCommand = NO_FLIGHT_PATH_EDIT; // Nothing
    TelemetryTestData.waypointNextDirectionsCommand = TOGGLE_HOLDING; // Start circling
    TelemetryTestData.initializingHomeBase = 0; // Get next directions

    editError = editFlightPath(&TelemetryTestData, cruisingStateManager, idArray);  
    // Send in set up input data  
    _GetNextDirectionsErrorCode pathError = pathFollow(&TelemetryTestData, cruisingStateManager, setup1, &TestOutputData, goingHome, inHold); // Set going home to true
    
    TelemetryTestData.waypointNextDirectionsCommand = REGULAR_PATH_FOLLOWING; // Regular path following
    
    pathError = pathFollow(&TelemetryTestData, cruisingStateManager, TestInputData, &TestOutputData, goingHome, inHold); // Get next directions (output)

    bool firstRunInHold = inHold; // Store current going home value

    TelemetryTestData.waypointNextDirectionsCommand = TOGGLE_HOLDING; // Start circling

    pathError = pathFollow(&TelemetryTestData, cruisingStateManager, TestInputData, &TestOutputData, goingHome, inHold); // Set going home to false
    setReturnValues(&TestReturnToGround, cruisingStateManager, editError, pathError);
  
    _OutputStatus returnedValuesSuccess = compare_output_data(ans1, &TestOutputData);

    // Remove heap-allocated stuff
    cruisingStateManager.clear_path_nodes();
    delete ans1;
    delete TelemetryTestData.waypoints[0]; delete TelemetryTestData.waypoints[1]; delete TelemetryTestData.waypoints[2]; delete TelemetryTestData.waypoints[3];

	/**********************ASSERTS**********************/

    EXPECT_EQ(TestReturnToGround.editingFlightPathErrorCode, 0);
    EXPECT_EQ(TestReturnToGround.pathFollowingErrorCode, 0);
    EXPECT_EQ(TestReturnToGround.currentWaypointId, 3);
    EXPECT_EQ(TestReturnToGround.currentWaypointIndex, 2); 
    EXPECT_EQ(TestReturnToGround.homeBaseInitialized, true);

    EXPECT_EQ(firstRunInHold, true);
    EXPECT_EQ(inHold, false);

    EXPECT_EQ(returnedValuesSuccess, OUTPUT_CORRECT);
}
