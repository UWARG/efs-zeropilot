#include "Interchip_A.h"
#include "imu.hpp"
#include "spi.h"

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if(hspi == &hspi1) {
        ImuTxRxCallback();
    } else {
        InterchipTxRxCallback();
    }   
}