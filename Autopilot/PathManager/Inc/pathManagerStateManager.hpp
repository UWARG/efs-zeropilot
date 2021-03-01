/**
 *  Path Manager State Machine Header
 */
#pragma once

#ifndef PATHMANAGERSTATEMANAGER_HPP
#define PATHMANAGERSTATEMANAGER_HPP

#include "pathManager.hpp"

class pathManager;

class pathManagerState
{
    public:
        virtual void enter(pathManager* pathMgr) = 0;
        virtual void execute(pathManager* pathMgr) = 0;
        virtual void exit(pathManager* pathMgr) = 0;

        bool operator==(const pathManagerState& rhs) const {return (this == &rhs);} 

        virtual ~pathManagerState() {}

};

#endif

