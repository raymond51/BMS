//includes ---------------------------------------------------------------------
#include "pic16F1719_internals.h"

//function ---------------------------------------------------------------------

/*
 @brief: Set to 32MHz
 */
void internal_32(void){
//Clock determined by FOSC in configuration bits
SCS0 = 0;
SCS1 = 0;
//Frequency select bits
IRCF0 = 0;
IRCF1 = 1;
IRCF2 = 1;
IRCF3 = 1;
//SET PLLx4 ON
SPLLEN = 1;
}


/*
 @brief: Set to 16MHz
 */
void internal_16(void){
//Clock determined by FOSC in configuration bits
SCS0 = 0;
SCS1 = 0;
//Frequency select bits
IRCF0 = 1;
IRCF1 = 1;
IRCF2 = 1;
IRCF3 = 1;
//SET PLLx4 OFF
SPLLEN = 0;
}

/*
 @brief: Set to 8MHz
 */
void internal_8(void){
//Clock determined by FOSC in configuration bits
SCS0 = 0;
SCS1 = 0;
//Frequency select bits
IRCF0 = 0;
IRCF1 = 1;
IRCF2 = 1;
IRCF3 = 1;
//SET PLLx4 OFF
SPLLEN = 0;
}

/*
 @brief: Set to 4MHz
 */
void internal_4(void){
//Clock determined by FOSC in configuration bits
SCS0 = 0;
SCS1 = 0;

IRCF0 = 1;
IRCF1 = 0;
IRCF2 = 1;
IRCF3 = 1;
//SET PLLx4 OFF
SPLLEN = 0;
}