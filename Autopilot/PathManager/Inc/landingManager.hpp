#pragma once
#include "waypointManager.hpp"

constexpr static double DISTANCE_OF_LANDING {10}; //in meters
constexpr static int METERS_PER_DEG_LAT {111320};
constexpr static int ANGLE_OF_LANDING {5}; //in degrees
constexpr static int STALL_SPEED_NO_PACKAGE {20}; //in km/h
constexpr static int STALL_SPEED_WITH_PACKAGE {30}; //in km/h
constexpr static double PI {3.14159265358979323846};
constexpr static int FLARE_ALTITUDE {2}; //in meters
constexpr static double DECRAB_ALTITUDE {0.7}; //in meters
constexpr static double TOUCHDOWN_ALTITUDE {0.05}; //in meters

struct _LandingPath
{
    _PathData stoppingPoint;
    _PathData aimingPoint;
    _PathData intersectionPoint;
};

struct _LandingInitialData
{
    double stoppingLatitude;
    double stoppingLongitude;
    double stoppingAltitude;
    double landingDirection;
};

enum _LandingStage{NOT_LANDING = 0, TRANSITION, SLOPE, FLARE, DECRAB, TOUCHDOWN};

class LandingManager : public WaypointManager{
    
    public:
        /*
            This function takes the horizontal position of the aircraft and determines the desired altitude
            Projects the position to the vertical plane relative to the aimingPoint, intersectionPoint, and stoppingPoint
            Takes the one of the horizontal coordinates, plug it into the equation of the slope, get an altitude
        */
        static double changingAltitude(_WaypointManager_Data_In input, _PathData aimingPoint, _PathData intersectionPoint, _PathData stoppingPoint);

        /*
            This function sets the desired altitude so that the throttle turns off
        */
        static double throttleOff(void);

        /*
            This function takes in the current magnitude of windspeed, and returns an approach speed the aircraft should follow
        */
        static double approachSpeed(double windSpeed, bool ifPackage);

        /* 
            This function takes the stall speed and multiplies it by a constant to return a slow flight speed
        */
        static double slowFlightSpeed(bool ifPackage);

        /*
            This function takes the coordinates of the landing spots and direction of landing
            Creates stopping points, aiming point, and intersection point
        */
        static _LandingPath createSlopeWaypoints(Telemetry_PIGO_t input);

};

