
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef IMU_HPP
#define IMU_HPP


/* Includes ------------------------------------------------------------------*/

#include <stdint.h>

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);


struct IMUData_t {
  double gyro_x, gyro_y, gyro_z; 
  double accel_x, accel_y, accel_z;
}; 

class IMU {
  public: 
    virtual void GetResult(IMUData_t& Data) = 0; 
}; 

class MPU6050 : public IMU {
  
  public: 
  
    static IMU& getInstance(); 

    MPU6050(const MPU6050*)=delete; 

    void GetResult(IMUData_t& Data); 

  private:

    MPU6050(); 

    void ImuInit(void); 



};



#endif 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
