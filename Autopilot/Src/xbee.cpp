#include "xbee.hpp"

XBEE* XBEE::xbeeInstance = nullptr;

XBEE* XBEE::getInstance() {
    if (!xbeeInstance) {
        xbeeInstance = new XBEE();
    }

    return xbeeInstance;
}

XBEE::XBEE() : dataIsNew {false}, data {} {}

void  XBEE::GetResult(Telemetry_PIGO_t &Data) {

	if (dataIsNew) {
		dataIsNew = false;

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
    POGI_Timestamp_t timeStampCommand = { Data.timeStamp };

    // four_bytes_int_cmd_t
    four_bytes_int_cmd_t airspeedCommand = { Data.curAirspeed };
    four_bytes_int_cmd_t waypointIdCommand = { Data.currentWaypointId };
    four_bytes_int_cmd_t waypointIndexCommand = { Data.currentWaypointIndex };

    // single_bool_cmd_t
    single_bool_cmd_t isLandedCommand = { Data.isLanded };
    single_bool_cmd_t isHomebaseInitializedCommand = { Data.homeBaseInit };

    // POGI_Euler_Angle_t
    POGI_Euler_Angle_t planeAngleCommand = { 
        (int32_t) (*(int32_t*) &Data.yaw), // Convert float memory to 32 bit integer
        (int32_t) (*(int32_t*) &Data.pitch), 
        (int32_t) (*(int32_t*) &Data.roll) 
    }; 

    POGI_Euler_Angle_t cameraAngleCommand = { 
        (int32_t) (*(int32_t*) &Data.camYaw), 
        (int32_t) (*(int32_t*) &Data.camPitch), 
        (int32_t) (*(int32_t*) &Data.camRoll) 
    };

    // POGI_GPS_t
    POGI_GPS_t gpsCoordinatesCommand = { 
        Data.gpsLatitude, 
        Data.gpsLongitude, 
        Data.curAltitude 
    };

    // one_byte_uint_cmd_t
    one_byte_uint_cmd_t errorCodeCommand = { Data.errorCode };
    one_byte_uint_cmd_t flightPathErrorCodeCommand = { Data.editingFlightPathErrorCode };
    one_byte_uint_cmd_t pathFollowErrorCodeCommand = { Data.flightPathFollowingErrorCode };

	/* 
		Encode and send data (Different datatypes separated by two spaces and in the same order as the structs were initialized)
	*/

	status = Mavlink_airside_encoder(MESSAGE_ID_TIMESTAMP, &encodedData, static_cast<const uint8_t*>(static_cast<void*>(&timeStampCommand)));
    if (status != MAVLINK_ENCODING_OKAY) {
        return MAVLINK_DECODING_ENCODING_FAILED;
    }
	Send_GS_Data(static_cast<uint8_t*>(static_cast<void*>(&encodedData)));

    
    status = Mavlink_airside_encoder(MESSAGE_ID_AIR_SPEED, &encodedData, static_cast<const uint8_t*>(static_cast<void*>(&airspeedCommand)));
    if (status != MAVLINK_ENCODING_OKAY) {
        return MAVLINK_DECODING_ENCODING_FAILED;
    }
	Send_GS_Data(static_cast<uint8_t*>(static_cast<void*>(&encodedData)));

    status = Mavlink_airside_encoder(MESSAGE_ID_CURRENT_WAYPOINT_ID, &encodedData, static_cast<const uint8_t*>(static_cast<void*>(&waypointIdCommand)));
	if (status != MAVLINK_ENCODING_OKAY) {
        return MAVLINK_DECODING_ENCODING_FAILED;
    }
	Send_GS_Data(static_cast<uint8_t*>(static_cast<void*>(&encodedData)));

    status = Mavlink_airside_encoder(MESSAGE_ID_CURRENT_WAYPOINT_INDEX, &encodedData, static_cast<const uint8_t*>(static_cast<void*>(&waypointIndexCommand)));
	if (status != MAVLINK_ENCODING_OKAY) {
        return MAVLINK_DECODING_ENCODING_FAILED;
    }
	Send_GS_Data(static_cast<uint8_t*>(static_cast<void*>(&encodedData)));


    status = Mavlink_airside_encoder(MESSAGE_ID_IS_LANDED, &encodedData, static_cast<const uint8_t*>(static_cast<void*>(&isLandedCommand)));
	if (status != MAVLINK_ENCODING_OKAY) {
        return MAVLINK_DECODING_ENCODING_FAILED;
    }
	Send_GS_Data(static_cast<uint8_t*>(static_cast<void*>(&encodedData)));

    status = Mavlink_airside_encoder(MESSAGE_ID_HOMEBASE_INITIALIZED, &encodedData, static_cast<const uint8_t*>(static_cast<void*>(&isHomebaseInitializedCommand)));
	if (status != MAVLINK_ENCODING_OKAY) {
        return MAVLINK_DECODING_ENCODING_FAILED;
    }
	Send_GS_Data(static_cast<uint8_t*>(static_cast<void*>(&encodedData)));


    status = Mavlink_airside_encoder(MESSAGE_ID_EULER_ANGLE_PLANE, &encodedData, static_cast<const uint8_t*>(static_cast<void*>(&planeAngleCommand)));
	if (status != MAVLINK_ENCODING_OKAY) {
        return MAVLINK_DECODING_ENCODING_FAILED;
    }
	Send_GS_Data(static_cast<uint8_t*>(static_cast<void*>(&encodedData)));

    status = Mavlink_airside_encoder(MESSAGE_ID_EULER_ANGLE_CAM, &encodedData, static_cast<const uint8_t*>(static_cast<void*>(&cameraAngleCommand)));
	if (status != MAVLINK_ENCODING_OKAY) {
        return MAVLINK_DECODING_ENCODING_FAILED;
    }
	Send_GS_Data(static_cast<uint8_t*>(static_cast<void*>(&encodedData)));


    status = Mavlink_airside_encoder(MESSAGE_ID_GPS, &encodedData, static_cast<const uint8_t*>(static_cast<void*>(&gpsCoordinatesCommand)));
	if (status != MAVLINK_ENCODING_OKAY) {
        return MAVLINK_DECODING_ENCODING_FAILED;
    }
	Send_GS_Data(static_cast<uint8_t*>(static_cast<void*>(&encodedData)));


    status = Mavlink_airside_encoder(MESSAGE_ID_ERROR_CODE, &encodedData, static_cast<const uint8_t*>(static_cast<void*>(&errorCodeCommand)));
	if (status != MAVLINK_ENCODING_OKAY) {
        return MAVLINK_DECODING_ENCODING_FAILED;
    }
	Send_GS_Data(static_cast<uint8_t*>(static_cast<void*>(&encodedData)));

    status = Mavlink_airside_encoder(MESSAGE_ID_EDITING_FLIGHT_PATH_ERROR_CODE, &encodedData, static_cast<const uint8_t*>(static_cast<void*>(&flightPathErrorCodeCommand)));
	if (status != MAVLINK_ENCODING_OKAY) {
        return MAVLINK_DECODING_ENCODING_FAILED;
    }
	Send_GS_Data(static_cast<uint8_t*>(static_cast<void*>(&encodedData)));

    status = Mavlink_airside_encoder(MESSAGE_ID_FLIGHT_PATH_FOLLOWING_ERROR_CODE, &encodedData, static_cast<const uint8_t*>(static_cast<void*>(&pathFollowErrorCodeCommand)));
	if (status != MAVLINK_ENCODING_OKAY) {
        return MAVLINK_DECODING_ENCODING_FAILED;
    }
	Send_GS_Data(static_cast<uint8_t*>(static_cast<void*>(&encodedData)));

    return MAVLINK_DECODING_ENCODING_OK;
}

void XBEE::Receive_GS_Data() {
	// Call XBEE API here to get data
}

void XBEE::Send_GS_Data(uint8_t* toSend) {
}




