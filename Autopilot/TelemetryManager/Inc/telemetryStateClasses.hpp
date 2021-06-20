/*
* Author: Nixon Chan, Dhruv Rawat
*/

#pragma once

#include "telemetryStateManager.hpp"
#include "telemetryManager.hpp"
#include "xbee.hpp"
#include "CommsWithPathManager.hpp"
#include "CommsWithTelemetry.hpp"
#include "TelemPathInterface.hpp"
#include "MathConstants.hpp"

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
        // Static getter to receive _rawPMData
        static Telemetry_PIGO_t* getRawPMData(void) { return &_rawPMData; } // CHANGE DATATYPE. ONLY USING THIS SO I CAN GET FLOW

        // #ifdef UNIT_TESTING
        //     static XBEE* getXBEE(void) { return ZPXbee; }
        // #endif

    private:
        obtainDataMode(){ ZPXbee = XBEE::getInstance(); }
        obtainDataMode(const obtainDataMode& other);
        obtainDataMode& operator =(const obtainDataMode& other);
        
        XBEE* ZPXbee;

        // Some static array to hold the received data. Name it _rawPMData
        static Telemetry_PIGO_t _rawPMData; // CHANGE DATATYPE. ONLY USING THIS SO I CAN GET FLOW
};

class decodeDataMode: public telemetryState
{
    public:
        void enter(telemetryManager* telemetryMgr){(void) telemetryMgr;}
        void execute(telemetryManager* telemetryMgr);
        void exit(telemetryManager* telemetryMgr){(void) telemetryMgr;}
        static telemetryState& getInstance();
        static Telemetry_PIGO_t* getDecodedPMData(void) { return &_decodedPMData; }

    private:
        decodeDataMode(){}
        decodeDataMode(const decodeDataMode& other);
        decodeDataMode& operator =(const decodeDataMode& other);

        static Telemetry_PIGO_t _decodedPMData;
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
        static Telemetry_POGI_t* getRawGSData(void) { return &_rawGSData; }

    private:
        readFromPathMode(){}
        readFromPathMode(const readFromPathMode& other);
        readFromPathMode& operator =(const readFromPathMode& other);

        static Telemetry_POGI_t _rawGSData;
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
        // Getter to receive _encodedGSData
        static Telemetry_POGI_t* getEncodedGSData(void) { return &_encodedGSData; } // CHANGE DATATYPE. ONLY USING THIS SO I CAN GET FLOW

    private:
        encodeDataMode(){}
        encodeDataMode(const encodeDataMode& other);
        encodeDataMode& operator =(const encodeDataMode& other);

        // Some data to store the encoded data. Call it _encodedGSData
        static Telemetry_POGI_t _encodedGSData; // CHANGE DATATYPE. ONLY USING THIS SO I CAN GET FLOW
};

class sendDataMode: public telemetryState
{
    public:
        void enter(telemetryManager* telemetryMgr){(void) telemetryMgr;}
        void execute(telemetryManager* telemetryMgr);
        void exit(telemetryManager* telemetryMgr){(void) telemetryMgr;}
        static telemetryState& getInstance();

    private:
        sendDataMode(){ ZPXbee = XBEE::getInstance(); }
        sendDataMode(const sendDataMode& other);
        sendDataMode& operator =(const sendDataMode& other);

        XBEE* ZPXbee;
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
