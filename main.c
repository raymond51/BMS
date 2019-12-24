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
#include <stdint.h>
#include <stdbool.h>
#include "pic16f1719_internals.h"
#include "I2C.h"
#include "EUSART.h"
#include "RGB.h"
#include "BQ76920.h"

/**************************************************************
 * Defines 
 **************************************************************/
//states 
#define AWAIT_AFE_CONN 0 //await for response from the AFE, response occurs after boot 
#define AFE_INIT 1
#define READ_AFE_DATA 2


//timer values
#define countSecond 61

//value = fclk/(4*prescaler*256)
/**************************************************************
 * Function 
 **************************************************************/
void initClock(void);
void init_EUSART(void);
void init_GPIO(void);
void init_TMR1(void);
void statemachine(void);

/**************************************************************
 * Globals
 **************************************************************/
uint8_t currState = AWAIT_AFE_CONN;
volatile uint8_t tmr1_flag = 0; //flag cleared when tmr1 overflows

/**************************************************************
 * Interrupts
 **************************************************************/
void __interrupt() myIsr(void) { // High priority interrupt

    static uint8_t count = 0;

    if (PIR1bits.TMR1IF && PIE1bits.TMR1IE) {//checking for overflow flag and if timer interrupt enabled
        PIR1bits.TMR1IF = 0; //clear interrupt flag for timer 1
        count++;

        // Blink every second
        if (count == countSecond) {
            tmr1_flag = 1;
            count = 0;
        }

    }

}

void main(void) {

    //Peripherals
    initClock(); //initialise and set internal high frequency clock
    init_GPIO(); //configuring PPS
    init_I2C(); //configure i2c to 100kHz
    EUSART_Initialize(9600); //begin UART communication at 9600baud
    init_TMR1(); //Enable timer 1 to repeatedly communicate with the AFE until boot
    init_RGB(); //set initially RGB all off

    while (1) {

        statemachine();

        __delay_ms(10);
    }

    return;
}

void statemachine(void) {

    switch (currState) {
        case AWAIT_AFE_CONN:

            if (tmr1_flag) {
                tmr1_flag = 0; //clear flag
                RGB_AWAIT_AFE_CONN(); //toggle to the color to indicate attempt to connect to AFE
                uint8_t success = beginAFEcommunication(); //send i2c command to request for communication

#ifdef BQ76920_DEBUG
                EUSART_Write_Text("Attempting to communicate with AFE...\n\r");
#endif
                if (success) {
                    T1CONbits.TMR1ON = 0; // disable timer1
                    PIE1bits.TMR1IE = 0; // disable timer1 interrupt
                    RGB_color(RGB_RED);
                    tmr1_flag = 0; //clear flag

#ifdef BQ76920_DEBUG
                    __delay_ms(5); //allow time for i2c communication to end
                    EUSART_Write_Text("Communication with BQ76920 AFE established!\n\r"); //print to terminal if success check 
                    snprintf(messageBuffer, messageBuf_size, "Obtained adcOffset = %i and adcGain = %i\n\r", adcOffset, adcGain);
                    EUSART_Write_Text(messageBuffer); //print to terminal if success check 
                    EUSART_Write_Text("Attempt to set initial system parameters to AFE...\n\r");
#endif
                    currState = AFE_INIT; //move to next state if communication was successful check the return value
                }


            }

            break;
        case AFE_INIT:
            //initialise AFE
            init_AFE();

#ifdef BQ76920_DEBUG
            __delay_ms(5); //allow time for i2c communication to end
            snprintf(messageBuffer, messageBuf_size, "Set short circuit current for AFE: %lu !\n\r", AFE_getSetShortCircuitCurrent());
            EUSART_Write_Text(messageBuffer);
            EUSART_Write_Text("Initial parameters for BQ76920 AFE set!\n\r");
            EUSART_Write_Text("Now reading AFE data at regular intervals.\n\r");
#endif
            RGB_color(RGB_GREEN);//if success set the rgb led to solid GREEN
            currState = READ_AFE_DATA; //move to next state if communication was successful check the return value
            break;
        case READ_AFE_DATA:
               
            //enable the 1 second timer again
            //perform update 
            //once update complete enter sleep mode 
            
            break;
    }
}


void initClock() {
    // Run at 16 MHz (internal clock)
    internal_16();

}

/*
 @brief: enable timer 1 to repeatedly communicate with AFE
 */
void init_TMR1(void) {

    // pre-scaler = 1
    T1CONbits.T1CKPS = 0b00;
    // Set TMR1 to 0
    TMR1 = 0;
    // enable timer1
    T1CONbits.TMR1ON = 1;
    // enable timer1 interrupt
    PIE1bits.TMR1IE = 1;
    // enable peripheral interrupt
    INTCONbits.PEIE = 1;
    // enable global interrupts
    ei();

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
    TRISAbits.TRISA4 = 0; //set as output for RED led
    TRISAbits.TRISA5 = 0; //set as output for GREEN led
    TRISEbits.TRISE0 = 0; //set as output for BLUE led

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