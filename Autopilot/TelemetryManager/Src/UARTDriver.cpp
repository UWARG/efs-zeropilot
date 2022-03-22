#include "UARTDriver.hpp"

const uint8_t START_BYTE = 0x24;
const uint8_t MAX_ARRAY_SIZE = 10;


void sendFOJI(struct foji msg_to_jetson){
	//Get msg_to_jetson from telemetry manager
	HAL_UART_Transmit(&huart2,  (uint8_t*)&START_BYTE, sizeof(START_BYTE), HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, (uint8_t*)&msg_to_jetson, sizeof(struct foji), HAL_MAX_DELAY);

}


//Creates byte queue
uint8_t buf[MAX_ARRAY_SIZE][sizeof(struct fijo)];

//The 2D array will work as a circular queue which means the head will keep track of where we need to dequeue, and the tail will keep track of where we need to enqueue
static uint8_t head = 0;
static uint8_t tail = 0;
static bool isEmpty = true;

//Update state of ring buffer to acknowledge that a new message has been appended
void enqueuebufNode() {
    //Dropping first appended element in case of overflow
    if(tail == head && !isEmpty){
        head++;
        if(head == MAX_ARRAY_SIZE){
            head = 0;
        }
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
    if(head == MAX_ARRAY_SIZE){
        head = 0;
    }
    isEmpty = (head == tail);
}

//Next byte to be recieved
static uint8_t byteIn = 0;

//Start listening for UART interrupts
void startInterrupt(){
    HAL_UART_Receive_IT(&huart2, &byteIn, 1);
}

//Checks if there is anything to send to PM
bool doesFIJODataExist(){
    return !isEmpty;
}

//Returns an FIJO struct to send to PM
struct fijo decodeFIJO(){
    struct fijo msg_from_jetson;
    if(!isEmpty){
		//Fills a temp buf array to cast to a struct
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
		byteIn = 0;
	}

	//Send msg_from_jetson into telemetry manager to then send to PM
    return msg_from_jetson;
}

void FW_CV_HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	static uint8_t rawIndex = 0;

	//Recieve byte directly into ring buffer, overwriting previous message if it was incomplete
	if(byteIn == START_BYTE){
		rawIndex = 0;
	} else {
		buf[tail][rawIndex] = byteIn;
		rawIndex++;
	}
	
	//Indicate that a complete message has been stored in the ring buffer
	if(rawIndex == sizeof(struct fijo)) {
		enqueuebufNode();
	}

	//Listen for next byte
	HAL_UART_Receive_IT(&huart2, &byteIn, 1);
}
