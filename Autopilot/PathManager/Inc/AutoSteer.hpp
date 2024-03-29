/**
 * Produces Roll, Pitch, Rudder, and airspeed required to achieve a desired heading and altitude.
 * Author: Anthony Berbari
 */

#ifndef AUTO_STEER_HPP
#define AUTO_STEER_HPP

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

#define MAX_BANK_ANGLE 20    // In degrees. This value limits how fast we can achieve a desired heading change. (See confluence docs on this module.)
                             // Increasing it means we can make faster turns, but makes the aircraft harder to control.

#define MAX_PITCH_ANGLE 20   // In degrees. This value limits how fast we can climb or descend.
                             // Increasing it means we can get to the target altitude faster, but it means we might risk having control problems or some sort of stall.

#define CRUISING_SPEED  15   // In m/s. This is the speed the aircraft will remain at during regular flight.

struct CoordinatedTurnInput_t
{
    /*
    By using currentHeadingTrack, the user can pass in whatever type of parameter they want to control
    For example, if they wanted to maintain a heading, simply pass in the desired heading and current heading
    If track needs to be controlled, simply pass tracks into these entries

    Each path manager stage will have its own control needs for its functions. Doing this will allow each
    stage to control the plane's attitude more precisely.
    Ex. waypoint manager needs to control track, but landingDecrabStage needs to control heading
    */

    float currentHeadingTrack;    // degrees
    float desiredHeadingTrack;    // degrees
    float accY;              // Y axis accelerometer reading from the IMU.
};

struct AltitudeAirspeedInput_t
{
    float currentAltitude;   // in meters
    float desiredAltitude;   // in meters
    float currentAirspeed;
    float desiredAirspeed;

};

struct CoordinatedTurnAttitudeManagerCommands_t
{
    float requiredRoll;             // radians
    float requiredRudderPosition;   // percentage from -100 to 100

};

struct AltitudeAirspeedCommands_t
{
    float requiredPitch;        // radians
    float requiredThrottlePercent;     // in percentage
};


/***********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************/

/**
* Initializes internal data.
* Call this function exactly once before anything is attempted to be done with the module.
*/
void AutoSteer_Init(void);

/**
* Computes the required commands to deliver to attitude manager to achieve the desired heading as fast as possible and without (too much) overshoot or oscillation.
* @param[in]    Input              Input struct containing info about current heading, some other sensor info and desired heading.
* @param[out]   AttManCommands     Output struct containing commands to be passed down directly to attitude manager.
*/
void AutoSteer_ComputeCoordinatedTurn(CoordinatedTurnInput_t *Input, CoordinatedTurnAttitudeManagerCommands_t *AttManCommands);

/**
* Computes the required commands to deliver to attitude manager to achieve the desired altitude as fast as possible and without (too much) overshoot or oscillation.
* This module also decides on what airspeed to command. For the moment, his will always be a fixed cruising airspeed.
* @param[in]    Input              Input struct containing info about current and desired altitude.
* @param[out]   AttManCommands     Output struct containing commands to be passed down directly to attitude manager.
*/
void AutoSteer_ComputeAltitudeAndAirspeed(AltitudeAirspeedInput_t *Input, AltitudeAirspeedCommands_t *AttManCommands);

#endif
