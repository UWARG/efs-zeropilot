/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef IMU_HPP
#define IMU_HPP


/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include "../boardfiles/Inc/i2c.h"

/* Exported functions prototypes ---------------------------------------------*/
// void Error_Handler();

struct IMUData_t {
  double gyro_x, gyro_y, gyro_z; 
  double accel_x, accel_y, accel_z;
  double mag_x, mag_y, mag_z;
}; 

class IMU {
  public: 
    virtual void GetResult(IMUData_t& Data) = 0; 
}; 

class BMX160 : public IMU {
  
  public: 
  
    static IMU& getInstance(); 

    BMX160(const BMX160*)=delete; 

    void GetResult(IMUData_t& Data); 
    void updateData();

  private:

    BMX160(); 
    void setAllPowerModesToNormal(void); 
    void configAcc(void);
    void configGyro(void);
    void configMag(void);

    void writeRegister(uint8_t reg, uint8_t val);
    void readRegister(uint8_t const regAddr, uint8_t *data, uint8_t len);

    void calibrate(void);

    void IMUInit(void); 

    bool scan(void); // Check that the slave device exists

    // Variables:
    uint8_t rawImuData[20];
    IMUData_t IMUCalibration;
};

#endif 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
