#include "imu.hpp"

#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_spi.h"
#include "stm32f7xx_hal_gpio.h"

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

#define BMX160_READ_BIT 0x80
#define BMX160_WRITE_BIT 0x00
#define BMX160_DUMMY_BYTE 0x00

#define CHIP_ID_REG 0x00
#define DATA_REG 0x04
#define STATUS_REG 0x1B
#define CMD_REG 0x7E
#define PMU_STATUS_REG 0x03
#define ACC_CONF_REG 0x40
#define ACC_RANGE_REG 0x41
#define GYR_CONF_REG 0x42
#define GYR_RANGE_REG 0x43
#define MAG_CONF_REG 0x44
#define MAG_IF_0_REG 0x4C
#define MAG_IF_1_REG 0x4D
#define MAG_IF_2_REG 0x4E
#define MAG_IF_3_REG 0x4F
#define MAG_REPZ_REG 0x52
#define MAG_REPXY_REG 0x51
#define MAG_MODE_REG 0x4B

#define ACC_NORMAL_MODE_CMD 0x11
#define GYRO_NORMAL_MODE_CMD 0x15
#define MAG_NORMAL_MODE_CMD 0x19

#define ACC_RANGE_8G 0x08
#define ACC_ODR_800_OSR4 0x0A   // samples are taken at 800 Hz but every 4 are averaged.
#define GYRO_RANGE_1000  0x01
#define GYRO_ODR_800_OSR4 0x0A
#define MAG_SETUP_EN 0x80
#define MAG_SETUP_DIS 0x00
#define REP_XY_REGULAR_PRESET 0x04 // TODO there is also a high accuracy preset. Not too sure what that's about.
#define REP_Z_REGULAR_PRESET 0x0E
#define MAG_IF_3_DATA_MODE 0x02
#define MAG_IF_2_DATA_MODE 0x4C
#define MAG_IF_1_DATA_MODE 0x42
#define MAG_REFRESH_50_HZ 0x07  // TODO mag should refresh at 200 Hz as well, but I cant seem to get that to work.
#define MAG_SLEEP_MODE 0x01

#define MAX_BUFF_SIZE 21

constexpr float GYRO_RANGE_1000_FACTOR = 1879.3f;  // LSB/rad/s
constexpr float ACC_RANGE_8_FACTOR = 4.09f;    // LSB/mg

/***********************************************************************************************************************
 * HAL SPI handle
 **********************************************************************************************************************/

extern SPI_HandleTypeDef hspi1;

/***********************************************************************************************************************
 * Static variables and static helper function prototypes
 **********************************************************************************************************************/

static bool dataIsNew;
static void AssertSlaveSelect(void);
static void DeassertSlaveSelect(void);

/***********************************************************************************************************************
 * Public methods
 ***********************************************************************:D**********************************************/

IMU& BMX160::getInstance()
{
    static BMX160 singleton;
    return singleton;
}

void BMX160::Begin_Measuring(void)
{
    Bmx160ReadReg(DATA_REG, rawImuData, 20);
}

void BMX160::GetResult(IMUData_t &Data)
{
    if (! dataIsNew)
    {
        Data.isDataNew = false;
        return;
    }

    int16_t *intImuDataPtr = (int16_t *) &(rawImuData[1]); // first byte is garbage. It's just what was on the line when we asked the IMU for data, which it started sending as of the second byte.

    int16_t magx = intImuDataPtr[0];
    int16_t magy = intImuDataPtr[1];
    int16_t magz = intImuDataPtr[2];
    int16_t gyrx = intImuDataPtr[4]; // missing 3 is not a mistake, there is a field here named RHall which has something to do with the magnetometer. But I can't quite figure that out yet.
    int16_t gyry = intImuDataPtr[5];
    int16_t gyrz = intImuDataPtr[6];
    int16_t accx = intImuDataPtr[7];
    int16_t accy = intImuDataPtr[8];
    int16_t accz = intImuDataPtr[9];

    Data.magx = static_cast<float> (magx);
    Data.magy = static_cast<float> (magy);
    Data.magz = static_cast<float> (magz);
    Data.accx = static_cast<float> (accx) / ACC_RANGE_8_FACTOR;
    Data.accy = static_cast<float> (accy) / ACC_RANGE_8_FACTOR;
    Data.accz = static_cast<float> (accz) / ACC_RANGE_8_FACTOR;
    Data.gyrx = static_cast<float> (gyrx) / GYRO_RANGE_1000_FACTOR;
    Data.gyry = static_cast<float> (gyry) / GYRO_RANGE_1000_FACTOR;
    Data.gyrz = static_cast<float> (gyrz) / GYRO_RANGE_1000_FACTOR;

    Data.isDataNew = true;
    Data.sensorStatus = 0;

    dataIsNew = false;
}

/***********************************************************************************************************************
 * Private methods
 **********************************************************************************************************************/

BMX160::BMX160()
{
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10, GPIO_PIN_SET);

    SetAllPowerModesToNormal();
    ConfigAcc();
    ConfigGyro();
    ConfigMag();

    dataIsNew = false;
}

void BMX160::SetAllPowerModesToNormal()
{

    Bmx160WriteReg(CMD_REG, ACC_NORMAL_MODE_CMD);
    Bmx160WriteReg(CMD_REG, GYRO_NORMAL_MODE_CMD);
    Bmx160WriteReg(CMD_REG, MAG_NORMAL_MODE_CMD);
}

void BMX160::ConfigAcc()
{

    Bmx160WriteReg(ACC_RANGE_REG, ACC_RANGE_8G);
    Bmx160WriteReg(ACC_CONF_REG, ACC_ODR_800_OSR4);
}


void BMX160::ConfigGyro()
{

    Bmx160WriteReg(GYR_RANGE_REG, GYRO_RANGE_1000);
    Bmx160WriteReg(GYR_CONF_REG, GYRO_ODR_800_OSR4);
}

void BMX160::ConfigMag()
{
    Bmx160WriteReg(MAG_IF_0_REG, MAG_SETUP_EN);


    SetMagConfig(MAG_MODE_REG, MAG_SLEEP_MODE);
    SetMagConfig(MAG_REPXY_REG, REP_XY_REGULAR_PRESET);
    SetMagConfig(MAG_REPZ_REG, REP_Z_REGULAR_PRESET);

    PrepareMagForDataMode();

    Bmx160WriteReg(MAG_CONF_REG, MAG_REFRESH_50_HZ);
    Bmx160WriteReg(MAG_IF_0_REG, MAG_SETUP_DIS);

}

void BMX160::SetMagConfig(uint8_t regAddr, uint8_t data)
{
    Bmx160WriteReg(MAG_IF_3_REG, data);
    Bmx160WriteReg(MAG_IF_2_REG, regAddr);
}

void BMX160::PrepareMagForDataMode(void)
{

    Bmx160WriteReg(MAG_IF_3_REG, MAG_IF_3_DATA_MODE);
    Bmx160WriteReg(MAG_IF_2_REG, MAG_IF_2_DATA_MODE);
    Bmx160WriteReg(MAG_IF_1_REG, MAG_IF_1_DATA_MODE);
}

void BMX160::Bmx160WriteReg(uint8_t reg, uint8_t val)
{
    uint8_t rx[2] = { 0, 0 }; // we don't care about what gets sent back to us in write mode
    uint8_t tx[2] = { static_cast<uint8_t> (reg | BMX160_WRITE_BIT), val }; // Set first bit to 0

    AssertSlaveSelect();

    HAL_SPI_TransmitReceive_IT(&hspi1, tx, rx, 2);

    HAL_Delay(20); // writes only happen in config. Allow some time for the writes to complete before moving on to the next write
}

void BMX160::Bmx160ReadReg(uint8_t const regAddr, uint8_t *pData, uint8_t len)
{
    uint8_t tx[MAX_BUFF_SIZE] = {0};

    tx[0] = static_cast<uint8_t> (regAddr | BMX160_READ_BIT);

    AssertSlaveSelect();
    
    HAL_SPI_TransmitReceive_IT(&hspi1, tx, pData, (len + 1));
}

/***********************************************************************************************************************
 * Interrupt Callbacks and static helpers functions
 **********************************************************************************************************************/

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
    (void) hspi; // tells the compiler "hey! we're not gonna use this garbage" and saves us an unused argument warning.

    DeassertSlaveSelect();

    dataIsNew = true;
}

static void AssertSlaveSelect(void)
{
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10, GPIO_PIN_RESET);
}

static void DeassertSlaveSelect(void)
{
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10, GPIO_PIN_SET);
}
