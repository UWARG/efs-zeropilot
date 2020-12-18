/**
 * Author: Anthony Berbari
 */

#include "AutoSteer.hpp"
#include "PID.hpp"

#include <cmath>

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

#ifndef M_PI
    #define M_PI 3.14159265358979323846f
#endif


#define DEG_TO_RAD(angleInDegrees) ((angleInDegrees) * M_PI / 180.0)

/***********************************************************************************************************************
 * Variables
 **********************************************************************************************************************/

static PIDController bankPid{1, 0, 0, 0, -MAX_BANK_ANGLE, MAX_BANK_ANGLE}; // PID gains need to be tuned
static PIDController rudderPid{1, 0, 0, 0, -100, 100}; // PID gains need to be tuned

static PIDController pitchPid{1, 0, 0, 0, -MAX_PITCH_ANGLE, MAX_PITCH_ANGLE}; // PID gains need to be tuned

static const float RUDDER_SCALING_FACTOR = 0.8f; // should be experimentally determined

/***********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************/

static float GetRudderPercent(float bankAngle);

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

void AutoSteer_Init(void)
{

}

void AutoSteer_ComputeCoordinatedTurn(CoordinatedTurnInput_t *Input, CoordinatedTurnAttitudeManagerCommands_t *AttManCommands)
{

    float bankAngle = bankPid.execute(Input->desiredHeading, Input->currentHeading);

    float rudderSetPoint = GetRudderPercent(bankAngle);

    float rudderCorrection = -1.0f * rudderPid.execute(0.0f, Input->accY);  // when accY is 0, the turn is coordinated. The multiplication by -1 comes from the way the axis is defined on the accelerometer.

    float rudderPercent = rudderSetPoint + rudderCorrection;

    AttManCommands->requiredRoll = DEG_TO_RAD(bankAngle);
    AttManCommands->requiredRudderPosition = rudderPercent;

}

void AutoSteer_ComputeAltitudeAndAirspeed(AltitudeAirspeedInput_t *Input, AltitudeAirspeedCommands_t *AttManCommands)
{
    float pitchAngle = pitchPid.execute(Input->desiredAltitude, Input->currentAltitude);

    AttManCommands->requiredPitch = DEG_TO_RAD(pitchAngle);
    AttManCommands->requiredAirspeed = CRUISING_SPEED;      // a simple constant to start with. As things get more complex, circumstances will demand variable airspeeds.

}


static float GetRudderPercent(float bankAngle)
{
    return ((RUDDER_SCALING_FACTOR * bankAngle) / (M_PI / 2.0f)) * 100.0f;   // very simple for now. Experiments may give us a better formula. The PID will fix any discrepancy though
}
