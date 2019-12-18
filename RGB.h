/* 
 * File:   RGB.h
 *PIC: 16F1719 @16MHZ
 * compiler: XC8
 * 
 *Program Description: Header file to control the on-board RGB module
*/

#ifndef RGB_H
#define	RGB_H


//includes ---------------------------------------------------------------------
#include <xc.h> 

//defines
#define RGB_RED 0
#define RGB_GREEN 1 
#define RGB_BLUE 2

//main function prototypes -----------------------------------------------------
void init_RGB();
void RGB_color(int color);


#endif

