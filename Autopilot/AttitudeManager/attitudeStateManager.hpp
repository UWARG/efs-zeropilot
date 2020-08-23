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
        virtual void enter(attitudeManager* attitudeMgr);
        virtual void toggle(attitudeManager* attitudeMgr);
        virtual void exit(attitudeManager* attitudeMgr);
        virtual ~attitudeState() {}

};


#endif
