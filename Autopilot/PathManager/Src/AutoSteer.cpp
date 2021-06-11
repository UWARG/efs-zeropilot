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

static PIDController bankPid{1, 3, 45, 5, -MAX_BANK_ANGLE, MAX_BANK_ANGLE}; // PID gains need to be tuned

static PIDController rudderPid{1.5, 0, 20, 0, -100, 100}; // PID gains need to be tuned

static PIDController pitchPid{0.04, 0, 50, 0, -MAX_PITCH_ANGLE, MAX_PITCH_ANGLE}; // PID gains need to be tuned

static PIDController altitudePid{25, 0, 200, 0, 0, 100}; //PID gains need to be tuned

static const float RUDDER_SCALING_FACTOR = 0.5f; // should be experimentally determined

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
    //normalizing the difference in heading for the PID 
    float directionError = Input->currentHeadingTrack - Input->desiredHeadingTrack;

    if(directionError<=(-180))
    {
        directionError += 360;
    }
    else if(directionError>180)
    {
        directionError -= 360;
    }

    float bankAngle = bankPid.execute(0, directionError);
    float rudderSetPoint = GetRudderPercent(bankAngle);

    float rudderCorrection = -1.0f * rudderPid.execute(0.0f, bankAngle);  // when accY is 0, the turn is coordinated. The multiplication by -1 comes from the way the axis is defined on the accelerometer.

    float rudderPercent = rudderCorrection;

    AttManCommands->requiredRoll = DEG_TO_RAD(bankAngle);
    AttManCommands->requiredRudderPosition = rudderPercent;

}

void AutoSteer_ComputeAltitudeAndAirspeed(AltitudeAirspeedInput_t *Input, AltitudeAirspeedCommands_t *AttManCommands)
{
    float pitchAngle =-1*pitchPid.execute(Input->desiredAirspeed, Input->currentAirspeed);

    AttManCommands->requiredThrottlePercent = altitudePid.execute(Input->desiredAltitude, Input->currentAltitude);
    AttManCommands->requiredPitch = DEG_TO_RAD(pitchAngle*AttManCommands->requiredThrottlePercent/100.0);
    

}


static float GetRudderPercent(float bankAngle)
{
    return ((RUDDER_SCALING_FACTOR * bankAngle) / (ZP_PI / 2.0f)) * 100.0f;   // very simple for now. Experiments may give us a better formula. The PID will fix any discrepancy though
}
