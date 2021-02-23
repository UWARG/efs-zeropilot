/**
 * Sends commands to the path manager and receives data from telemetry.
 * Author: Anthony Bertnyk, Messed with by Gordon Fountain to use in Telemetry
 */

#ifndef TELEM_TO_PATH_MANAGER_HPP
#define TELEM_TO_PATH_MANAGER_HPP

#include "TelemPathInterface.hpp"

/**
 * Initiate communication with the path manager.
 */
void CommWithPMInit();

/**
 * Send commands to the path manager.
 * @param[in]   commands   Pointer to struct with commands.
 */
void SendCommandsForPM(TelemToPMData *commands);

/**
 * Retrieve data from telemetry.
 * @param[out]   data   Pointer to struct to recieve data.
 * @return True if data was recieved.
 */
bool GetTelemData(TelemToPMData *data);

#endif
