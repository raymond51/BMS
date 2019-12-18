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
#include <stdlib.h>
#include "pic16f1719_internals.h"
#include "I2C.h"
#include "EUSART.h"
#include "RGB.h"

/**************************************************************
 * Function 
 **************************************************************/
void initClock(void);
void init_EUSART(void);
void init_GPIO(void);
void init_AFE(void);
void statemachine(void);

void main(void) {
    
    //Peripherals
    initClock(); //initialise and set internal high frequency clock
    init_GPIO(); //configuring PPS
    init_I2C(); //configure i2c to 100kHz
    EUSART_Initialize(19200);
    init_RGB(); //set initially RGB all off
    RGB_color(RGB_RED);//set the RGB led red to initially to signal no communication with AFE chip
    //BMS boot/initialisation
    init_AFE(); 
    
    while(1){
    
      statemachine();
    
    }
    
    return;
}

void statemachine(void){
        
    
    
}

void init_AFE(void){
    //begin communication
    //set temperature limit
    //set shunt resistor value
    //set short circuit protection
    //set over current charge protection
    //set overcurrent discharge protection
    //set cell under voltage protection
    //set cell overvoltage protection
    
    //set balancing threshold
    //set idle current threshold
    //enable auto cell balancing
    //set the green led output for the RGB led
}

void initClock() {
    // Run at 16 MHz (internal clock)
    internal_16();

}

void init_GPIO() {


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
    // Setup interrupt pin for ALERT
    ////////////////////
    
    /////////////////////
    // Setup RGB LED pins
    ////////////////////
    TRISAbits.TRISA4 = 0;//set as output for RED led
    TRISAbits.TRISA5 = 0;//set as output for GREEN led
    TRISEbits.TRISE0 = 0;//set as output for BLUE led
    
    /////////////////////
    // Define peripheral pin select
    ////////////////////

    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 0x00; // unlock PPS


    //Configure i2c pins
    RC4PPSbits.RC4PPS = 0x0011; //RC4->MSSP:SDA;
    SSPDATPPSbits.SSPDATPPS = 0x0014; //RC4->MSSP:SDA;
    SSPCLKPPSbits.SSPCLKPPS = 0x0015; //RC5->MSSP:SCL;
    RC5PPSbits.RC5PPS = 0x0010; //RC5->MSSP:SCL;
    
    //Configure the UART pins
    RB2PPSbits.RB2PPS = 0x14; //RB2->EUSART:TX;
    RXPPSbits.RXPPS = 0x0B; //RB3->EUSART:RX;

    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 0x01; // lock PPS

}