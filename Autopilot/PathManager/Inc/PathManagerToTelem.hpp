/**
 * Send useful data to telemetry from path manager.
 * Author: Anthony Bertnyk, Messed with by Gordon Fountain <-- Blame this guy if it breaks
 */

#ifndef PATH_MANAGER_TO_TELEM_HPP
#define PATH_MANAGER_TO_TELEM_HPP

#include "TelemPathInterface.hpp"

/**
 * Initiate communication with telemetry.
 */
void CommWithTelemInit();

/**
 * Send path manager data to telemetry.
 * @param[in]   data   Pointer to struct with data.
 */
void SendPathData(TelemToPMData *data);

/**
 * Retrieve commands from the path manager.
 * @param[out]   commands   Pointer to struct to recieve commands.
 * @return True if commands were recieved.
 */
bool GetCommands(TelemToPMData *commands);

#endif
