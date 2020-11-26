#include <gtest/gtest.h>

#include "waypointManager.hpp"

using namespace std;
using ::testing::Test;

/***********************************************************************************************************************
 * Mocks
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Tests
 **********************************************************************************************************************/

enum _ArrayStatus{ARRAY_SUCCESS = 0, ARRAY_DIFFERENT};

static _ArrayStatus compare_arrays(_PathData ** ans, _PathData ** testArray, int numElements) {
    // don't be stupid, go through like a linked list
    _PathData * nextWaypoint;
    nextWaypoint = testArray[0];
    
    // Checks if nexts are linked properly
    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        if(ans[i]->waypointId == nextWaypoint->waypointId && ans[i]->longitude == nextWaypoint->longitude && ans[i]->latitude == nextWaypoint->latitude && ans[i]->altitude == nextWaypoint->altitude && ans[i]->waypointType == nextWaypoint->waypointType && ans[i]->turnRadius == nextWaypoint->turnRadius) {
            nextWaypoint = nextWaypoint->next;
        } else {
            cout << "Next Check Index: " << i << " | " << ans[i]->waypointId << " " << nextWaypoint->waypointId << " | " << ans[i]->longitude << " " << nextWaypoint->longitude << " | " << ans[i]->latitude << " " << nextWaypoint->latitude << " | " << ans[i]->altitude << " " << nextWaypoint->altitude << " | " << ans[i]->waypointType << " " << nextWaypoint->waypointType << " | " << ans[i]->turnRadius << " " << nextWaypoint->turnRadius << endl;
            return ARRAY_DIFFERENT;
        }
    }

    nextWaypoint = testArray[PATH_BUFFER_SIZE - 1];

    // Checks if previous are linked properly
    for(int i = PATH_BUFFER_SIZE-1; i >= 0; i--) {
        if(ans[i]->waypointId == nextWaypoint->waypointId && ans[i]->longitude == nextWaypoint->longitude && ans[i]->latitude == nextWaypoint->latitude && ans[i]->altitude == nextWaypoint->altitude && ans[i]->waypointType == nextWaypoint->waypointType && ans[i]->turnRadius == nextWaypoint->turnRadius) {
            nextWaypoint = nextWaypoint->previous;
        } else {
            cout << "Previous Check Index: " << i << " | " << ans[i]->waypointId << " " << nextWaypoint->waypointId << " | " << ans[i]->longitude << " " << nextWaypoint->longitude << " | " << ans[i]->latitude << " " << nextWaypoint->latitude << " | " << ans[i]->altitude << " " << nextWaypoint->altitude << " | " << ans[i]->waypointType << " " << nextWaypoint->waypointType << " | " << ans[i]->turnRadius << " " << nextWaypoint->turnRadius << endl;
            return ARRAY_DIFFERENT;
        }
    }

    // Checks if indexes are the same
    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        nextWaypoint = testArray[i];
        if(ans[i]->waypointId != nextWaypoint->waypointId && ans[i]->longitude != nextWaypoint->longitude && ans[i]->latitude != nextWaypoint->latitude && ans[i]->altitude != nextWaypoint->altitude && ans[i]->waypointType != nextWaypoint->waypointType && ans[i]->turnRadius != nextWaypoint->turnRadius) {
            cout << "Array Equality Check Index: " << i << " | " << ans[i]->waypointId << " " << nextWaypoint->waypointId << " | " << ans[i]->longitude << " " << nextWaypoint->longitude << " | " << ans[i]->latitude << " " << nextWaypoint->latitude << " | " << ans[i]->altitude << " " << nextWaypoint->altitude << " | " << ans[i]->waypointType << " " << nextWaypoint->waypointType << " | " << ans[i]->turnRadius << " " << nextWaypoint->turnRadius << endl;
            return ARRAY_DIFFERENT;
        }
    }

    return ARRAY_SUCCESS;
}

TEST(Waypoint_Manager, InitializedFlightPathAndHomeBase) {

   	/***********************SETUP***********************/

	WaypointManager * w = new WaypointManager(); // Creates object

    // Creates the initial flight path and home base

    const int numPaths = 50;

    _PathData ** initialPaths = new _PathData*[PATH_BUFFER_SIZE];
    _PathData ** testArray = new _PathData*[PATH_BUFFER_SIZE];
    _PathData * homeBase;

    long double longitude = 1;
    long double latitude = 10;
    int altitude = 100;
    int waypointType = 0;
	
    int id_array[500];
    int nextElement = 0;

    //Initializes ID array
    for(int i = 0; i < 500; i++) {
        id_array[i] = 0;
    }

	/********************STEPTHROUGH********************/

    homeBase = w->initialize_waypoint(1000,500, 5, 0);

    for(int i = 0; i < numPaths/2; i++) {
        initialPaths[i] = w->initialize_waypoint(longitude, latitude, altitude, waypointType);
        id_array[i] = initialPaths[i]->waypointId;
        nextElement = i+1;
        longitude++;
        latitude++;
        altitude++;
    }

    waypointType = 2;
    float turnRadius = 10;

    for(int i = numPaths/2; i < numPaths; i++) {
        initialPaths[i] = w->initialize_waypoint(longitude, latitude, altitude, waypointType, turnRadius);
        id_array[i] = initialPaths[i]->waypointId;
        nextElement = i+1;
        longitude++;
        latitude++;
        altitude++;
        turnRadius++;
    }

    // Initializes waypint maanger with the flight path
    _WaypointStatus e = w->initialize_flight_path(initialPaths, numPaths, homeBase);

    testArray = w->get_waypoint_buffer();

    _ArrayStatus a = compare_arrays(initialPaths, testArray, numPaths);
	
	/**********************ASSERTS**********************/

	ASSERT_EQ(a, ARRAY_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(e, WAYPOINT_SUCCESS); // Tests equality of the two parameters
}
















