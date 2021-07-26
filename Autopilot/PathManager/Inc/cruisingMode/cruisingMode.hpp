#ifndef CRUISING_MODE_HPP
#define CRUISING_MODE_HPP

#include "pathMode.hpp"
#include "cruisingModeStageManager.hpp"
#include "pathModeSelector.hpp"

class CruisingModeStageManager;

// Mode class as depicted in https://uwarg-docs.atlassian.net/wiki/spaces/ZP/pages/1866989569/Proposed+Redesign
class CruisingMode : public PathMode { 
    public:
        static PathMode& getInstance();
        
        inline CruisingModeStageManager* getCurrentStage() const { return current_stage; }
        void execute();
        
        // No need for a setStage or getStage method as we only have one stage
    private: 
        CruisingMode();

        CruisingModeStageManager* current_stage;
        PathModeSelector* mode_selector;        
};

#endif