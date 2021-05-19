/**
 * Author: Anthony Berbari
 */

#include "AutoSteer.hpp"
#include "PID.hpp"

#include <cmath>
#include "MathConstants.hpp"

/***********************************************************************************************************************
 * Variables
 **********************************************************************************************************************/

static PIDController bankPid{1, 0, 0, 0, -MAX_BANK_ANGLE, MAX_BANK_ANGLE}; // PID gains need to be tuned
static PIDController rudderPid{1, 0, 0, 0, -100, 100}; // PID gains need to be tuned

static PIDController pitchPid{1, 0, 0, 0, -MAX_PITCH_ANGLE, MAX_PITCH_ANGLE}; // PID gains need to be tuned

static PIDController altitudePid{1, 0, 0, 0, -100, 100}; //PID gains need to be tuned

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
    float bankAngle = bankPid.execute(Input->desiredHeadingTrack, Input->currentHeadingTrack);
    float rudderSetPoint = GetRudderPercent(bankAngle);

    float rudderCorrection = -1.0f * rudderPid.execute(0.0f, Input->accY);  // when accY is 0, the turn is coordinated. The multiplication by -1 comes from the way the axis is defined on the accelerometer.

    float rudderPercent = rudderSetPoint + rudderCorrection;

    AttManCommands->requiredRoll = DEG_TO_RAD(bankAngle);
    AttManCommands->requiredRudderPosition = rudderPercent;

}

void AutoSteer_ComputeAltitudeAndAirspeed(AltitudeAirspeedInput_t *Input, AltitudeAirspeedCommands_t *AttManCommands)
{
    float pitchAngle = pitchPid.execute(Input->desiredAirspeed, Input->currentAirspeed);

    AttManCommands->requiredPitch = DEG_TO_RAD(pitchAngle);
    AttManCommands->requiredThrottlePercent = altitudePid.execute(Input->desiredAltitude, Input->currentAltitude);

}


static float GetRudderPercent(float bankAngle)
{
    return ((RUDDER_SCALING_FACTOR * bankAngle) / (ZP_PI / 2.0f)) * 100.0f;   // very simple for now. Experiments may give us a better formula. The PID will fix any discrepancy though
}
