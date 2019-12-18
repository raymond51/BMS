//includes ---------------------------------------------------------------------
#include "I2C.h"

//function ---------------------------------------------------------------------

/*
 @brief: setup MSSP as I2C master mode, clock speed: 100Khz and define pins using PPS
 */

void init_I2C() {

    //set master mode and clock speed-------------------------------------------
    
    SSPCONbits.SSPM = 0x08; // I2C Master mode, clock = Fosc/(4 * (SSPADD+1)) gives us 100khz when sspadd  = 39, check datasheet
    SSPCONbits.SSPEN = 1; // enable MSSP port
    // The SSPADD register value is used to determine the clock rate for I2C
    // communication.
    // Equation for I2C clock rate: Fclock = Fosc/[(SSPADD +1)*4]

    // For this example we want the the standard 100Khz I2C clock rate and our
    // internal Fosc is 16Mhz so we get: 100000 = 16000000/ [(SSPADD+1)*4]
    // or solving for SSPADD = [(16000000/100000)-4]/4
    // and we get SSPADD = 39
    SSPADD = 39; // set Baud rate clock divider
    //Eq: CLOCK_FREQ/(4*i2cFreq))-1;

    __delay_ms(10); // let everything settle.
}

/*
 @brief: send one byte
 */
void send_I2C_data(unsigned int databyte) {
    PIR1bits.SSP1IF = 0; // clear SSP interrupt bit
    SSPBUF = databyte; // send databyte
    while (!PIR1bits.SSP1IF); // Wait for interrupt flag to go high indicating transmission is complete
}

/*
 @brief: read one byte
 */
unsigned int read_I2C_data(void) {
    PIR1bits.SSP1IF = 0; // clear SSP interrupt bit
    SSPCON2bits.RCEN = 1; // set the receive enable bit to initiate a read of 8 bits from the serial EEPROM
    while (!PIR1bits.SSP1IF); // Wait for interrupt flag to go high indicating transmission is complete
    return (SSPBUF); // Data from EEPROM is now in the SSPBUF so return that value
}

/*
 @brief: Send control byte to SEE (this includes 4 bits of device code, block select bits and the R/W bit)
 */
void send_I2C_controlByte(unsigned int BlockAddress, unsigned int RW_bit) {
    PIR1bits.SSP1IF = 0; // clear SSP interrupt bit
    // Assemble the control byte from device code, block address bits and R/W bit
    // so it looks like this: CCCCBBBR
    // where 'CCCC' is the device control code
    // 'BBB' is the block address
    // and 'R' is the Read/Write bit
    SSPBUF = (((0b0000 << 4) | (BlockAddress << 1)) + RW_bit);
    // send the control byte
    while (!PIR1bits.SSP1IF); // Wait for interrupt
}

void send_I2C_startBit(void) {
    PIR1bits.SSP1IF = 0; // clear SSP interrupt bit
    SSPCON2bits.SEN = 1; // send start bit
    while (!PIR1bits.SSP1IF); // Wait for the SSPIF bit to go back high before we load the data buffer
}

void send_I2C_stopBit(void) {
    PIR1bits.SSP1IF = 0; // clear SSP interrupt bit
    SSPCON2bits.PEN = 1; // send stop bit
    while (!PIR1bits.SSP1IF); // Wait for interrupt flag to go high indicating\transmission is complete
}

void send_I2C_ACK(void) {
    PIR1bits.SSP1IF = 0; // clear SSP interrupt bit
    SSPCON2bits.ACKDT = 0; // clear the Acknowledge Data Bit - this means we are sending an Acknowledge or 'ACK'
    SSPCON2bits.ACKEN = 1; // set the ACK enable bit to initiate transmission of the ACK bit to the serial EEPROM
    while (!PIR1bits.SSP1IF); // Wait for interrupt flag to go high indicating transmission is complete
}

void send_I2C_NACK(void) {
    PIR1bits.SSP1IF = 0; // clear SSP interrupt bit
    SSPCON2bits.ACKDT = 1; // set the Acknowledge Data Bit- this means we are sending a No-Ack or 'NAK'
    SSPCON2bits.ACKEN = 1; // set the ACK enable bit to initiate transmission of the ACK bit to the serial EEPROM
    while (!PIR1bits.SSP1IF); // Wait for interrupt flag to go high/indicating transmission is complete
}

//------------------------------------------------------------------------------------------------------Communication procedures
/*
 @brief: Retrieve canbus data from the ATmega328 (arduino) through I2C, data received in order related to enum
 */
void retrieve_data_ATmega328(void) {
    
    send_I2C_startBit();
    send_I2C_controlByte(ATmega328_address, READ);

    /*
    for (int i = 0; i < BUFFER_SIZE; i++) {
        if (i < BUFFER_SIZE) {
            incoming_data[i] = read_I2C_data();
            send_I2C_ACK();
        } else {
            incoming_data[i] = read_I2C_data();
            send_I2C_NACK();
        }
    }
       */
    send_I2C_stopBit();

}

/*
 @brief: Send the data to address register of slave
 */
void I2C_writeRegister(int slaveAddress,int regAddress, int data){
    send_I2C_startBit();
    send_I2C_controlByte(slaveAddress, WRITE);
    send_I2C_data(regAddress); //write to the reg address of slave 
    send_I2C_data(data);
    send_I2C_stopBit();
}

/*
 @brief: Retrieve a single byte of data from the slave
 */

/*
void readRegister(int regAddress, int data){
    send_I2C_startBit();
    send_I2C_controlByte(regAddress, WRITE);
    send_I2C_data(data)
    send_I2C_stopBit();
}
 */