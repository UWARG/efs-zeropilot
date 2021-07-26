#ifndef CRUISING_MODE_STAGES_HPP
#define CRUISING_MODE_STAGES_HPP

#include "cruisingModeStageManager.hpp"
#include "cruisingMode.hpp"
#include "waypointManager.hpp"
#include "TelemPathInterface.hpp"
#include "Autosteer.hpp"

class CruisingFlight : public CruisingModeStageManager {
    public:
        void enter(CruisingMode* cruiseMode) { (void) cruiseMode; }
        void execute(CruisingMode* cruiseMode);
        void exit(CruisingMode* cruiseMode) { (void) cruiseMode; }
        static CruisingModeStageManager& getInstance();

    private: 
        CruisingFlight() {}
        CruisingFlight(const CruisingFlight& other);
        CruisingFlight& operator=(const CruisingFlight& other);
};

#endif