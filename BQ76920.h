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

#define MAX_NUMBER_OF_CELLS 5
#define MAX_NUMBER_OF_THERMISTORS 1
// IC type/size
#define bq76920 1
#define BMS_I2C_ADDRESS 0x18



// output information to serial console for debugging
#define BQ769X0_DEBUG 1



#endif	/* XC_HEADER_TEMPLATE_H */

