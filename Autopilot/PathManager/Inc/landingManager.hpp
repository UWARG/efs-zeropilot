#include "waypointManager.hpp"

class LandingManager : public WaypointManager{

    /*
        This function takes the horizontal position of the aircraft and determines the desired altitude
        Projects the position to the vertical plane relative to the aimingPoint, intersectionPoint, and stoppingPoint
        Takes the horizontal coordinates, plug it into the line of the slope, get an altitude
    */
    double changingAltitude(_WaypointManager_Data_In * input, _PathData * aimingPoint, _PathData * intersectionPoint, _PathData * stoppingPoint);

    /*
        This function takes the current horizontal position of the plane and outputs a desired TRACK
        Looks at the plane in horizontal 2d, compares the position of the plane relative to the glide path
    
    double horizontalAdjustment(_WaypointManager_Data_In input, _PathData * aimingPoint, _PathData * intersectionPoint);
    */
    /*
        This function sets the desired altitude so that the throttle turns off
    */
    double throttleOff();

    /* 
        This function outputs the desired HEADING used in the decrab state
        calculates the heading from the aiming point and intersection point
    */
    double alignHeading(_PathData * aimingPoint, _PathData * intersectionPoint);

    /*
        This function takes the coordinates of the landing spots and direction of landing
        Creates stopping points, aiming point, and intersection point
    */
    void createSlopeWaypoints(double stoppingLatitude, double stoppingLongitude, double stoppingAltitude, double directionLanding);

};

