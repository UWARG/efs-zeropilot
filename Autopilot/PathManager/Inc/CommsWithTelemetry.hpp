/**
 * Send useful data to telemetry from path manager.
 * Author: Anthony Bertnyk, Messed with by Gordon Fountain <-- Blame this guy if it breaks
 */

#ifndef COMMS_WITH_TELEMETRY_HPP
#define COMMS_WITH_TELEMETRY_HPP

#include "TelemPathInterface.hpp"
#include "pathManager.hpp"

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
 * Retrieve commands from the path manager.
 * @param[out]   commands   Pointer to struct to recieve commands.
 * @return True if commands were recieved.
 */
#if IS_FIXED_WING 
bool GetTelemetryCommands(Telemetry_PIGO_t *commands);

#else
bool GetTelemetryCommands(fijo *commands);

#endif

#endif
