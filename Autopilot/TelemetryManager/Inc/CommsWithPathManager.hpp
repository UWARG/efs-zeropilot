/**
 * Sends commands to the path manager and receives data from telemetry.
 * Author: Anthony Bertnyk, Messed with by Gordon Fountain to use in Telemetry
 */

#ifndef COMMS_WITH_PATH_MANAGER_HPP
#define COMMS_WITH_PATH_MANAGER_HPP

#include "TelemPathInterface.hpp"

/**
 * Initiate communication with the path manager.
 */
void TelemCommWithPMInit();

/**
 * Send commands to the path manager.
 * @param[in]   commands   Pointer to struct with commands.
 */
void SendCommandsForPM(Telemetry_PIGO_t *commands);

/**
 * Retrieve data from telemetry.
 * @param[out]   data   Pointer to struct to recieve data.
 * @return True if data was recieved.
 */
bool GetTelemData(Telemetry_POGI_t *data);

#endif
