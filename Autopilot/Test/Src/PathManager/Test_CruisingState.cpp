/*
* Author: Dhruv Rawat
*/

#include <gtest/gtest.h>

#include "pathManager.hpp"
#include "pathStateClasses.hpp"
#include "waypointManager.hpp"
#include "cruisingState.hpp"

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






