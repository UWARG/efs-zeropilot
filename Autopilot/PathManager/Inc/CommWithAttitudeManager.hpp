/**
 * Communicates with the attitude manager to send commands and receive useful data.
 * Author: Anthony Bertnyk
 */

#ifndef COMM_WITH_ATTITUDE_MANAGER_HPP
#define COMM_WITH_ATTITUDE_MANAGER_HPP

#include "AttitudePathInterface.hpp"

/**
 * Initiate communication with the attitude manager.
 */
void CommWithAMInit();

/**
 * Send commands to the attitude manager.
 * @param[in]   commands   Pointer to struct with commands.
 */
void SendCommandsForAM(CommandsForAM *commands);

/**
 * Retrieve attitude/airspeed data from attitude manager.
 * @param[out]   data   Pointer to struct to recieve attitude/airspeed data.
 * @return True if data was recieved.
 */
bool GetAttitudeData(AttitudeData *data);

#endif
