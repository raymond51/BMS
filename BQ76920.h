/* 
 * File:   BQ76920.h
 *PIC: 16F1719 @16MHZ
 * compiler: XC8
 * 
 *Program Description: Header file to control the BQ76920 AFE chip
 */

#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>
#include "BQ76920_registers.h"
#include "I2C.h"
#include "EUSART.h"
#include <math.h>
#include "algorithms.h"
//----------------------------------DEFINES


//array size macro
#define arrSize(x)  (sizeof(x) / sizeof((x)[0]))

#define MAX_NUMBER_OF_CELLS 5

// IC type/size
#define bq76920 1
#define AFE_BQ76920_I2C_ADDRESS 0x18


// output information to serial console for debugging, comment out to remove
//#define BQ76920_DEBUG


int adcGain=0; // uV/LSB values updated during beginnging of communication with AFE
int adcOffset=0; // mV

// Temperature limits (°C/10)
int minCellTempCharge;
int minCellTempDischarge;
int maxCellTempCharge;
int maxCellTempDischarge;

//status flags
int XR_error = 0;
int alert_error = 0;
int uv_error = 0;
int ov_error = 0;
int scd_error = 0;
int ocd_error = 0;

int chg_fet_enable = 0;
int dschg_fet_enable = 0;

//cell parameters
int cellVoltages[MAX_NUMBER_OF_CELLS];          // mV
int cellSOC[MAX_NUMBER_OF_CELLS];          // mV
int maxCellVoltage; //read from eeprom and compared with varying batvoltage
int minCellVoltage;
long batVoltage=0;                                // mV
long batCurrent=0;                                // mA
long temperatureThermistor=0;    //in centi Temp 
int thermistorBetaValue = 3435;  // typical value for Semitec 103AT-5 thermistor
//Static variables - //When a global variable is made static, its scope is restricted to the current file.

float shuntResistorValue_mOhm; //Shunt resistor value
//static regPROTECT1_t protect1;
//static regPROTECT2_t protect2;
//static regPROTECT3_t protect3;

regPROTECT1_t protect1;
regPROTECT2_t protect2;
regPROTECT3_t protect3;

//temporary cell parameters
int samsung_cell_max_charge = 2500;
int cellCharge[MAX_NUMBER_OF_CELLS];  

//main function prototypes -----------------------------------------------------
void init_AFE(void);
int beginAFEcommunication(void);

void setTemperatureLimitsint(int minDischarge_degC, int maxDischarge_degC, int minCharge_degC, int maxCharge_degC);
void setShuntResistorValue(float res_mOhm);
void setShortCircuitProtection(long current_mA, int delay_us);
void setOverCurrentDischargeProtection(long current_mA, int delay_ms);
void setCellUndervoltageProtection(int voltage_mv, int delay_s);
void setCellOvervoltageProtection(int voltage_mV, int delay_s);

void AFE_UPDATE(void);

void updateCurrent(void);
void updateVoltages(void);
void updateTemperatures(void);

void enableDischarging(unsigned int enable);
void enableCharging(unsigned int enable);

//Printout serial monitor helper functions -------------------------------------
long AFE_getSetShortCircuitCurrent(void);
float AFE_getSetCurrentSenseRes(void);
long AFE_getOverCurrentDischargeCurrent(void);

void printotAFERegisters(void);
void printcellParameters(void);


#endif	/* XC_HEADER_TEMPLATE_H */

