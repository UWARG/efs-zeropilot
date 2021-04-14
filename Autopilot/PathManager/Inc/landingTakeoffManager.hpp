#pragma once


#include "waypointManager.hpp"
#include "vectorClass.hpp"
#include "AutoSteer.hpp"

enum _FlightStage{ROLL=0, CLIMB, CRUISING, TRANSITION, SLOPE, FLARE, DECRAB, TOUCHDOWN};

struct _ManualControl
{
    double rollPercent;
    bool rollManual;

    double rudderPercent;
    bool rudderManual;

    double elevatorPercent;
    bool elevatorManual;

    double throttlePercent;
    bool throttleManual;
};

/************************
    LANDING SECTION
************************/

constexpr static double DISTANCE_OF_LANDING {10}; //in meters
constexpr static double METERS_PER_DEG_LAT {111320.0};
constexpr static double ANGLE_OF_LANDING {5}; //in degrees
constexpr static int STALL_SPEED_NO_PACKAGE {2}; //in m/s
constexpr static int STALL_SPEED_WITH_PACKAGE {4}; //in m/s
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

struct _LandingTakeoffInput
{
    Telemetry_PIGO_t telemetryData;
    SFOutput_t sensorOutput;
};

struct _LandingTakeoffOutput
{
    double desiredHeading;
    double desiredTrack;
    double useHeading; //pay attention
    double desiredAltitude;
    double desiredAirspeed;
    _PassbyControl controlDetails;
};


class LandingManager{
    
    public:
        /*
            This function takes the horizontal position of the aircraft and determines the desired altitude
            Projects the position to the vertical plane relative to the aimingPoint, intersectionPoint, and stoppingPoint
            Takes the one of the horizontal coordinates, plug it into the equation of the slope, get an altitude
        */
        static double changingAltitude(Telemetry_PIGO_t input, _PathData aimingPoint, _PathData intersectionPoint, _PathData stoppingPoint);

        /*
            This function takes in the current magnitude of windspeed, and returns an approach speed the aircraft should follow
        */
        static double approachSpeed(bool ifPackage);

        /* 
            This function takes the stall speed and multiplies it by a constant to return a slow flight speed
        */
        static double slowFlightSpeed(bool ifPackage);

        /*
            This function takes the coordinates of the landing spots and direction of landing
            Creates stopping points, aiming point, and intersection point
        */
        static _LandingPath createSlopeWaypoints(Telemetry_PIGO_t input, double currentAltitude);

        /*
            This function translates waypoint output data into landing takeoff output data
            Takes in a waypoint data out structure, reformats it, and outputs a landingTakeoffOutput instance
        */
        static _LandingTakeoffOutput translateWaypointCommands(_WaypointManager_Data_Out outputData);

        /*
            This function translates landing takeoff data to output data
            Takes in a landingTakeoffOutput instance, translates its information into the 2 types of coordinated turn inputs
        */
        static void translateLTSFCommandsToCoordTurns(_LandingTakeoffOutput* outputData, SFOutput_t* sensorOutput, CoordinatedTurnInput_t* turnInput, AltitudeAirspeedInput_t* altitudeAirspeedInput);
};

/************************
    TAKEOFF SECTION
************************/

constexpr static double EXIT_TAKEOFF_ALTITUDE {50}; //in meters, difference in altitude
constexpr static double ROTATION_SPEED_NO_PACKAGE {4}; //in meters per second
constexpr static double ROTATION_SPEED_WITH_PACKAGE {6}; //in meters per second
constexpr static double CLIMB_SPEED_NO_PACKAGE {5}; // in meters per second
constexpr static double CLIMB_SPEED_WITH_PACKAGE {7}; // in meters per second
constexpr static double DISTANCE_OF_TAKEOFF {100}; // in meters, just used to approximate the horizontal location

class TakeoffManager{

    public:
        /*
            This function returns the desired rotation speed limit, where the plane will transition from roll to climb
        */
        static double desiredRotationSpeed(bool ifPackage);

        /*
            This function returns the desired climb speed to be maintained throughout the climb
        */
        static double desiredClimbSpeed(bool ifPackage);
        /*
            This function returns a waypoint that the plane should follow horizontally
            This waypoint does not have a vertical component, as max throttle is kept throughout the climb
        */
        static _PathData createTakeoffWaypoint(double currentLatitude, double currentLongitude, double currentAltitude, double takeoffDirection);
};

