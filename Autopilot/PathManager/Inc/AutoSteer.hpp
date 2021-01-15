/**
 * Produces Roll, Pitch, Rudder, and airspeed required to achieve a desired heading and altitude.
 * Author: Anthony Berbari
 */

#ifndef AUTO_STEER_HPP
#define AUTO_STEER_HPP

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

#define MAX_BANK_ANGLE 30    // In degrees. This value limits how fast we can achieve a desired heading change. (See confluence docs on this module.)
                             // Increasing it means we can make faster turns, but makes the aircraft harder to control.

#define MAX_PITCH_ANGLE 30   // In degrees. This value limits how fast we can climb or descend.
                             // Increasing it means we can get to the target altitude faster, but it means we might risk having control problems or some sort of stall.

#define CRUISING_SPEED  15   // In m/s. This is the speed the aircraft will remain at during regular flight.

typedef struct
{
    float currentHeading;    // degrees
    float desiredHeading;    // degrees
    float accY;              // Y axis accelerometer reading from the IMU.

}CoordinatedTurnInput_t;

typedef struct
{
    float currentAltitude;   // in meters
    float desiredAltitude;   // in meters

}AltitudeAirspeedInput_t;

typedef struct
{
    float requiredRoll;             // radians
    float requiredRudderPosition;   // percentage from -100 to 100

}CoordinatedTurnAttitudeManagerCommands_t;

typedef struct
{
    float requiredPitch;        // radians
    float requiredAirspeed;     // in m/s
}AltitudeAirspeedCommands_t;


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
