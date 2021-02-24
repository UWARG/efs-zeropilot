#include "waypointManager.hpp"

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
        double changingAltitude(_WaypointManager_Data_In * input, _PathData * aimingPoint, _PathData * intersectionPoint, _PathData * stoppingPoint);

        /*
            This function sets the desired altitude so that the throttle turns off
        */
        double throttleOff(void);

        /*
            This function takes in the current magnitude of windspeed, and returns an approach speed the aircraft should follow
        */
        double approachSpeed(double windSpeed, bool ifPackage);

        /* 
            This function takes the stall speed and multiplies it by a constant to return a slow flight speed
        */
        double slowFlightSpeed(bool ifPackage);

        /*
            This function takes the coordinates of the landing spots and direction of landing
            Creates stopping points, aiming point, and intersection point
        */
        _LandingPath createSlopeWaypoints(_WaypointManager_Data_In * input, float stoppingLatitude, float stoppingLongitude, float stoppingAltitude, double directionLanding);

};

