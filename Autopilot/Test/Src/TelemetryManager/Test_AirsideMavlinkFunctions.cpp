/*
* Author: Dhruv Rawat
*/

#include <gtest/gtest.h>
#include <fff.h>

#include "airsideMavlinkDecodingEncoding.hpp"
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

// #include <iostream>
// using namespace std;

_ResultStatus compareByteArrays(uint8_t* encoded, uint8_t* answer, int len) {
	for (int i = 0; i < len; i++) {
		// cout << i << ": " << encoded[i] << " | " << answer[i] << endl;
		if (encoded[i] != answer[i]) {
			return ENCODE_DECODE_RESULT_BAD;
		}
	}

	return ENCODE_DECODE_RESULT_OK;
}

_ResultStatus comparePIGOStructs(Telemetry_PIGO_t* decoded, Telemetry_PIGO_t* answer) {
	return ENCODE_DECODE_RESULT_OK;
}

TEST(AirsideMavlinkDecodingEncoding, Decoding) {
    EXPECT_EQ(1, 1);
}

TEST(AirsideMavlinkDecodingEncoding, Encoding) {

	Telemetry_POGI_t rawData {};

	rawData.errorCode = 0;
	rawData.timeStamp = 100;
	rawData.gpsLatitude = 1.03928;
	rawData.gpsLongitude = 2.03928;
	rawData.curAltitude = 20;
	rawData.curAirspeed = 10;
	rawData.roll = 1.1;
	rawData.pitch = 1.2;
	rawData.yaw = 1.3;
	rawData.camRoll = 2.1;
	rawData.camPitch = 2.2;
	rawData.camYaw = 2.3;
	rawData.isLanded = true;
	rawData.editingFlightPathErrorCode = 1;
	rawData.flightPathFollowingErrorCode = 2;
	rawData.currentWaypointId = 69;
	rawData.currentWaypointIndex = 70;
	rawData.homeBaseInit = true;

	mavlink_message_t encodedData;
	memset(&encodedData, 0x00, sizeof(mavlink_message_t));

	_AirsideMavlinkDecodingEncoding encodingResult = encode(&rawData, &encodedData);

	uint8_t* encodedByteArray = static_cast<uint8_t*>(static_cast<void*>(&encodedData));

	uint8_t answer[100] = {0, 1, 3 ,4 ,5 ,6};

	_ResultStatus res = compareByteArrays(encodedByteArray, answer, 100);

	EXPECT_EQ(encodingResult, MAVLINK_DECODING_ENCODING_OK);
    // EXPECT_EQ(res, ENCODE_DECODE_RESULT_OK);
}


