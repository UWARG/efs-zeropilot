#include "xbee.hpp"

XBEE* XBEE::xbeeInstance = nullptr;

XBEE* XBEE::getInstance() {
    if (!xbeeInstance) {
        xbeeInstance = new XBEE();
    }

    return xbeeInstance;
}

XBEE::XBEE() : dataIsNew {false}, data {} {}

void XBEE::GetResult(Telemetry_PIGO_t &Data) {

	if (dataIsNew) {
		Data.numWaypoints = data.numWaypoints;

		Data.waypointModifyFlightPathCommand = data.waypointModifyFlightPathCommand;
		Data.initializingHomeBase = data.initializingHomeBase;
		Data.waypointNextDirectionsCommand = data.waypointNextDirectionsCommand;
		Data.holdingAltitude = data.holdingAltitude;
		Data.holdingTurnRadius = data.holdingTurnRadius;
		Data.holdingTurnDirection = data.holdingTurnDirection;

		Data.nextId = data.nextId;
		Data.prevId = data.prevId;
		Data.modifyId = data.modifyId;

		Data.waypoints = data.waypoints;
		Data.homebase = data.homebase;

		Data.beginLanding = data.beginLanding;
		Data.beginTakeoff = data.beginTakeoff;
		Data.stoppingDirectionHeading = data.stoppingDirectionHeading;
		Data.stoppingLongitude = data.stoppingLongitude;
		Data.stoppingLatitude = data.stoppingLatitude;
		Data.stoppingAltitude = data.stoppingAltitude;
		Data.takeoffDirectionHeading = data.takeoffDirectionHeading;

		Data.gimbalPitch = data.gimbalPitch;
		Data.gimbalYaw = data.gimbalYaw;

		Data.groundCommandsHeading = data.groundCommandsHeading;
		Data.latestDistance = data.latestDistance;

		dataIsNew = false;
		data.waypoints.clear(); // Clears waypoints so we don't duplicate them
	}
}

_AirsideMavlinkDecodingEncoding XBEE::SendResult(Telemetry_POGI_t &Data) {

	mavlink_encoding_status_t status = MAVLINK_ENCODING_OKAY;
	mavlink_message_t encodedData;
	memset(&encodedData, 0x00, sizeof(mavlink_message_t));

	/*
	 	Create structs to be sent out 
	*/

	// POGI_Timestamp_t
    POGI_Timestamp_t timeStampCommand = { *reinterpret_cast<uint32_t*>(&Data.timeStamp) };

    // four_bytes_int_cmd_t
    four_bytes_int_cmd_t airspeedCommand = { *reinterpret_cast<int32_t*>(&Data.curAirspeed) }; // Odd
    four_bytes_int_cmd_t waypointIdCommand = { *reinterpret_cast<int32_t*>(&Data.currentWaypointId) };
    four_bytes_int_cmd_t waypointIndexCommand = { *reinterpret_cast<int32_t*>(&Data.currentWaypointIndex) };

    // single_bool_cmd_t
    single_bool_cmd_t isLandedCommand = { Data.isLanded };
    single_bool_cmd_t isHomebaseInitializedCommand = { Data.homeBaseInit };

    // POGI_Euler_Angle_t
    POGI_Euler_Angle_t planeAngleCommand = { 
        *reinterpret_cast<int32_t*>(&Data.yaw), // Convert float memory to 32 bit integer
		*reinterpret_cast<int32_t*>(&Data.pitch),
		*reinterpret_cast<int32_t*>(&Data.roll)
    }; 

    POGI_Euler_Angle_t cameraAngleCommand = { 
        *reinterpret_cast<int32_t*>(&Data.camYaw), // Convert float memory to 32 bit integer
		*reinterpret_cast<int32_t*>(&Data.camPitch),
		*reinterpret_cast<int32_t*>(&Data.camRoll)
    };

    // POGI_GPS_t
    POGI_GPS_t gpsCoordinatesCommand = { 
        *reinterpret_cast<int32_t*>(&Data.gpsLatitude), 
        *reinterpret_cast<int32_t*>(&Data.gpsLongitude), 
        *reinterpret_cast<int32_t*>(&Data.curAltitude) 
    };

    // one_byte_uint_cmd_t
    one_byte_uint_cmd_t errorCodeCommand = { *reinterpret_cast<uint8_t*>(&Data.errorCode) };
    one_byte_uint_cmd_t flightPathErrorCodeCommand = { *reinterpret_cast<uint8_t*>(&Data.editingFlightPathErrorCode) };
    one_byte_uint_cmd_t pathFollowErrorCodeCommand = { *reinterpret_cast<uint8_t*>(&Data.flightPathFollowingErrorCode) };

	/* 
		Encode and send data (Different datatypes separated by two spaces and in the same order as the structs were initialized)
	*/

	status = Mavlink_airside_encoder(MESSAGE_ID_TIMESTAMP, &encodedData, static_cast<const uint8_t*>(static_cast<void*>(&timeStampCommand)));
    if (status != MAVLINK_ENCODING_OKAY) {
        return MAVLINK_DECODING_ENCODING_FAILED;
    }
	Send_GS_Data(static_cast<uint8_t*>(static_cast<void*>(&encodedData)));
	memset(&encodedData, 0x00, sizeof(mavlink_message_t));

    
    status = Mavlink_airside_encoder(MESSAGE_ID_AIR_SPEED, &encodedData, static_cast<const uint8_t*>(static_cast<void*>(&airspeedCommand)));
    if (status != MAVLINK_ENCODING_OKAY) {
        return MAVLINK_DECODING_ENCODING_FAILED;
    }
	Send_GS_Data(static_cast<uint8_t*>(static_cast<void*>(&encodedData)));
	memset(&encodedData, 0x00, sizeof(mavlink_message_t));

    status = Mavlink_airside_encoder(MESSAGE_ID_CURRENT_WAYPOINT_ID, &encodedData, static_cast<const uint8_t*>(static_cast<void*>(&waypointIdCommand)));
	if (status != MAVLINK_ENCODING_OKAY) {
        return MAVLINK_DECODING_ENCODING_FAILED;
    }
	Send_GS_Data(static_cast<uint8_t*>(static_cast<void*>(&encodedData)));
	memset(&encodedData, 0x00, sizeof(mavlink_message_t));

    status = Mavlink_airside_encoder(MESSAGE_ID_CURRENT_WAYPOINT_INDEX, &encodedData, static_cast<const uint8_t*>(static_cast<void*>(&waypointIndexCommand)));
	if (status != MAVLINK_ENCODING_OKAY) {
        return MAVLINK_DECODING_ENCODING_FAILED;
    }
	Send_GS_Data(static_cast<uint8_t*>(static_cast<void*>(&encodedData)));
	memset(&encodedData, 0x00, sizeof(mavlink_message_t));


    status = Mavlink_airside_encoder(MESSAGE_ID_IS_LANDED, &encodedData, static_cast<const uint8_t*>(static_cast<void*>(&isLandedCommand)));
	if (status != MAVLINK_ENCODING_OKAY) {
        return MAVLINK_DECODING_ENCODING_FAILED;
    }
	Send_GS_Data(static_cast<uint8_t*>(static_cast<void*>(&encodedData)));
	memset(&encodedData, 0x00, sizeof(mavlink_message_t));

    status = Mavlink_airside_encoder(MESSAGE_ID_HOMEBASE_INITIALIZED, &encodedData, static_cast<const uint8_t*>(static_cast<void*>(&isHomebaseInitializedCommand)));
	if (status != MAVLINK_ENCODING_OKAY) {
        return MAVLINK_DECODING_ENCODING_FAILED;
    }
	Send_GS_Data(static_cast<uint8_t*>(static_cast<void*>(&encodedData)));
	memset(&encodedData, 0x00, sizeof(mavlink_message_t));


    status = Mavlink_airside_encoder(MESSAGE_ID_EULER_ANGLE_PLANE, &encodedData, static_cast<const uint8_t*>(static_cast<void*>(&planeAngleCommand)));
	if (status != MAVLINK_ENCODING_OKAY) {
        return MAVLINK_DECODING_ENCODING_FAILED;
    }
	Send_GS_Data(static_cast<uint8_t*>(static_cast<void*>(&encodedData)));
	memset(&encodedData, 0x00, sizeof(mavlink_message_t));

    status = Mavlink_airside_encoder(MESSAGE_ID_EULER_ANGLE_CAM, &encodedData, static_cast<const uint8_t*>(static_cast<void*>(&cameraAngleCommand)));
	if (status != MAVLINK_ENCODING_OKAY) {
        return MAVLINK_DECODING_ENCODING_FAILED;
    }
	Send_GS_Data(static_cast<uint8_t*>(static_cast<void*>(&encodedData)));
	memset(&encodedData, 0x00, sizeof(mavlink_message_t));


    status = Mavlink_airside_encoder(MESSAGE_ID_GPS, &encodedData, static_cast<const uint8_t*>(static_cast<void*>(&gpsCoordinatesCommand)));
	if (status != MAVLINK_ENCODING_OKAY) {
        return MAVLINK_DECODING_ENCODING_FAILED;
    }
	Send_GS_Data(static_cast<uint8_t*>(static_cast<void*>(&encodedData)));
	memset(&encodedData, 0x00, sizeof(mavlink_message_t));


    status = Mavlink_airside_encoder(MESSAGE_ID_ERROR_CODE, &encodedData, static_cast<const uint8_t*>(static_cast<void*>(&errorCodeCommand)));
	if (status != MAVLINK_ENCODING_OKAY) {
        return MAVLINK_DECODING_ENCODING_FAILED;
    }
	Send_GS_Data(static_cast<uint8_t*>(static_cast<void*>(&encodedData)));
	memset(&encodedData, 0x00, sizeof(mavlink_message_t));

    status = Mavlink_airside_encoder(MESSAGE_ID_EDITING_FLIGHT_PATH_ERROR_CODE, &encodedData, static_cast<const uint8_t*>(static_cast<void*>(&flightPathErrorCodeCommand)));
	if (status != MAVLINK_ENCODING_OKAY) {
        return MAVLINK_DECODING_ENCODING_FAILED;
    }
	Send_GS_Data(static_cast<uint8_t*>(static_cast<void*>(&encodedData)));
	memset(&encodedData, 0x00, sizeof(mavlink_message_t));

    status = Mavlink_airside_encoder(MESSAGE_ID_FLIGHT_PATH_FOLLOWING_ERROR_CODE, &encodedData, static_cast<const uint8_t*>(static_cast<void*>(&pathFollowErrorCodeCommand)));
	if (status != MAVLINK_ENCODING_OKAY) {
        return MAVLINK_DECODING_ENCODING_FAILED;
    }
	Send_GS_Data(static_cast<uint8_t*>(static_cast<void*>(&encodedData)));
	memset(&encodedData, 0x00, sizeof(mavlink_message_t));

    return MAVLINK_DECODING_ENCODING_OK;
}

_AirsideMavlinkDecodingEncoding XBEE::decodeMavlinkPacket(uint8_t* toDecode) {
	_AirsideMavlinkDecodingEncoding toReturn = MAVLINK_DECODING_ENCODING_OK;

	mavlink_decoding_status_t decoderStatus = MAVLINK_DECODING_INCOMPLETE; // We are not done decoding

	uint8_t decodedMessageBuffer[256]; // We only process one piece of info at a time. 256 used as array size since max datatype is 32 bytes 

	PIGO_Message_IDs_e messageType = MESSAGE_ID_NONE; // This is going to be set by the decoder function called in the for loop. We are only initializing to prevent undefined behaviour

	bool done = false;
	// Go through raw data and process it. When we have reached the end of a packet, we return MAVLINK_DECODING_OKAY and exit the loop
	for (int i = 0; i < 500 && !done; i++) {
		if (decoderStatus != MAVLINK_DECODING_OKAY) {
			decoderStatus = Mavlink_airside_decoder(&messageType, toDecode[i], decodedMessageBuffer);
		} else {
			done = true;
		}
	}

	if (decoderStatus == MAVLINK_DECODING_OKAY) {
		// Set our return struct based on the type of message we decoded
		switch (messageType) {
			// PIGO_GPS_LANDING_SPOT_t
			case MESSAGE_ID_GPS_LANDING_SPOT:
			{
				PIGO_GPS_LANDING_SPOT_t landingSpotDecoded;
				memcpy(&landingSpotDecoded, &decodedMessageBuffer, sizeof(PIGO_GPS_LANDING_SPOT_t));

				data.stoppingLatitude = *reinterpret_cast<float*>(&landingSpotDecoded.latitude);
				data.stoppingLongitude = *reinterpret_cast<float*>(&landingSpotDecoded.longitude);
				data.stoppingAltitude = *reinterpret_cast<float*>(&landingSpotDecoded.altitude);
				data.stoppingDirectionHeading = *reinterpret_cast<float*>(&landingSpotDecoded.landingDirection);
			}
			break;

			// PIGO_GROUND_COMMAND_t
			case MESSAGE_ID_GROUND_CMD:
			{
				PIGO_GROUND_COMMAND_t groundCommandDecoded;
				memcpy(&groundCommandDecoded, &decodedMessageBuffer, sizeof(PIGO_GROUND_COMMAND_t));

				data.groundCommandsHeading = *reinterpret_cast<float*>(&groundCommandDecoded.heading);
				data.latestDistance = *reinterpret_cast<float*>(&groundCommandDecoded.latestDistance);
			}
			break;
			
			// PIGO_GIMBAL_t
			case MESSAGE_ID_GIMBAL_CMD:
			{
				PIGO_GIMBAL_t gimbalCommandDecoded;
				memcpy(&gimbalCommandDecoded, &decodedMessageBuffer, sizeof(PIGO_GIMBAL_t));

				data.gimbalPitch = *reinterpret_cast<float*>(&gimbalCommandDecoded.pitch);
				data.gimbalYaw = *reinterpret_cast<float*>(&gimbalCommandDecoded.yaw);
			}
			break;
			
			// single_bool_cmd_t
			case MESSAGE_ID_BEGIN_LANDING:
			{
				single_bool_cmd_t landingCommandDecoded;
				memcpy(&landingCommandDecoded, &decodedMessageBuffer, sizeof(single_bool_cmd_t));

				data.beginLanding = *reinterpret_cast<bool*>(&landingCommandDecoded.cmd);
			}
			break;
			
			case MESSAGE_ID_BEGIN_TAKEOFF:
			{
				single_bool_cmd_t takeoffCommandDecoded;
				memcpy(&takeoffCommandDecoded, &decodedMessageBuffer, sizeof(single_bool_cmd_t));

				data.beginTakeoff = *reinterpret_cast<bool*>(&takeoffCommandDecoded.cmd);
			}
			break;

			case MESSAGE_ID_INITIALIZING_HOMEBASE:
			{
				single_bool_cmd_t holdingAltitudeCommandDecoded;
				memcpy(&holdingAltitudeCommandDecoded, &decodedMessageBuffer, sizeof(single_bool_cmd_t));

				data.holdingAltitude = *reinterpret_cast<bool*>(&holdingAltitudeCommandDecoded.cmd);
			}
			break;
			
			// four_bytes_int_cmd_t
			case MESSAGE_ID_NUM_WAYPOINTS:
			{
				four_bytes_int_cmd_t numWaypointsCommandDecoded;
				memcpy(&numWaypointsCommandDecoded, &decodedMessageBuffer, sizeof(four_bytes_int_cmd_t));

				data.numWaypoints = *reinterpret_cast<int*>(&numWaypointsCommandDecoded.cmd);
			}
			break;

			case MESSAGE_ID_HOLDING_ALTITUDE:
			{
				four_bytes_int_cmd_t holdingRadiusCommandDecoded;
				memcpy(&holdingRadiusCommandDecoded, &decodedMessageBuffer, sizeof(four_bytes_int_cmd_t));

				data.holdingTurnRadius = *reinterpret_cast<int*>(&holdingRadiusCommandDecoded.cmd);
			}
			break;
			
			case MESSAGE_ID_HOLDING_TURN_RADIUS:
			{
				four_bytes_int_cmd_t holdingRadiusCommandDecoded;
				memcpy(&holdingRadiusCommandDecoded, &decodedMessageBuffer, sizeof(four_bytes_int_cmd_t));

				data.holdingTurnRadius = *reinterpret_cast<int*>(&holdingRadiusCommandDecoded.cmd);
			}
			break;

			case MESSAGE_ID_PATH_MODIFY_NEXT_ID:
			{
				single_bool_cmd_t nextIdCommandDecoded;
				memcpy(&nextIdCommandDecoded, &decodedMessageBuffer, sizeof(four_bytes_int_cmd_t));

				data.nextId = *reinterpret_cast<int*>(&nextIdCommandDecoded.cmd);
			}
			break;
			
			case MESSAGE_ID_PATH_MODIFY_PREV_ID:
			{
				single_bool_cmd_t prevIdCommandDecoded;
				memcpy(&prevIdCommandDecoded, &decodedMessageBuffer, sizeof(four_bytes_int_cmd_t));

				data.prevId = *reinterpret_cast<int*>(&prevIdCommandDecoded.cmd);
			}
			break;
			
			case MESSAGE_ID_PATH_MODIFY_ID:
			{
				single_bool_cmd_t modifyIdCommandDecoded;
				memcpy(&modifyIdCommandDecoded, &decodedMessageBuffer, sizeof(four_bytes_int_cmd_t));

				data.modifyId = *reinterpret_cast<int*>(&modifyIdCommandDecoded.cmd);
			}
			break;

			// one_byte_uint_cmd_t
			case MESSAGE_ID_WAYPOINT_MODIFY_PATH_CMD:
			{
				one_byte_uint_cmd_t modifyPathCommandDecoded;
				memcpy(&modifyPathCommandDecoded, &decodedMessageBuffer, sizeof(one_byte_uint_cmd_t));

				data.waypointModifyFlightPathCommand = (_ModifyFlightPathCommand) *reinterpret_cast<uint8_t*>(&modifyPathCommandDecoded.cmd);
			}
			break;
			
			case MESSAGE_ID_WAYPOINT_NEXT_DIRECTIONS_CMD:
			{
				one_byte_uint_cmd_t nextDirectionsCommandDecoded;
				memcpy(&nextDirectionsCommandDecoded, &decodedMessageBuffer, sizeof(one_byte_uint_cmd_t));

				data.waypointNextDirectionsCommand = (_GetNextDirectionsCommand) *reinterpret_cast<uint8_t*>(&nextDirectionsCommandDecoded.cmd);
			}
			break;
			
			case MESSAGE_ID_HOLDING_TURN_DIRECTION:
			{
				one_byte_uint_cmd_t holdingTurnDirectionCommandDecoded;
				memcpy(&holdingTurnDirectionCommandDecoded, &decodedMessageBuffer, sizeof(one_byte_uint_cmd_t));

				data.holdingTurnDirection = *reinterpret_cast<uint8_t*>(&holdingTurnDirectionCommandDecoded.cmd);
			}
			break;
			
			// PIGO_WAYPOINTS_t
			case MESSAGE_ID_WAYPOINTS:
			{
				PIGO_WAYPOINTS_t waypointCommandsDecoded;
				memcpy(&waypointCommandsDecoded, &decodedMessageBuffer, sizeof(PIGO_WAYPOINTS_t));

				Telemetry_Waypoint_Data_t waypoint {};
				
				waypoint.latitude = *reinterpret_cast<float*>(&waypointCommandsDecoded.latitude);
				waypoint.longitude = *reinterpret_cast<float*>(&waypointCommandsDecoded.longitude);
				waypoint.altitude = *reinterpret_cast<int*>(&waypointCommandsDecoded.altitude);
				waypoint.turnRadius = *reinterpret_cast<float*>(&waypointCommandsDecoded.turnRadius);
				waypoint.waypointType = *reinterpret_cast<uint8_t*>(&waypointCommandsDecoded.waypointType);

				data.waypoints.push_back(waypoint);
			}
			break;
			
			case MESSAGE_ID_HOMEBASE:
			{
				PIGO_WAYPOINTS_t homebaseCommandsDecoded;
				memcpy(&homebaseCommandsDecoded, &decodedMessageBuffer, sizeof(PIGO_WAYPOINTS_t));

				data.homebase.latitude = *reinterpret_cast<float*>(&homebaseCommandsDecoded.latitude);
				data.homebase.longitude = *reinterpret_cast<float*>(&homebaseCommandsDecoded.longitude);
				data.homebase.altitude = *reinterpret_cast<int*>(&homebaseCommandsDecoded.altitude);
				data.homebase.turnRadius = *reinterpret_cast<float*>(&homebaseCommandsDecoded.turnRadius);
				data.homebase.waypointType = *reinterpret_cast<uint8_t*>(&homebaseCommandsDecoded.waypointType);
			}
			break;
			
			default: 
				break;
		}
	} else { // Will not tolerate errors
		toReturn = MAVLINK_DECODING_ENCODING_FAILED;
	}

	return toReturn;
}

void XBEE::set_usart_dat_new(bool isNew) {
	dataIsNew = isNew;
}

void XBEE::Receive_GS_Data() {
	if(dataIsNew) {
		decodeMavlinkPacket(uint8_t* byte_collection_buffer);
	}
}

void XBEE::Send_GS_Data(uint8_t* toSend) {
	uint8_t magic = toSend[0];
	uint8_t len = toSend[1];
	uint_t sendArray[(uint8_t)11 + len];
	sendArray[0] = magic;
	sendArray[1] = len;
	for(unsigned int i = 2; i < 11 + len; i++) {
		sendArray[i] = toSend[i];
	}
	HAL_USART_Transmit_DMA(&husart2, (uint8_t*)sendArray, 11 + len);
}

uint8_t* XBEE::get_byte_collection_buffer() {
	return byte_collection_buffer;
}

void HAL_USART_RxCpltCallback(USART_HandleTypeDef *huart) {
	XBEE * airXbee = XBEE::GetInstance();

	XBEE->set_usart_dat_new(true);

	HAL_USART_Receive_DMA(&huart2, airXbee->get_byte_collection_buffer(), GPS_UART_BUFFER_SIZE);
}



