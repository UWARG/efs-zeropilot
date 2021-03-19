/**
 *  Attitude State Machine Header
 */
#pragma once
#ifndef ATTITUDESTATEMANAGER_HPP
#define ATTITUDESTATEMANAGER_HPP

#include "attitudeManager.hpp"

class attitudeManager;

class attitudeState
{
    public:
        virtual void enter(attitudeManager* attitudeMgr) = 0;
        virtual void execute(attitudeManager* attitudeMgr) = 0;
        virtual void exit(attitudeManager* attitudeMgr) = 0;

        bool operator==(const attitudeState& rhs) const {return (this == &rhs);}

        virtual ~attitudeState() {}

};

#endif
