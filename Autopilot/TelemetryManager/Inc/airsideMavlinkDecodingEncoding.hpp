#ifndef MAVLINK_DECODING_ENCODING_HPP
#define MAVLINK_DECODING_ENCODING_HPP

#include "TelemPathInterface.hpp" // To have access to PIGO and POGI structs
#include "Mavlink2_lib/common/mavlink.h"

#include <vector> // Use vectors because knowing the length of the arrays are important
#include <stdint.h>

// Error code for the module
enum _AirsideMavlinkDecodingEncoding { MAVLINK_DECODING_ENCODING_OK = 0, MAVLINK_DECODING_ENCODING_FAILED };

/**
 * Decodes data received from ground
 * 
 * @param rawData -> byte array containing all info sent via xbee 
 * @param decodedData -> result of rawData after decoding by Airside Mavlink algos. 
 * 
 * @return error code
 */ 
_AirsideMavlinkDecodingEncoding decode(std::vector<uint8_t> rawData, Telemetry_PIGO_t* decodedData);

/**
 * Encodes data that will be sent to ground
 * 
 * @param rawData -> data collected from path manager and other modules in autopilot
 * @param decodedData -> encoded data (byte array) that will be sent to ground via xbees
 * 
 * @return error code
 */ 
_AirsideMavlinkDecodingEncoding encode(Telemetry_POGI_t* rawData, mavlink_message_t* encodedData);

#endif


