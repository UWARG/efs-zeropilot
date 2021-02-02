/*
* Author: Nixon Chan
*/

#pragma once
#include "telemetryStateManager.hpp"
#include "telemetryManager.hpp"

//each state's classes
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

class obtainDataMode: public telemetryState
{
    public:
        void enter(telemetryManager* telemetryMgr){(void) telemetryMgr;}
        void execute(telemetryManager* telemetryMgr);
        void exit(telemetryManager* telemetryMgr){(void) telemetryMgr;}
        static telemetryState& getInstance();

    private:
        obtainDataMode(){}
        obtainDataMode(const obtainDataMode& other);
        obtainDataMode& operator =(const obtainDataMode& other);
};

class decodeDataMode: public telemetryState
{
    public:
        void enter(telemetryManager* telemetryMgr){(void) telemetryMgr;}
        void execute(telemetryManager* telemetryMgr);
        void exit(telemetryManager* telemetryMgr){(void) telemetryMgr;}
        static telemetryState& getInstance();

    private:
        decodeDataMode(){}
        decodeDataMode(const decodeDataMode& other);
        decodeDataMode& operator =(const decodeDataMode& other);
};

class passToPathMode: public telemetryState
{
    public:
        void enter(telemetryManager* telemetryMgr){(void) telemetryMgr;}
        void execute(telemetryManager* telemetryMgr);
        void exit(telemetryManager* telemetryMgr){(void) telemetryMgr;}
        static telemetryState& getInstance();

    private:
        passToPathMode(){}
        passToPathMode(const passToPathMode& other);
        passToPathMode& operator =(const passToPathMode& other);
};

class readFromPathMode: public telemetryState
{
    public:
        void enter(telemetryManager* telemetryMgr){(void) telemetryMgr;}
        void execute(telemetryManager* telemetryMgr);
        void exit(telemetryManager* telemetryMgr){(void) telemetryMgr;}
        static telemetryState& getInstance();

    private:
        readFromPathMode(){}
        readFromPathMode(const readFromPathMode& other);
        readFromPathMode& operator =(const readFromPathMode& other);
};

class analyzeDataMode: public telemetryState
{
    public:
        void enter(telemetryManager* telemetryMgr){(void) telemetryMgr;}
        void execute(telemetryManager* telemetryMgr);
        void exit(telemetryManager* telemetryMgr){(void) telemetryMgr;}
        static telemetryState& getInstance();

    private:
        analyzeDataMode(){}
        analyzeDataMode(const analyzeDataMode& other);
        analyzeDataMode& operator =(const analyzeDataMode& other);
};

class reportMode: public telemetryState
{
    public:
        void enter(telemetryManager* telemetryMgr){(void) telemetryMgr;}
        void execute(telemetryManager* telemetryMgr);
        void exit(telemetryManager* telemetryMgr){(void) telemetryMgr;}
        static telemetryState& getInstance();

    private:
        reportMode(){}
        reportMode(const reportMode& other);
        reportMode& operator =(const reportMode& other);
};

class encodeDataMode: public telemetryState
{
    public:
        void enter(telemetryManager* telemetryMgr){(void) telemetryMgr;}
        void execute(telemetryManager* telemetryMgr);
        void exit(telemetryManager* telemetryMgr){(void) telemetryMgr;}
        static telemetryState& getInstance();

    private:
        encodeDataMode(){}
        encodeDataMode(const encodeDataMode& other);
        encodeDataMode& operator =(const encodeDataMode& other);
};

class sendDataMode: public telemetryState
{
    public:
        void enter(telemetryManager* telemetryMgr){(void) telemetryMgr;}
        void execute(telemetryManager* telemetryMgr);
        void exit(telemetryManager* telemetryMgr){(void) telemetryMgr;}
        static telemetryState& getInstance();

    private:
        sendDataMode(){}
        sendDataMode(const sendDataMode& other);
        sendDataMode& operator =(const sendDataMode& other);
};

class failureMode: public telemetryState
{
    public:
        void enter(telemetryManager* telemetryMgr){(void) telemetryMgr;}
        void execute(telemetryManager* telemetryMgr);
        void exit(telemetryManager* telemetryMgr){(void) telemetryMgr;}
        static telemetryState& getInstance();

    private:
        failureMode(){}
        failureMode(const failureMode& other);
        failureMode& operator =(const failureMode& other);
};
