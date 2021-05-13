/**
 * Communicates with the path manager to send useful data and receive commands.
 * Author: Anthony Bertnyk
 */

#ifndef COMM_WITH_PATH_MANAGER_HPP
#define COMM_WITH_PATH_MANAGER_HPP

#include "AttitudePathInterface.hpp"

/**
 * Initiate sending of data to the path manager.
 */
void CommFromAMToPMInit();

/**
 * Send attitude/airspeed data to the path manager. CommFromAMToPMInit must be called once before this function can be called.
 * @param[in]   data   Pointer to struct with data.
 */
void SendFromAMToPM(AttitudeData *data);

/**
 * Retrieve commands from the path manager.
 * @param[out]   commands   Pointer to struct to recieve commands.
 * @return True if commands were recieved.
 */
bool GetFromPMToAM(CommandsForAM *commands);

#endif
