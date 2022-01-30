#include "Interchip_A.h"
#include "imu.hpp"
#include "gps.hpp"
#include "comms.hpp"
#include "usart.h"
#include "spi.h"

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if(hspi == &hspi1) {
        ImuTxRxCallback();
    } else {
        InterchipTxRxCallback();
    }   
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if(huart == &huart4) {
	    NEOM8 * neoM8N = NEOM8::GetInstance();

	    neoM8N->set_uart_data_is_new(true);

	    HAL_UART_Receive_DMA(&huart4, neoM8N->get_byte_collection_buffer(), GPS_UART_BUFFER_SIZE);
    }
    if(huart == &huart2) {
        HAL_UART_Receive_DMA(&huart4, Comms::GetInstance()->rxBuffer, 22);
    }
}