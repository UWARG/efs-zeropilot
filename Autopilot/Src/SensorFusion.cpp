/*
* Sensor Fusion Algorithms:
*   Uses Madgwick for attitude
*   Uses Kalman filter for position - Based on following implementation:
*       https://timdelbruegger.wordpress.com/2016/01/05/altitude-sensor-fusion/
* Authors: Lucy Gong, Dhruv Rawat, Anthony Bertnyk
*/
#include "SensorFusion.hpp"
#include "MadgwickAHRS.h"
#include <math.h>


SFError_t SF_GetResult(SFOutput_t *Output, IMU_Data_t *imudata, Airspeed_Data_t *airspeeddata) {
    
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
    Output->IMUpitch = imu_PitchAngle;
    Output->IMUroll = imu_RollAngle;
    Output->IMUyaw = imu_YawAngle;

    Output->IMUpitchrate = imu_PitchRate;
    Output->IMUrollrate = imu_RollRate;
    Output->IMUyawrate = imu_YawRate;

    //Transfer Airspeed data
    Output->Airspeed = airspeeddata->airspeed;

    return SFError;
}

SFError_t SF_GetPosition(SFPositionOutput_t *Output, AltimeterData_t *altimeterdata, GpsData_t *gpsdata, IMU_Data_t *imudata, float dt)
{
    /*
    TODO:
    Set up input/output.
    Convert imu data to world space using attitude data.
    Combine with SF_GetResult into single function?
    Documentation.
    */

    //Error output
    SFError_t SFError;

    SFError.errorCode = 0;

    /* Time Update */

    //Defines for estimate

    const int16_t DIM = 6;

    arm_matrix_instance_f32 x;

    arm_matrix_instance_f32 prevX;
    
    arm_matrix_instance_f32 f;
    float32_t fData[DIM*DIM] =
    {
        1, dt, 0, 0,  0, 0,
        0, 1,  0, 0,  0, 0,
        0, 0,  1, dt, 0, 0,
        0, 0,  0, 1,  0, 0,
        0, 0,  0, 0,  1, dt,
        0, 0,  0, 0,  0, 1
    };
    arm_mat_init_f32(&f, DIM, DIM, fData);
    
    const int16_t U_DIM = 3;

    float32_t ddt = pow(dt,2)/2;

    arm_matrix_instance_f32 u;

    arm_matrix_instance_f32 b;
    float32_t bData[DIM*U_DIM] =
    {
        ddt, 0,   0,
        dt,  0,   0,
        0,   ddt, 0,
        0,   dt,  0,
        0,   0,   ddt,
        0,   0,   dt
    };
    arm_mat_init_f32(&f, DIM, U_DIM, bData);

    //Calculate estimate: x = f*prevX + b*u
    
    arm_matrix_instance_f32 fMultPrevX;
    arm_mat_mult_f32(&f, &prevX, &fMultPrevX);

    arm_matrix_instance_f32 bMultU;
    arm_mat_mult_f32(&b, &u, &bMultU);
    
    arm_mat_add_f32(&fMultPrevX, &bMultU, &x);
    
    //Defines for error covariance

    arm_matrix_instance_f32 p;
    
    arm_matrix_instance_f32 q;
    float32_t qData[DIM*DIM] =
    {
        0.3, 0,   0,   0,   0,   0,
        0,   0.5, 0,   0,   0,   0,
        0,   0,   0.4, 0,   0,   0,
        0,   0,   0,   0.5, 0,   0,
        0,   0,   0,   0,   0.4, 0,
        0,   0,   0,   0,   0,   0.5
    };
    arm_mat_init_f32(&q, DIM, DIM, qData);

    //Calculate error covariance: p = f*prevP*transpose(f) + q

    arm_matrix_instance_f32 fMultPrevP;
    arm_mat_mult_f32(&f, &p, &fMultPrevP);

    arm_matrix_instance_f32 transF;
    arm_mat_trans_f32(&f, &transF);

    arm_mat_mult_f32(&fMultPrevP, &transF, &p);


    /* Measurement Update */

    //Defines for Kalman gain
    
    arm_matrix_instance_f32 k;

    const int16_t NUM_MEASUREMENTS = 4;

    arm_matrix_instance_f32 h;
    float32_t hData[NUM_MEASUREMENTS*DIM] =
    {
        1, 0, 0, 0, 0, 0,
        1, 0, 0, 0, 0, 0,
        0, 0, 1, 0, 0, 0,
        0, 0, 0, 0, 1, 0,
    };
    arm_mat_init_f32(&h, DIM, DIM, hData);

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
    
    arm_matrix_instance_f32 r;
    float32_t rData[DIM*DIM] =
    {
        baroCovar, 0,         0,         0,
        0,         gpsCovar,
        0,         0,         gpsCovar,
        0,         0,         0,         gpsCovar
    };
    arm_mat_init_f32(&h, DIM, DIM, rData);

    //Calculate Kalman gain: k = p*transpose(h) * (h*p*transpose(h) + r)^-1

    arm_matrix_instance_f32 transH;
    arm_mat_trans_f32(&h, &transH);

    arm_matrix_instance_f32 pMultTransH;
    arm_mat_mult_f32(&p, &transH, &pMultTransH);

    arm_matrix_instance_f32 hPMultTransH;
    arm_mat_mult_f32(&h, &pMultTransH, &hPMultTransH);

    arm_matrix_instance_f32 hPMultTransHPlusR;
    arm_mat_add_f32(&hPMultTransH, &r, &hPMultTransHPlusR);

    arm_matrix_instance_f32 hPMultTransHPlusRInv;
    arm_mat_inverse_f32(&hPMultTransHPlusR, &hPMultTransHPlusRInv);

    arm_mat_mult_f32(&pMultTransH, &hPMultTransHPlusRInv, &k);

    //Defines for updating estimate

    arm_matrix_instance_f32 z;

    //Update estimate: newX = x + k*(z - h*x)

    arm_matrix_instance_f32 hMultX;
    arm_mat_mult_f32(&h, &x, &hMultX);

    arm_matrix_instance_f32 zMinHMultX;
    arm_mat_sub_f32(&z, &hMultX, &zMinHMultX);

    arm_matrix_instance_f32 kMultZMinHMultX;
    arm_mat_mult_f32(&k, &zMinHMultX, &kMultZMinHMultX);

    arm_matrix_instance_f32 newX;
    arm_mat_add_f32(&x, &kMultZMinHMultX, &newX);

    //Defines for updating error covariance

    arm_matrix_instance_f32 I;
    float32_t IData[DIM*DIM] =
    {
        1, 0, 0, 0, 0, 0,
        0, 1, 0, 0, 0, 0,
        0, 0, 1, 0, 0, 0,
        0, 0, 0, 1, 0, 0,
        0, 0, 0, 0, 1, 0,
        0, 0, 0, 0, 0, 1
    };
    arm_mat_init_f32(&h, DIM, DIM, IData);

    //Update error covariance: newP = (I - k*h)*p

    arm_matrix_instance_f32 kMultH;
    arm_mat_mult_f32(&k, &h, &kMultH);

    arm_matrix_instance_f32 IMinKMultH;
    arm_mat_sub_f32(&I, &kMultH, &IMinKMultH);

    arm_matrix_instance_f32 newP;
    arm_mat_mult_f32(&IMinKMultH, &p, &newP);

    return SFError;
}
