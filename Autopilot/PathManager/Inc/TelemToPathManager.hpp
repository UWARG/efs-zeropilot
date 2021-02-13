/**
 * Communicates with the path manager to send useful data and receive commands.
 * Author: Anthony Bertnyk, Messed with by Gordon Fountain <-- Blame this guy if it breaks
 */

#ifndef TELEM_TO_PATH_MANAGER_HPP
#define TELEM_TO_PATH_MANAGER_HPP

#include "TelemPathInterface.hpp"

/**
 * Initiate communication with the path manager.
 */
void CommWithPMInit();

/**
 * Send attitude/airspeed data to the path manager.
 * @param[in]   data   Pointer to struct with data.
 */
void SendPathData(AttitudeData *data);

/**
 * Retrieve commands from the path manager.
 * @param[out]   commands   Pointer to struct to recieve commands.
 * @return True if commands were recieved.
 */
bool GetCommands(CommandsForPM *commands);

#endif
