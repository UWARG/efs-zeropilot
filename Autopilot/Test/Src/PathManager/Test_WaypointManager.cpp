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
            // cout << "Previous Check Index: " << i << " | " << ans[i]->waypointId << " " << nextWaypoint->waypointId << " | " << ans[i]->longitude << " " << nextWaypoint->longitude << " | " << ans[i]->latitude << " " << nextWaypoint->latitude << " | " << ans[i]->altitude << " " << nextWaypoint->altitude << " | " << ans[i]->waypointType << " " << nextWaypoint->waypointType << " | " << ans[i]->turnRadius << " " << nextWaypoint->turnRadius << endl;
            return ARRAY_DIFFERENT;
        }
    }

    // Checks if indexes are the same
    for(int i = 0; i < numElements; i++) {
        nextWaypoint = testArray[i];
        if(ans[i]->waypointId != nextWaypoint->waypointId && ans[i]->longitude != nextWaypoint->longitude && ans[i]->latitude != nextWaypoint->latitude && ans[i]->altitude != nextWaypoint->altitude && ans[i]->waypointType != nextWaypoint->waypointType && ans[i]->turnRadius != nextWaypoint->turnRadius) {
            // cout << "Array Equality Check Index: " << i << " | " << ans[i]->waypointId << " " << nextWaypoint->waypointId << " | " << ans[i]->longitude << " " << nextWaypoint->longitude << " | " << ans[i]->latitude << " " << nextWaypoint->latitude << " | " << ans[i]->altitude << " " << nextWaypoint->altitude << " | " << ans[i]->waypointType << " " << nextWaypoint->waypointType << " | " << ans[i]->turnRadius << " " << nextWaypoint->turnRadius << endl;
            return ARRAY_DIFFERENT;
        }
    }

    return ARRAY_SUCCESS;
}

static _ArrayStatus compare_buffer_status(_WaypointBufferStatus * ans, WaypointManager * waypointManagerInstance) {
    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        if(ans[i] != waypointManagerInstance->get_status_of_index(i)) {
            // cout << "Buffer Compare Index: " << i << " | " << ans[i] << " " << waypointManagerInstance->get_status_of_index(i) << endl;
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
    if(ans->desiredAltitude == test->desiredAltitude && ans->desiredHeading == test->desiredHeading && ans->distanceToNextWaypoint == round(test->distanceToNextWaypoint) && ans->radius == test->radius && ans->turnDirection == test->turnDirection && ans->out_type == test->out_type) {
        return OUTPUT_CORRECT;
    } else {
        // cout << "Comparing Output Data: Alt " << ans->desiredAltitude << " " << test->desiredAltitude << " | Heading " << ans->desiredHeading << " " << test->desiredHeading << " | Distance " << ans->distanceToNextWaypoint << " " << test->distanceToNextWaypoint << " | Radius " << ans->radius << " " << test->radius << " | Direction " << ans->turnDirection << " " << test->turnDirection << " | OutType " << ans->out_type << " " << test->out_type << endl;
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

	WaypointManager * waypointManagerInstance = new WaypointManager(43.467998128, 80.537331184); // Creates object

    // Creates the initial flight path and home base
    int numPaths = 20;
    
    _PathData * initialPaths[PATH_BUFFER_SIZE]; // Used to initialize waypointBuffer 
    _PathData ** testArray = nullptr; // Used to check validity of waypointBuffer
    _PathData * homeBase = nullptr; // Used to initialize homeBase
    _PathData * testHomeBase = nullptr; // Used to check validity of homeBase

    _WaypointBufferStatus status[PATH_BUFFER_SIZE]; // This array is used to check the validity of the waypointBufferStatus array

    // Initializes status array  
    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        status[i] = FREE;
    }

    long double longitude = 1;
    long double latitude = 10;
    int altitude = 100;
    _WaypointOutputType waypointType = PATH_FOLLOW;

	/********************STEPTHROUGH********************/

    homeBase = waypointManagerInstance->initialize_waypoint(1000,500, 5, HOLD_WAYPOINT); // Initializes homeBase (will be passed to module)

    // Initializes original flight path
    for(int i = 0; i < numPaths/2; i++) {
        initialPaths[i] = waypointManagerInstance->initialize_waypoint(longitude, latitude, altitude, waypointType); // Calls method to create waypoint on the heap
        // Just does some stuff to record statuses and stuff
        status[i] = FULL;
        longitude++;
        latitude++;
        altitude++;
    }

    waypointType = ORBIT_FOLLOW;
    float turnRadius = 10;

    for(int i = numPaths/2; i < numPaths; i++) {
        initialPaths[i] = waypointManagerInstance->initialize_waypoint(longitude, latitude, altitude, waypointType, turnRadius); // Calls method to create waypoint on the heap
        // Just does some stuff to record statuses and stuff
        status[i] = FULL;
        longitude++;
        latitude++;
        altitude++;
        turnRadius++;
    }

    // Initializes waypint maanger with the flight path
    _WaypointStatus initialize_check = waypointManagerInstance->initialize_flight_path(initialPaths, numPaths, homeBase);

    testArray = waypointManagerInstance->get_waypoint_buffer(); // Gets the waypointBuffer array 
    _ArrayStatus waypoint_buffer_check = compare_arrays(initialPaths, testArray, numPaths); // Compares the waypointBuffer arrays

    testHomeBase = waypointManagerInstance->get_home_base(); // gets the homeBase parameter
    _WaypointSt home_base_check = compare_waypoint(homeBase, testHomeBase);  // Checks if the homeBase waypoints are the same 

    _ArrayStatus waypoint_status_check = compare_buffer_status(status, waypointManagerInstance); // Compares the waypointBufferStatus arrays

    // Clears heap
    // Deleting WaypointManager object takes care of calling clear_path_nodes() and clear_home_base(). 
    delete waypointManagerInstance; 

	/**********************ASSERTS**********************/

    EXPECT_EQ(home_base_check, WAYPOINT_CORRECT);  
	EXPECT_EQ(waypoint_buffer_check, ARRAY_SUCCESS);  
    EXPECT_EQ(waypoint_status_check, ARRAY_SUCCESS);  
    EXPECT_EQ(initialize_check, WAYPOINT_SUCCESS);  
}


/************************ TESTING GETTING DESIRED HEADING/ALTITUDE/ETC ************************/


TEST(Waypoint_Manager, DesiredHeadingForOrbit) {
    /***********************SETUP***********************/

    WaypointManager * waypointManagerInstance = new WaypointManager(43.467998128, -80.537331184); // Creates object

    // Stores outputs from four tests
    _WaypointManager_Data_Out * out1 = new _WaypointManager_Data_Out;
    _WaypointManager_Data_Out * out2 = new _WaypointManager_Data_Out;

    // Creates two test values!
    _WaypointManager_Data_In setup1 = {43.467998128, -80.537331184, 100, 100};  // latitude, longitude, altitude, heading
    _WaypointManager_Data_In setup2 = {43.467998128, -80.537331184, 100, 30};  // latitude, longitude, altitude, heading

    _WaypointManager_Data_In input1 = {43.467998128, -80.537331184, 100, 100};  // latitude, longitude, altitude, heading
    _WaypointManager_Data_In input2 = {43.467998128, -80.537331184, 100, 30};  // latitude, longitude, altitude, heading

    // Stores answers for four tests
    float center_ans1[3] = {-80.54500000, 43.47138889, 78}; // longitude, latitude, altitude
    _WaypointManager_Data_Out * ans1 = new _WaypointManager_Data_Out;
    ans1->desiredHeading = 100;        
    ans1->desiredAltitude = 78;
    ans1->distanceToNextWaypoint = 0;
    ans1->radius = 100;
    ans1->turnDirection = -1;
    ans1->errorCode = WAYPOINT_SUCCESS;
    ans1->isDataNew = true;
    ans1->timeOfData = 0;
    ans1->out_type = ORBIT_FOLLOW;

    float center_ans2[3] = {-80.54527778, 43.47250000, 110}; 
    _WaypointManager_Data_Out * ans2 = new _WaypointManager_Data_Out;
    ans2->desiredHeading = 30;       
    ans2->desiredAltitude = 110;
    ans2->distanceToNextWaypoint = 0;
    ans2->radius = 30;
    ans2->turnDirection = 1;
    ans2->errorCode = WAYPOINT_SUCCESS;
    ans2->isDataNew = true;
    ans2->timeOfData = 0;
    ans2->out_type = ORBIT_FOLLOW;


    /********************STEPTHROUGH********************/

    int turnRadius[2] = {100, 30};
    int turnDirection[2] = {-1, 1}; // 1 = CW, 2 = CCW
    int altitude[2] = {78, 110};

    _WaypointStatus circling_start_check_1 = waypointManagerInstance->start_circling(setup1, turnRadius[0], turnDirection[0], altitude[0], false); // Sets circling

    _WaypointStatus get_directions_check_1 = waypointManagerInstance->get_next_directions(input1, out1);

    float response[3]; // longitude, latitude
    response[0] = waypointManagerInstance->orbitCentreLong;
    response[1] = waypointManagerInstance->orbitCentreLat;
    response[2] = waypointManagerInstance->orbitCentreAlt;

    _OutputStatus test1_center = compare_coordinates(center_ans1, response);
    _OutputStatus test1_output = compare_output_data(ans1, out1);

    _WaypointStatus circling_start_check_2 = waypointManagerInstance->start_circling(setup2, turnRadius[1], turnDirection[1], altitude[1], false); // Sets circling

    _WaypointStatus get_directions_check_2 =waypointManagerInstance->get_next_directions(input2, out2);

    response[0] = waypointManagerInstance->orbitCentreLong;
    response[1] = waypointManagerInstance->orbitCentreLat;
    response[2] = waypointManagerInstance->orbitCentreAlt;

    _OutputStatus test2_center = compare_coordinates(center_ans2, response);
    _OutputStatus test2_output = compare_output_data(ans2, out2);

    delete out1; delete out2; delete ans1; delete ans2; delete waypointManagerInstance;

    /**********************ASSERTS**********************/

    EXPECT_EQ(get_directions_check_1, WAYPOINT_SUCCESS);
    EXPECT_EQ(get_directions_check_2, WAYPOINT_SUCCESS);

    EXPECT_EQ(circling_start_check_1, WAYPOINT_SUCCESS);
    EXPECT_EQ(circling_start_check_2, WAYPOINT_SUCCESS);

    EXPECT_EQ(test1_center, OUTPUT_CORRECT);
    EXPECT_EQ(test1_output, OUTPUT_CORRECT);

    EXPECT_EQ(test2_center, OUTPUT_CORRECT);
    EXPECT_EQ(test2_output, OUTPUT_CORRECT);
}

TEST(Waypoint_Manager, DesiredHeadingStraightPathFollow) {
    WaypointManager * waypointManagerInstance = new WaypointManager(43.467998128, -80.537331184); // Creates object

    // Stores outputs from four tests
    _WaypointManager_Data_Out * out1 = new _WaypointManager_Data_Out;
    _WaypointManager_Data_Out * out2 = new _WaypointManager_Data_Out;

    // Creates two test values!    
    _WaypointManager_Data_In input1 = {43.467998128, -80.537331184, 11, 100};  // latitude, longitude, altitude, heading
    _WaypointManager_Data_In input2 = {43.469649460242174, -80.55044911526599, 34, 86};  // latitude, longitude, altitude, heading

    // Stores answers for tests
    _WaypointManager_Data_Out * ans1 = new _WaypointManager_Data_Out;
    ans1->desiredHeading = 294;         
    ans1->desiredAltitude = 33;
    ans1->distanceToNextWaypoint = 960;
    ans1->radius = 0;
    ans1->turnDirection = 0;
    ans1->errorCode = WAYPOINT_SUCCESS;
    ans1->isDataNew = true;
    ans1->timeOfData = 0;
    ans1->out_type = PATH_FOLLOW;

    _WaypointManager_Data_Out * ans2 = new _WaypointManager_Data_Out;
    ans2->desiredHeading = 153;        
    ans2->desiredAltitude = 33;
    ans2->distanceToNextWaypoint = 625;
    ans2->radius = 0;
    ans2->turnDirection = 0;
    ans2->errorCode = WAYPOINT_SUCCESS;
    ans2->isDataNew = true;
    ans2->timeOfData = 0;
    ans2->out_type = PATH_FOLLOW;

    const int numPaths = 6;
    // Defines flight path
    /*
    Waypoints:
    1. 43.47075830402289, -80.5479053969044
    2. 43.469649460242174, -80.55044911526599
    3. 43.46764349709017, -80.54172626568685
    4. 43.46430420301871, -80.54806720987989
    5. 43.461854997441996, -80.5406705046026
    6. 43.46144872072057, -80.53505945389745
    */
    float latitudes[numPaths] = {43.47075830402289, 43.469649460242174, 43.46764349709017, 43.46430420301871, 43.461854997441996, 43.46144872072057};
    float longitudes[numPaths] = {-80.5479053969044, -80.55044911526599, -80.54172626568685, -80.54806720987989, -80.5406705046026, -80.53505945389745};
    float altitudes[numPaths] = {10, 20, 30, 33, 32, 50};

    // Creates waypoint array
    _PathData * initialPaths[PATH_BUFFER_SIZE];
    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        initialPaths[i] = nullptr;
    }   
    
    for(int i = 0; i < numPaths; i++) {
        initialPaths[i] = waypointManagerInstance->initialize_waypoint(longitudes[i], latitudes[i], altitudes[i], PATH_FOLLOW);
    }

    _WaypointStatus initialize_check = waypointManagerInstance->initialize_flight_path(initialPaths, numPaths);

    /********************STEPTHROUGH********************/

    _WaypointStatus get_directions_check_1 = waypointManagerInstance->get_next_directions(input1, out1);
    _OutputStatus output_check_1 = compare_output_data(ans1, out1);

    _WaypointStatus get_directions_check_2 = waypointManagerInstance->get_next_directions(input2, out2);
    _OutputStatus output_check_2 = compare_output_data(ans2, out2);

    delete out1; delete out2; delete ans1; delete ans2; delete waypointManagerInstance;

	/**********************ASSERTS**********************/

    EXPECT_EQ(initialize_check, WAYPOINT_SUCCESS);
    EXPECT_EQ(get_directions_check_1, WAYPOINT_SUCCESS);
    EXPECT_EQ(get_directions_check_2, WAYPOINT_SUCCESS);
    
    EXPECT_EQ(output_check_1, OUTPUT_CORRECT);
    EXPECT_EQ(output_check_2, OUTPUT_CORRECT);
}

TEST(Waypoint_Manager, DesiredHeadingWhenNextToNextWaypointNotDefined) {
    /***********************SETUP***********************/
    WaypointManager * waypointManagerInstance = new WaypointManager(43.467998128, -80.537331184); // Creates object

    // Stores outputs from four tests
    _WaypointManager_Data_Out * out1 = new _WaypointManager_Data_Out;
    _WaypointManager_Data_Out * out2 = new _WaypointManager_Data_Out;

    // Creates two test values!    
    _WaypointManager_Data_In input1 = {43.467998128, -80.537331184, 11, 100};  // latitude, longitude, altitude, heading
    _WaypointManager_Data_In input2 = {43.469649460242174, -80.55044911526599, 34, 86};  // latitude, longitude, altitude, heading

    // Stores answers for tests
    _WaypointManager_Data_Out * ans1 = new _WaypointManager_Data_Out;
    ans1->desiredHeading = 294;         
    ans1->desiredAltitude = 33;
    ans1->distanceToNextWaypoint = 960;
    ans1->radius = 0;
    ans1->turnDirection = 0;
    ans1->errorCode = WAYPOINT_SUCCESS;
    ans1->isDataNew = true;
    ans1->timeOfData = 0;
    ans1->out_type = PATH_FOLLOW;

    _WaypointManager_Data_Out * ans2 = new _WaypointManager_Data_Out;
    ans2->desiredHeading = 153;        
    ans2->desiredAltitude = 33;
    ans2->distanceToNextWaypoint = 625;
    ans2->radius = 0;
    ans2->turnDirection = 0;
    ans2->errorCode = WAYPOINT_SUCCESS;
    ans2->isDataNew = true;
    ans2->timeOfData = 0;
    ans2->out_type = PATH_FOLLOW;

    const int numPaths = 4;
    // Defines flight path
    /*
    Waypoints:
    1. 43.47075830402289, -80.5479053969044
    2. 43.469649460242174, -80.55044911526599
    3. 43.46764349709017, -80.54172626568685
    4. 43.46430420301871, -80.54806720987989
    */
    float latitudes[numPaths] = {43.47075830402289, 43.469649460242174, 43.46764349709017, 43.46430420301871};
    float longitudes[numPaths] = {-80.5479053969044, -80.55044911526599, -80.54172626568685, -80.54806720987989};
    float altitudes[numPaths] = {10, 20, 30, 33};

    // Creates waypoint array
    _PathData * initialPaths[PATH_BUFFER_SIZE];
    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        initialPaths[i] = nullptr;
    }

    for(int i = 0; i < numPaths; i++) {
        initialPaths[i] = waypointManagerInstance->initialize_waypoint(longitudes[i], latitudes[i], altitudes[i], PATH_FOLLOW);
    }

    _WaypointStatus initialize_check = waypointManagerInstance->initialize_flight_path(initialPaths, numPaths);

    /********************STEPTHROUGH********************/

    _WaypointStatus get_directions_check_1 = waypointManagerInstance->get_next_directions(input1, out1);
    _OutputStatus output_check_1 = compare_output_data(ans1, out1);

    // cout << endl;

    _WaypointStatus get_directions_check_2 = waypointManagerInstance->get_next_directions(input2, out2);
    _OutputStatus output_check_2 = compare_output_data(ans2, out2);

    delete out1; delete out2; delete ans1; delete ans2; delete waypointManagerInstance;

	/**********************ASSERTS**********************/

    EXPECT_EQ(initialize_check, WAYPOINT_SUCCESS);
    EXPECT_EQ(get_directions_check_1, WAYPOINT_SUCCESS);
    EXPECT_EQ(get_directions_check_2, WAYPOINT_SUCCESS);
    
    EXPECT_EQ(output_check_1, OUTPUT_CORRECT);
    EXPECT_EQ(output_check_2, OUTPUT_CORRECT);
}

TEST(Waypoint_Manager, DesiredHeadingNextWaypointNotDefined) {
    /***********************SETUP***********************/
    WaypointManager * waypointManagerInstance = new WaypointManager(43.467998128, -80.537331184); // Creates object

    // Stores outputs from four tests
    _WaypointManager_Data_Out * out1 = new _WaypointManager_Data_Out;
    _WaypointManager_Data_Out * out2 = new _WaypointManager_Data_Out;

    // Creates two test values!    
    _WaypointManager_Data_In input1 = {43.467998128, -80.537331184, 11, 100};  // latitude, longitude, altitude, heading
    _WaypointManager_Data_In input2 = {43.469649460242174, -80.55044911526599, 34, 86};  // latitude, longitude, altitude, heading

    // Stores answers for tests
    _WaypointManager_Data_Out * ans1 = new _WaypointManager_Data_Out;
    ans1->desiredHeading = 263;         
    ans1->desiredAltitude = 30;
    ans1->distanceToNextWaypoint = 358;
    ans1->radius = 0;
    ans1->turnDirection = 0;
    ans1->errorCode = WAYPOINT_SUCCESS;
    ans1->isDataNew = true;
    ans1->timeOfData = 0;
    ans1->out_type = PATH_FOLLOW;

    _WaypointManager_Data_Out * ans2 = new _WaypointManager_Data_Out;
    ans2->desiredHeading = 107;        
    ans2->desiredAltitude = 30;
    ans2->distanceToNextWaypoint = 739;
    ans2->radius = 0;
    ans2->turnDirection = 0;
    ans2->errorCode = WAYPOINT_SUCCESS;
    ans2->isDataNew = true;
    ans2->timeOfData = 0;
    ans2->out_type = PATH_FOLLOW;

    const int numPaths = 3;
    // Defines flight path
    /*
    Waypoints:
    1. 43.47075830402289, -80.5479053969044
    2. 43.469649460242174, -80.55044911526599
    3. 43.46764349709017, -80.54172626568685
    */
    float latitudes[numPaths] = {43.47075830402289, 43.469649460242174, 43.46764349709017};
    float longitudes[numPaths] = {-80.5479053969044, -80.55044911526599, -80.54172626568685};
    float altitudes[numPaths] = {10, 20, 30};

    // Creates waypoint array
    _PathData * initialPaths[PATH_BUFFER_SIZE];
    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        initialPaths[i] = nullptr;
    }

    for(int i = 0; i < numPaths; i++) {
        initialPaths[i] = waypointManagerInstance->initialize_waypoint(longitudes[i], latitudes[i], altitudes[i], PATH_FOLLOW);
    }

    _WaypointStatus initialize_check = waypointManagerInstance->initialize_flight_path(initialPaths, numPaths);

    /********************STEPTHROUGH********************/

    _WaypointStatus get_directions_check_1 = waypointManagerInstance->get_next_directions(input1, out1);
    _OutputStatus output_check_1 = compare_output_data(ans1, out1);

    // cout << endl;

    _WaypointStatus get_directions_check_2 = waypointManagerInstance->get_next_directions(input2, out2);
    _OutputStatus output_check_2 = compare_output_data(ans2, out2);

    delete out1; delete out2; delete ans1; delete ans2; delete waypointManagerInstance;

	/**********************ASSERTS**********************/

    EXPECT_EQ(initialize_check, WAYPOINT_SUCCESS);
    EXPECT_EQ(get_directions_check_1, WAYPOINT_SUCCESS);
    EXPECT_EQ(get_directions_check_2, WAYPOINT_SUCCESS);
    
    EXPECT_EQ(output_check_1, OUTPUT_CORRECT);
    EXPECT_EQ(output_check_2, OUTPUT_CORRECT);
}

TEST(Waypoint_Manager, DesiredHeadingWhenGoingHomeSetTrue) {
    /***********************SETUP***********************/
    WaypointManager * waypointManagerInstance = new WaypointManager(43.467998128, -80.537331184); // Creates object

    // Stores outputs from four tests
    _WaypointManager_Data_Out * out1 = new _WaypointManager_Data_Out;
    _WaypointManager_Data_Out * out2 = new _WaypointManager_Data_Out;
    _WaypointManager_Data_Out * out3 = new _WaypointManager_Data_Out;
    
    // Creates two test values!    
    _WaypointManager_Data_In input1 = {43.567998128, -80.437331184, 11, 100};  // latitude, longitude, altitude, heading
    _WaypointManager_Data_In input2 = {43.369649460242174, -80.37044911526599, 34, 86};  // latitude, longitude, altitude, heading
    _WaypointManager_Data_In input3 = {43.469649460242174, -80.55044911526599, 34, 86};  // latitude, longitude, altitude, heading

    // Stores answers for four tests
    _WaypointManager_Data_Out * ans1 = new _WaypointManager_Data_Out;
    ans1->desiredHeading = 215;        
    ans1->desiredAltitude = 45;
    ans1->distanceToNextWaypoint = 13755;
    ans1->radius = 0;
    ans1->turnDirection = 0;
    ans1->errorCode = WAYPOINT_SUCCESS;
    ans1->isDataNew = true;
    ans1->timeOfData = 0;
    ans1->out_type = PATH_FOLLOW;

    _WaypointManager_Data_Out * ans2 = new _WaypointManager_Data_Out;
    ans2->desiredHeading = 310;       
    ans2->desiredAltitude = 45;
    ans2->distanceToNextWaypoint = 17368;
    ans2->radius = 0;
    ans2->turnDirection = 0;
    ans2->errorCode = WAYPOINT_SUCCESS;
    ans2->isDataNew = true;
    ans2->timeOfData = 0;
    ans2->out_type = PATH_FOLLOW;

    _WaypointManager_Data_Out * ans3 = new _WaypointManager_Data_Out;
    ans3->desiredHeading = 153;        
    ans3->desiredAltitude = 33;
    ans3->distanceToNextWaypoint = 625;
    ans3->radius = 0;
    ans3->turnDirection = 0;
    ans3->errorCode = WAYPOINT_SUCCESS;
    ans3->isDataNew = true;
    ans3->timeOfData = 0;
    ans3->out_type = PATH_FOLLOW;

    const int numPaths = 6;

    // Defines flight path
    /*
    Waypoints:
    1. 43.47075830402289, -80.5479053969044
    2. 43.469649460242174, -80.55044911526599
    3. 43.46764349709017, -80.54172626568685
    4. 43.46430420301871, -80.54806720987989
    5. 43.461854997441996, -80.5406705046026
    6. 43.46144872072057, -80.53505945389745
    */
    float latitudes[numPaths] = {43.47075830402289, 43.469649460242174, 43.46764349709017, 43.46430420301871, 43.461854997441996, 43.46144872072057};
    float longitudes[numPaths] = {-80.5479053969044, -80.55044911526599, -80.54172626568685, -80.54806720987989, -80.5406705046026, -80.53505945389745};
    float altitudes[numPaths] = {10, 20, 30, 33, 32, 50};

    // Creates waypoint array
    _PathData * initialPaths[PATH_BUFFER_SIZE];
    _WaypointBufferStatus status[PATH_BUFFER_SIZE];
    _PathData ** testArray = nullptr;
    _PathData * ansArray[PATH_BUFFER_SIZE];

    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        status[i] = FREE;
        ansArray[i] = nullptr;
        initialPaths[i] = nullptr;
    }

    for(int i = 0; i < numPaths; i++) {
        initialPaths[i] = waypointManagerInstance->initialize_waypoint(longitudes[i], latitudes[i], altitudes[i], PATH_FOLLOW);
    }

    // Creates home object
    _PathData * homeBase = waypointManagerInstance->initialize_waypoint(-80.537331184, 43.467998128, 45, HOLD_WAYPOINT);

    _WaypointStatus initialize_check = waypointManagerInstance->initialize_flight_path(initialPaths, numPaths, homeBase); // Creates flight path

	/********************STEPTHROUGH********************/

    _HeadHomeStatus home_check_1 = waypointManagerInstance->head_home(true);

    testArray = waypointManagerInstance->get_waypoint_buffer();

    _ArrayStatus path_compare_1 = compare_arrays(ansArray, testArray, 0);
    _ArrayStatus path_status_compare_1 = compare_buffer_status(status, waypointManagerInstance);

    _WaypointStatus get_directions_check_1 = waypointManagerInstance->get_next_directions(input1, out1);
    _OutputStatus output_check_1 = compare_output_data(ans1, out1);

    _WaypointStatus get_directions_check_2 = waypointManagerInstance->get_next_directions(input2, out2);
    _OutputStatus output_check_2 = compare_output_data(ans2, out2);

    // Re-initializes flight path
    for(int i = 0; i < numPaths; i++) {
        initialPaths[i] = waypointManagerInstance->initialize_waypoint(longitudes[i], latitudes[i], altitudes[i], PATH_FOLLOW);
        status[i] = FULL;
    }

    // Re-initializes flight path and gets next direction
    _WaypointStatus reinitialize_check = waypointManagerInstance->initialize_flight_path(initialPaths, numPaths); // Creates flight path
    _HeadHomeStatus home_check_2 = waypointManagerInstance->head_home(false); // Cancels holding
    _ArrayStatus path_status_compare_2 = compare_buffer_status(status, waypointManagerInstance);
    _WaypointStatus get_directions_check_3 = waypointManagerInstance->get_next_directions(input3, out3); 
    _OutputStatus output_check_3 = compare_output_data(ans3, out3);

    delete out1; delete out2; delete out3; delete ans1; delete ans2; delete ans3; delete waypointManagerInstance;
	
	/**********************ASSERTS**********************/

    EXPECT_EQ(initialize_check, WAYPOINT_SUCCESS);
    EXPECT_EQ(get_directions_check_1, WAYPOINT_SUCCESS);
    EXPECT_EQ(get_directions_check_2, WAYPOINT_SUCCESS);
    EXPECT_EQ(reinitialize_check, WAYPOINT_SUCCESS);
    EXPECT_EQ(get_directions_check_3, WAYPOINT_SUCCESS);

    EXPECT_EQ(home_check_1, HOME_TRUE);
    EXPECT_EQ(home_check_2, HOME_FALSE);

    EXPECT_EQ(path_compare_1, ARRAY_SUCCESS);
    EXPECT_EQ(path_status_compare_1, ARRAY_SUCCESS);
    EXPECT_EQ(path_status_compare_2, ARRAY_SUCCESS);

    EXPECT_EQ(output_check_1, OUTPUT_CORRECT);
    EXPECT_EQ(output_check_2, OUTPUT_CORRECT);
    EXPECT_EQ(output_check_3, OUTPUT_CORRECT);
}


/************************ TESTING MODIFYING THE FLIGHT PATH ************************/


TEST(Waypoint_Manager, AppendElementToNotFilledArray) {

   	/***********************SETUP***********************/

	WaypointManager * waypointManagerInstance = new WaypointManager(43.467998128, 80.537331184); // Creates object

    // Creates the initial flight path and home base
    int numPaths = 50;

    _PathData * initialPaths[PATH_BUFFER_SIZE];
    _PathData ** testArray = nullptr;

    _WaypointBufferStatus status[PATH_BUFFER_SIZE];

    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        status[i] = FREE;
        initialPaths[i] = nullptr;
    }

    long double longitude = 1;
    long double latitude = 10;
    int altitude = 100;
    _WaypointOutputType waypointType = PATH_FOLLOW;

	/********************STEPTHROUGH********************/

    for(int i = 0; i < numPaths; i++) {
        initialPaths[i] = waypointManagerInstance->initialize_waypoint(longitude, latitude, altitude, waypointType);
        status[i] = FULL;
        longitude++;
        latitude++;
        altitude++;
    }

    _WaypointStatus initialize_check = waypointManagerInstance->initialize_flight_path(initialPaths, numPaths); // Checks if initialization was successful

    // Appends new element
    _PathData * appendElement = waypointManagerInstance->initialize_waypoint(1000, 1000, 1000, HOLD_WAYPOINT);
    initialPaths[numPaths] = appendElement;
    status[numPaths] = FULL;
    numPaths++;

    // Updates the waypointBuffer and waypointBufferStatus and compares values
    _WaypointStatus update_success_check = waypointManagerInstance->update_path_nodes(appendElement, APPEND_WAYPOINT, 0, 0, 0); // Checks if updating waypointBuffer is successful
    testArray = waypointManagerInstance->get_waypoint_buffer();
    _ArrayStatus paths_equal_check = compare_arrays(initialPaths, testArray, numPaths);
    _ArrayStatus paths_status_equal_check = compare_buffer_status(status, waypointManagerInstance);

    delete waypointManagerInstance;
	
	/**********************ASSERTS**********************/

	EXPECT_EQ(paths_equal_check, ARRAY_SUCCESS); 
    EXPECT_EQ(paths_status_equal_check, ARRAY_SUCCESS); 
    EXPECT_EQ(initialize_check, WAYPOINT_SUCCESS); 
    EXPECT_EQ(update_success_check, WAYPOINT_SUCCESS); 
}

TEST(Waypoint_Manager, AppendElementToFirstElement) {

   	/***********************SETUP***********************/

	WaypointManager * waypointManagerInstance = new WaypointManager(43.467998128, 80.537331184); // Creates object

    // Creates the initial flight path and home base
    int numPaths = 0;

    _PathData * initialPaths[PATH_BUFFER_SIZE];
    _PathData ** testArray = nullptr;

    _WaypointBufferStatus status[PATH_BUFFER_SIZE];

    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        status[i] = FREE;
        initialPaths[i] = nullptr;
    }

    long double longitude = 1;
    long double latitude = 10;
    int altitude = 100;
    _WaypointOutputType waypointType = PATH_FOLLOW;

	/********************STEPTHROUGH********************/

    for(int i = 0; i < numPaths; i++) {
        initialPaths[i] = waypointManagerInstance->initialize_waypoint(longitude, latitude, altitude, waypointType);
        status[i] = FULL;
        longitude++;
        latitude++;
        altitude++;
    }

    _WaypointStatus initialize_check = waypointManagerInstance->initialize_flight_path(initialPaths, numPaths);

    // Appends new element
    _PathData * appendElement = waypointManagerInstance->initialize_waypoint(1000, 1000, 1000, HOLD_WAYPOINT);
    initialPaths[numPaths] = appendElement;
    status[numPaths] = FULL;
    numPaths++;

    // Updates the waypointBuffer and waypointBufferStatus and compares values
    _WaypointStatus update_success_check = waypointManagerInstance->update_path_nodes(appendElement, APPEND_WAYPOINT, 0, 0, 0);
    testArray = waypointManagerInstance->get_waypoint_buffer();
    _ArrayStatus paths_equal_check = compare_arrays(initialPaths, testArray, numPaths);
    _ArrayStatus paths_status_equal_check = compare_buffer_status(status, waypointManagerInstance);

    delete waypointManagerInstance;
	
	/**********************ASSERTS**********************/

	EXPECT_EQ(paths_equal_check, ARRAY_SUCCESS);  
    EXPECT_EQ(paths_status_equal_check, ARRAY_SUCCESS);  
    EXPECT_EQ(initialize_check, WAYPOINT_SUCCESS);  
    EXPECT_EQ(update_success_check, WAYPOINT_SUCCESS);  
}

TEST(Waypoint_Manager, AppendElementToNinteyNineElementArray) {

   	/***********************SETUP***********************/

	WaypointManager * waypointManagerInstance = new WaypointManager(43.467998128, 80.537331184); // Creates object

    // Creates the initial flight path and home base

    int numPaths = 99;

    _PathData * initialPaths[PATH_BUFFER_SIZE];
    _PathData ** testArray = nullptr;

    _WaypointBufferStatus status[PATH_BUFFER_SIZE];

    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        status[i] = FREE;
        initialPaths[i] = nullptr;
    }

    long double longitude = 1;
    long double latitude = 10;
    int altitude = 100;
    _WaypointOutputType waypointType = PATH_FOLLOW;	

	/********************STEPTHROUGH********************/

    for(int i = 0; i < numPaths; i++) {
        initialPaths[i] = waypointManagerInstance->initialize_waypoint(longitude, latitude, altitude, waypointType);
        status[i] = FULL;
        longitude++;
        latitude++;
        altitude++;
    }

    _WaypointStatus initialize_check = waypointManagerInstance->initialize_flight_path(initialPaths, numPaths);

    // Appemds new element
    _PathData * appendElement = waypointManagerInstance->initialize_waypoint(1000, 1000, 1000, HOLD_WAYPOINT);
    initialPaths[numPaths] = appendElement;
    status[numPaths] = FULL;
    numPaths++;

    // Updates the waypointBuffer and waypointBufferStatus and compares values
    _WaypointStatus update_success_check = waypointManagerInstance->update_path_nodes(appendElement, APPEND_WAYPOINT, 0, 0, 0);
    testArray = waypointManagerInstance->get_waypoint_buffer();
    _ArrayStatus paths_equal_check = compare_arrays(initialPaths, testArray, numPaths);
    _ArrayStatus paths_status_equal_check = compare_buffer_status(status, waypointManagerInstance);

    delete waypointManagerInstance;
	
	/**********************ASSERTS**********************/

	EXPECT_EQ(paths_equal_check, ARRAY_SUCCESS);  
    EXPECT_EQ(paths_status_equal_check, ARRAY_SUCCESS);  
    EXPECT_EQ(initialize_check, WAYPOINT_SUCCESS);  
    EXPECT_EQ(update_success_check, WAYPOINT_SUCCESS);  
}

TEST(Waypoint_Manager, AppendElementToFullAndReturnError) {

   	/***********************SETUP***********************/

	WaypointManager * waypointManagerInstance = new WaypointManager(43.467998128, 80.537331184); // Creates object

    // Creates the initial flight path and home base

    int numPaths = PATH_BUFFER_SIZE;

    _PathData * initialPaths[PATH_BUFFER_SIZE];

    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        initialPaths[i] = nullptr;
    }

    long double longitude = 1;
    long double latitude = 10;
    int altitude = 100;
    _WaypointOutputType waypointType = PATH_FOLLOW;

	/********************STEPTHROUGH********************/

    for(int i = 0; i < numPaths; i++) {
        initialPaths[i] = waypointManagerInstance->initialize_waypoint(longitude, latitude, altitude, waypointType);
        longitude++;
        latitude++;
        altitude++;
    }

    _WaypointStatus initialize_check = waypointManagerInstance->initialize_flight_path(initialPaths, numPaths);

    // Should not append new element
    _PathData * appendElement = waypointManagerInstance->initialize_waypoint(1000, 1000, 1000, HOLD_WAYPOINT);
    // initialPaths[numPaths] = appendElement;
    // numPaths++;

    // Updates the waypointBuffer and waypointBufferStatus and compares values
    _WaypointStatus update_fail_check = waypointManagerInstance->update_path_nodes(appendElement, APPEND_WAYPOINT, 0, 0, 0); // Only need to check that it returns an error

    delete waypointManagerInstance;
	
	/**********************ASSERTS**********************/

    EXPECT_EQ(initialize_check, WAYPOINT_SUCCESS);  
    EXPECT_EQ(update_fail_check, INVALID_PARAMETERS);  
}

TEST(Waypoint_Manager, InsertElementToFirstIndexAndReturnError) {

   	/***********************SETUP***********************/

	WaypointManager * waypointManagerInstance = new WaypointManager(43.467998128, 80.537331184); // Creates object

    // Creates the initial flight path and home base
    int numPaths = 50;

    _PathData * initialPaths[PATH_BUFFER_SIZE];
    _PathData ** testArray = nullptr;

    _WaypointBufferStatus status[PATH_BUFFER_SIZE];

    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        status[i] = FREE;
        initialPaths[i] = nullptr;
    }

    long double longitude = 1;
    long double latitude = 10;
    int altitude = 100;
    _WaypointOutputType waypointType = PATH_FOLLOW;
	
    int id_array[150];
    //Initializes ID array
    for(int i = 0; i < 150; i++) {
        id_array[i] = 0;
    }

	/********************STEPTHROUGH********************/

    for(int i = 0; i < numPaths; i++) {
        initialPaths[i] = waypointManagerInstance->initialize_waypoint(longitude, latitude, altitude, waypointType);
        status[i] = FULL;
        id_array[i] = initialPaths[i]->waypointId;
        longitude++;
        latitude++;
        altitude++;
    }

    _WaypointStatus initialize_check = waypointManagerInstance->initialize_flight_path(initialPaths, numPaths);

    // Should not insert new element
    _PathData * insertElement = waypointManagerInstance->initialize_waypoint(1000, 1000, 1000, PATH_FOLLOW);

    // Updates the waypointBuffer and waypointBufferStatus and compares values
    _WaypointStatus update_fail_check = waypointManagerInstance->update_path_nodes(insertElement, INSERT_WAYPOINT, 0, 0, id_array[0]);
    testArray = waypointManagerInstance->get_waypoint_buffer();
    _ArrayStatus paths_equal_check = compare_arrays(initialPaths, testArray, numPaths);
    _ArrayStatus paths_status_equal_check = compare_buffer_status(status, waypointManagerInstance);

    delete waypointManagerInstance;
	
	/**********************ASSERTS**********************/

	EXPECT_EQ(paths_equal_check, ARRAY_SUCCESS);  
    EXPECT_EQ(paths_status_equal_check, ARRAY_SUCCESS);  
    EXPECT_EQ(initialize_check, WAYPOINT_SUCCESS);  
    EXPECT_EQ(update_fail_check, INVALID_PARAMETERS);  
}

TEST(Waypoint_Manager, InsertElementToLastIndexArrayNotFullAndReturnError) {

   	/***********************SETUP***********************/

	WaypointManager * waypointManagerInstance = new WaypointManager(43.467998128, 80.537331184); // Creates object

    // Creates the initial flight path and home base
    int numPaths = 50;

    _PathData * initialPaths[PATH_BUFFER_SIZE];
    _PathData ** testArray = nullptr;

    _WaypointBufferStatus status[PATH_BUFFER_SIZE];

    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        status[i] = FREE;
        initialPaths[i] = nullptr;
    }

    long double longitude = 1;
    long double latitude = 10;
    int altitude = 100;
    _WaypointOutputType waypointType = PATH_FOLLOW;
	
    int id_array[150];
    //Initializes ID array
    for(int i = 0; i < 150; i++) {
        id_array[i] = 0;
    }

	/********************STEPTHROUGH********************/

    for(int i = 0; i < numPaths; i++) {
        initialPaths[i] = waypointManagerInstance->initialize_waypoint(longitude, latitude, altitude, waypointType);
        status[i] = FULL;
        id_array[i] = initialPaths[i]->waypointId;
        longitude++;
        latitude++;
        altitude++;
    }

    _WaypointStatus initialize_check = waypointManagerInstance->initialize_flight_path(initialPaths, numPaths);

    // Should not insert new element
    _PathData * insertElement = waypointManagerInstance->initialize_waypoint(1000, 1000, 1000, PATH_FOLLOW);

    // Updates the waypointBuffer and waypointBufferStatus and compares values
    _WaypointStatus update_success_check = waypointManagerInstance->update_path_nodes(insertElement, INSERT_WAYPOINT, 0, id_array[numPaths-1], 0);
    testArray = waypointManagerInstance->get_waypoint_buffer();
    _ArrayStatus paths_equal_check = compare_arrays(initialPaths, testArray, numPaths);
    _ArrayStatus paths_status_equal_check = compare_buffer_status(status, waypointManagerInstance);

    delete waypointManagerInstance;
	
	/**********************ASSERTS**********************/

	EXPECT_EQ(paths_equal_check, ARRAY_SUCCESS);  
    EXPECT_EQ(paths_status_equal_check, ARRAY_SUCCESS);  
    EXPECT_EQ(initialize_check, WAYPOINT_SUCCESS);  
    EXPECT_EQ(update_success_check, INVALID_PARAMETERS); 
}

TEST(Waypoint_Manager, InsertElementToFullArrayAndReturnError) {

   	/***********************SETUP***********************/

	WaypointManager * waypointManagerInstance = new WaypointManager(43.467998128, 80.537331184); // Creates object

    // Creates the initial flight path and home base
    int numPaths = 100;

    _PathData * initialPaths[PATH_BUFFER_SIZE];
    _PathData ** testArray = nullptr;

    _WaypointBufferStatus status[PATH_BUFFER_SIZE];

    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        status[i] = FREE;
        initialPaths[i] = nullptr;
    }

    long double longitude = 1;
    long double latitude = 10;
    int altitude = 100;
    _WaypointOutputType waypointType = PATH_FOLLOW;
	
    int id_array[150];
    //Initializes ID array
    for(int i = 0; i < 150; i++) {
        id_array[i] = 0;
    }

	/********************STEPTHROUGH********************/

    for(int i = 0; i < numPaths; i++) {
        initialPaths[i] = waypointManagerInstance->initialize_waypoint(longitude, latitude, altitude, waypointType);
        status[i] = FULL;
        id_array[i] = initialPaths[i]->waypointId;
        longitude++;
        latitude++;
        altitude++;
    }

    _WaypointStatus initialize_check = waypointManagerInstance->initialize_flight_path(initialPaths, numPaths);

    // Updates the waypointBuffer and waypointBufferStatus and compares values
    _PathData * insertElement = waypointManagerInstance->initialize_waypoint(1000, 1000, 1000, PATH_FOLLOW);

    // Initializes waypint maanger with the flight path
    _WaypointStatus update_fail_check = waypointManagerInstance->update_path_nodes(insertElement, INSERT_WAYPOINT, 0, id_array[99], 0);
    testArray = waypointManagerInstance->get_waypoint_buffer();
    _ArrayStatus paths_equal_check = compare_arrays(initialPaths, testArray, numPaths);
    _ArrayStatus paths_status_equal_check = compare_buffer_status(status, waypointManagerInstance);

    delete waypointManagerInstance;
	
	/**********************ASSERTS**********************/

	EXPECT_EQ(paths_equal_check, ARRAY_SUCCESS);  
    EXPECT_EQ(paths_status_equal_check, ARRAY_SUCCESS);  
    EXPECT_EQ(initialize_check, WAYPOINT_SUCCESS);  
    EXPECT_EQ(update_fail_check, INVALID_PARAMETERS);   
}

TEST(Waypoint_Manager, InsertToArrayInMiddle) {

   	/***********************SETUP***********************/

	WaypointManager * waypointManagerInstance = new WaypointManager(43.467998128, 80.537331184); // Creates object

    // Creates the initial flight path and home base
    int numPaths = 30;
    int insertIndex = 15;

    _PathData * initialPaths[PATH_BUFFER_SIZE];
    _PathData * insertPaths[PATH_BUFFER_SIZE];
    _PathData ** testArray = nullptr;

    _WaypointBufferStatus status[PATH_BUFFER_SIZE];

    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        status[i] = FREE;
        initialPaths[i] = nullptr;
        insertPaths[i] = nullptr;
    }

    _PathData * insertElement = waypointManagerInstance->initialize_waypoint(1000, 1000, 1000, PATH_FOLLOW);

    long double longitude = 1;
    long double latitude = 10;
    int altitude = 100;
    _WaypointOutputType waypointType = PATH_FOLLOW;
	
    int id_array[150];
    //Initializes ID array
    for(int i = 0; i < 150; i++) {
        id_array[i] = 0;
    }

	/********************STEPTHROUGH********************/

    for(int i = 0; i < numPaths+1; i++) {
        if(i < numPaths) {
            initialPaths[i] = waypointManagerInstance->initialize_waypoint(longitude, latitude, altitude, waypointType);            
            status[i] = FULL;
            id_array[i] = initialPaths[i]->waypointId;
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

    _WaypointStatus initialize_check = waypointManagerInstance->initialize_flight_path(initialPaths, numPaths);

    // Updates arrays and constants to account for inserting of new element
    status[numPaths] = FULL;
    numPaths++;

    // Updates the waypointBuffer and waypointBufferStatus and compares values
    _WaypointStatus update_success_check = waypointManagerInstance->update_path_nodes(insertElement, INSERT_WAYPOINT, 0, id_array[insertIndex-1], id_array[insertIndex]);
    testArray = waypointManagerInstance->get_waypoint_buffer();
    _ArrayStatus paths_equal_check = compare_arrays(insertPaths, testArray, numPaths);
    _ArrayStatus paths_status_equal_check = compare_buffer_status(status, waypointManagerInstance);

    delete waypointManagerInstance;
	
	/**********************ASSERTS**********************/
	
    EXPECT_EQ(initialize_check, WAYPOINT_SUCCESS);  
    EXPECT_EQ(update_success_check, WAYPOINT_SUCCESS);

    EXPECT_EQ(paths_equal_check, ARRAY_SUCCESS);  
    EXPECT_EQ(paths_status_equal_check, ARRAY_SUCCESS);  
}

TEST(Waypoint_Manager, deleteMiddleElement) {

   	/***********************SETUP***********************/

	WaypointManager * waypointManagerInstance = new WaypointManager(43.467998128, 80.537331184); // Creates object

    // Creates the initial flight path and home base

    int numPaths = 57;
    int deleteIndex = 50;

    _PathData * initialPaths[PATH_BUFFER_SIZE];
    _PathData * deleteArray[PATH_BUFFER_SIZE];
    _PathData ** testArray = nullptr;

    _WaypointBufferStatus status[PATH_BUFFER_SIZE];

    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        status[i] = FREE;
        initialPaths[i] = nullptr;
        deleteArray[i] = nullptr;
    }

    long double longitude = 1;
    long double latitude = 10;
    int altitude = 100;
    _WaypointOutputType waypointType = PATH_FOLLOW;
	
    int id_array[150];
    //Initializes ID array
    for(int i = 0; i < 150; i++) {
        id_array[i] = 0;
    }

	/********************STEPTHROUGH********************/

    for(int i = 0; i < numPaths; i++) {
        initialPaths[i] = waypointManagerInstance->initialize_waypoint(longitude, latitude, altitude, waypointType);
        id_array[i] = initialPaths[i]->waypointId;
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

    _WaypointStatus initialize_check = waypointManagerInstance->initialize_flight_path(initialPaths, numPaths);

    // Adjusts constants
    numPaths--;

    // Updates the waypointBuffer and waypointBufferStatus and compares values
    _WaypointStatus update_success_check = waypointManagerInstance->update_path_nodes(nullptr, DELETE_WAYPOINT, id_array[deleteIndex], 0, 0);
    testArray = waypointManagerInstance->get_waypoint_buffer();
    _ArrayStatus paths_equal_check = compare_arrays(deleteArray, testArray, numPaths);
    _ArrayStatus paths_status_equal_check = compare_buffer_status(status, waypointManagerInstance);

    delete waypointManagerInstance;
	
	/**********************ASSERTS**********************/

	EXPECT_EQ(paths_equal_check, ARRAY_SUCCESS);  
    EXPECT_EQ(paths_status_equal_check, ARRAY_SUCCESS);  
    EXPECT_EQ(initialize_check, WAYPOINT_SUCCESS);  
    EXPECT_EQ(update_success_check, WAYPOINT_SUCCESS);
}

TEST(Waypoint_Manager, deleteFirstElement) {

    /***********************SETUP***********************/

	WaypointManager * waypointManagerInstance = new WaypointManager(43.467998128, 80.537331184); // Creates object

    // Creates the initial flight path and home base

    int numPaths = 57;
    int deleteIndex = 0;

    _PathData * initialPaths[PATH_BUFFER_SIZE];
    _PathData * deleteArray[PATH_BUFFER_SIZE];
    _PathData ** testArray = nullptr;

    _WaypointBufferStatus status[PATH_BUFFER_SIZE];

    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        status[i] = FREE;
        initialPaths[i] = nullptr;
        deleteArray[i] = nullptr;
    }

    long double longitude = 1;
    long double latitude = 10;
    int altitude = 100;
    _WaypointOutputType waypointType = PATH_FOLLOW;
	
    int id_array[150];
    //Initializes ID array
    for(int i = 0; i < 150; i++) {
        id_array[i] = 0;
    }

	/********************STEPTHROUGH********************/

    for(int i = 0; i < numPaths; i++) {
        initialPaths[i] = waypointManagerInstance->initialize_waypoint(longitude, latitude, altitude, waypointType);
        id_array[i] = initialPaths[i]->waypointId;
        longitude++;
        latitude++;
        altitude++;
        
        if (i > 0) {
            deleteArray[i-1] = initialPaths[i];
            status[i-1] = FULL;
        } 
    }

    _WaypointStatus initialize_check = waypointManagerInstance->initialize_flight_path(initialPaths, numPaths);

    // Adjusts constants
    numPaths--;

    // Updates the waypointBuffer and waypointBufferStatus and compares values
    _WaypointStatus update_success_check = waypointManagerInstance->update_path_nodes(nullptr, DELETE_WAYPOINT, id_array[deleteIndex], 0, 0);
    testArray = waypointManagerInstance->get_waypoint_buffer();
    _ArrayStatus paths_equal_check = compare_arrays(deleteArray, testArray, numPaths);
    _ArrayStatus paths_status_equal_check = compare_buffer_status(status, waypointManagerInstance);

    delete waypointManagerInstance;
	
	/**********************ASSERTS**********************/

	EXPECT_EQ(paths_equal_check, ARRAY_SUCCESS);  
    EXPECT_EQ(paths_status_equal_check, ARRAY_SUCCESS);  
    EXPECT_EQ(initialize_check, WAYPOINT_SUCCESS);  
    EXPECT_EQ(update_success_check, WAYPOINT_SUCCESS);  
}

TEST(Waypoint_Manager, deleteLastElement) {

   	/***********************SETUP***********************/

	WaypointManager * waypointManagerInstance = new WaypointManager(43.467998128, 80.537331184); // Creates object

    // Creates the initial flight path and home base

    int numPaths = 57;
    int deleteIndex = numPaths-1;

    _PathData * initialPaths[PATH_BUFFER_SIZE];
    _PathData * deleteArray[PATH_BUFFER_SIZE];
    _PathData ** testArray = nullptr;

    _WaypointBufferStatus status[PATH_BUFFER_SIZE];

    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        status[i] = FREE;
        initialPaths[i] = nullptr;
        deleteArray[i] = nullptr;
    }

    long double longitude = 1;
    long double latitude = 10;
    int altitude = 100;
    _WaypointOutputType waypointType = PATH_FOLLOW;
	
    int id_array[150];
    //Initializes ID array
    for(int i = 0; i < 150; i++) {
        id_array[i] = 0;
    }

	/********************STEPTHROUGH********************/

    for(int i = 0; i < numPaths; i++) {
        initialPaths[i] = waypointManagerInstance->initialize_waypoint(longitude, latitude, altitude, waypointType);
        id_array[i] = initialPaths[i]->waypointId;
        longitude++;
        latitude++;
        altitude++;

        if(i < deleteIndex) {
            deleteArray[i] = initialPaths[i];
            status[i] = FULL;
        }
    }

    _WaypointStatus initialize_check = waypointManagerInstance->initialize_flight_path(initialPaths, numPaths);

    // Adjusts constants
    numPaths--;

    // Updates the waypointBuffer and waypointBufferStatus and compares values
    _WaypointStatus update_success_check = waypointManagerInstance->update_path_nodes(nullptr, DELETE_WAYPOINT, id_array[deleteIndex], 0, 0);
    testArray = waypointManagerInstance->get_waypoint_buffer();
    _ArrayStatus paths_equal_check = compare_arrays(deleteArray, testArray, numPaths);
    _ArrayStatus paths_status_equal_check = compare_buffer_status(status, waypointManagerInstance);

    delete waypointManagerInstance;
	
	/**********************ASSERTS**********************/

	EXPECT_EQ(paths_equal_check, ARRAY_SUCCESS);  
    EXPECT_EQ(paths_status_equal_check, ARRAY_SUCCESS);  
    EXPECT_EQ(initialize_check, WAYPOINT_SUCCESS);  
    EXPECT_EQ(update_success_check, WAYPOINT_SUCCESS);  
}

TEST(Waypoint_Manager, deleteLastElementFullArray) {

   	/***********************SETUP***********************/

	WaypointManager * waypointManagerInstance = new WaypointManager(43.467998128, 80.537331184); // Creates object

    // Creates the initial flight path and home base

    int numPaths = 100;
    int deleteIndex = numPaths-1;

    _PathData * initialPaths[PATH_BUFFER_SIZE];
    _PathData * deleteArray[PATH_BUFFER_SIZE];
    _PathData ** testArray = nullptr;

    _WaypointBufferStatus status[PATH_BUFFER_SIZE];

    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        status[i] = FREE;
        initialPaths[i] = nullptr;
        deleteArray[i] = nullptr;
    }

    long double longitude = 1;
    long double latitude = 10;
    int altitude = 100;
    _WaypointOutputType waypointType = PATH_FOLLOW;
	
    int id_array[150];
    //Initializes ID array
    for(int i = 0; i < 150; i++) {
        id_array[i] = 0;
    }

	/********************STEPTHROUGH********************/

    for(int i = 0; i < numPaths; i++) {
        initialPaths[i] = waypointManagerInstance->initialize_waypoint(longitude, latitude, altitude, waypointType);
        id_array[i] = initialPaths[i]->waypointId;
        longitude++;
        latitude++;
        altitude++;

        if(i < deleteIndex) {
            deleteArray[i] = initialPaths[i];
            status[i] = FULL;
        }
    }

    _WaypointStatus initialize_check = waypointManagerInstance->initialize_flight_path(initialPaths, numPaths);

    // Adjusts constants
    numPaths--;

    // Updates the waypointBuffer and waypointBufferStatus and compares values
    _WaypointStatus update_success_check = waypointManagerInstance->update_path_nodes(nullptr, DELETE_WAYPOINT, id_array[deleteIndex], 0, 0);
    testArray = waypointManagerInstance->get_waypoint_buffer();
    _ArrayStatus paths_equal_check = compare_arrays(deleteArray, testArray, numPaths);
    _ArrayStatus paths_status_equal_check = compare_buffer_status(status, waypointManagerInstance);

    delete waypointManagerInstance;
	
	/**********************ASSERTS**********************/

	EXPECT_EQ(paths_equal_check, ARRAY_SUCCESS);  
    EXPECT_EQ(paths_status_equal_check, ARRAY_SUCCESS);  
    EXPECT_EQ(initialize_check, WAYPOINT_SUCCESS);  
    EXPECT_EQ(update_success_check, WAYPOINT_SUCCESS);  
}

TEST(Waypoint_Manager, updateFirstElement) {

    /***********************SETUP***********************/

	WaypointManager * waypointManagerInstance = new WaypointManager(43.467998128, 80.537331184); // Creates object

    // Creates the initial flight path and home base

    int numPaths = 50;
    int updateElement = 0;

    _PathData * initialPaths[PATH_BUFFER_SIZE];
    _PathData ** testArray = nullptr;

    _WaypointBufferStatus status[PATH_BUFFER_SIZE];

    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        status[i] = FREE;
        initialPaths[i] = nullptr;
    }

    long double longitude = 1;
    long double latitude = 10;
    int altitude = 100;
    _WaypointOutputType waypointType = PATH_FOLLOW;
	
    int id_array[150];
    //Initializes ID array
    for(int i = 0; i < 150; i++) {
        id_array[i] = 0;
    }

	/********************STEPTHROUGH********************/

    for(int i = 0; i < numPaths; i++) {
        initialPaths[i] = waypointManagerInstance->initialize_waypoint(longitude, latitude, altitude, waypointType);
        status[i] = FULL;
        id_array[i] = initialPaths[i]->waypointId;
        longitude++;
        latitude++;
        altitude++;
    }

    _WaypointStatus initialize_check = waypointManagerInstance->initialize_flight_path(initialPaths, numPaths);

    // Updates new element
    _PathData * updateWaypoint = waypointManagerInstance->initialize_waypoint(1000, 1000, 1000, HOLD_WAYPOINT);
    initialPaths[updateElement] = updateWaypoint;

    // Updates the waypointBuffer and waypointBufferStatus and compares values
    _WaypointStatus update_success_check = waypointManagerInstance->update_path_nodes(updateWaypoint, UPDATE_WAYPOINT, id_array[updateElement], 0, 0);
    testArray = waypointManagerInstance->get_waypoint_buffer();
    _ArrayStatus paths_equal_check = compare_arrays(initialPaths, testArray, numPaths);
    _ArrayStatus paths_status_equal_check = compare_buffer_status(status, waypointManagerInstance);

    delete waypointManagerInstance;
	
	/**********************ASSERTS**********************/

	EXPECT_EQ(paths_equal_check, ARRAY_SUCCESS);  
    EXPECT_EQ(paths_status_equal_check, ARRAY_SUCCESS);  
    EXPECT_EQ(initialize_check, WAYPOINT_SUCCESS);  
    EXPECT_EQ(update_success_check, WAYPOINT_SUCCESS);
}

TEST(Waypoint_Manager, updateLastElement) {

    /***********************SETUP***********************/

	WaypointManager * waypointManagerInstance = new WaypointManager(43.467998128, 80.537331184); // Creates object

    // Creates the initial flight path and home base

    int numPaths = 50;
    int updateElement = numPaths-1;

    _PathData * initialPaths[PATH_BUFFER_SIZE];
    _PathData ** testArray = nullptr;

    _WaypointBufferStatus status[PATH_BUFFER_SIZE];

    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        status[i] = FREE;
        initialPaths[i] = nullptr;
    }

    long double longitude = 1;
    long double latitude = 10;
    int altitude = 100;
    _WaypointOutputType waypointType = PATH_FOLLOW;
	
    int id_array[150];
    //Initializes ID array
    for(int i = 0; i < 150; i++) {
        id_array[i] = 0;
    }

	/********************STEPTHROUGH********************/

    for(int i = 0; i < numPaths; i++) {
        initialPaths[i] = waypointManagerInstance->initialize_waypoint(longitude, latitude, altitude, waypointType);
        status[i] = FULL;
        id_array[i] = initialPaths[i]->waypointId;
        longitude++;
        latitude++;
        altitude++;
    }

    _WaypointStatus initialize_check = waypointManagerInstance->initialize_flight_path(initialPaths, numPaths);

    // Updates new element
    _PathData * updateWaypoint = waypointManagerInstance->initialize_waypoint(1000, 1000, 1000, HOLD_WAYPOINT);
    initialPaths[updateElement] = updateWaypoint;

    // Updates the waypointBuffer and waypointBufferStatus and compares values
    _WaypointStatus update_success_check = waypointManagerInstance->update_path_nodes(updateWaypoint, UPDATE_WAYPOINT, id_array[updateElement], 0, 0);
    testArray = waypointManagerInstance->get_waypoint_buffer();
    _ArrayStatus paths_equal_check = compare_arrays(initialPaths, testArray, numPaths);
    _ArrayStatus paths_status_equal_check = compare_buffer_status(status, waypointManagerInstance);

    delete waypointManagerInstance;
	
	/**********************ASSERTS**********************/

	EXPECT_EQ(paths_equal_check, ARRAY_SUCCESS);  
    EXPECT_EQ(paths_status_equal_check, ARRAY_SUCCESS);  
    EXPECT_EQ(initialize_check, WAYPOINT_SUCCESS);  
    EXPECT_EQ(update_success_check, WAYPOINT_SUCCESS);
}

TEST(Waypoint_Manager, updateMiddleElement) {

    /***********************SETUP***********************/

	WaypointManager * waypointManagerInstance = new WaypointManager(43.467998128, 80.537331184); // Creates object

    // Creates the initial flight path and home base

    int numPaths = 50;
    int updateElement = 20;

    _PathData * initialPaths[PATH_BUFFER_SIZE];
    _PathData ** testArray = nullptr;

    _WaypointBufferStatus status[PATH_BUFFER_SIZE];

    for(int i = 0; i < PATH_BUFFER_SIZE; i++) {
        status[i] = FREE;
        initialPaths[i] = nullptr;
    }

    long double longitude = 1;
    long double latitude = 10;
    int altitude = 100;
    _WaypointOutputType waypointType = PATH_FOLLOW;
	
    int id_array[150];
    //Initializes ID array
    for(int i = 0; i < 150; i++) {
        id_array[i] = 0;
    }

	/********************STEPTHROUGH********************/

    for(int i = 0; i < numPaths; i++) {
        initialPaths[i] = waypointManagerInstance->initialize_waypoint(longitude, latitude, altitude, waypointType);
        status[i] = FULL;
        id_array[i] = initialPaths[i]->waypointId;
        longitude++;
        latitude++;
        altitude++;
    }

    // Initializes waypint maanger with the flight path
    _WaypointStatus initialize_check = waypointManagerInstance->initialize_flight_path(initialPaths, numPaths);

    // Updates new element
    _PathData * updateWaypoint = waypointManagerInstance->initialize_waypoint(1000, 1000, 1000, HOLD_WAYPOINT);
    initialPaths[updateElement] = updateWaypoint;

    // Updates the waypointBuffer and waypointBufferStatus and compares values
    _WaypointStatus update_success_check = waypointManagerInstance->update_path_nodes(updateWaypoint, UPDATE_WAYPOINT, id_array[updateElement], 0, 0);
    testArray = waypointManagerInstance->get_waypoint_buffer();
    _ArrayStatus paths_equal_check = compare_arrays(initialPaths, testArray, numPaths);
    _ArrayStatus paths_status_equal_check = compare_buffer_status(status, waypointManagerInstance);

    delete waypointManagerInstance;
	
	/**********************ASSERTS**********************/

	EXPECT_EQ(paths_equal_check, ARRAY_SUCCESS);  
    EXPECT_EQ(paths_status_equal_check, ARRAY_SUCCESS);  
    EXPECT_EQ(initialize_check, WAYPOINT_SUCCESS);  
    EXPECT_EQ(update_success_check, WAYPOINT_SUCCESS);  
}

