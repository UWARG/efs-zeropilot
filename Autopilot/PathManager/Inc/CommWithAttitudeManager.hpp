#ifndef GET_FROM_PATH_MANAGER_HPP
#define GET_FROM_PATH_MANAGER_HPP

//Commands to send to attitude manager.
struct CommandsForAM{
	float roll,pitch,yaw;	// commanded orientation (radians)
	float airspeed;			// commanded airspeed (m/s)
};

//Data to receive from attitude manager.
struct AttitudeData{
	float roll,pitch,yaw;	// current orientation (radians)
	float airspeed;			// current airspeed (m/s)
};

/**
 * Sends commands to the attitude manager.
 * @param[out]   Commands   Pointer to struct with commands.
 */
void SendCommandsForAM(CommandsForAM *Commands);

/**
 * Retrieves attitude/airspeed data from attitude manager.
 * @return Attitude/airspeed data.
 */
AttitudeData GetAttitudeData();

#endif