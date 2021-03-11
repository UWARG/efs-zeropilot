#include <gtest/gtest.h>

#include "landingManager.hpp"
#include "pathDatatypes.hpp"
#include "waypointManager.hpp"
#include "vectorClass.hpp"

using namespace std; 
using ::testing::Test;


TEST(LandingFunctions, Test_changingAltitude){ 
    /***SETUP***/
    _PathData aimingPoint;
    _PathData intersectionPoint;
    _PathData stoppingPoint;
    _WaypointManager_Data_In input;

    aimingPoint.longitude = 0;
    aimingPoint.latitude = 0;
    aimingPoint.altitude = 1;

    stoppingPoint.longitude = -1;
    stoppingPoint.latitude = -1;
    stoppingPoint.altitude = 1;

    intersectionPoint.longitude = 1;
    intersectionPoint.latitude = 1;
    intersectionPoint.altitude = 2;

    input.longitude = 1;
    input.latitude = 0;
    input.altitude = 0;
    /***STEPTHROUGH***/
    
    double desiredAltitude = LandingManager::changingAltitude(input, aimingPoint, intersectionPoint, stoppingPoint);
    /***ASSERTS***/
    EXPECT_NEAR(desiredAltitude, 1.5, 0.000001); //6 decimal places for altitude
}

TEST(LandingFunctions, Test_approachSpeed_packageTrue){ 
    /***SETUP***/
    double windSpeed = 5;
    bool ifPackage = true;
    /***STEPTHROUGH***/
    double desiredSpeed = LandingManager::approachSpeed(windSpeed, ifPackage);
    /***ASSERTS***/
    EXPECT_EQ(desiredSpeed, 10.2);
}

TEST(LandingFunctions, Test_approachSpeed_packageFalse){ 
    /***SETUP***/
    double windSpeed = 5;
    bool ifPackage = false;
    /***STEPTHROUGH***/
    double desiredSpeed = LandingManager::approachSpeed(windSpeed, ifPackage);
    /***ASSERTS***/
    EXPECT_EQ(desiredSpeed, 7.6);
}

TEST(LandingFunctions, Test_slowFlightSpeed_packageTrue){ 
    /***SETUP***/
    bool ifPackage = true;
    /***STEPTHROUGH***/
    double desiredSpeed = LandingManager::slowFlightSpeed(ifPackage);
    /***ASSERTS***/
    EXPECT_EQ(desiredSpeed, 6);
}

TEST(LandingFunctions, Test_slowFlightSpeed_packageFalse){ 
    /***SETUP***/
    bool ifPackage = false;
    /***STEPTHROUGH***/
    double desiredSpeed = LandingManager::slowFlightSpeed(ifPackage);
    /***ASSERTS***/
    EXPECT_EQ(desiredSpeed, 4);
}

TEST(LandingFunctions, Test_createSlopeWaypoints){ 
    /***SETUP***/
    Telemetry_PIGO_t input;
    input.longitude = 1;
    input.latitude = 2;
    input.altitude = 3;
    input.stoppingDirectionHeading = 45;
    double currentAltitude = 100;
    _LandingPath path;
    /***STEPTHROUGH***/
    path = LandingManager::createSlopeWaypoints(input, currentAltitude);
    
    /***ASSERTS***/
    //error of 6 decimal places allowed in difference of degrees, translates to 1 m
    EXPECT_NEAR(path.stoppingPoint.longitude, 1, 0.000001);
    EXPECT_NEAR(path.stoppingPoint.latitude, 2, 0.000001);
    EXPECT_NEAR(path.stoppingPoint.altitude, 3, 0.000001);

    EXPECT_NEAR(path.aimingPoint.longitude, 0.9999364408, 0.000001);
    EXPECT_NEAR(path.aimingPoint.latitude, 1.99993648, 0.000001);
    EXPECT_NEAR(path.aimingPoint.altitude, 3, 0.000001);
    
    EXPECT_NEAR(path.intersectionPoint.longitude,  0.9928895334, 0.000001);
    EXPECT_NEAR(path.intersectionPoint.latitude, 1.9928939, 0.000001);
    EXPECT_NEAR(path.intersectionPoint.altitude, 100, 0.000001);
}

