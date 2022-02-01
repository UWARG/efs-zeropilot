/**
 Implementation of the DSHOT protocol...
 ect. add more detial on what dshot is here.
 */

#ifndef DSHOT_HPP
#define	DSHOT_HPP

#include <stdint.h>

#define DSHOT_DATA_FRAME_LEN 16

typedef enum dshotType
{
    DSHOT150,
    DSHOT300,
    DSHOT600
} dshotType;

class Dshot 
{

public:
/**
  * @brief Constructor to create an instance of Dshot
  * @param type Type of Dshot to be used
  * @retval None
  */
    Dshot(dshotType type);

/**
  * @brief Calculates and sends the Dshot signal to the ESC via PWM
  * @param throttlePercentage Trottle percentage output from the PID loop, 0-100%
  * @retval None
  */
    void setDshot(uint8_t trottlePercentage);

/**
  * @brief Initializes Dshot peripherals
  * @param None
  * @retval None
  */
    void initDshot(); //do we need this?


private:

/**
  * @brief Prepares the Dshot data frame including the data, telemetry bit, and checksum
  * @param throttlePercentage Trottle percentage output from the PID loop, 0-100%
  * @param telemetry Bool indicating whether to set the telemetry bit or not
  * @retval 16 bit dshot data frame
  */    
    uint16_t prepareFrame(uint8_t throttlePercentage, bool telemetry);

/**
  * @brief Prepares the DMA buffer using the data frame
  * @param dmaBuffer Pointer to the DMA buffer
  * @param frame DSHOT data frame
  * @retval None
  */    
    void prepareDMABuffer(uint32_t * dmaBuffer, uint16_t frame);



};
#endif /* DSHOT_HPP */
