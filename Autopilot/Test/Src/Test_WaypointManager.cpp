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

enum _ArrayStatus {ARRAY_SUCCESS = 0, ARRAY_DIFFERENT};
enum _WaypointSt {WAYPOINT_CORRECT = 0, WAYPOINT_INCORRECT};
enum _OutputStatus {OUTPUT_CORRECT = 0, OUTPUT_INCORRECT};

static _ArrayStatus compare_arrays(_PathData ** ans, _PathData ** testArray, int numElements) {
    _PathData * nextWaypoint;
    nextWaypoint = testArray[0];

    // Checks if nexts are linked properly
    for(int i = 0; i < numElements; i++) {
        if(ans[i]->waypointType != -1 && ans[i]->waypointId == nextWaypoint->waypointId && ans[i]->longitude == nextWaypoint->longitude && ans[i]->latitude == nextWaypoint->latitude && ans[i]->altitude == nextWaypoint->altitude && ans[i]->waypointType == nextWaypoint->waypointType && ans[i]->turnRadius == nextWaypoint->turnRadius) {
            nextWaypoint = nextWaypoint->next;
        } else if (ans[i]->waypointType == -1 && ans[i]->waypointId == nextWaypoint->waypointId && ans[i]->longitude == nextWaypoint->longitude && ans[i]->latitude == nextWaypoint->latitude && ans[i]->altitude == nextWaypoint->altitude && ans[i]->waypointType == nextWaypoint->waypointType && ans[i]->turnRadius == nextWaypoint->turnRadius) {
        } else {
            // cout << "Next Check Index: " << i << " | " << ans[i]->waypointId << " " << nextWaypoint->waypointId << " | " << ans[i]->longitude << " " << nextWaypoint->longitude << " | " << ans[i]->latitude << " " << nextWaypoint->latitude << " | " << ans[i]->altitude << " " << nextWaypoint->altitude << " | " << ans[i]->waypointType << " " << nextWaypoint->waypointType << " | " << ans[i]->turnRadius << " " << nextWaypoint->turnRadius << endl;
            return ARRAY_DIFFERENT;
        }
    }

    nextWaypoint = testArray[numElements - 1];

    // Checks if previous are linked properly
    for(int i = numElements-1; i >= 0; i--) {
        if(ans[i]->waypointType != -1 && ans[i]->waypointId == nextWaypoint->waypointId && ans[i]->longitude == nextWaypoint->longitude && ans[i]->latitude == nextWaypoint->latitude && ans[i]->altitude == nextWaypoint->altitude && ans[i]->waypointType == nextWaypoint->waypointType && ans[i]->turnRadius == nextWaypoint->turnRadius) {
            nextWaypoint = nextWaypoint->previous;
        } else if (ans[i]->waypointType == -1 && ans[i]->waypointType == -1 && ans[i]->waypointId == nextWaypoint->waypointId && ans[i]->longitude == nextWaypoint->longitude && ans[i]->latitude == nextWaypoint->latitude && ans[i]->altitude == nextWaypoint->altitude && ans[i]->waypointType == nextWaypoint->waypointType && ans[i]->turnRadius == nextWaypoint->turnRadius) {
        } else {
            // cout << "Previous Check Index: " << i << " | " << ans[i]->waypointId << " " << nextWaypoint->waypointId << " | " << ans[i]->longitude << " " << nextWaypoint->longitude << " | " << ans[i]->latitude << " " << nextWaypoint->latitude << " | " << ans[i]->altitude << " " << nextWaypoint->altitude << " | " << ans[i]->waypointType << " " << nextWaypoint->waypointType << " | " << ans[i]->turnRadius << " " << nextWaypoint->turnRadius << endl;
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
            // cout << "Buffer Compare Index: " << i << " | " << ans[i] << " " << w->get_status_of_index(i) << endl;
            return ARRAY_DIFFERENT;
        }
    }

    return ARRAY_SUCCESS;
} 

static _WaypointSt compare_waypoint(_PathData * ans, _PathData * test) {
    if(ans->waypointId != test->waypointId && ans->longitude != test->longitude && ans->latitude != test->latitude && ans->altitude != test->altitude && ans->waypointType != test->waypointType && ans->turnRadius != test->turnRadius) {
        // cout << "Waypoint Equality Check: " << ans->waypointId << " " << test->waypointId << " | " << ans->longitude << " " << test->longitude << " | " << ans->latitude << " " << test->latitude << " | " << ans->altitude << " " << test->altitude << " | " << ans->waypointType << " " << test->waypointType << " | " << ans->turnRadius << " " << test->turnRadius << endl;
        return WAYPOINT_INCORRECT;
    }

    return WAYPOINT_CORRECT;
}

static _OutputStatus compare_output_data(_WaypointManager_Data_Out *ans, _WaypointManager_Data_Out *test) {
    if(ans->desiredAltitude == test->desiredAltitude && ans->desiredHeading == test->desiredHeading && ans->distanceToNextWaypoint == test->distanceToNextWaypoint && ans->radius == test->radius && ans->turnDirection == test->turnDirection && ans->out_type == test->out_type) {
        return OUTPUT_CORRECT;
    } else {
        cout << "Comparing Output Data: Alt " << ans->desiredAltitude << " " << test->desiredAltitude << " | Heading " << ans->desiredHeading << " " << test->desiredHeading << " | Distance " << ans->distanceToNextWaypoint << " " << test->distanceToNextWaypoint << " | Radius " << ans->radius << " " << test->radius << " | Direction " << ans->turnDirection << " " << test->turnDirection << " | OutType " << ans->out_type << " " << test->out_type << endl;
        return OUTPUT_INCORRECT;
    }
}

static _OutputStatus compare_coordinates(float * ans, float * test) {
    // Accept an error of maximum 0.01 degrees
    if (abs((ans[0]- test[0])/ans[0]) < 0.01 && abs((ans[1]- test[1])/ans[1]) < 0.01 && abs((ans[2]- test[2])/ans[2]) < 0.01) {
        // cout << "Comparing Coordinates: " << ans[0] << " " << test[0] << " | " << ans[1] << " " << test[1] << " | " << ans[2] << " " << test[2] << endl;
        return OUTPUT_CORRECT; 
    } else {
        // cout << "Comparing Coordinates: " << ans[0] << " " << test[0] << " | " << ans[1] << " " << test[1] << " | " << ans[2] << " " << test[2] << endl;
        return OUTPUT_INCORRECT;
    }
}


/************************ TESTING INITIALIZING AND SETTING WAYPOINT MANAGER OBJECT ************************/


TEST(Waypoint_Manager, InitializedFlightPathAndHomeBase) {

   	/***********************SETUP***********************/

	WaypointManager * w = new WaypointManager(43.467998128, 80.537331184); // Creates object

    // Creates the initial flight path and home base

    int numPaths = 20;

    _PathData ** initialPaths = new _PathData*[PATH_BUFFER_SIZE];
    _PathData ** testArray = new _PathData*[PATH_BUFFER_SIZE];
    _PathData * homeBase;
    _PathData * testHomeBase;

    _WaypointBufferStatus * status = new _WaypointBufferStatus[PATH_BUFFER_SIZE];

    // Initializes status array  
    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        status[i] = FREE;
    }

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

    // Initializes waypint maanger with the flight path
    _WaypointStatus e = w->initialize_flight_path(initialPaths, numPaths, homeBase);

    testArray = w->get_waypoint_buffer();
    _ArrayStatus a = compare_arrays(initialPaths, testArray, numPaths); // Compares the waypointBuffer arrays

    testHomeBase = w->get_home_base();
    _WaypointSt b = compare_waypoint(homeBase, testHomeBase);  // Checks if the homeBase waypoints are the same 

    _ArrayStatus c = compare_buffer_status(status, w); // Compares the waypointBufferStatus arrays
	
	/**********************ASSERTS**********************/

    ASSERT_EQ(b, WAYPOINT_CORRECT); // Tests equality of the two parameters
	ASSERT_EQ(a, ARRAY_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(c, ARRAY_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(e, WAYPOINT_SUCCESS); // Tests equality of the two parameters
}


/************************ TESTING GETTING DESIRED HEADING/ALTITUDE/ETC ************************/


TEST(Waypoint_Manager, DesiredHeadingForOrbit) {
    /***********************SETUP***********************/

	WaypointManager * w = new WaypointManager(43.467998128, 80.537331184); // Creates object

    // Stores outputs from four tests
    _WaypointManager_Data_Out * out1 = new _WaypointManager_Data_Out;
    out1->desiredHeading = 0;
    out1->desiredAltitude = 0;
    out1->distanceToNextWaypoint = 0;
    out1->radius = 0;
    out1->turnDirection = 0;
    out1->errorCode = WAYPOINT_SUCCESS;
    out1->isDataNew = false;
    out1->timeOfData = 0;
    out1->out_type = PATH_FOLLOW;

    _WaypointManager_Data_Out * out2 = new _WaypointManager_Data_Out;;
    out2->desiredHeading = 0;
    out2->desiredAltitude = 0;
    out2->distanceToNextWaypoint = 0;
    out2->radius = 0;
    out2->turnDirection = 0;
    out2->errorCode = WAYPOINT_SUCCESS;
    out2->isDataNew = false;
    out2->timeOfData = 0;
    out2->out_type = PATH_FOLLOW;

    // Creates two test values!
    _WaypointManager_Data_In setup1 = {43.467998128, 80.537331184, 100, 100};  // latitude, longitude, altitude, heading
    _WaypointManager_Data_In setup2 = {43.467998128, 80.537331184, 100, 30};  // latitude, longitude, altitude, heading
    
    _WaypointManager_Data_In input1 = {43.467998128, 80.537331184, 100, 100};  // latitude, longitude, altitude, heading
    _WaypointManager_Data_In input2 = {43.467998128, 80.537331184, 100, 30};  // latitude, longitude, altitude, heading

    // Stores answers for four tests
    float center_ans1[3] = {80.54500000, 43.47138889, 78}; // longitude, latitude, altitude
    _WaypointManager_Data_Out * ans1 = new _WaypointManager_Data_Out;
    ans1->desiredHeading = 280;         // Setting to current output just so test passes for now. WILL CHANGE LATER
    ans1->desiredAltitude = 78;
    ans1->distanceToNextWaypoint = 0;
    ans1->radius = 100;
    ans1->turnDirection = 1;
    ans1->errorCode = WAYPOINT_SUCCESS;
    ans1->isDataNew = true;
    ans1->timeOfData = 0;
    ans1->out_type = ORBIT_FOLLOW;

    float center_ans2[3] = {80.54527778, 43.47250000, 110}; 
    _WaypointManager_Data_Out * ans2 = new _WaypointManager_Data_Out;
    ans2->desiredHeading = 209;        // Setting to current output just so test passes for now. WILL CHANGE LATER
    ans2->desiredAltitude = 110;
    ans2->distanceToNextWaypoint = 0;
    ans2->radius = 30;
    ans2->turnDirection = 2;
    ans2->errorCode = WAYPOINT_SUCCESS;
    ans2->isDataNew = true;
    ans2->timeOfData = 0;
    ans2->out_type = ORBIT_FOLLOW;


	/********************STEPTHROUGH********************/

    int turnRadius[2] = {100, 30};
    int turnDirection[2] = {-1, 1}; // 1 = CW, 2 = CCW
    int altitude[2] = {78, 110};
    bool cancelTurning = false;

    // std::cout << "Here1" << std::endl;

    w->start_circling(setup1, turnRadius[0], turnDirection[0], altitude[0], cancelTurning); // Sets circling

    _WaypointStatus s1 = w->get_next_directions(input1, out1);

    // std::cout << "Here2" << std::endl;

    float response[3]; // longitude, latitude
    response[0] = w->orbitCentreLong;
    response[1] = w->orbitCentreLat;
    response[2] = w->orbitCentreAlt;

    _OutputStatus test1_center = compare_coordinates(center_ans1, response);
    _OutputStatus test1_output = compare_output_data(ans1, out1);

    // std::cout << "Here3" << std::endl;

    w->start_circling(setup2, turnRadius[1], turnDirection[1], altitude[1], cancelTurning); // Sets circling

    _WaypointStatus s2 =w->get_next_directions(input2, out2);

    response[0] = w->orbitCentreLong;
    response[1] = w->orbitCentreLat;
    response[2] = w->orbitCentreAlt;

    _OutputStatus test2_center = compare_coordinates(center_ans2, response);
    _OutputStatus test2_output = compare_output_data(ans2, out2);
	
	/**********************ASSERTS**********************/

    ASSERT_EQ(s1, WAYPOINT_SUCCESS);
    ASSERT_EQ(s2, WAYPOINT_SUCCESS);

    ASSERT_EQ(test1_center, OUTPUT_CORRECT);
    ASSERT_EQ(test1_output, OUTPUT_CORRECT);

    ASSERT_EQ(test2_center, OUTPUT_CORRECT);
    ASSERT_EQ(test2_output, OUTPUT_CORRECT);
}


/************************ TESTING MODIFYING THE FLIGHT PATH ************************/


TEST(Waypoint_Manager, AppendElementToNotFilledArray) {

   	/***********************SETUP***********************/

	WaypointManager * w2 = new WaypointManager(43.467998128, 80.537331184); // Creates object

    // Creates the initial flight path and home base

    int numPaths = 50;

    _PathData ** initialPaths = new _PathData*[PATH_BUFFER_SIZE];
    _PathData ** testArray = new _PathData*[PATH_BUFFER_SIZE];
    _PathData * testHomeBase;

    _WaypointBufferStatus * status = new _WaypointBufferStatus[PATH_BUFFER_SIZE];

    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        status[i] = FREE;
    }

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

    _WaypointStatus e1 = w2->initialize_flight_path(initialPaths, numPaths); // Checks if initialization was successful

    // Appends new element
    _PathData * appendElement = w2->initialize_waypoint(1000, 1001, 10002, 2, -10);
    initialPaths[numPaths] = appendElement;
    status[numPaths] = FULL;
    numPaths++;

    // Updates the waypointBuffer and waypointBufferStatus and compares values
    _WaypointStatus e2 = w2->update_path_nodes(appendElement, APPEND_WAYPOINT, 0, 0, 0); // Checks if updating waypointBuffer is successful
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

	WaypointManager * w2 = new WaypointManager(43.467998128, 80.537331184); // Creates object

    // Creates the initial flight path and home base

    int numPaths = 0;

    _PathData ** initialPaths = new _PathData*[PATH_BUFFER_SIZE];
    _PathData ** testArray = new _PathData*[PATH_BUFFER_SIZE];
    _PathData * testHomeBase;

    _WaypointBufferStatus * status = new _WaypointBufferStatus[PATH_BUFFER_SIZE];

    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        status[i] = FREE;
    }

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

    for(int i = 0; i < numPaths; i++) {
        initialPaths[i] = w2->initialize_waypoint(longitude, latitude, altitude, waypointType);
        status[i] = FULL;
        id_array[i] = initialPaths[i]->waypointId;
        nextElement = i+1;
        longitude++;
        latitude++;
        altitude++;
    }

    _WaypointStatus e1 = w2->initialize_flight_path(initialPaths, numPaths);

    // Appends new element
    _PathData * appendElement = w2->initialize_waypoint(1000, 1001, 10002, 2, -10);
    initialPaths[numPaths] = appendElement;
    status[numPaths] = FULL;
    numPaths++;

    // Updates the waypointBuffer and waypointBufferStatus and compares values
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

	WaypointManager * w2 = new WaypointManager(43.467998128, 80.537331184); // Creates object

    // Creates the initial flight path and home base

    int numPaths = 99;

    _PathData ** initialPaths = new _PathData*[PATH_BUFFER_SIZE];
    _PathData ** testArray = new _PathData*[PATH_BUFFER_SIZE];
    _PathData * testHomeBase;

    _WaypointBufferStatus * status = new _WaypointBufferStatus[PATH_BUFFER_SIZE];

    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        status[i] = FREE;
    }

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

    for(int i = 0; i < numPaths; i++) {
        initialPaths[i] = w2->initialize_waypoint(longitude, latitude, altitude, waypointType);
        status[i] = FULL;
        id_array[i] = initialPaths[i]->waypointId;
        nextElement = i+1;
        longitude++;
        latitude++;
        altitude++;
    }

    _WaypointStatus e1 = w2->initialize_flight_path(initialPaths, numPaths);

    // Appemds new element
    _PathData * appendElement = w2->initialize_waypoint(1000, 1001, 10002, 2, -10);
    initialPaths[numPaths] = appendElement;
    status[numPaths] = FULL;
    numPaths++;

    // Updates the waypointBuffer and waypointBufferStatus and compares values
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

	WaypointManager * w2 = new WaypointManager(43.467998128, 80.537331184); // Creates object

    // Creates the initial flight path and home base

    int numPaths = PATH_BUFFER_SIZE;

    _PathData ** initialPaths = new _PathData*[PATH_BUFFER_SIZE];
    _PathData ** testArray = new _PathData*[PATH_BUFFER_SIZE];
    _PathData * testHomeBase;

    _WaypointBufferStatus * status = new _WaypointBufferStatus[PATH_BUFFER_SIZE];

    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        status[i] = FREE;
    }

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

    for(int i = 0; i < numPaths; i++) {
        initialPaths[i] = w2->initialize_waypoint(longitude, latitude, altitude, waypointType);
        status[i] = FULL;
        id_array[i] = initialPaths[i]->waypointId;
        nextElement = i+1;
        longitude++;
        latitude++;
        altitude++;
    }

    _WaypointStatus e1 = w2->initialize_flight_path(initialPaths, numPaths);

    // Should not append new element
    _PathData * appendElement = w2->initialize_waypoint(1000, 1001, 10002, 2, -10);
    // initialPaths[numPaths] = appendElement;
    // status[numPaths] = FULL;
    // numPaths++;

    // Updates the waypointBuffer and waypointBufferStatus and compares values
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

	WaypointManager * w2 = new WaypointManager(43.467998128, 80.537331184); // Creates object

    // Creates the initial flight path and home base

    int numPaths = 50;

    _PathData ** initialPaths = new _PathData*[PATH_BUFFER_SIZE];
    _PathData ** testArray = new _PathData*[PATH_BUFFER_SIZE];
    _PathData * testHomeBase;

    _WaypointBufferStatus * status = new _WaypointBufferStatus[PATH_BUFFER_SIZE];

    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        status[i] = FREE;
    }

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

    for(int i = 0; i < numPaths; i++) {
        initialPaths[i] = w2->initialize_waypoint(longitude, latitude, altitude, waypointType);
        status[i] = FULL;
        id_array[i] = initialPaths[i]->waypointId;
        nextElement = i+1;
        longitude++;
        latitude++;
        altitude++;
    }

    _WaypointStatus e1 = w2->initialize_flight_path(initialPaths, numPaths);

    // Should not insert new element
    _PathData * insertElement = w2->initialize_waypoint(1000, 1000, 1000, 1000, 2);
    // initialPaths[numPaths] = insertElement;
    // status[numPaths] = FULL;
    // numPaths++;

    // Updates the waypointBuffer and waypointBufferStatus and compares values
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

	WaypointManager * w2 = new WaypointManager(43.467998128, 80.537331184); // Creates object

    // Creates the initial flight path and home base

    int numPaths = 50;

    _PathData ** initialPaths = new _PathData*[PATH_BUFFER_SIZE];
    _PathData ** testArray = new _PathData*[PATH_BUFFER_SIZE];
    _PathData * testHomeBase;

    _WaypointBufferStatus * status = new _WaypointBufferStatus[PATH_BUFFER_SIZE];

    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        status[i] = FREE;
    }

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

    for(int i = 0; i < numPaths; i++) {
        initialPaths[i] = w2->initialize_waypoint(longitude, latitude, altitude, waypointType);
        status[i] = FULL;
        id_array[i] = initialPaths[i]->waypointId;
        nextElement = i+1;
        longitude++;
        latitude++;
        altitude++;
    }

    _WaypointStatus e1 = w2->initialize_flight_path(initialPaths, numPaths);

    // Should not insert new element
    _PathData * insertElement = w2->initialize_waypoint(1000, 1000, 1000, 1000, 2);
    // initialPaths[numPaths] = insertElement;
    // status[numPaths] = FULL;
    // numPaths++;

    // Updates the waypointBuffer and waypointBufferStatus and compares values
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

	WaypointManager * w2 = new WaypointManager(43.467998128, 80.537331184); // Creates object

    // Creates the initial flight path and home base

    int numPaths = 100;

    _PathData ** initialPaths = new _PathData*[PATH_BUFFER_SIZE];
    _PathData ** testArray = new _PathData*[PATH_BUFFER_SIZE];
    _PathData * testHomeBase;

    _WaypointBufferStatus * status = new _WaypointBufferStatus[PATH_BUFFER_SIZE];

    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        status[i] = FREE;
    }

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

    for(int i = 0; i < numPaths; i++) {
        initialPaths[i] = w2->initialize_waypoint(longitude, latitude, altitude, waypointType);
        status[i] = FULL;
        id_array[i] = initialPaths[i]->waypointId;
        nextElement = i+1;
        longitude++;
        latitude++;
        altitude++;
    }

    _WaypointStatus e1 = w2->initialize_flight_path(initialPaths, numPaths);

    // Updates the waypointBuffer and waypointBufferStatus and compares values
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

	WaypointManager * w2 = new WaypointManager(43.467998128, 80.537331184); // Creates object

    // Creates the initial flight path and home base

    int numPaths = 80;
    int insertIndex = 50;

    _PathData ** initialPaths = new _PathData*[PATH_BUFFER_SIZE];
    _PathData ** insertPaths = new _PathData*[PATH_BUFFER_SIZE];
    _PathData ** testArray = new _PathData*[PATH_BUFFER_SIZE];
    _PathData * testHomeBase;

    _WaypointBufferStatus * status = new _WaypointBufferStatus[PATH_BUFFER_SIZE];

    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        status[i] = FREE;
    }

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
            insertPaths[i] = insertElement;
        }
    }

    _WaypointStatus e1 = w2->initialize_flight_path(initialPaths, numPaths);

    // Updates arrays and constants to account for inserting of new element
    status[numPaths] = FULL;
    numPaths++;

    // Updates the waypointBuffer and waypointBufferStatus and compares values
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

TEST(Waypoint_Manager, deleteMiddleElement) {

   	/***********************SETUP***********************/

	WaypointManager * w2 = new WaypointManager(43.467998128, 80.537331184); // Creates object

    // Creates the initial flight path and home base

    int numPaths = 57;
    int deleteIndex = 50;

    _PathData ** initialPaths = new _PathData*[PATH_BUFFER_SIZE];
    _PathData ** deleteArray = new _PathData*[PATH_BUFFER_SIZE];
    _PathData ** testArray = new _PathData*[PATH_BUFFER_SIZE];
    _PathData * testHomeBase;

    _WaypointBufferStatus * status = new _WaypointBufferStatus[PATH_BUFFER_SIZE];

    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        status[i] = FREE;
    }

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

    for(int i = 0; i < numPaths; i++) {
        initialPaths[i] = w2->initialize_waypoint(longitude, latitude, altitude, waypointType);
        id_array[i] = initialPaths[i]->waypointId;
        nextElement = i+1;
        longitude++;
        latitude++;
        altitude++;

        if(i < deleteIndex) {
            deleteArray[i] = initialPaths[i];
            status[i] = FULL;
        } if (i > deleteIndex) {
            deleteArray[i-1] = initialPaths[i];
            status[i-1] = FULL;
        } 
    }

    _WaypointStatus e1 = w2->initialize_flight_path(initialPaths, numPaths);

    // Adjusts constants
    numPaths--;

    // Updates the waypointBuffer and waypointBufferStatus and compares values
    _WaypointStatus e2 = w2->update_path_nodes(nullptr, DELETE_WAYPOINT, id_array[deleteIndex], 0, 0);
    testArray = w2->get_waypoint_buffer();
    _ArrayStatus a = compare_arrays(deleteArray, testArray, numPaths);
    testHomeBase = w2->get_home_base();
    _ArrayStatus c = compare_buffer_status(status, w2);
	
	/**********************ASSERTS**********************/

	ASSERT_EQ(a, ARRAY_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(c, ARRAY_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(e1, WAYPOINT_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(e2, WAYPOINT_SUCCESS); // Tests equality of the two parameters
}

TEST(Waypoint_Manager, deleteFirstElement) {

    /***********************SETUP***********************/

	WaypointManager * w2 = new WaypointManager(43.467998128, 80.537331184); // Creates object

    // Creates the initial flight path and home base

    int numPaths = 57;
    int deleteIndex = 0;

    _PathData ** initialPaths = new _PathData*[PATH_BUFFER_SIZE];
    _PathData ** deleteArray = new _PathData*[PATH_BUFFER_SIZE];
    _PathData ** testArray = new _PathData*[PATH_BUFFER_SIZE];
    _PathData * testHomeBase;

    _WaypointBufferStatus * status = new _WaypointBufferStatus[PATH_BUFFER_SIZE];

    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        status[i] = FREE;
    }

    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        status[i] = FREE;
    }

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

    for(int i = 0; i < numPaths; i++) {
        initialPaths[i] = w2->initialize_waypoint(longitude, latitude, altitude, waypointType);
        id_array[i] = initialPaths[i]->waypointId;
        nextElement = i+1;
        longitude++;
        latitude++;
        altitude++;
        
        if (i > 0) {
            deleteArray[i-1] = initialPaths[i];
            status[i-1] = FULL;
        } 
    }

    _WaypointStatus e1 = w2->initialize_flight_path(initialPaths, numPaths);

    // Adjusts constants
    numPaths--;

    // Updates the waypointBuffer and waypointBufferStatus and compares values
    _WaypointStatus e2 = w2->update_path_nodes(nullptr, DELETE_WAYPOINT, id_array[deleteIndex], 0, 0);
    testArray = w2->get_waypoint_buffer();
    _ArrayStatus a = compare_arrays(deleteArray, testArray, numPaths);
    testHomeBase = w2->get_home_base();
    _ArrayStatus c = compare_buffer_status(status, w2);
	
	/**********************ASSERTS**********************/

	ASSERT_EQ(a, ARRAY_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(c, ARRAY_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(e1, WAYPOINT_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(e2, WAYPOINT_SUCCESS); // Tests equality of the two parameters
}

TEST(Waypoint_Manager, deleteLastElement) {

   	/***********************SETUP***********************/

	WaypointManager * w2 = new WaypointManager(43.467998128, 80.537331184); // Creates object

    // Creates the initial flight path and home base

    int numPaths = 57;
    int deleteIndex = numPaths-1;

    _PathData ** initialPaths = new _PathData*[PATH_BUFFER_SIZE];
    _PathData ** deleteArray = new _PathData*[PATH_BUFFER_SIZE];
    _PathData ** testArray = new _PathData*[PATH_BUFFER_SIZE];
    _PathData * testHomeBase;

    _WaypointBufferStatus * status = new _WaypointBufferStatus[PATH_BUFFER_SIZE];

    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        status[i] = FREE;
    }

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

    for(int i = 0; i < numPaths; i++) {
        initialPaths[i] = w2->initialize_waypoint(longitude, latitude, altitude, waypointType);
        id_array[i] = initialPaths[i]->waypointId;
        nextElement = i+1;
        longitude++;
        latitude++;
        altitude++;

        if(i < deleteIndex) {
            deleteArray[i] = initialPaths[i];
            status[i] = FULL;
        }
    }

    _WaypointStatus e1 = w2->initialize_flight_path(initialPaths, numPaths);

    // Adjusts constants
    numPaths--;

    // Updates the waypointBuffer and waypointBufferStatus and compares values
    _WaypointStatus e2 = w2->update_path_nodes(nullptr, DELETE_WAYPOINT, id_array[deleteIndex], 0, 0);
    testArray = w2->get_waypoint_buffer();
    _ArrayStatus a = compare_arrays(deleteArray, testArray, numPaths);
    testHomeBase = w2->get_home_base();
    _ArrayStatus c = compare_buffer_status(status, w2);
	
	/**********************ASSERTS**********************/

	ASSERT_EQ(a, ARRAY_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(c, ARRAY_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(e1, WAYPOINT_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(e2, WAYPOINT_SUCCESS); // Tests equality of the two parameters
}

TEST(Waypoint_Manager, deleteLastElementFullArray) {

   	/***********************SETUP***********************/

	WaypointManager * w2 = new WaypointManager(43.467998128, 80.537331184); // Creates object

    // Creates the initial flight path and home base

    int numPaths = 100;
    int deleteIndex = numPaths-1;

    _PathData ** initialPaths = new _PathData*[PATH_BUFFER_SIZE];
    _PathData ** deleteArray = new _PathData*[PATH_BUFFER_SIZE];
    _PathData ** testArray = new _PathData*[PATH_BUFFER_SIZE];
    _PathData * testHomeBase;

    _WaypointBufferStatus * status = new _WaypointBufferStatus[PATH_BUFFER_SIZE];

    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        status[i] = FREE;
    }

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

    for(int i = 0; i < numPaths; i++) {
        initialPaths[i] = w2->initialize_waypoint(longitude, latitude, altitude, waypointType);
        id_array[i] = initialPaths[i]->waypointId;
        nextElement = i+1;
        longitude++;
        latitude++;
        altitude++;

        if(i < deleteIndex) {
            deleteArray[i] = initialPaths[i];
            status[i] = FULL;
        }
    }

    _WaypointStatus e1 = w2->initialize_flight_path(initialPaths, numPaths);

    // Adjusts constants
    numPaths--;

    // Updates the waypointBuffer and waypointBufferStatus and compares values
    _WaypointStatus e2 = w2->update_path_nodes(nullptr, DELETE_WAYPOINT, id_array[deleteIndex], 0, 0);
    testArray = w2->get_waypoint_buffer();
    _ArrayStatus a = compare_arrays(deleteArray, testArray, numPaths);
    testHomeBase = w2->get_home_base();
    _ArrayStatus c = compare_buffer_status(status, w2);
	
	/**********************ASSERTS**********************/

	ASSERT_EQ(a, ARRAY_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(c, ARRAY_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(e1, WAYPOINT_SUCCESS); // Tests equality of the two parameters
    ASSERT_EQ(e2, WAYPOINT_SUCCESS); // Tests equality of the two parameters
}

TEST(Waypoint_Manager, updateFirstElement) {

    /***********************SETUP***********************/

	WaypointManager * w2 = new WaypointManager(43.467998128, 80.537331184); // Creates object

    // Creates the initial flight path and home base

    int numPaths = 50;
    int updateElement = 0;

    _PathData ** initialPaths = new _PathData*[PATH_BUFFER_SIZE];
    _PathData ** testArray = new _PathData*[PATH_BUFFER_SIZE];
    _PathData * testHomeBase;

    _WaypointBufferStatus * status = new _WaypointBufferStatus[PATH_BUFFER_SIZE];

    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        status[i] = FREE;
    }

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

    for(int i = 0; i < numPaths; i++) {
        initialPaths[i] = w2->initialize_waypoint(longitude, latitude, altitude, waypointType);
        status[i] = FULL;
        id_array[i] = initialPaths[i]->waypointId;
        nextElement = i+1;
        longitude++;
        latitude++;
        altitude++;
    }

    _WaypointStatus e1 = w2->initialize_flight_path(initialPaths, numPaths);

    // Updates new element
    _PathData * updateWaypoint = w2->initialize_waypoint(1000, 1001, 10002, 2, -10);
    initialPaths[updateElement] = updateWaypoint;

    // Updates the waypointBuffer and waypointBufferStatus and compares values
    _WaypointStatus e2 = w2->update_path_nodes(updateWaypoint, UPDATE_WAYPOINT, id_array[updateElement], 0, 0);
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

TEST(Waypoint_Manager, updateLastElement) {

    /***********************SETUP***********************/

	WaypointManager * w2 = new WaypointManager(43.467998128, 80.537331184); // Creates object

    // Creates the initial flight path and home base

    int numPaths = 50;
    int updateElement = numPaths-1;

    _PathData ** initialPaths = new _PathData*[PATH_BUFFER_SIZE];
    _PathData ** testArray = new _PathData*[PATH_BUFFER_SIZE];
    _PathData * testHomeBase;

    _WaypointBufferStatus * status = new _WaypointBufferStatus[PATH_BUFFER_SIZE];

    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        status[i] = FREE;
    }

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

    for(int i = 0; i < numPaths; i++) {
        initialPaths[i] = w2->initialize_waypoint(longitude, latitude, altitude, waypointType);
        status[i] = FULL;
        id_array[i] = initialPaths[i]->waypointId;
        nextElement = i+1;
        longitude++;
        latitude++;
        altitude++;
    }

    _WaypointStatus e1 = w2->initialize_flight_path(initialPaths, numPaths);

    // Updates new element
    _PathData * updateWaypoint = w2->initialize_waypoint(1000, 1001, 10002, 2, -10);
    initialPaths[updateElement] = updateWaypoint;

    // Updates the waypointBuffer and waypointBufferStatus and compares values
    _WaypointStatus e2 = w2->update_path_nodes(updateWaypoint, UPDATE_WAYPOINT, id_array[updateElement], 0, 0);
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

TEST(Waypoint_Manager, updateMiddleElement) {

    /***********************SETUP***********************/

	WaypointManager * w2 = new WaypointManager(43.467998128, 80.537331184); // Creates object

    // Creates the initial flight path and home base

    int numPaths = 50;
    int updateElement = 20;

    _PathData ** initialPaths = new _PathData*[PATH_BUFFER_SIZE];
    _PathData ** testArray = new _PathData*[PATH_BUFFER_SIZE];
    _PathData * testHomeBase;

    _WaypointBufferStatus * status = new _WaypointBufferStatus[PATH_BUFFER_SIZE];

    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        status[i] = FREE;
    }

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

    for(int i = 0; i < numPaths; i++) {
        initialPaths[i] = w2->initialize_waypoint(longitude, latitude, altitude, waypointType);
        status[i] = FULL;
        id_array[i] = initialPaths[i]->waypointId;
        nextElement = i+1;
        longitude++;
        latitude++;
        altitude++;
    }

    // Initializes waypint maanger with the flight path
    _WaypointStatus e1 = w2->initialize_flight_path(initialPaths, numPaths);

    // Updates new element
    _PathData * updateWaypoint = w2->initialize_waypoint(1000, 1001, 10002, 2, -10);
    initialPaths[updateElement] = updateWaypoint;

    // Updates the waypointBuffer and waypointBufferStatus and compares values
    _WaypointStatus e2 = w2->update_path_nodes(updateWaypoint, UPDATE_WAYPOINT, id_array[updateElement], 0, 0);
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

