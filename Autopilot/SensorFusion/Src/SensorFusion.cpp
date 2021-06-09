/*
* Sensor Fusion Algorithms - uses Madgwick/Kalmann/etc. tbd PLEASE EDIT THIS AND ADD MORE INFO :))))))))) 
* Author: Lucy Gong, Dhruv Rawat
*/
#include "SensorFusion.hpp"
#include "MadgwickAHRS.h"
#include <math.h>
#include "IMU.hpp"
#include "gps.hpp"
#include "altimeter.hpp"
#include "airspeed.hpp"

#ifdef __cplusplus
extern "C"
{
#endif
    #include "CControlFunctions.h"
#ifdef __cplusplus
}
#endif

typedef struct {
    float roll, pitch, yaw; //rad
    float rollRate, pitchRate, yawRate; //rad/s
    float airspeed; //m/s
} SFAttitudeOutput_t ;

struct SFPositionOutput_t{
    float altitude; //m
    float rateOfClimb; //m/s
    long double latitude; //Decimal degrees
    float latitudeSpeed; //m/s
    long double longitude; //Decimal degrees
    float longitudeSpeed; //m/s
};

const int NUM_KALMAN_VALUES = 6; 

struct SFIterationData_t{
    float prevX[NUM_KALMAN_VALUES];
    float prevP[NUM_KALMAN_VALUES*NUM_KALMAN_VALUES];
};

static IMU *imuObj;
static Gps *gpsObj;
static Altimeter *altimeterObj;
static airspeed *airspeedObj;
static SFIterationData_t iterData;
static SFOutput_t SFOutput;

void SF_Init(void)
{
#ifdef TARGET_BUILD

  imuObj = &BMX160::getInstance();
  gpsObj = NEOM8::GetInstance();
  //Waiting for definitions
  //altimeterObj = MS5637::GetInstance();
  //airspeedObj = dummyairspeed::GetInstance();

#elif defined(TEST_BUILD)

  imuObj = TestIMU::GetInstance();
  gpsObj = TestGps::GetInstance();
  altimeterObj = TestAltimeter::GetInstance();
  airspeedObj = TestAirspeed::GetInstance();
#endif
}

void localToGlobalAccel(IMUData_t *imudata, float *u)
{
    u[0] = imudata->accy; //Vertical acceleration
    u[1] = imudata->accx; //Latitudinal acceleration
    u[2] = imudata->accz; //Longitudinal acceleration
}

SFError_t SF_GetAttitude(SFAttitudeOutput_t *Output, IMUData_t *imudata, airspeedData_t *airspeeddata) {
    
    //Error output
    SFError_t SFError;

    SFError.errorCode = 0;

    //IMU integration outputs
    float imu_RollAngle = 0;
    float imu_PitchAngle = 0;
    float imu_YawAngle = 0;

    float imu_RollRate = 0;
    float imu_PitchRate = 0;
    float imu_YawRate = 0;

    //Abort if both sensors are busy or failed data collection
    if(imudata->sensorStatus != 0 || airspeeddata->sensorStatus != 0)
    {  

        /************************************************************************************************
         * THIS WILL PUT THE STATE MACHINE INTO FATAL FAILURE MODE... WE NEED TO DECIDE IF THIS IS WHAT
         * WE WANT OR IF WE SHOULD RETHINK HOW WE WANT THIS MODULE TO RETURN A SENSOR ERROR! 
         ************************************************************************************************/

        SFError.errorCode = -1;
        return SFError;
    }

    //Check if data is old
    if(!imudata->isDataNew || !airspeeddata->isDataNew){
        SFError.errorCode = 1;
    }

    // Checks if magnetometer values are not a number (NAN) and converts them to zero if they are (ensures Madgwick does not break)
    // NOTE TO FUTURE DEVELOPERS: At the time of making, our IMU did not have a magnetometer (so for now we set the values to NAN). 
    // If your IMU does have one, you can remove this
    if (isnan(imudata->magx)) {
        imudata->magx = 0.0f;
    }
    if (isnan(imudata->magy)) {
        imudata->magy = 0.0f;
    }
    if (isnan(imudata->magz)) {
        imudata->magz = 0.0f;
    }

    MadgwickAHRSupdate(imudata->gyrx, imudata->gyry, imudata->gyrz, imudata->accx, imudata->accy, imudata->accz, imudata->magx, imudata->magy, imudata->magz);

    //Convert quaternion output to angles (in deg)
    imu_RollAngle = atan2f(q0 * q1 + q2 * q3, 0.5f - q1 * q1 - q2 * q2) * 57.29578f;
    imu_PitchAngle = asinf(-2.0f * (q1 * q3 - q0 * q2)) * 57.29578f;
    imu_YawAngle = atan2f(q1 * q2 + q0 * q3, 0.5f - q2 * q2 - q3 * q3) * 57.29578f + 180.0f;

    //Convert rate of change of quaternion to angular velocity (in deg/s)
    imu_RollRate = atan2f(qDot1 * qDot2 + qDot3 * qDot4, 0.5f - qDot2 * qDot2 - qDot3 * qDot3) * 57.29578f;
    imu_PitchRate = asinf(-2.0f * (qDot2 * qDot4 - qDot1 * qDot3)) * 57.29578f;
    imu_YawRate = atan2f(qDot2 * qDot3 + qDot1 * qDot4, 0.5f - qDot3 * qDot3 - qDot4 * qDot4) * 57.29578f + 180.0f;

    //Transfer Fused IMU data into SF Output struct
    Output->pitch = imu_PitchAngle;
    Output->roll = imu_RollAngle;
    Output->yaw = imu_YawAngle;

    Output->pitchRate = imu_PitchRate;
    Output->rollRate = imu_RollRate;
    Output->yawRate = imu_YawRate;

    //Transfer airspeed data
    Output->airspeed = airspeeddata->airspeed;

    return SFError;
}

SFError_t SF_GetPosition(SFPositionOutput_t *Output, AltimeterData_t *altimeterdata, GpsData_t *gpsdata, IMUData_t *imudata, SFAttitudeOutput_t *attitudedata,  SFIterationData_t *iterdata)
{

    //Error output
    SFError_t SFError;
    SFError.errorCode = 0;

    float freq = 512;
    float dt = 1/freq;

    /* Time Update */

    //Defines for estimate

    const int16_t DIM = 6;

    float x[DIM*1];

    //float * prevX = iterdata->prevX;

    float prevX[DIM*1];
    for (int i = 0; i < DIM*1; i++) prevX[i] = iterdata->prevX[i];

    float f[DIM*DIM] =
    {
        1, dt, 0, 0,  0, 0,
        0, 1,  0, 0,  0, 0,
        0, 0,  1, dt, 0, 0,
        0, 0,  0, 1,  0, 0,
        0, 0,  0, 0,  1, dt,
        0, 0,  0, 0,  0, 1
    };
    
    const int16_t U_DIM = 3;

    float ddt = pow(dt,2)/2;

    float u[U_DIM*1] =
    {
        0,
        0,
        0
    };
    localToGlobalAccel(imudata, u);

    float b[DIM*U_DIM] =
    {
        ddt, 0,   0,
        dt,  0,   0,
        0,   ddt, 0,
        0,   dt,  0,
        0,   0,   ddt,
        0,   0,   dt
    };

    //Calculate estimate: x = f*prevX + b*u
    
    float fMultPrevX[DIM*1];
    mul(f, prevX, fMultPrevX, DIM, DIM, 1);

    float bMultU[DIM*1];
    mul(b, u, bMultU, DIM, U_DIM, 1);

    for (int i = 0; i < DIM*1; i++) x[i] = fMultPrevX[i] + bMultU[i];
    
    //Defines for error covariance

    float p[DIM*DIM];

    float prevP[DIM*DIM];
    for (int i = 0; i < DIM*DIM; i++) 
    {
    prevP[i] = iterdata->prevP[i];
    }

    float q[DIM*DIM]=
    {
        0.3, 0,   0,   0,   0,   0,
        0,   0.5, 0,   0,   0,   0,
        0,   0,   0.4, 0,   0,   0,
        0,   0,   0,   0.5, 0,   0,
        0,   0,   0,   0,   0.4, 0,
        0,   0,   0,   0,   0,   0.5
    };
    
    //Calculate error covariance: p = f*prevP*transpose(f) + q

    float fMultPrevP[DIM*DIM];
    mul(f, prevP, fMultPrevP, DIM, DIM, DIM);

    float transF[DIM*DIM];
    for (int i = 0; i < DIM*DIM; i++) transF[i] = f[i];
    tran(transF, DIM, DIM);

    float fMultPrevPMultTransF[DIM*DIM];
    mul(fMultPrevP, transF, fMultPrevPMultTransF, DIM, DIM, DIM);

    for (int i = 0; i < DIM*DIM; i++) p[i] = fMultPrevPMultTransF[i] + q[i];

    /* Measurement Update */

    //Defines for Kalman gain
    
    const int16_t NUM_MEASUREMENTS = 4;

    float k[DIM*NUM_MEASUREMENTS];

    float h[NUM_MEASUREMENTS*DIM] =
    {
        1, 0, 0, 0, 0, 0,
        1, 0, 0, 0, 0, 0,
        0, 0, 1, 0, 0, 0,
        0, 0, 0, 0, 1, 0
    };
    
    int16_t baroCovar = 1;
    int16_t gpsCovar;
    if(gpsdata->numSatellites >= 3)
    {
        gpsCovar = 1 + pow(gpsdata->numSatellites, -0.5);
    }
    else
    {
        gpsCovar = 1000;
    }

    float r[NUM_MEASUREMENTS*DIM]
    {
        baroCovar, 0,         0,         0,
        0,         gpsCovar,  0,         0,
        0,         0,         gpsCovar,  0,
        0,         0,         0,         gpsCovar
    };

    //Calculate Kalman gain: k = p*transpose(h) * (h*p*transpose(h) + r)^-1

    float transH[DIM*NUM_MEASUREMENTS];
    for (int i = 0; i < NUM_MEASUREMENTS*DIM; i++) transH[i] = h[i];
    tran(transH, NUM_MEASUREMENTS, DIM);

    float pMultTransH[DIM*NUM_MEASUREMENTS];
    mul(p, transH, pMultTransH, DIM, DIM, NUM_MEASUREMENTS);

    float hPMultTransH[NUM_MEASUREMENTS*NUM_MEASUREMENTS];
    mul(h, pMultTransH, hPMultTransH, NUM_MEASUREMENTS, DIM, NUM_MEASUREMENTS);

    float hPMultTransHPlusR[NUM_MEASUREMENTS*NUM_MEASUREMENTS];
    for (int i = 0; i < NUM_MEASUREMENTS*NUM_MEASUREMENTS; i++) hPMultTransHPlusR[i] = hPMultTransH[i] + r[i];

    float hPMultTransHPlusRInv[NUM_MEASUREMENTS*NUM_MEASUREMENTS];
    for (int i = 0; i < NUM_MEASUREMENTS*NUM_MEASUREMENTS; i++) hPMultTransHPlusRInv[i] = hPMultTransHPlusR[i];
    inv(hPMultTransHPlusRInv, NUM_MEASUREMENTS);

    mul(pMultTransH, hPMultTransHPlusRInv, k, DIM, NUM_MEASUREMENTS, NUM_MEASUREMENTS);


    //Defines for updating estimate

    float z[NUM_MEASUREMENTS*1]=
    {
        altimeterdata->altitude,
        gpsdata->altitude,
        gpsdata->latitude,
        gpsdata->longitude
    };

    //Update estimate: newX = x + k*(z - h*x)

    float hMultX[NUM_MEASUREMENTS*1];
    mul(h, x, hMultX, NUM_MEASUREMENTS, DIM, 1);

    float zMinHMultX[NUM_MEASUREMENTS*1];
    for (int i = 0; i < NUM_MEASUREMENTS*1; i++) zMinHMultX[i] = z[i] - hMultX[i];

    float kMultZMinHMultX[DIM*1];
    mul(k, zMinHMultX, kMultZMinHMultX, DIM, NUM_MEASUREMENTS, 1);

    float newX[DIM*1];
    for (int i = 0; i < DIM*1; i++) newX[i] = x[i] + kMultZMinHMultX[i];

    //Defines for updating error covariance

    float I[DIM*DIM] =
    {
        1, 0, 0, 0, 0, 0,
        0, 1, 0, 0, 0, 0,
        0, 0, 1, 0, 0, 0,
        0, 0, 0, 1, 0, 0,
        0, 0, 0, 0, 1, 0,
        0, 0, 0, 0, 0, 1
    };

    //Update error covariance: newP = (I - k*h)*p

    float kMultH[DIM*DIM];
    mul(k, h, kMultH, DIM, NUM_MEASUREMENTS, DIM);

    float IMinKMultH[DIM*DIM];
    for (int i = 0; i < DIM*DIM; i++) IMinKMultH[i] = I[i] - kMultH[i];

    float newP[DIM*DIM];
    mul(IMinKMultH, p, newP, DIM, DIM, DIM);


    /*Output*/

    Output->altitude = x[0];
    Output->latitude = x[2];
    Output->longitude = x[4];

    for (int i = 0; i < DIM*1; i++) iterdata->prevX[i] = newX[i];
    for (int i = 0; i < DIM*DIM; i++) iterdata->prevP[i] = newP[i];

    return SFError;
}

#include <iostream>

SFError_t SF_GenerateNewResult()
{
    SFError_t SFError;
    SFError.errorCode = 0;
    
    IMUData_t imuData;
    GpsData_t GpsData;
    AltimeterData_t altimeterData;
    airspeedData_t airspeedData;
    //imuData = imuObj->getResult();
    //GpsData = gpsObj->getResult();
    //altimeterData = altimeterObj->getResult();
    //airspeedData = airspeedObj->getResult();

    SFAttitudeOutput_t attitudeOutput;
    SFPositionOutput_t positionOutput;

    SFError = SF_GetAttitude(&attitudeOutput, &imuData, &airspeedData);
    if(SFError.errorCode != 0) return SFError;

    SFError = SF_GetPosition(&positionOutput, &altimeterData, &GpsData, &imuData, &attitudeOutput, &iterData);
    if(SFError.errorCode != 0) return SFError;

    SFOutput.pitch = attitudeOutput.pitch;
    SFOutput.roll = attitudeOutput.roll;
    SFOutput.yaw = attitudeOutput.yaw;

    return SFError;
}

SFError_t SF_GetResult(SFOutput_t *output)
{
    SFError_t SFError;
    SFError.errorCode = 0;

    output = &SFOutput;

    return SFError;
}

IMU_Data_t SF_GetRawIMU()
{
    IMUData_t imuData;
    //imuData = imuObj->getResult();

    IMU_Data_t imuOutput;

    imuOutput.accx = imuData.accx;
    imuOutput.accy = imuData.accy;
    imuOutput.accz = imuData.accz;
    imuOutput.gyrx = imuData.gyrx;
    imuOutput.gyry = imuData.gyry;
    imuOutput.gyrz = imuData.gyrz;
    imuOutput.magx = imuData.magx;
    imuOutput.magy = imuData.magy;
    imuOutput.magz = imuData.magz;
    imuOutput.isDataNew = imuData.isDataNew;
    imuOutput.sensorStatus = imuData.sensorStatus;

    return imuOutput;
}

Airspeed_Data_t SF_GetRawAirspeed()
{
    airspeedData_t airspeedData;
    //airspeedData = airspeedObj->getResult();

    Airspeed_Data_t airspeedOutput;

    airspeedOutput.airspeed = airspeedData.airspeed;
    airspeedOutput.utcTime = airspeedData.utcTime;
    airspeedOutput.isDataNew = airspeedData.isDataNew;
    airspeedOutput.sensorStatus = airspeedData.sensorStatus;

    return airspeedOutput;
}
