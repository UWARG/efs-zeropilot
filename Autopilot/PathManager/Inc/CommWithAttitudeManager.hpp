/**
 * Communicates with the attitude manager to send commands and receive useful data.
 * Author: Anthony Bertnyk
 */

#ifndef COMM_WITH_ATTITUDE_MANAGER_HPP
#define COMM_WITH_ATTITUDE_MANAGER_HPP

#include "AttitudePathInterface.hpp"

/**
 * Initiate sending of data to the attitude manager.
 */
void CommFromPMToAMInit();

/**
 * Send commands to the attitude manager. CommFromPMToAMInit must be called once before this function can be called.
 * @param[in]   commands   Pointer to struct with commands.
 */
void SendFromPMToAM(CommandsForAM *commands);

/**
 * Retrieve attitude/airspeed data from attitude manager.
 * @param[out]   data   Pointer to struct to recieve attitude/airspeed data.
 * @return True if data was recieved.
 */
bool GetFromAMToPM(AttitudeData *data);

#endif
