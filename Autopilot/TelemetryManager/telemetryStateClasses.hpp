#pragma once
#include "telemetryStateManager.hpp"
#include "telemetryManager.hpp"

class initialMode: public telemetryState
{
    public:
        void enter(telemetryManager* telemetryMgr){(void) telemetryMgr;}
        void execute(telemetryManager* telemetryMgr);
        void exit(telemetryManager* telemetryMgr){(void) telemetryMgr;}
        static telemetryState& getInstance();

    private:
        initialMode(){}
        initialMode(const initialMode& other);
        initialMode& operator =(const initialMode& other);
};

class groundDataPathMode: public telemetryState
{
    public:
        void enter(telemetryManager* telemetryMgr){(void) telemetryMgr;}
        void execute(telemetryManager* telemetryMgr);
        void exit(telemetryManager* telemetryMgr){(void) telemetryMgr;}
        static telemetryState& getInstance();

    private:
        groundDataPathMode(){}
        groundDataPathMode(const groundDataPathMode& other);
        groundDataPathMode& operator =(const groundDataPathMode& other);
};

class errorReportMode: public telemetryState
{
    public:
        void enter(telemetryManager* telemetryMgr){(void) telemetryMgr;}
        void execute(telemetryManager* telemetryMgr);
        void exit(telemetryManager* telemetryMgr){(void) telemetryMgr;}
        static telemetryState& getInstance();

    private:
        errorReportMode(){}
        errorReportMode(const errorReportMode& other);
        errorReportMode& operator =(const errorReportMode& other);
};

class regularReportMode: public telemetryState
{
    public:
        void enter(telemetryManager* telemetryMgr){(void) telemetryMgr;}
        void execute(telemetryManager* telemetryMgr);
        void exit(telemetryManager* telemetryMgr){(void) telemetryMgr;}
        static telemetryState& getInstance();

    private:
        regularReportMode(){}
        regularReportMode(const regularReportMode& other);
        regularReportMode& operator =(const regularReportMode& other);
};