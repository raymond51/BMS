/*
 * File: Main.c
 * Author: Raymond
 * PIC: 16F1719 w/Int OSC @ 16MHz, 5v
 * Compiler: XC8 (v1.38, MPLAX X v3.40)
 *
 * Program Description: This Program Allows PIC16F1719 to communicate with the Analog front end chip
 *
 * Created November, 2019
 */

/**************************************************************
 *Includes 
 **************************************************************/
#include <xc.h>
#include <stdio.h>
#include <stdlib.h> // used for random number generation
#include "pic16f1719_internals.h"
#include "I2C.h"
#include "EUSART.h"

/**************************************************************
 * Function 
 **************************************************************/
void initClock(void);
void init_EUSART(void);
void init_main(void);


void main(void) {
    
    initClock(); //initialise and set internal high frequency clock
    init_main();
    //init_I2C();


    //EUSART_Initialize(19200);
    
    
    return;
}

void initClock() {
    // Run at 16 MHz
    //internal_16();

}

void init_main() {


    /////////////////////
    // Setup EUSART
    ////////////////////
    TRISBbits.TRISB2 = 0;
    ANSELBbits.ANSB2 = 0;
    TRISBbits.TRISB3 = 1;
    ANSELBbits.ANSB3 = 0;

    /////////////////////
    // Setup pins for I2C
    ////////////////////
    ANSELCbits.ANSC4 = 0;
    ANSELCbits.ANSC5 = 0;
    TRISCbits.TRISC4 = 1;
    TRISCbits.TRISC5 = 1;


    /////////////////////
    // Define peripheral pin select
    ////////////////////

    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 0x00; // unlock PPS


    RC4PPSbits.RC4PPS = 0x0011; //RC4->MSSP:SDA;
    SSPDATPPSbits.SSPDATPPS = 0x0014; //RC4->MSSP:SDA;

    SSPCLKPPSbits.SSPCLKPPS = 0x0015; //RC5->MSSP:SCL;
    RC5PPSbits.RC5PPS = 0x0010; //RC5->MSSP:SCL;

    RB2PPSbits.RB2PPS = 0x14; //RB2->EUSART:TX;
    RXPPSbits.RXPPS = 0x0B; //RB3->EUSART:RX;

    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 0x01; // lock PPS

}