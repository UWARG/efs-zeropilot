/**
 *  Path Manager State Machine Header
 */
#pragma once

#ifndef PATHMANAGERSTATEMANAGER_HPP
#define PATHMANAGERSTATEMANAGER_HPP

#include "pathManager.hpp"

class pathManager; //WHY HAVE THIS HERE?!?!? !!QUESTION!!

class pathManagerState
{
    public:
        virtual void enter(pathManager* pathMgr) = 0;
        virtual void execute(pathManager* pathMgr) = 0;
        virtual void exit(attitudeManager* pathMgr) = 0;

        bool operator==(const pathManagerState& rhs) const {return (this == &rhs);} //WHAT IS THIS?? !!QUESTION!!

        virtual ~pathManagerState() {}

};

#endif