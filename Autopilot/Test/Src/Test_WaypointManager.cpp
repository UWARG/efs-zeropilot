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
enum _WaypointSt{WAYPOINT_CORRECT = 0, WAYPOINT_INCORRECT};

static _ArrayStatus compare_arrays(_PathData ** ans, _PathData ** testArray, int numElements) {
    // don't be stupid, go through like a linked list
    _PathData * nextWaypoint;
    nextWaypoint = testArray[0];

    // Checks if nexts are linked properly
    for(int i = 0; i < numElements; i++) {
        if(ans[i]->waypointType != -1 && ans[i]->waypointId == nextWaypoint->waypointId && ans[i]->longitude == nextWaypoint->longitude && ans[i]->latitude == nextWaypoint->latitude && ans[i]->altitude == nextWaypoint->altitude && ans[i]->waypointType == nextWaypoint->waypointType && ans[i]->turnRadius == nextWaypoint->turnRadius) {
            nextWaypoint = nextWaypoint->next;
        } else if (ans[i]->waypointType == -1 && ans[i]->waypointId == nextWaypoint->waypointId && ans[i]->longitude == nextWaypoint->longitude && ans[i]->latitude == nextWaypoint->latitude && ans[i]->altitude == nextWaypoint->altitude && ans[i]->waypointType == nextWaypoint->waypointType && ans[i]->turnRadius == nextWaypoint->turnRadius) {
            cout << "Here24234" << endl;
        } else {
            cout << "Next Check Index: " << i << " | " << ans[i]->waypointId << " " << nextWaypoint->waypointId << " | " << ans[i]->longitude << " " << nextWaypoint->longitude << " | " << ans[i]->latitude << " " << nextWaypoint->latitude << " | " << ans[i]->altitude << " " << nextWaypoint->altitude << " | " << ans[i]->waypointType << " " << nextWaypoint->waypointType << " | " << ans[i]->turnRadius << " " << nextWaypoint->turnRadius << endl;
            return ARRAY_DIFFERENT;
        }
    }

    // cout << "Here" << endl;

    nextWaypoint = testArray[numElements - 1];

    // Checks if previous are linked properly
    for(int i = numElements-1; i >= 0; i--) {
        if(ans[i]->waypointId == nextWaypoint->waypointId && ans[i]->longitude == nextWaypoint->longitude && ans[i]->latitude == nextWaypoint->latitude && ans[i]->altitude == nextWaypoint->altitude && ans[i]->waypointType == nextWaypoint->waypointType && ans[i]->turnRadius == nextWaypoint->turnRadius) {
            nextWaypoint = nextWaypoint->previous;
        } else if (ans[i]->waypointType == -1 && ans[i]->waypointId == nextWaypoint->waypointId && ans[i]->longitude == nextWaypoint->longitude && ans[i]->latitude == nextWaypoint->latitude && ans[i]->altitude == nextWaypoint->altitude && ans[i]->waypointType == nextWaypoint->waypointType && ans[i]->turnRadius == nextWaypoint->turnRadius) {
            cout << "Here24234" << endl;
        } else {
            cout << "Previous Check Index: " << i << " | " << ans[i]->waypointId << " " << nextWaypoint->waypointId << " | " << ans[i]->longitude << " " << nextWaypoint->longitude << " | " << ans[i]->latitude << " " << nextWaypoint->latitude << " | " << ans[i]->altitude << " " << nextWaypoint->altitude << " | " << ans[i]->waypointType << " " << nextWaypoint->waypointType << " | " << ans[i]->turnRadius << " " << nextWaypoint->turnRadius << endl;
            return ARRAY_DIFFERENT;
        }
    }

    // Checks if indexes are the same
    for(int i = 0; i < numElements; i++) {
        nextWaypoint = testArray[i];
        if(ans[i]->waypointId != nextWaypoint->waypointId && ans[i]->longitude != nextWaypoint->longitude && ans[i]->latitude != nextWaypoint->latitude && ans[i]->altitude != nextWaypoint->altitude && ans[i]->waypointType != nextWaypoint->waypointType && ans[i]->turnRadius != nextWaypoint->turnRadius) {
            cout << "Array Equality Check Index: " << i << " | " << ans[i]->waypointId << " " << nextWaypoint->waypointId << " | " << ans[i]->longitude << " " << nextWaypoint->longitude << " | " << ans[i]->latitude << " " << nextWaypoint->latitude << " | " << ans[i]->altitude << " " << nextWaypoint->altitude << " | " << ans[i]->waypointType << " " << nextWaypoint->waypointType << " | " << ans[i]->turnRadius << " " << nextWaypoint->turnRadius << endl;
            return ARRAY_DIFFERENT;
        }
    }

    return ARRAY_SUCCESS;
}

static _ArrayStatus compare_buffer_status(_WaypointBufferStatus * ans, WaypointManager * w) {
    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        if(ans[i] != w->get_status_of_index(i)) {
            return ARRAY_DIFFERENT;
        }
    }

    return ARRAY_SUCCESS;
} 

static _WaypointSt compare_waypoint(_PathData * ans, _PathData * test) {
    if(ans->waypointId != test->waypointId && ans->longitude != test->longitude && ans->latitude != test->latitude && ans->altitude != test->altitude && ans->waypointType != test->waypointType && ans->turnRadius != test->turnRadius) {
        cout << "Waypoint Equality Check: " << ans->waypointId << " " << test->waypointId << " | " << ans->longitude << " " << test->longitude << " | " << ans->latitude << " " << test->latitude << " | " << ans->altitude << " " << test->altitude << " | " << ans->waypointType << " " << test->waypointType << " | " << ans->turnRadius << " " << test->turnRadius << endl;
        return WAYPOINT_INCORRECT;
    }

    return WAYPOINT_CORRECT;
}

TEST(Waypoint_Manager, InitializedFlightPathAndHomeBase) {

   	/***********************SETUP***********************/

	WaypointManager * w = new WaypointManager(); // Creates object

    // Creates the initial flight path and home base

    int numPaths = 20;

    _PathData ** initialPaths = new _PathData*[PATH_BUFFER_SIZE];
    _PathData ** testArray = new _PathData*[PATH_BUFFER_SIZE];
    _PathData * homeBase;
    _PathData * testHomeBase;

    _WaypointBufferStatus * status = new _WaypointBufferStatus[PATH_BUFFER_SIZE];

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
        status[i] = FULL;
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
        status[i] = FULL;
        id_array[i] = initialPaths[i]->waypointId;
        nextElement = i+1;
        longitude++;
        latitude++;
        altitude++;
        turnRadius++;
    }

    for(int i = numPaths; i < PATH_BUFFER_SIZE; i++) {
        initialPaths[i] = w->initialize_waypoint();
    }

    // Initializes waypint maanger with the flight path
    _WaypointStatus e = w->initialize_flight_path(initialPaths, numPaths, homeBase);
    testArray = w->get_waypoint_buffer();
    _ArrayStatus a = compare_arrays(initialPaths, testArray, numPaths);
    testHomeBase = w->get_home_base();
    _WaypointSt b = compare_waypoint(homeBase, testHomeBase);
    _ArrayStatus c = compare_buffer_status(status, w);
	
	/**********************ASSERTS**********************/

    ASSERT_EQ(b, WAYPOINT_CORRECT); // Tests equality of the two parameters
	ASSERT_EQ(a, ARRAY_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(c, ARRAY_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(e, WAYPOINT_SUCCESS); // Tests equality of the two parameters
}

TEST(Waypoint_Manager, AppendElementToNotFilledArray) {

   	/***********************SETUP***********************/

	WaypointManager * w2 = new WaypointManager(); // Creates object

    // Creates the initial flight path and home base

    int numPaths = 50;

    _PathData ** initialPaths = new _PathData*[PATH_BUFFER_SIZE];
    _PathData ** testArray = new _PathData*[PATH_BUFFER_SIZE];
    _PathData * testHomeBase;

    _WaypointBufferStatus * status = new _WaypointBufferStatus[PATH_BUFFER_SIZE];

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

    for(int i = 0; i < numPaths/2; i++) {
        initialPaths[i] = w2->initialize_waypoint(longitude, latitude, altitude, waypointType);
        status[i] = FULL;
        id_array[i] = initialPaths[i]->waypointId;
        nextElement = i+1;
        longitude++;
        latitude++;
        altitude++;
    }

    waypointType = 2;
    float turnRadius = 10;

    for(int i = numPaths/2; i < numPaths; i++) {
        initialPaths[i] = w2->initialize_waypoint(longitude, latitude, altitude, waypointType, turnRadius);
        status[i] = FULL;
        id_array[i] = initialPaths[i]->waypointId;
        nextElement = i+1;
        longitude++;
        latitude++;
        altitude++;
        turnRadius++;
    }

    _WaypointStatus e1 = w2->initialize_flight_path(initialPaths, numPaths);

    // Inserts new element
    _PathData * appendElement = w2->initialize_waypoint(1000, 1001, 10002, 2, -10);
    initialPaths[numPaths] = appendElement;
    status[numPaths] = FULL;
    numPaths++;

    // Initializes waypint maanger with the flight path
    _WaypointStatus e2 = w2->update_path_nodes(appendElement, APPEND_WAYPOINT, 0, 0, 0);
    testArray = w2->get_waypoint_buffer();
    _ArrayStatus a = compare_arrays(initialPaths, testArray, numPaths);
    testHomeBase = w2->get_home_base();
    _ArrayStatus c = compare_buffer_status(status, w2);
	
	/**********************ASSERTS**********************/

	ASSERT_EQ(a, ARRAY_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(c, ARRAY_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(e1, WAYPOINT_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(e2, WAYPOINT_SUCCESS); // Tests equality of the two parameters
}

TEST(Waypoint_Manager, AppendElementToFirstElement) {

   	/***********************SETUP***********************/

	WaypointManager * w2 = new WaypointManager(); // Creates object

    // Creates the initial flight path and home base

    int numPaths = 0;

    _PathData ** initialPaths = new _PathData*[PATH_BUFFER_SIZE];
    _PathData ** testArray = new _PathData*[PATH_BUFFER_SIZE];
    _PathData * testHomeBase;

    _WaypointBufferStatus * status = new _WaypointBufferStatus[PATH_BUFFER_SIZE];

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

    for(int i = 0; i < numPaths/2; i++) {
        initialPaths[i] = w2->initialize_waypoint(longitude, latitude, altitude, waypointType);
        status[i] = FULL;
        id_array[i] = initialPaths[i]->waypointId;
        nextElement = i+1;
        longitude++;
        latitude++;
        altitude++;
    }

    waypointType = 2;
    float turnRadius = 10;

    for(int i = numPaths/2; i < numPaths; i++) {
        initialPaths[i] = w2->initialize_waypoint(longitude, latitude, altitude, waypointType, turnRadius);
        status[i] = FULL;
        id_array[i] = initialPaths[i]->waypointId;
        nextElement = i+1;
        longitude++;
        latitude++;
        altitude++;
        turnRadius++;
    }

    _WaypointStatus e1 = w2->initialize_flight_path(initialPaths, numPaths);

    // Appends new element
    _PathData * appendElement = w2->initialize_waypoint(1000, 1001, 10002, 2, -10);
    initialPaths[numPaths] = appendElement;
    status[numPaths] = FULL;
    numPaths++;

    // Initializes waypint maanger with the flight path
    _WaypointStatus e2 = w2->update_path_nodes(appendElement, APPEND_WAYPOINT, 0, 0, 0);
    testArray = w2->get_waypoint_buffer();
    _ArrayStatus a = compare_arrays(initialPaths, testArray, numPaths);
    testHomeBase = w2->get_home_base();
    _ArrayStatus c = compare_buffer_status(status, w2);
	
	/**********************ASSERTS**********************/

	ASSERT_EQ(a, ARRAY_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(c, ARRAY_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(e1, WAYPOINT_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(e2, WAYPOINT_SUCCESS); // Tests equality of the two parameters
}

TEST(Waypoint_Manager, AppendElementToNinteyNineElementArray) {

   	/***********************SETUP***********************/

	WaypointManager * w2 = new WaypointManager(); // Creates object

    // Creates the initial flight path and home base

    int numPaths = 99;

    _PathData ** initialPaths = new _PathData*[PATH_BUFFER_SIZE];
    _PathData ** testArray = new _PathData*[PATH_BUFFER_SIZE];
    _PathData * testHomeBase;

    _WaypointBufferStatus * status = new _WaypointBufferStatus[PATH_BUFFER_SIZE];

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

    for(int i = 0; i < numPaths/2; i++) {
        initialPaths[i] = w2->initialize_waypoint(longitude, latitude, altitude, waypointType);
        status[i] = FULL;
        id_array[i] = initialPaths[i]->waypointId;
        nextElement = i+1;
        longitude++;
        latitude++;
        altitude++;
    }

    waypointType = 2;
    float turnRadius = 10;

    for(int i = numPaths/2; i < numPaths; i++) {
        initialPaths[i] = w2->initialize_waypoint(longitude, latitude, altitude, waypointType, turnRadius);
        status[i] = FULL;
        id_array[i] = initialPaths[i]->waypointId;
        nextElement = i+1;
        longitude++;
        latitude++;
        altitude++;
        turnRadius++;
    }

    _WaypointStatus e1 = w2->initialize_flight_path(initialPaths, numPaths);

    // Appemds new element
    _PathData * appendElement = w2->initialize_waypoint(1000, 1001, 10002, 2, -10);
    initialPaths[numPaths] = appendElement;
    status[numPaths] = FULL;
    numPaths++;

    // Initializes waypint maanger with the flight path
    _WaypointStatus e2 = w2->update_path_nodes(appendElement, APPEND_WAYPOINT, 0, 0, 0);
    testArray = w2->get_waypoint_buffer();
    _ArrayStatus a = compare_arrays(initialPaths, testArray, numPaths);
    testHomeBase = w2->get_home_base();
    _ArrayStatus c = compare_buffer_status(status, w2);
	
	/**********************ASSERTS**********************/

	ASSERT_EQ(a, ARRAY_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(c, ARRAY_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(e1, WAYPOINT_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(e2, WAYPOINT_SUCCESS); // Tests equality of the two parameters
}

TEST(Waypoint_Manager, AppendElementToFull) {

   	/***********************SETUP***********************/

	WaypointManager * w2 = new WaypointManager(); // Creates object

    // Creates the initial flight path and home base

    int numPaths = PATH_BUFFER_SIZE;

    _PathData ** initialPaths = new _PathData*[PATH_BUFFER_SIZE];
    _PathData ** testArray = new _PathData*[PATH_BUFFER_SIZE];
    _PathData * testHomeBase;

    _WaypointBufferStatus * status = new _WaypointBufferStatus[PATH_BUFFER_SIZE];

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

    for(int i = 0; i < numPaths/2; i++) {
        initialPaths[i] = w2->initialize_waypoint(longitude, latitude, altitude, waypointType);
        status[i] = FULL;
        id_array[i] = initialPaths[i]->waypointId;
        nextElement = i+1;
        longitude++;
        latitude++;
        altitude++;
    }

    waypointType = 2;
    float turnRadius = 10;

    for(int i = numPaths/2; i < numPaths; i++) {
        initialPaths[i] = w2->initialize_waypoint(longitude, latitude, altitude, waypointType, turnRadius);
        status[i] = FULL;
        id_array[i] = initialPaths[i]->waypointId;
        nextElement = i+1;
        longitude++;
        latitude++;
        altitude++;
        turnRadius++;
    }

    _WaypointStatus e1 = w2->initialize_flight_path(initialPaths, numPaths);

    // Should not append new element
    _PathData * appendElement = w2->initialize_waypoint(1000, 1001, 10002, 2, -10);
    // initialPaths[numPaths] = appendElement;
    // status[numPaths] = FULL;
    // numPaths++;

    // Initializes waypint maanger with the flight path
    _WaypointStatus e2 = w2->update_path_nodes(appendElement, APPEND_WAYPOINT, 0, 0, 0);
    testArray = w2->get_waypoint_buffer();
    _ArrayStatus a = compare_arrays(initialPaths, testArray, numPaths);
    testHomeBase = w2->get_home_base();
    _ArrayStatus c = compare_buffer_status(status, w2);
	
	/**********************ASSERTS**********************/

	ASSERT_EQ(a, ARRAY_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(c, ARRAY_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(e1, WAYPOINT_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(e2, UNDEFINED_FAILURE); // Tests equality of the two parameters
}


TEST(Waypoint_Manager, InsertElementToFirstIndex) {

   	/***********************SETUP***********************/

	WaypointManager * w2 = new WaypointManager(); // Creates object

    // Creates the initial flight path and home base

    int numPaths = 50;

    _PathData ** initialPaths = new _PathData*[PATH_BUFFER_SIZE];
    _PathData ** testArray = new _PathData*[PATH_BUFFER_SIZE];
    _PathData * testHomeBase;

    _WaypointBufferStatus * status = new _WaypointBufferStatus[PATH_BUFFER_SIZE];

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

    for(int i = 0; i < numPaths/2; i++) {
        initialPaths[i] = w2->initialize_waypoint(longitude, latitude, altitude, waypointType);
        status[i] = FULL;
        id_array[i] = initialPaths[i]->waypointId;
        nextElement = i+1;
        longitude++;
        latitude++;
        altitude++;
    }

    waypointType = 2;
    float turnRadius = 10;

    for(int i = numPaths/2; i < numPaths; i++) {
        initialPaths[i] = w2->initialize_waypoint(longitude, latitude, altitude, waypointType, turnRadius);
        status[i] = FULL;
        id_array[i] = initialPaths[i]->waypointId;
        nextElement = i+1;
        longitude++;
        latitude++;
        altitude++;
        turnRadius++;
    }

    _WaypointStatus e1 = w2->initialize_flight_path(initialPaths, numPaths);

    // Should not insert new element
    _PathData * insertElement = w2->initialize_waypoint(1000, 1000, 1000, 1000, 2);
    // initialPaths[numPaths] = insertElement;
    // status[numPaths] = FULL;
    // numPaths++;

    // Initializes waypint maanger with the flight path
    _WaypointStatus e2 = w2->update_path_nodes(insertElement, INSERT_WAYPOINT, 0, 0, id_array[0]);
    testArray = w2->get_waypoint_buffer();
    _ArrayStatus a = compare_arrays(initialPaths, testArray, numPaths);
    testHomeBase = w2->get_home_base();
    _ArrayStatus c = compare_buffer_status(status, w2);
	
	/**********************ASSERTS**********************/

	ASSERT_EQ(a, ARRAY_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(c, ARRAY_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(e1, WAYPOINT_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(e2, UNDEFINED_FAILURE); // Tests equality of the two parameters
}

TEST(Waypoint_Manager, InsertElementToLastIndexArrayNotFull) {

   	/***********************SETUP***********************/

	WaypointManager * w2 = new WaypointManager(); // Creates object

    // Creates the initial flight path and home base

    int numPaths = 50;

    _PathData ** initialPaths = new _PathData*[PATH_BUFFER_SIZE];
    _PathData ** testArray = new _PathData*[PATH_BUFFER_SIZE];
    _PathData * testHomeBase;

    _WaypointBufferStatus * status = new _WaypointBufferStatus[PATH_BUFFER_SIZE];

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

    for(int i = 0; i < numPaths/2; i++) {
        initialPaths[i] = w2->initialize_waypoint(longitude, latitude, altitude, waypointType);
        status[i] = FULL;
        id_array[i] = initialPaths[i]->waypointId;
        nextElement = i+1;
        longitude++;
        latitude++;
        altitude++;
    }

    waypointType = 2;
    float turnRadius = 10;

    for(int i = numPaths/2; i < numPaths; i++) {
        initialPaths[i] = w2->initialize_waypoint(longitude, latitude, altitude, waypointType, turnRadius);
        status[i] = FULL;
        id_array[i] = initialPaths[i]->waypointId;
        nextElement = i+1;
        longitude++;
        latitude++;
        altitude++;
        turnRadius++;
    }

    _WaypointStatus e1 = w2->initialize_flight_path(initialPaths, numPaths);

    // Should not insert new element
    _PathData * insertElement = w2->initialize_waypoint(1000, 1000, 1000, 1000, 2);
    // initialPaths[numPaths] = insertElement;
    // status[numPaths] = FULL;
    // numPaths++;

    // Initializes waypint maanger with the flight path
    _WaypointStatus e2 = w2->update_path_nodes(insertElement, INSERT_WAYPOINT, 0, id_array[numPaths-1], 0);
    testArray = w2->get_waypoint_buffer();
    _ArrayStatus a = compare_arrays(initialPaths, testArray, numPaths);
    testHomeBase = w2->get_home_base();
    _ArrayStatus c = compare_buffer_status(status, w2);
	
	/**********************ASSERTS**********************/

	ASSERT_EQ(a, ARRAY_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(c, ARRAY_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(e1, WAYPOINT_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(e2, UNDEFINED_FAILURE); // Tests equality of the two parameters
}

TEST(Waypoint_Manager, InsertElementToFullArray) {

   	/***********************SETUP***********************/

	WaypointManager * w2 = new WaypointManager(); // Creates object

    // Creates the initial flight path and home base

    int numPaths = 100;

    _PathData ** initialPaths = new _PathData*[PATH_BUFFER_SIZE];
    _PathData ** testArray = new _PathData*[PATH_BUFFER_SIZE];
    _PathData * testHomeBase;

    _WaypointBufferStatus * status = new _WaypointBufferStatus[PATH_BUFFER_SIZE];

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

    for(int i = 0; i < numPaths/2; i++) {
        initialPaths[i] = w2->initialize_waypoint(longitude, latitude, altitude, waypointType);
        status[i] = FULL;
        id_array[i] = initialPaths[i]->waypointId;
        nextElement = i+1;
        longitude++;
        latitude++;
        altitude++;
    }

    waypointType = 2;
    float turnRadius = 10;

    for(int i = numPaths/2; i < numPaths; i++) {
        initialPaths[i] = w2->initialize_waypoint(longitude, latitude, altitude, waypointType, turnRadius);
        status[i] = FULL;
        id_array[i] = initialPaths[i]->waypointId;
        nextElement = i+1;
        longitude++;
        latitude++;
        altitude++;
        turnRadius++;
    }

    _WaypointStatus e1 = w2->initialize_flight_path(initialPaths, numPaths);

    // Should not insert new element
    _PathData * insertElement = w2->initialize_waypoint(1000, 1000, 1000, 1000, 2);
    // initialPaths[numPaths] = insertElement;
    // status[numPaths] = FULL;
    // numPaths++;

    // Initializes waypint maanger with the flight path
    _WaypointStatus e2 = w2->update_path_nodes(insertElement, INSERT_WAYPOINT, 0, id_array[99], 0);
    testArray = w2->get_waypoint_buffer();
    _ArrayStatus a = compare_arrays(initialPaths, testArray, numPaths);
    testHomeBase = w2->get_home_base();
    _ArrayStatus c = compare_buffer_status(status, w2);
	
	/**********************ASSERTS**********************/

	ASSERT_EQ(a, ARRAY_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(c, ARRAY_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(e1, WAYPOINT_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(e2, UNDEFINED_FAILURE); // Tests equality of the two parameters
}

TEST(Waypoint_Manager, InsertToArrayInMiddle) {

   	/***********************SETUP***********************/

	WaypointManager * w2 = new WaypointManager(); // Creates object

    // Creates the initial flight path and home base

    int numPaths = 80;
    int insertIndex = 50;

    _PathData ** initialPaths = new _PathData*[PATH_BUFFER_SIZE];
    _PathData ** insertPaths = new _PathData*[PATH_BUFFER_SIZE];
    _PathData ** testArray = new _PathData*[PATH_BUFFER_SIZE];
    _PathData * testHomeBase;

    _WaypointBufferStatus * status = new _WaypointBufferStatus[PATH_BUFFER_SIZE];

    _PathData * insertElement = w2->initialize_waypoint(1000, 1000, 1000, 1000, 2);

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

    for(int i = 0; i < numPaths+1; i++) {
        if(i < numPaths) {
            initialPaths[i] = w2->initialize_waypoint(longitude, latitude, altitude, waypointType);
            status[i] = FULL;
            id_array[i] = initialPaths[i]->waypointId;
            nextElement = i+1;
            longitude++;
            latitude++;
            altitude++;
        }

        if(i < insertIndex) {
            insertPaths[i] = initialPaths[i];
        } if (i > insertIndex) {
            insertPaths[i] = initialPaths[i-1];
        } else if (i == insertIndex) {
            // std::cout << "Hello there" << std::endl;
            insertPaths[i] = insertElement;
        }
    }

    waypointType = 2;
    float turnRadius = 10;

    _WaypointStatus e1 = w2->initialize_flight_path(initialPaths, numPaths);

    // Should not insert new element
    status[numPaths] = FULL;
    numPaths++;

    // Initializes waypint maanger with the flight path
    _WaypointStatus e2 = w2->update_path_nodes(insertElement, INSERT_WAYPOINT, 0, id_array[insertIndex-1], id_array[insertIndex]);
    testArray = w2->get_waypoint_buffer();
    _ArrayStatus a = compare_arrays(insertPaths, testArray, numPaths);
    testHomeBase = w2->get_home_base();
    _ArrayStatus c = compare_buffer_status(status, w2);
	
	/**********************ASSERTS**********************/

	ASSERT_EQ(a, ARRAY_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(c, ARRAY_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(e1, WAYPOINT_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(e2, WAYPOINT_SUCCESS); // Tests equality of the two parameters
}
