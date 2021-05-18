/**
 * Communicates with the path manager to send useful data and receive commands.
 * All functions here should be called by the attitude manager.
 * Author: Anthony Bertnyk
 */

#ifndef COMM_WITH_PATH_MANAGER_HPP
#define COMM_WITH_PATH_MANAGER_HPP

#include "AttitudePathInterface.hpp"

/**
 * Initiate sending of data to the path manager. Called by attitude manager.
 */
void CommFromAMToPMInit();

/**
 * Send attitude/airspeed data to the path manager. Called by attitude manager. CommFromAMToPMInit must be called once before this function can be called.
 * @param[in]   data   Pointer to struct with data.
 */
void SendFromAMToPM(AttitudeData *data);

/**
 * Retrieve new commands from the path manager if available. Called by attitude manager at any time.
 * @param[out]   commands   Pointer to struct to recieve commands.
 * @return True if commands were recieved.
 */
bool GetFromPMToAM(CommandsForAM *commands);

#endif
