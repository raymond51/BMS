#include "BQ76920.h"

//When a global variable is made static, its scope is restricted to the current file.
static regPROTECT1_t protect1;

void init_AFE(void) {

    setTemperatureLimitsint(-20, 45, 0, 45); //set temperature limit - minDischarge_Deb, maxDischarge_degC, minCharge_degC, maxCharge_degC
    setShuntResistorValue(0.02); //set shunt resistor value table, our value of resistor is 20mOhms
    setShortCircuitProtection(4000, 200); //set short circuit protection
    //set over current charge protection
    //set overcurrent discharge protection
    //set cell under voltage protection
    //set cell overvoltage protection

    //set balancing threshold
    //set idle current threshold
    //enable auto cell balancing
    //set the green led output for the RGB led
}

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
        I2C_writeRegister(AFE_BQ76920_I2C_ADDRESS, SYS_CTRL1, 0x18); // switch external thermistor and ADC on - in byte: B00011000
        I2C_writeRegister(AFE_BQ76920_I2C_ADDRESS, SYS_CTRL2, 0x40); // switch CC_EN on - in byte: B01000000

        // get ADC offset and gain
        adcOffset = (signed int) readRegister(AFE_BQ76920_I2C_ADDRESS, ADCOFFSET); // convert from 2's complement
        adcGain = 365 + (((readRegister(AFE_BQ76920_I2C_ADDRESS, ADCGAIN1) & 0x0C) << 1) |
                ((readRegister(AFE_BQ76920_I2C_ADDRESS, ADCGAIN2) & 0xE0) >> 5)); // uV/LSB

    }



    return commSuccess;

}

/*
 @brief: 
 */
void setTemperatureLimitsint(int minDischarge_degC, int maxDischarge_degC, int minCharge_degC, int maxCharge_degC) {
    // Temperature limits (°C/10)
    minCellTempDischarge = minDischarge_degC * 10;
    maxCellTempDischarge = maxDischarge_degC * 10;
    minCellTempCharge = minCharge_degC * 10;
    maxCellTempCharge = maxCharge_degC * 10;
}

/*
 @brief: Select the usage of reference table in short circuit in discharge threshold setting (SCD register in AFE)
 * Only choice avaliable is value of 1 or 0 as we have two tables to select from
 */
void setShuntResistorValue(float res_mOhm) {
    shuntResistorValue_mOhm = res_mOhm;
}

/*
 @brief: sequential search in the SCD sense resistor value to find if set value exists, in the table we have 8 configurable current (for table RSNS = 1) threshold settings
 * we use the same method is used for setting the delay check time (or short circuit sample time)
 */
void setShortCircuitProtection(long current_mA, int delay_us) {
    //minimum allowable current short possible is 2.2A according to table
    /*
     v=IR if lowest voltage differential is 40mV then I = 40mV/20mOhms  = 2.2A
     */
    float scaler = 1000.0; //negate mv unit
    protect1.bits.SCD_THRESH = 0;//set initial current limiting if inserted value is below threshold
        for (int i = 0; i < 8; i++) {
            if ( ((current_mA * shuntResistorValue_mOhm * scaler) / 1000.0) >= SCD_threshold_setting[i]) {
                protect1.bits.SCD_THRESH = i;
            }
        }

        protect1.bits.SCD_DELAY = 0;
        for (int i = sizeof (SCD_delay_setting) - 1; i > 0; i--) {
            if (delay_us >= SCD_delay_setting[i]) {
                protect1.bits.SCD_DELAY = i;
                break;
            }
        }

        //write these data to BQchip
    }

    /**************************************************************
     * Printout serial monitor helper functions
     **************************************************************/

    float AFE_getSetCurrentSenseRes() {
        return (float) shuntResistorValue_mOhm;
    }

    /*
    uint8_t AFE_getSetTemperatureLimit(){
        return (uint8_t) ;
    }
     */

    long AFE_getSetShortCircuitCurrent(){
        return (long) (SCD_threshold_setting[protect1.bits.SCD_THRESH]) / shuntResistorValue_mOhm;
    }

