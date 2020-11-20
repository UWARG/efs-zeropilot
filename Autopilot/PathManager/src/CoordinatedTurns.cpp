/**
 * Author: Anthony Berbari
 */

#include "CoordinatedTurns.hpp"
#include "PID.hpp"

#include <math.h>

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

#define RUDDER_SCALING_FACTOR 0.8f // should be experimentally determined

#define DEG_TO_RAD(angleInDegrees) ((angleInDegrees) * M_PI / 180.0)

/***********************************************************************************************************************
 * Variables
 **********************************************************************************************************************/

static PIDController bankPid{1, 0, 0, 0, -MAX_BANK_ANGLE, MAX_BANK_ANGLE}; // PID gains need to be tuned
static PIDController rudderPid{1, 0, 0, 0, -MAX_RUDDER_ANGLE, MAX_RUDDER_ANGLE}; // PID gains need to be tuned

/***********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************/

static float GetRudderAngle(float bankAngle);

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

void CoordinatedTurns_Init(void)
{

}

void CoordinatedTurns_Compute(CoordinatedTurnInput_t *Input, CoordinatedTurnAttitudeManagerCommands_t *AttManCommands)
{

    float bankAngle = bankPid.execute(Input->desiredHeading, Input->currentHeading);

    float rudderSetPoint = GetRudderAngle(bankAngle);

    float rudderCorrection = -1.0f * rudderPid.execute(0.0f, Input->accY);  // when accY is 0, the turn is coordinated. The multiplication by -1 comes from the way the axis is defined on the accelerometer.

    float rudderAngle = rudderSetPoint + rudderCorrection;

    AttManCommands->desiredRoll = DEG_TO_RAD(bankAngle);
    AttManCommands->desiredRudderPosition = DEG_TO_RAD(rudderAngle);

}

static float GetRudderAngle(float bankAngle)
{
    return RUDDER_SCALING_FACTOR * bankAngle;   // very simple for now. Experiments may give us a better formula. The PID will fix any discrepancy though
}
