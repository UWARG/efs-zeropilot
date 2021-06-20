#ifndef PATH_DATATYPES_HPP
#define PATH_DATATYPES_HPP

#include <cstdint>

/********************************************************************
* Enums
********************************************************************/

enum _FlightStage{ROLL=0, CLIMB, CRUISING, TRANSITION, SLOPE, FLARE, DECRAB, TOUCHDOWN}; //used to determine the stage of flight

/********************************************************************
* Important Structs
********************************************************************/

/*
* Structure stores manual control information
* If a certain variable needs to be controlled, set the percent to the desired value and its respective boolean to true
*/
struct _PassbyControl{
    
    double rollPercent;
    bool rollPassby;

    double rudderPercent;
    bool rudderPassby;

    double pitchPercent;
    bool pitchPassby;

    double throttlePercent;
    bool throttlePassby;
    _PassbyControl() : rollPercent(0), rollPassby(false), rudderPercent(0), rudderPassby(false), pitchPercent(0), pitchPassby(false), throttlePercent(0), throttlePassby(false)
    {}
};

#endif

