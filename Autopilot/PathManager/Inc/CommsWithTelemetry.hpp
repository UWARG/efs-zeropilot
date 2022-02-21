/**
 * Send useful data to telemetry from path manager.
 * Author: Anthony Bertnyk, Messed with by Gordon Fountain <-- Blame this guy if it breaks
 */

#ifndef COMMS_WITH_TELEMETRY_HPP
#define COMMS_WITH_TELEMETRY_HPP

#include "TelemPathInterface.hpp"

/**
 * Initiate communication with telemetry.
 */
void CommWithTelemInit();

/**
 * Send path manager data to telemetry.
 * @param[in]   data   Pointer to struct with data.
 */
void SendPathData(POGI *data);

/**
 * Retrieve commands from ground/CV.
 * @param[out]   commands   Pointer to struct to recieve commands.
 * @return True if commands were recieved.
 */
bool GetTelemetryCommands(Telemetry_PIGO_t *commands);

#endif
