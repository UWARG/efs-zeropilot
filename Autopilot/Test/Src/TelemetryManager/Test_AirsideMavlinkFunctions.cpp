/*
* Author: Dhruv Rawat
*/

#include <gtest/gtest.h>
#include <fff.h>

#include "xbee.hpp"
#include "Airside_Functions.hpp"
#include "Mavlink2_lib/common/mavlink.h"


using namespace std; 
using ::testing::Test;


/*
    Cases we want to test:

        - Decoding
        - Encoding
*/

enum _ResultStatus { ENCODE_DECODE_RESULT_OK = 0, ENCODE_DECODE_RESULT_BAD };

#include <iostream>
using namespace std;

_ResultStatus comparePIGOStructs(Telemetry_PIGO_t* decoded, Telemetry_PIGO_t* answer) {
	return ENCODE_DECODE_RESULT_OK;
}

TEST(AirsideMavlinkDecodingEncoding, DecodingModifyFlightPathCommand) {
    uint8_t rawData[39] = {
        0xfd,
        0x1b,
        0x00,
        0x00,
        0x00,
        0x01,
        0x00,
        0x21,
        0x00,
        0x00,
        0x07,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x03,
        0x2a,
        0x09
    };

    XBEE* x1 = XBEE::getInstance();

    Telemetry_PIGO_t decodedData {};

    _AirsideMavlinkDecodingEncoding decodingResult = x1->decodePacketBypass(rawData);
    decodedData = x1->getData();
    
    EXPECT_EQ(decodingResult, MAVLINK_DECODING_ENCODING_OK);
    EXPECT_EQ(decodedData.waypointModifyFlightPathCommand, INSERT);
}

TEST(AirsideMavlinkDecodingEncoding, Encoding) {

	Telemetry_POGI_t rawData {};

	rawData.errorCode = 1;
	rawData.timeStamp = 100;
	rawData.gpsLatitude = 1.03928;
	rawData.gpsLongitude = 2.03928;
	rawData.curAltitude = 20;
	rawData.curAirspeed = 100;
	rawData.roll = 90;
	rawData.pitch = 1;
	rawData.yaw = 20;
	rawData.camRoll = 2.1;
	rawData.camPitch = 2.2;
	rawData.camYaw = 2.3;
	rawData.isLanded = false;
	rawData.editingFlightPathErrorCode = 1;
	rawData.flightPathFollowingErrorCode = 2;
	rawData.currentWaypointId = 69;
	rawData.currentWaypointIndex = 70;
	rawData.homeBaseInit = true;

	mavlink_message_t encodedData;
	memset(&encodedData, 0x00, sizeof(mavlink_message_t));

    XBEE* x1 = XBEE::getInstance();

	_AirsideMavlinkDecodingEncoding encodingResult = x1->SendResult(rawData);

	EXPECT_EQ(encodingResult, MAVLINK_DECODING_ENCODING_OK);
}


