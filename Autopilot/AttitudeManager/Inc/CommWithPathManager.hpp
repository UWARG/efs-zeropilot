/**
 * Communicates with the path manager to send useful data and receive commands.
 * Author: Anthony Bertnyk
 */

#ifndef COMM_WITH_PATH_MANAGER_HPP
#define COMM_WITH_PATH_MANAGER_HPP

#include "AttitudePathInterface.hpp"

/**
 * Initiate communication with the path manager.
 */
void CommWithPMInit();

/**
 * Send attitude/airspeed data to the path manager.
 * @param[in]   data   Pointer to struct with data.
 */
void SendAttitudeData(AttitudeData *data);

/**
 * Retrieve commands from the path manager.
 * @param[out]   commands   Pointer to struct to recieve commands.
 * @return True if commands were recieved.
 */
bool GetCommands(CommandsForAM *commands);

#endif
