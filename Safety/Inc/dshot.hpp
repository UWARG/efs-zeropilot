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
    Dshot(dshotType type);
    void setDshot(uint8_t trottlePercentage);
    void initDshot(); //do we need this?


private:
    uint16_t prepareFrame(uint8_t throttlePercentage, bool telemetry);
    void prepareDMABuffer(uint32_t * dmaBuffer, uint16_t frame);
    


};
#endif /* DSHOT_HPP */
