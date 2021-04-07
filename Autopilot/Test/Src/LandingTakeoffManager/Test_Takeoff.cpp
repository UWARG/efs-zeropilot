#include <gtest/gtest.h>

#include "landingTakeoffManager.hpp"
#include "pathDatatypes.hpp"
#include "waypointManager.hpp"

using namespace std; 
using ::testing::Test;


TEST(TakeoffFunctions, Test_desiredRotationNoPackage){ 
    /***SETUP***/
    double wind = 10;
    bool ifPackage = false;
    /***STEPTHROUGH***/
    double desiredSpeed = TakeoffManager::desiredRotationSpeed(wind, ifPackage);
    /***ASSERTS***/
    EXPECT_NEAR(desiredSpeed, 14.0, 0.000001); 
}

TEST(TakeoffFunctions, Test_desiredRotationWithPackage){ 
    /***SETUP***/
    double wind = 10;
    bool ifPackage = true;
    /***STEPTHROUGH***/
    double desiredSpeed = TakeoffManager::desiredRotationSpeed(wind, ifPackage);
    /***ASSERTS***/
    EXPECT_NEAR(desiredSpeed, 16.0, 0.000001); 
}

TEST(TakeoffFunctions, Test_desiredClimbNoPackage){ 
    /***SETUP***/
    double wind = 10;
    bool ifPackage = false;
    /***STEPTHROUGH***/
    double desiredSpeed = TakeoffManager::desiredClimbSpeed(wind, ifPackage);
    /***ASSERTS***/
    EXPECT_NEAR(desiredSpeed, 15.0, 0.000001); 
}

TEST(TakeoffFunctions, Test_desiredClimbWithPackage){ 
    /***SETUP***/
    double wind = 10;
    bool ifPackage = true;
    /***STEPTHROUGH***/
    double desiredSpeed = TakeoffManager::desiredClimbSpeed(wind, ifPackage);
    /***ASSERTS***/
    EXPECT_NEAR(desiredSpeed, 17.0, 0.000001); 
}

TEST(TakeoffFunctions, Test_createTakeoffWaypoint){
    /***SETUP***/
    _PathData desiredTakeoffPoint;
    /***STEPTHROUGH***/
    desiredTakeoffPoint = TakeoffManager::createTakeoffWaypoint(1,2,3, 45);
    /***ASSERTS***/
    EXPECT_NEAR(desiredTakeoffPoint.latitude, 1.0006353018, 0.000001);
    EXPECT_NEAR(desiredTakeoffPoint.longitude, 2.0006352019, 0.000001);
    EXPECT_NEAR(desiredTakeoffPoint.altitude, 3, 0.000001);
}

