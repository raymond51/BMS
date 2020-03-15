/* 
 * File:   Algorithms.h
 *PIC: 16F1719 @16MHZ
 * compiler: XC8
 * 
 *Program Description: Header file to enable safety algorithms for the lithium ion cells
*/

#ifndef ALGORITHMS_H
#define	ALGORITHMS_H

//includes ---------------------------------------------------------------------
#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdio.h>
#include "pic16f1719_internals.h"
#include "RGB.h"
#include "BQ76920.h"

/**************************************************************
 * Defines 
 **************************************************************/
//states 
#define AWAIT_AFE_CONN 0 //await for response from the AFE, response occurs after boot 
#define AFE_INIT 1 //provide the BQ AFE chip information via i2c
#define READ_AFE_DATA 2 //state to constantly read and update the BQ AFE chip, also kick watchdog 
#define AFE_COMM_ERROR 3

#define MEASUREMENT_DELAY 5000 // the duration between each measurement


uint8_t currState = AWAIT_AFE_CONN;

//const and variables ----------------------------------------------------------

//function prototypes ----------------------------------------------------------
void watchdog_timeout_shutdown(void);
void shutdown_BMS(void);
void calibrate_BATTSOC(void);
void coulomb_counter(void);

#endif

