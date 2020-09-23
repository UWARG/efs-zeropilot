#include "altimeter.hpp"

//Address for the MS5637. Needs to be shifted for write/read (ref:datasheet)
#define MS5637_ADDR 0x76
#define MS5637_WRITE_ADDR 0xEC
#define MS5637_READ_ADDR 0xED
#define MS5637_I2C I2C2 //Port... I think?
//registers
//Hex codes for polling, ref: Datasheet pg.9
#define MS5637_RESET_CMD 0x1E
#define MS5637_START_PRESSURE_CONVERSION 0x40
#define MS5637_START_TEMP_CONVERSION 0x50
#define MS5637_READ_ADC 0x00
#define MS5637_INTERN_MEM_ADDRESS 0x00 //There isn't actually one. Datasheet and stack exchange confirms

//PROM commands
#define MS5637_PROM_C1 0xA2
#define MS5637_PROM_C2 0xA4
#define MS5637_PROM_C3 0xA6
#define MS5637_PROM_C4 0xA8
#define MS5637_PROM_C5 0xAA
#define MS5637_PROM_C6 0xAC

static bool isI2CBusDefined = false;

static I2C_HandleTypeDef* hi2c;

 void MS5637::Init()
 {
   if(!isI2CBusDefined)
   {
     hi2c = I2C_GetHandle(MS5637_I2C);

     if (HAL_I2C_IsDeviceReady(hi2c, MS5637_WRITE_ADDR, 2, 5) != HAL_OK) {
       //Debug code here?
     }
   }

 }

uint32_t MS5637::readFromMS5637(uint32_t commandToWrite) {
  I2C_WriteByte(hi2c, MS5637_WRITE_ADDR, MS5637_INTERN_MEM_ADDRESS, commandToWrite);
   uint8_t data[3]; //3 integers to store 24 bits worth of data.
   I2C_ReadBytes(hi2c, MS5637_READ_ADDR, MS5637_READ_ADC, data, 3);
   uint32_t returnData = (data[2] << 16) + (data[1] << 8) + data[0];
   return (returnData);
}

void MS5637::getRawPressureAndTemperature(int64_t *rawPressure, int64_t *rawTemperature) {
  //Calcs & variable names inside datasheet
   uint32_t C1 = readFromMS5637(MS5637_PROM_C1);
   uint32_t C2 = readFromMS5637(MS5637_PROM_C2);
   uint32_t C3 = readFromMS5637(MS5637_PROM_C3);
   uint32_t C4 = readFromMS5637(MS5637_PROM_C4);
   uint32_t C5 = readFromMS5637(MS5637_PROM_C5);
   uint32_t C6 = readFromMS5637(MS5637_PROM_C6);

   uint32_t D1 = readFromMS5637(MS5637_START_PRESSURE_CONVERSION);
   uint32_t D2 = readFromMS5637(MS5637_START_TEMP_CONVERSION);

   int32_t dT = D2 - C5 * 256; //difference between measured and reference temperature
   int32_t TEMP = (2000 + (dT*C6/8388608)); //actual temperature in degrees c * 100,
   int64_t OFF = C2*131072 + C4*dT/64; //offset at actual temperature
   int64_t SENS = C1*65536 + C3*dT/128;


   /*****2ND ORDER CONVERSIONS for that epic low temp accuracy****/
   uint32_t T2 = 0;
   float OFF2 = 0;
   float SENS2 = 0;
   if(TEMP >= 2000) {
     T2 = 5*dT*dT/274877906944;
     OFF2 = 0;
     SENS2 = 0;
   }
   else if(TEMP < 2000)
   {
     T2 = 3*dT*dT/8589934592;
     OFF2 = 61*(TEMP-2000)*(TEMP-2000)/(16);
     SENS2 = 29*(TEMP-2000)*(TEMP-2000)/(16);

     if(TEMP < -1500)
     {
       OFF2 = OFF2 + 17*(TEMP+1500)*(TEMP+1500);
       SENS2 = SENS2 + 9*(TEMP+1500)*(TEMP+1500);
     }

   }
   
   TEMP = TEMP - T2;
	 OFF = OFF - OFF2;
	 SENS = SENS - SENS2;
   
   int64_t P = ((D1*SENS/2097152-OFF)/32768); //Pressure, 1000 -> 120000

   *rawPressure = P;
   *rawTemperature = TEMP;
   

}

 float MS5637::getPressure() {
   
   int64_t rawPressure = 0, rawTemperature = 0;
   getRawPressureAndTemperature(&rawPressure, &rawTemperature);

   float displayPressure = rawPressure/100.0; //10 to 1200 mbar with 0.01mbar resolution
   return displayPressure;
 }

 float MS5637::getTemperature()
 {
   int64_t rawPressure = 0, rawTemperature = 0;
   getRawPressureAndTemperature(&rawPressure, &rawTemperature);

   float displayTemp = rawTemperature/100.0; //10 to 1200 mbar with 0.01mbar resolution
   return displayTemp;

 }

 float MS5637::getAltitude()
 {
   const float conversionFactor = 121.92/(998.689-1013.25);
   return getPressure()*conversionFactor + 8484;
 }

 void MS5637::GetResult(AltimeterData_t *Data)
 {
   Data->altitude = getAltitude();
   Data->pressure = getPressure();
   Data->temp = getTemperature();
 }

