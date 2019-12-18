/*
 * File:   EUSART.c
 * Author: Raymond
 *Library Description: Provides ability to communicate with the transiever module
 * Created on 08 August 2019, 19:11
 */


#ifndef EUSART_H
#define	EUSART_H

//includes ---------------------------------------------------------------------
#include "pic16f1719_internals.h"
//defines-----------------------------------------------------------------------

//const and variables ----------------------------------------------------------

//main function prototypes -----------------------------------------------------
char EUSART_Initialize(const long int baudrate);
uint8_t EUSART_Read(void);
void EUSART_Write(uint8_t txData);
void EUSART_Write_Text(char *text);
void EUSART_Read_Text(char *Output, unsigned int length);
#endif
