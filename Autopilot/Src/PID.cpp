/**
 * @file PID.cpp
 * @authors Ian Frosst, Anthony Berbari
 */

#include "PID.hpp"

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

PIDController::PIDController(float _kp, float _ki, float _kd, float _i_max, float _min_output, float _max_output){
	kp = _kp;
	kd = _kd;
	ki = _ki;

	i_max = _i_max;
	min_output = _min_output;
	max_output = _max_output;

	integral = 0.0f;
	historicalValue[0] = 0.0f;
	historicalValue[1] = 0.0f;
	historicalValue[2] = 0.0f;
}

float PIDController::execute(float desired, float actual, float actualRate) {

	float error = desired - actual;
	float derivative;

	integral +=  error;

	// avoid integral windup
	if (integral < -i_max)
	{
		integral = -i_max;
	}
	else if (integral > i_max)
	{
		integral = i_max;
	}

	// if we are provided with a measured derivative (say from a gyroscope), it is always less noisy to use that than to compute it ourselves.
	if ( ! std::isnan(actualRate))
	{
		derivative = actualRate;
	}
	else
	{
		historicalValue[2] = historicalValue[1];
		historicalValue[1] = historicalValue[0];
		historicalValue[0] = actual;

		// Finite difference approximation gets rid of noise much better than first order derivative computation
		derivative = ((3 * historicalValue[0]) - (4 * historicalValue[1]) + (historicalValue[2]));
	}

	float ret = ((kp * error) + (ki * integral) - (kd * derivative));

	if (ret < min_output)
	{
		ret = min_output;
	}
	else if (ret > max_output)
	{
		ret = max_output;
	}

	return ret;
}
