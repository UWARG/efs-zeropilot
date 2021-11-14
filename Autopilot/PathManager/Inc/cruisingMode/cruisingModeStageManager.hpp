#ifndef CRUISING_MODE_STAGE_MANAGER_HPP
#define CRUISING_MODE_STAGE_MANAGER_HPP

#include "cruisingMode.hpp"

class CruisingMode;

// Like ModeStage class as depicted in https://uwarg-docs.atlassian.net/wiki/spaces/ZP/pages/1866989569/Proposed+Redesign
class CruisingModeStageManager { 
    public:
        virtual void enter(CruisingMode* cruise_mode) = 0;
        virtual void execute(CruisingMode* cruise_mode) = 0;
        virtual void exit(CruisingMode* cruise_mode) = 0;

        bool operator==(const CruisingModeStageManager& rhs) const {return (this == &rhs);} 

        virtual ~CruisingModeStageManager() {}
};

#endif

