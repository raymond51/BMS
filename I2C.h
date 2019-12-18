/* 
 * File:   I2C.h
 * Author: Ray
 *
 * Created on 12 May 2019, 12:41
 */

/* 
 * File:   pic16f1719_internals.h
 * Author: Ray
 *PIC: 16F1719 @16MHZ
 * compiler: XC8
 * 
 *Program Description: Header enables setup of I2C protocol
 * Created on 09 May 2019, 23:18
*/

#ifndef I2C_H
#define	I2C_H

//includes ---------------------------------------------------------------------
#include "pic16f1719_internals.h"
//defines
#define WRITE 0 
#define READ 1
//const and variables ----------------------------------------------------------
//-------------------ATMega328 
const int ATmega328_address = 0x02; // set block address to receive message from canbus

//main I2C function prototypes ----------------------------------------------------------
void init_I2C(void);
void send_I2C_data(unsigned int databyte);
unsigned int read_I2C_data(void);
void send_I2C_controlByte(unsigned int BlockAddress,unsigned int RW_bit);
void send_I2C_startBit(void);
void send_I2C_stopBit(void);
void send_I2C_ACK(void);
void send_I2C_NACK(void);
//sub I2C function prototypes ----------------------------------------------------------
void retrieve_data_ATmega328(void);

#endif	/* I2C_H */

