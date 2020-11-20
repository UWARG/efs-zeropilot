/**
 * Produces Roll, Pitch, and Rudder commands to achieve a desired change in heading.
 * Author: Anthony Berbari
 */

#ifndef COORDUNATED_TURNS_HPP
#define COORDUNATED_TURNS_HPP

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

#define MAX_BANK_ANGLE 30    // In degrees. This value limits how fast we can achieve a desired heading change. (See confluence docs on this module.)
                             // Increasing it means we can make faster turns, but makes the aircraft harder to control.

#define MAX_RUDDER_ANGLE 45 // In degrees. This should not be a user set thing, rather it is a physical constraint of the control surfaces of the aircraft.

typedef struct
{
    float currentHeading;    // degrees
    float desiredHeading;    // degrees
    float accY;              // Y axis accelerometer reading from the IMU.

}CoordinatedTurnInput_t;

typedef struct
{
    float desiredRoll;             // radians
    float desiredRudderPosition;   // percentage from -100 to 100

}CoordinatedTurnAttitudeManagerCommands_t;

/***********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************/

/**
* Initializes internal data.
* Call this function exactly once before anything is attempted to be done with the module.
*/
void CoordinatedTurns_Init(void);

/**
* Computes the required commands to deliver to attitude manager to achieve the desired heading as fast as possible and without overshoot.
* Note that this module does not ever deal with pitch or altitude, that is the work of another module.
* @param[in]    Input              Input struct containing info about current heading, some other sensor info and desired heading.
* @param[out]   AttManCommands     Output struct containing commands to be passed down directly to attitude manager.
*/
void CoordinatedTurns_Compute(CoordinatedTurnInput_t *Input, CoordinatedTurnAttitudeManagerCommands_t *AttManCommands);

#endif
