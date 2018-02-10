#include "main.h"
//Bit Mask Bit Shifts
typedef enum {
    STABILIZE,
    ALT_HOLD, //Altitude hold
    LOITER,//Hold at current point
    RTL, //Return to Launch
    AUTO //Full autopilot path following
} CtrlType;

class Stabilizer{
  public:
  Stabilizer();
  void setFlightMode(CtrlType _flight_mode){flight_mode = _flight_mode;}
  int16_t *Stabilize();

  private:
  CtrlType flight_mode;
}