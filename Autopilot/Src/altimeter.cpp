#include "altimeter.hpp"

#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_i2c.h"

#include <algorithm>

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

#define I2C_BUS 1
#define LEFT_SHIFTED_ALTIMETER_SLAVE_ADDRESS 0xC0 // actual slave address is 0x60. But for whatever reason, the cubeMX drivers don't left shift the address so I have to do that myself
#define ALT_CTRL_REG1 0x26
#define ALT_OUT_P_MSB_REG 0x01
#define STANDBY_MODE_8_OS 0x98
#define ACTIVE_MODE_8_OS 0x99

#define MEM_SIZE_1_B 1

/***********************************************************************************************************************
 * Static variables
 **********************************************************************************************************************/

static bool dataIsNew;

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
    HAL_I2C_Mem_Read(&hi2c4, LEFT_SHIFTED_ALTIMETER_SLAVE_ADDRESS, ALT_OUT_P_MSB_REG, MEM_SIZE_1_B, rawAltimeter.byte, 1, 10);
}

void MPL3115A2::GetResult(AltimeterData_t &Data)
{
    static int j = 0;

    std::reverse(rawAltimeter.byte, rawAltimeter.byte + 3);

    float tmp = static_cast<float> (rawAltimeter.Q16point4 >> 4);  // the 4 lowest bits of the data register are reserved
    tmp /= 16.0f;   // conversion from Q16.4 fixed point to float
    Data.altitude = tmp - altimeterCalibrationFinal;

    Data.isDataNew = dataIsNew;
    dataIsNew = false;
}

/***********************************************************************************************************************
 * Private methods
 **********************************************************************************************************************/

MPL3115A2::MPL3115A2()
{
    HAL_I2C_MspInit(&hi2c4);

    ConfigAltimeter();

    //Calibrate();

    altimeterCalibrationFinal = 0;

    dataIsNew = false;

}


void MPL3115A2::Calibrate(void)
{
    constexpr int nSamplesForReliableAverage = 100;

    AltimeterData_t tempAltimeterData;
    
    float altimeterCalibration = 0;

    for (int i = 0; i < nSamplesForReliableAverage; i++)
    {
        this->Begin_Measuring();

        HAL_Delay(7);

        this->GetResult(tempAltimeterData);

        altimeterCalibration += tempAltimeterData.altitude;

    }

    altimeterCalibration /= static_cast<float> (nSamplesForReliableAverage);

    altimeterCalibrationFinal = altimeterCalibration;

}

void MPL3115A2::ConfigAltimeter(void)
{
    HAL_NVIC_DisableIRQ(I2C4_EV_IRQn);

    uint8_t txByte;

    // Puts the altimeter in standby mode. This just allows us to access all its registers.
    txByte = STANDBY_MODE_8_OS;
    HAL_I2C_Mem_Write(&hi2c4, LEFT_SHIFTED_ALTIMETER_SLAVE_ADDRESS, ALT_CTRL_REG1, MEM_SIZE_1_B, &txByte, 1, 1000);

    HAL_Delay(20);
    
    // In active mode, the altimeter is actually measuring. * OS corresponds to a measurement refresh every 6 ms. (We couldn't do this if the altimeter was not in standby mode before).
    txByte = ACTIVE_MODE_8_OS;
    HAL_I2C_Mem_Write(&hi2c4, LEFT_SHIFTED_ALTIMETER_SLAVE_ADDRESS, ALT_CTRL_REG1, MEM_SIZE_1_B, &txByte, 1, 1000);

    HAL_Delay(20);

    HAL_NVIC_EnableIRQ(I2C4_EV_IRQn);

}

/***********************************************************************************************************************
 * Interrupt Callback
 **********************************************************************************************************************/

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    dataIsNew = true;
}
