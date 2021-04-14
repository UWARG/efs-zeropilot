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
    double desiredSpeed = TakeoffManager::desiredRotationSpeed(ifPackage);
    /***ASSERTS***/
    EXPECT_NEAR(desiredSpeed, 4.0, 0.000001); 
}

TEST(TakeoffFunctions, Test_desiredRotationWithPackage){ 
    /***SETUP***/
    bool ifPackage = true;
    /***STEPTHROUGH***/
    double desiredSpeed = TakeoffManager::desiredRotationSpeed(ifPackage);
    /***ASSERTS***/
    EXPECT_NEAR(desiredSpeed, 6.0, 0.000001); 
}

TEST(TakeoffFunctions, Test_desiredClimbNoPackage){ 
    /***SETUP***/
    bool ifPackage = false;
    /***STEPTHROUGH***/
    double desiredSpeed = TakeoffManager::desiredClimbSpeed(ifPackage);
    /***ASSERTS***/
    EXPECT_NEAR(desiredSpeed, 5.0, 0.000001); 
}

TEST(TakeoffFunctions, Test_desiredClimbWithPackage){ 
    /***SETUP***/
    bool ifPackage = true;
    /***STEPTHROUGH***/
    double desiredSpeed = TakeoffManager::desiredClimbSpeed(ifPackage);
    /***ASSERTS***/
    EXPECT_NEAR(desiredSpeed, 7.0, 0.000001); 
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

