
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MPU6050_DRIVER_HPP
#define MPU6050_DRIVER_HPP


/* Includes ------------------------------------------------------------------*/

#include <stdint.h>

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);


struct ImuData {
  double gyro_x, gyro_y, gyro_z; 
  double accel_x, accel_y, accel_z;
}; 

class Imu {
  public: 
    virtual void getResult(ImuData& Data) = 0; 
}; 

class MPU6050 : public Imu {
  
  public: 
  
    static Imu& getInstance(); 

    MPU6050(const MPU6050*)=delete; 

    void getResult(ImuData& Data); 

  private:

    MPU6050(); 

    void ImuInit(void); 



};



#endif 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
