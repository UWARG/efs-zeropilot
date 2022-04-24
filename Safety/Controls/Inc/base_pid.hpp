/**
 * @file base_pid.hpp
 * @authors Anthony Luo & (Ian Frosst, Anthony Berbari)
 * @date  October 30, 2021
 * @copyright Waterloo Aerial Robotics Group 2021
 */

#ifndef PID_HPP
#define	PID_HPP

#include <cmath>
#include <cstdint>

/***********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************/

class PIDController
{
	public:
		/**
		* Initialises the Pid object.
		* @param[in]	_kp 	The proportional gain.
		* @param[in]	_ki 	The integral gain.
		* @param[in]	_kd  	The derivative gain.
		* @param[in]	_i_max	Max value the integral should ever be allowed to take on.
		* @param[in]	_min_output		The minimum value that can be output, if computations return smaller, the output will be set to this value.
		* @param[in]	_max_output		The maximum value that can be output, if computations return larger, the output will be set to this value.
		*/
		PIDController(float _kp, float _ki, float _kd, float _i_max, float _min_output, float _max_output);

		/**
		* Executes a PID computation.
		* The PID algorithm uses the derivative of the actual values, rather than the derivatives of the error. That makes it immune to sudden changes in commanded set points.
		* THIS METHOD MUST BE CALLED AT A REGULAR INTERVAL!!! UNDEFINED BEHAVIOUR WILL OCCUR OTHERWISE.
		* Will use the measured derivative if provided with one. Otherwise, will compute the derivative from the given measurements.
		* @param[in]	desired 	The point we wish to reach.
		* @param[in]	actual 		The current point.
		* @param[in]	actualRate  The current measured derivative (This parameter is optional).
		* @return					The result of the PID computation.
		*/
		float execute(float desired, float actual, float actualRate = std::nanf(""));
		float execute_p(float desired, float actual);

	private:

		float kp, kd, ki;
		float i_max;
		float integral;
		float historicalValue[3];
		float min_output;
		float max_output;

};

#endif
