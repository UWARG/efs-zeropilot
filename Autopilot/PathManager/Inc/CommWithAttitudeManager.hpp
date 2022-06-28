/**
 * Communicates with the attitude manager to send commands and receive useful data.
 * All functions here should be called by the path manager.
 * Author: Anthony Bertnyk
 */

#ifndef COMM_WITH_ATTITUDE_MANAGER_HPP
#define COMM_WITH_ATTITUDE_MANAGER_HPP

#include "AttitudePathInterface.hpp"

/**
 * Initiate sending of data to the attitude manager. Called once by path manager.
 */
void CommFromPMToAMInit();

#if IS_FIXED_WING

/**
 * Send commands to the attitude manager. Called by path manager. CommFromPMToAMInit must be called once before this function can be called.
 * @param[in]   commands   Pointer to struct with commands.
 */
void SendFromPMToAM(CommandsForAM *commands);

/**
 * Retrieve new attitude/airspeed data from attitude manager if available. Called by path manager at any time.
 * @param[out]   data   Pointer to struct to recieve attitude/airspeed data.
 * @return True if data was recieved.
 */
bool GetFromAMToPM(AttitudeData *data);

#else 

void SendFromPMToAM(CommandsForAM *commands); 

#endif

#endif
