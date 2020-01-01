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
//const and variables ----------------------------------------------------------


//function prototypes ----------------------------------------------------------
void watchdog_timeout_shutdown(void);


#endif

