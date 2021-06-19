#include "altimeter.hpp"

#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_i2c.h"


/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

#define I2C_BUS 1
#define ALTIMETER_SLAVE_ADDRESS 0x60
#define ALT_CTRL_REG1 0x26
#define ALT_OUT_P_MSB_REG 0x01
#define STANDBY_MODE_8_OS 0x98
#define ACTIVE_MODE_8_OS 0x99

#define MEM_SIZE_1_B 1


/***********************************************************************************************************************
 * HAL SPI handle
 **********************************************************************************************************************/

extern I2C_HandleTypeDef hi2c4;

/***********************************************************************************************************************
 * Public methods
 ***********************************************************************:o**********************************************/

Altimeter& MPL3115A2::getInstance()
{
    static MPL3115A2 singleton;
    return singleton;
}

void MPL3115A2::Begin_Measuring(void)
{

    HAL_I2C_Mem_Read(&hi2c4, ALTIMETER_SLAVE_ADDRESS, ALT_OUT_P_MSB_REG, MEM_SIZE_1_B, &(rawAltimeter.byte[0]), 3, 1000);

}

void MPL3115A2::GetResult(AltimeterData_t &Data)
{
    float tmp = static_cast<float> (rawAltimeter.Q16point4 >> 4);  // the 4 lowest bits of the data register are reserved
    tmp /= 16.0f;   // conversion from Q16.4 fixed point to float
    Data.altitude = tmp;


#if 0
    altimeterDataReady = false;

    if (altimeterDataReady)
    {
        altimeterDataReady = false;        
    }
#endif
}

/***********************************************************************************************************************
 * Private methods
 **********************************************************************************************************************/

MPL3115A2::MPL3115A2()
{

uint8_t txByte;

txByte = STANDBY_MODE_8_OS;
HAL_I2C_Mem_Write(&hi2c4, ALTIMETER_SLAVE_ADDRESS, ALT_CTRL_REG1, MEM_SIZE_1_B, &txByte, 1, 1000);

HAL_Delay(500);

txByte = ACTIVE_MODE_8_OS;
HAL_I2C_Mem_Write(&hi2c4, ALTIMETER_SLAVE_ADDRESS, ALT_CTRL_REG1, MEM_SIZE_1_B, &txByte, 1, 1000);

#if 0
    I2C_Init(I2C_BUS);
    I2C_DisableInterrupt(I2C_BUS);
    I2C_Start(I2C_BUS);
    I2C_BlockUntilReady(I2C_BUS);
    I2C_SendSlaveAddressWrite(ALTIMETER_SLAVE_ADDRESS, I2C_BUS);
    I2C_BlockUntilReady(I2C_BUS);
    I2C_SendSlaveRegister(ALT_CTRL_REG1, I2C_BUS);
    I2C_BlockUntilReady(I2C_BUS);
    I2C_SendData(STANDBY_MODE_8_OS, I2C_BUS);
    I2C_BlockUntilReady(I2C_BUS);
    I2C_RepeatStart(I2C_BUS);
    I2C_BlockUntilReady(I2C_BUS);
    I2C_SendSlaveAddressWrite(ALTIMETER_SLAVE_ADDRESS, I2C_BUS);
    I2C_BlockUntilReady(I2C_BUS);
    I2C_SendSlaveRegister(ALT_CTRL_REG1, I2C_BUS);
    I2C_BlockUntilReady(I2C_BUS);
    I2C_SendData(ACTIVE_MODE_8_OS, I2C_BUS);
    I2C_BlockUntilReady(I2C_BUS);
    I2C_Stop(I2C_BUS);
    I2C_EnableInterrupt(I2C_BUS);
#endif
}



#if 0
ISR(TWI1_vect)  // interrupt based altimeter reading
{
    static uint8_t i;

    switch (I2C_STATUS_BUS_1)
    {
        case START_SUCCEEDED :
            I2C_SendSlaveAddressWrite(ALTIMETER_SLAVE_ADDRESS, I2C_BUS);
            break;

        case SLAVE_WRITE_SUCCEEDED :
            I2C_SendSlaveRegister(ALT_OUT_P_MSB_REG, I2C_BUS);
            break;

        case DATA_TRANSMIT_SUCCEEDED :
            I2C_RepeatStart(I2C_BUS);
            break;

        case REPEAT_START_SUCCEEDED :
            I2C_SendSlaveAddressRead(ALTIMETER_SLAVE_ADDRESS, I2C_BUS);
            break;

        case SLAVE_READ_SUCCEEDED :
            I2C_AskForAnotherByte(I2C_BUS);
            break;

        case SLAVE_SENT_NEXT_BYTE :
            rawAltimeter.byte[2-i] = I2C_Read(I2C_BUS);

            if(i == 2)
            {
                I2C_EndDataRead(I2C_BUS);
                i = 0;
            }
            else
            {
                I2C_AskForAnotherByte(I2C_BUS);
                i++;
            }
            break;

        case END_READ_SUCCEEDED :
            I2C_Stop(I2C_BUS);
            altimeterDataReady = true;
            altimeterMeasInProgress = false;
            break;

        default:
            altimeterMeasInProgress = false;
            altimeterFailure = true;
    }
}
#endif
