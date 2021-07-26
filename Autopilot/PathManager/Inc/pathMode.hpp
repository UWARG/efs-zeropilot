#ifndef PATH_MODE_HPP
#define PATH_MODE_HPP

#include "pathManager.hpp"

// Parent for the Mode class in https://uwarg-docs.atlassian.net/wiki/spaces/ZP/pages/1866989569/Proposed+Redesign
class PathMode {      
    public:
        /**
         * Execute method called by PathModeSelector::execute([:params]) to execute the current stage of our current mode of flight
         * 
         * @return none
         */
        virtual void execute() = 0;

    protected:
        PathMan::_Path_Manager_Cycle_Status current_status;
};

#endif