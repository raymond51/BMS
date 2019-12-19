#include "BQ76920.h"

/*
 @brief: atempt to communicate with the AFE by sending i2c address, await for address response 
 */
int beginAFEcommunication(void) {

    int commSuccess = 0;

    // initialize variables
    for (int i = 0; i < 4; i++) {
        cellVoltages[i] = 0;
    }

    // test communication
    I2C_writeRegister(AFE_BQ76920_I2C_ADDRESS, CC_CFG, 0x19); // should be set to 0x19 according to datasheet

    if (readRegister(AFE_BQ76920_I2C_ADDRESS, CC_CFG) == 0x19) {
        commSuccess = 1;


        // initial settings for bq769x0
        I2C_writeRegister(AFE_BQ76920_I2C_ADDRESS,SYS_CTRL1, 0x18); // switch external thermistor and ADC on - in byte: B00011000
        I2C_writeRegister(AFE_BQ76920_I2C_ADDRESS,SYS_CTRL2, 0x40); // switch CC_EN on - in byte: B01000000

        // get ADC offset and gain
        adcOffset = (signed int) readRegister(AFE_BQ76920_I2C_ADDRESS,ADCOFFSET); // convert from 2's complement
        adcGain = 365 + (((readRegister(AFE_BQ76920_I2C_ADDRESS,ADCGAIN1) & 0x0C) << 1) |
                ((readRegister(AFE_BQ76920_I2C_ADDRESS,ADCGAIN2) & 0xE0) >> 5)); // uV/LSB
        
    }



    return commSuccess;

}