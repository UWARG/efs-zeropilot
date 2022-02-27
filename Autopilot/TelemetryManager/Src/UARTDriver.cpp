#include "UARTDriver.hpp"

const uint8_t START_BYTE = 0x24;
const uint8_t MAX_ARRAY_SIZE = 10;

//Need to assign UART pins
// UART_HandleTypeDef huart2;

void sendFOJI(struct foji msg_to_jetson){
	//Get msg_to_jetson from telemetry manager
	// struct foji msg_to_jetson;
	HAL_UART_Transmit(&huart2,  (uint8_t*)&START_BYTE, sizeof(START_BYTE), HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, (uint8_t*)&msg_to_jetson, sizeof(struct foji), HAL_MAX_DELAY);

}


//Creates byte array
uint8_t buf[MAX_ARRAY_SIZE][sizeof(struct fijo)];

uint8_t head = 0; 
uint8_t tail = 0;
bool isEmpty = true;

void enqueuebufNode(uint8_t *new_msg_from_jetson) {
    //Dropping first appended element in case of overflow
    if(tail == head && !isEmpty){
        head++;
        if(head == MAX_ARRAY_SIZE){
            head = 0;
        }
    }

    //Copy new message into buf
    for(int i = 0; i < sizeof(struct fijo); i++){
		buf[tail][i] = new_msg_from_jetson[i];
	}

    tail++;

    if(tail == MAX_ARRAY_SIZE){
        tail = 0;
    }

    isEmpty = false;

}

void dequeuebufNode(uint8_t *tempBuf){
    //Copy old message into tempBuf
    for(int i = 0; i < sizeof(struct fijo); i++){
		tempBuf[i] = buf[head][i];
	}

    head++;

    isEmpty = (head == tail);
}

//Creates the byte used to confirm if the received bytes are a valid message
uint8_t check;

void startInterrupt(){
    HAL_UART_Receive_IT(&huart2, &check, 1);
}

bool doesFIJODataExist(){

    return !isEmpty;

}

struct fijo decodeFIJO(){
    struct fijo msg_from_jetson;
    if(!isEmpty){
		// decodeBuf = q.pop();
		uint8_t tempBuf[sizeof(struct fijo)];
		dequeuebufNode(tempBuf);
		//Convert the byte array into an fijo struct
		struct fijo *byteToStruct = (struct fijo*)tempBuf;
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

void FW_CV_HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	//If check is the correct byte, the following bytes can be stored in the buffer array to then be parsed into an fijo struct
	if(check == START_BYTE){
		uint8_t tempBuf[sizeof(struct fijo)];
		HAL_UART_Receive(&huart2, tempBuf, sizeof(tempBuf), HAL_MAX_DELAY);
		enqueuebufNode(tempBuf);
	}
    //Makes it so any messages sent from CV will always be appended to the queue wihtout constantly polling
    HAL_UART_Receive_IT(&huart2, &check, 1);
}