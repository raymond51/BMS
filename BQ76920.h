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

//array size macro
#define arrSize(x)  (sizeof(x) / sizeof((x)[0]))

#define MAX_NUMBER_OF_CELLS 5
#define MAX_NUMBER_OF_THERMISTORS 1
// IC type/size
#define bq76920 1
#define AFE_BQ76920_I2C_ADDRESS 0x18

//cell data
int cellVoltages[MAX_NUMBER_OF_CELLS]; // mV


// output information to serial console for debugging, comment out to remove
#define BQ76920_DEBUG


int adcGain; // uV/LSB
int adcOffset; // mV

// Temperature limits (°C/10)
int minCellTempCharge;
int minCellTempDischarge;
int maxCellTempCharge;
int maxCellTempDischarge;

//Shunt resistor value
static float shuntResistorValue_mOhm;

//main function prototypes -----------------------------------------------------
void init_AFE(void);
int beginAFEcommunication(void);

void setTemperatureLimitsint(int minDischarge_degC, int maxDischarge_degC, int minCharge_degC, int maxCharge_degC);
void setShuntResistorValue(float res_mOhm);
void setShortCircuitProtection(long current_mA, int delay_us);

//Printout serial monitor helper functions -------------------------------------
long AFE_getSetShortCircuitCurrent(void);
float AFE_getSetCurrentSenseRes(void);

#endif	/* XC_HEADER_TEMPLATE_H */

