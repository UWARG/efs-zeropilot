#ifndef LANDING_MODE_STAGE_MANAGER_HPP
#define LANDING_MODE_STAGE_MANAGER_HPP

#include "landingMode.hpp"

class LandingMode;

class LandingModeStageManager {
    public:
        virtual void enter(LandingMode* land_mode) = 0;
        virtual void execute(LandingMode* land_mode) = 0;
        virtual void exit(LandingMode* land_mode) = 0;

        bool operator==(const LandingModeStageManager& rhs) const {return (this == &rhs);} 

        virtual ~LandingModeStageManager() {}
};

#endif

