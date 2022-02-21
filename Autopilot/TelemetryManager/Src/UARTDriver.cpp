#include "UARTDriver.hpp"

uint8_t START_BYTE = 0x24;

//Need to assign UART pins
// UART_HandleTypeDef huart2;

void sendFOJI(struct foji msg_to_jetson){
	//Get msg_to_jetson from telemetry manager
	// struct foji msg_to_jetson;
	HAL_UART_Transmit(&huart2,  &START_BYTE, sizeof(START_BYTE), HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, (uint8_t*)&msg_to_jetson, sizeof(struct foji), HAL_MAX_DELAY);

}

//Creates byte array
uint8_t buf[sizeof(struct fijo)];

//Creates the byte used to confirm if the received bytes are a valid message
uint8_t check;


bool obtainFIJO(){

	//Receives a byte to check if its the same as START_BYTE
	HAL_UART_Receive_IT(&huart2, &check, 1);

    return check == START_BYTE;

}

struct fijo decodeFIJO(){
    struct fijo msg_from_jetson;
    if(check == START_BYTE){

		//Convert the byte array into an fijo struct
		struct fijo *byteToStruct = (struct fijo*)buf;
		msg_from_jetson.takeoffCommand = byteToStruct->takeoffCommand;
		msg_from_jetson.qrScanFlag = byteToStruct->qrScanFlag;
		msg_from_jetson.detectFlag = byteToStruct->detectFlag;
		msg_from_jetson.lattitude = byteToStruct->lattitude;
		msg_from_jetson.longtitude = byteToStruct->longtitude;

		//Resets the check byte
		check = 0;
	}

	//Send msg_from_jetson into telemetry manager
    return msg_from_jetson;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	//If check is the correct byte, the following bytes can be stored in the buffer array to then be parsed into an fijo struct
	if(check == START_BYTE){
		HAL_UART_Receive(&huart2, buf, sizeof(buf), HAL_MAX_DELAY);
	}
}