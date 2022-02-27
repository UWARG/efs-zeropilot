#include "Interchip_A.h"
#include "imu.hpp"
#include "spi.h"

#include"gps.hpp"
#include"UARTDriver.hpp"

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if(hspi == &hspi1) {
        ImuTxRxCallback();
    } else {
        InterchipTxRxCallback();
    }   
}

//This function is used to allpw both gps.cpp and UARTDriver.cpp to have a callback
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

    if(huart == &huart4){
        GPS_HAL_UART_RxCpltCallback(huart);
    }
    else if(huart == &huart2){
        FW_CV_HAL_UART_RxCpltCallback(huart);
    }
}