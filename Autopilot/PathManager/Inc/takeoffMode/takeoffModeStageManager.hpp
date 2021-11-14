#ifndef TAKEOFF_MODE_STAGE_MANAGER_HPP
#define TAKEOFF_MODE_STAGE_MANAGER_HPP

#include "takeoffMode.hpp"

class TakeoffMode;

class TakeoffModeStageManager {
    public:
        virtual void enter(TakeoffMode* takeoff_mode) = 0;
        virtual void execute(TakeoffMode* takeoff_mode) = 0;
        virtual void exit(TakeoffMode* takeoff_mode) = 0;

        bool operator==(const TakeoffModeStageManager& rhs) const {return (this == &rhs);} 

        virtual ~TakeoffModeStageManager() {}
};
#endif
