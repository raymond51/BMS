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

#define MAX_NUMBER_OF_CELLS 5
#define MAX_NUMBER_OF_THERMISTORS 1
// IC type/size
#define bq76920 1
#define AFE_BQ76920_I2C_ADDRESS 0x18

//cell data
int cellVoltages[MAX_NUMBER_OF_CELLS]; // mV


// output information to serial console for debugging, comment out to remove
#define BQ76920_DEBUG


//main function prototypes -----------------------------------------------------
int beginAFEcommunication(void);

#endif	/* XC_HEADER_TEMPLATE_H */

