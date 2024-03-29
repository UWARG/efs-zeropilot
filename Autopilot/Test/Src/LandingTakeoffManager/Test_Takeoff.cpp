#include <gtest/gtest.h>

#include "landingTakeoffManager.hpp"
#include "pathDatatypes.hpp"
#include "waypointManager.hpp"

using namespace std; 
using ::testing::Test;


TEST(TakeoffFunctions, Test_desiredRotationNoPackage){ 
    /***SETUP***/
    bool ifPackage = false;
    /***STEPTHROUGH***/
    double desiredSpeed = LandingTakeoffManager::desiredRotationSpeed(ifPackage);
    /***ASSERTS***/
    EXPECT_NEAR(desiredSpeed, 7, 0.000001); 
}

TEST(TakeoffFunctions, Test_desiredRotationWithPackage){ 
    /***SETUP***/
    bool ifPackage = true;
    /***STEPTHROUGH***/
    double desiredSpeed = LandingTakeoffManager::desiredRotationSpeed(ifPackage);
    /***ASSERTS***/
    EXPECT_NEAR(desiredSpeed, 7, 0.000001); 
}

TEST(TakeoffFunctions, Test_desiredClimbNoPackage){ 
    /***SETUP***/
    bool ifPackage = false;
    /***STEPTHROUGH***/
    double desiredSpeed = LandingTakeoffManager::desiredClimbSpeed(ifPackage);
    /***ASSERTS***/
    EXPECT_NEAR(desiredSpeed, 15.0, 0.000001); 
}

TEST(TakeoffFunctions, Test_desiredClimbWithPackage){ 
    /***SETUP***/
    bool ifPackage = true;
    /***STEPTHROUGH***/
    double desiredSpeed = LandingTakeoffManager::desiredClimbSpeed(ifPackage);
    /***ASSERTS***/
    EXPECT_NEAR(desiredSpeed, 15.0, 0.000001); 
}

TEST(TakeoffFunctions, Test_createTakeoffWaypoint){
    /***SETUP***/
    _PathData desiredTakeoffPoint;
    /***STEPTHROUGH***/
    long double currentLatitude = 1;
    long double currentLongitude = 2;
    float currentAltitude = 3;
    float takeoffDirection = 45;

    desiredTakeoffPoint = LandingTakeoffManager::createTakeoffWaypoint(currentLatitude, currentLongitude, currentAltitude, takeoffDirection);
    /***ASSERTS***/
    EXPECT_NEAR(desiredTakeoffPoint.latitude, 1.0063520192, 0.000001);
    EXPECT_NEAR(desiredTakeoffPoint.longitude, 2.0063530185, 0.000001);
    EXPECT_NEAR(desiredTakeoffPoint.altitude, 3, 0.000001);
}

