#include "BQ76920.h"

void init_AFE(void) {

    setTemperatureLimitsint(-20, 45, 0, 45); //set temperature limit - minDischarge_Deb, maxDischarge_degC, minCharge_degC, maxCharge_degC
    setShuntResistorValue(0.02); //set shunt resistor value table, our value of resistor is 20mOhms
    //setShortCircuitProtection(2500, 200); //set short circuit protection
    
    setShortCircuitProtection(500, 200); //set short circuit protection
    
    //set over current charge protection
    //setOverCurrentDischargeProtection(20000, 320); //set overcurrent discharge protection
    
    setOverCurrentDischargeProtection(10, 320); //set overcurrent discharge protection
    setCellUndervoltageProtection(2900, 2); //set cell under voltage protection
    setCellOvervoltageProtection(4100,2);//set cell overvoltage protection

    //***Balance algorithms
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
    I2C_writeRegister(AFE_BQ76920_I2C_ADDRESS, CC_CFG, 0x19); // should be set to 0x19 according to datasheet, a message should be returned

    if (readRegister(AFE_BQ76920_I2C_ADDRESS, CC_CFG) == 0x19) {
        commSuccess = 1;


        // initial settings for bq769x0
        I2C_writeRegister(AFE_BQ76920_I2C_ADDRESS, SYS_CTRL1, 0x18); // switch external thermistor and ADC on - in byte: B00011000, seen in datasheet
        I2C_writeRegister(AFE_BQ76920_I2C_ADDRESS, SYS_CTRL2, 0x40); // switch CC_EN on - in byte: B01000000, enable continuous reading of couloumb counting

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
    protect1.bits.SCD_THRESH = 0; //set initial current limiting if inserted value is below threshold
    for (int i = 0; i < arrSize(SCD_threshold_setting) - 1; i++) {
        if (((current_mA * shuntResistorValue_mOhm * scaler) / 1000.0) >= SCD_threshold_setting[i]) {
            protect1.bits.SCD_THRESH = i;
        }
    }

    protect1.bits.SCD_DELAY = 0;
    for (int i = 0; i < arrSize(SCD_threshold_setting) - 1; i++) {
        if (delay_us >= SCD_delay_setting[i]) {
            protect1.bits.SCD_DELAY = i;
        }
    }
    protect1.bits.RSNS = 0; //enable lower input range for detection of lower current trip
    
    I2C_writeRegister(AFE_BQ76920_I2C_ADDRESS, PROTECT1, protect1.regByte); //transmit compact bit field as byte to AFE

}

/*
 @brief:
 */
void setOverCurrentDischargeProtection(long current_mA, int delay_ms) {


    float scaler = 1000.0; //negate mv unit
    protect2.bits.OCD_THRESH = 0;
    for (int i = 0; i < arrSize(OCD_threshold_setting) - 1; i++) {
        if (((current_mA * shuntResistorValue_mOhm * scaler) / 1000.0) >= OCD_threshold_setting[i]) {
            protect2.bits.OCD_THRESH = i;
        }
    }

    protect2.bits.OCD_DELAY = 0;
    for (int i = 0; i < arrSize(OCD_delay_setting) - 1; i++) {
        if (delay_ms >= OCD_delay_setting[i]) {
            protect2.bits.OCD_DELAY = i;
        }
    }

    I2C_writeRegister(AFE_BQ76920_I2C_ADDRESS, PROTECT2, protect2.regByte); //transmit compact bit field as byte to AFE
}

/*
 @brief:
 */
void setCellUndervoltageProtection(int voltage_mV, int delay_s) {
    uint8_t uv_trip = 0;
    minCellVoltage = voltage_mV;

    protect3.regByte = readRegister(AFE_BQ76920_I2C_ADDRESS, PROTECT3);
    uv_trip = ((((long) voltage_mV - adcOffset) * 1000 / adcGain) >> 4) & 0x00FF;
    uv_trip += 1; // always round up for lower cell voltage
    I2C_writeRegister(AFE_BQ76920_I2C_ADDRESS, UV_TRIP, uv_trip);

    protect3.bits.UV_DELAY = 0;
    for (int i = 0; i < arrSize(UV_delay_setting) - 1; i++) {
        if (delay_s >= UV_delay_setting[i]) {
            protect3.bits.UV_DELAY = i;
        }
    }
    I2C_writeRegister(AFE_BQ76920_I2C_ADDRESS, PROTECT3, protect3.regByte);
}

/*
 @brief:
 */
void setCellOvervoltageProtection(int voltage_mV, int delay_s) {
    uint8_t ov_trip = 0;
    maxCellVoltage = voltage_mV;
    protect3.regByte = readRegister(AFE_BQ76920_I2C_ADDRESS, PROTECT3);
    ov_trip = ((((long) voltage_mV - adcOffset) * 1000 / adcGain) >> 4) & 0x00FF;
    I2C_writeRegister(AFE_BQ76920_I2C_ADDRESS, OV_TRIP, ov_trip);
    
    for (int i = 0; i < arrSize(OV_delay_setting) - 1; i++) {
        if (delay_s >= OV_delay_setting[i]) {
            protect3.bits.UV_DELAY = i;
        }
    }
    I2C_writeRegister(AFE_BQ76920_I2C_ADDRESS,PROTECT3, protect3.regByte);
}

/*
 @brief: Function to read values from AFE via i2c communication for cell info
 */
void AFE_UPDATE(){
    updateCurrent();//update the current reading
    updateVoltages();//update the voltages reading from 5 cells
    updateTemperatures();//update the temperature value reading from the battery pack
    
    //ypdate the balancing switch [for charging]
    enableDischarging(1);//enable dicharging [held in SYS_CTRL2 on 2nd bit], MUST BE ENALED ONLY WHEN NO SYSTEM ERROR
    enableCharging(1);//enable charging [held in SYS_CTRL2 on 1st bit], MUST BE ENALED ONLY WHEN NO SYSTEM ERROR

}

/*
 @brief: check if the flag CC new reading reg as it indicates new coulomb measurement
 */
void updateCurrent(){
   int adcVal = 0;
    regSYS_STAT_t sys_stat;
    sys_stat.regByte = readRegister(AFE_BQ76920_I2C_ADDRESS,SYS_STAT);
   
    //if (sys_stat.bits.CC_READY == 1){
       
    adcVal = (readRegister(AFE_BQ76920_I2C_ADDRESS, CC_HI_BYTE) << 8)| readRegister(AFE_BQ76920_I2C_ADDRESS, CC_LO_BYTE);
    batCurrent = -(adcVal * 8.44 / 5.0);  // mA, invert polarity due to correct hardware orientation
    
    if (batCurrent > -10 && batCurrent < 10)
    {
      batCurrent = 0;
    }
    
    I2C_writeRegister(AFE_BQ76920_I2C_ADDRESS, SYS_STAT, 0x80); // Clear CC ready flag by wriring 1 to it as specified in the datasheet
    //}
     
}

/*
 @brief: reads all cell voltages to array cellVoltages and updates the battery pack voltage (access the cell voltage register which is two bytes long must be be bit shifted)
 */
void updateVoltages(){
    
  long adcVal = 0;
  // read battery pack voltage
  adcVal = (readRegister(AFE_BQ76920_I2C_ADDRESS, BAT_HI_BYTE) << 8) | readRegister(AFE_BQ76920_I2C_ADDRESS, BAT_LO_BYTE);
  batVoltage = 4.0 * adcGain * adcVal / 1000.0 + 4 * adcOffset;
  
   //Functionality not 100%, concept remains. Required to have repeated start but the function hangs on i2c. Started reading manually via byte address
  /*
    //begin reading 
    send_I2C_startBit();
    send_I2C_controlByte(AFE_BQ76920_I2C_ADDRESS, WRITE);
    send_I2C_data(VC1_HI_BYTE);
    //send_I2C_repeatedStartCondition();
    //send_I2C_startBit();
    send_I2C_controlByte(AFE_BQ76920_I2C_ADDRESS, READ);
    for(int i=0;i<(MAX_NUMBER_OF_CELLS);i++){
     adcVal = 0;
     adcVal = ((read_I2C_data() & 0x3F ) << 8);
     send_I2C_ACK();
     adcVal = adcVal | read_I2C_data();
     if(i<(MAX_NUMBER_OF_CELLS)){ send_I2C_ACK(); }else{send_I2C_NACK(); }
     //(i < numberOfCells) ?  send_I2C_ACK() : send_I2C_NACK();
     cellVoltages[i] = (adcVal * adcGain / 1000) + adcOffset;
    }
    send_I2C_stopBit();
    */

    adcVal = ((readRegister(AFE_BQ76920_I2C_ADDRESS, VC1_HI_BYTE) & 0x3F) << 8) | readRegister(AFE_BQ76920_I2C_ADDRESS, VC1_LO_BYTE);
    cellVoltages[0] = (adcVal * adcGain / 1000) + adcOffset;
    adcVal = ((readRegister(AFE_BQ76920_I2C_ADDRESS, VC2_HI_BYTE) & 0x3F) << 8) | readRegister(AFE_BQ76920_I2C_ADDRESS, VC2_LO_BYTE);
    cellVoltages[1] = (adcVal * adcGain / 1000) + adcOffset;
    adcVal = ((readRegister(AFE_BQ76920_I2C_ADDRESS, VC3_HI_BYTE) & 0x3F) << 8) | readRegister(AFE_BQ76920_I2C_ADDRESS, VC3_LO_BYTE);
    cellVoltages[2] = (adcVal * adcGain / 1000) + adcOffset;
    adcVal = ((readRegister(AFE_BQ76920_I2C_ADDRESS, VC4_HI_BYTE) & 0x3F) << 8) | readRegister(AFE_BQ76920_I2C_ADDRESS, VC4_LO_BYTE);
    cellVoltages[3] = (adcVal * adcGain / 1000) + adcOffset;
    adcVal = ((readRegister(AFE_BQ76920_I2C_ADDRESS, VC5_HI_BYTE) & 0x3F) << 8) | readRegister(AFE_BQ76920_I2C_ADDRESS, VC5_LO_BYTE);
    cellVoltages[4] = (adcVal * adcGain / 1000) + adcOffset;
    
}

/*
 @brief: function to read the temperature on battery pack according to 10k NTC thermistor datasheet
 */
void updateTemperatures(){
     float tmp = 0;
     int adcVal = 0;
     int vtsx = 0;
     unsigned long rts = 0;
     
     adcVal = ((readRegister(AFE_BQ76920_I2C_ADDRESS, TS1_HI_BYTE) & 0x3F) << 8) | readRegister(AFE_BQ76920_I2C_ADDRESS, TS1_LO_BYTE);
     vtsx = adcVal * 0.382; // mV
     rts = 10000.0 * vtsx / (3300.0 - vtsx); // Ohm
         // Temperature calculation using Beta equation
    // - According to bq769x0 datasheet, only 10k thermistors should be used
    // - 25°C reference temperature for Beta equation assumed
     //tmp = 1.0/(1.0/(273.15+25) + 1.0/thermistorBetaValue*log(rts/10000.0)); // K
    
     //co-efficient a,b,c
     tmp = 1/ (-0.017428133935807 + 0.003137120345556*log(rts) + -0.000010382213834 *(log(rts)*log(rts)*log(rts)));
     
     temperatureThermistor = (tmp - 273.15)*100; 
  
}

/*
 @brief: function to set the AFE to enable the discharge fet, function can only be enabled if not serious fault detected
 */
void enableDischarging(unsigned int enable){
    uint8_t sys_ctrl2;
    sys_ctrl2 = readRegister(AFE_BQ76920_I2C_ADDRESS, SYS_CTRL2);
    if(enable){
    I2C_writeRegister(AFE_BQ76920_I2C_ADDRESS, SYS_CTRL2, sys_ctrl2 | 0x02); // switch DSCHG on
    }else{
    I2C_writeRegister(AFE_BQ76920_I2C_ADDRESS, SYS_CTRL2, sys_ctrl2 & ~(0x02));
    }
}

void enableCharging(unsigned int enable){
    uint8_t sys_ctrl2;
    sys_ctrl2 = readRegister(AFE_BQ76920_I2C_ADDRESS, SYS_CTRL2);
    if(enable){
    I2C_writeRegister(AFE_BQ76920_I2C_ADDRESS, SYS_CTRL2, sys_ctrl2 | 0x01); // switch CHG on
    }else{
    I2C_writeRegister(AFE_BQ76920_I2C_ADDRESS, SYS_CTRL2, sys_ctrl2 & ~(0x01));
    }
}


/**************************************************************
 * Printout serial monitor helper functions
 **************************************************************/

float AFE_getSetCurrentSenseRes() {
    return (float) shuntResistorValue_mOhm;
}

long AFE_getSetShortCircuitCurrent() {
    return (long) (SCD_threshold_setting[protect1.bits.SCD_THRESH]) / shuntResistorValue_mOhm;
}

long AFE_getOverCurrentDischargeCurrent() {
    return (long) (OCD_threshold_setting[protect2.bits.OCD_THRESH]) / shuntResistorValue_mOhm;
}

void printcellParameters() {
    snprintf(messageBuffer, messageBuf_size, "Cell batt: %i ,%d, %d , %d, %d, %d Batt Curr: %i Temp: %i CTRL2: %i \n\r", batVoltage,cellVoltages[0],cellVoltages[1],cellVoltages[2],cellVoltages[3],cellVoltages[4], batCurrent, temperatureThermistor,readRegister(AFE_BQ76920_I2C_ADDRESS, SYS_CTRL2));
    EUSART_Write_Text(messageBuffer);
    snprintf(messageBuffer, messageBuf_size, "0x05 SYS_CTRL2: %i \n\r", readRegister(AFE_BQ76920_I2C_ADDRESS, SYS_CTRL2));
    EUSART_Write_Text(messageBuffer);
    snprintf(messageBuffer, messageBuf_size, "0x00 SYS_STAT: %i \n\r", readRegister(AFE_BQ76920_I2C_ADDRESS, SYS_STAT));
    EUSART_Write_Text(messageBuffer);
    snprintf(messageBuffer, messageBuf_size, "0x06 PROTECT1: %i \n\r", readRegister(AFE_BQ76920_I2C_ADDRESS, PROTECT1)); //expected 159
    EUSART_Write_Text(messageBuffer);
    snprintf(messageBuffer, messageBuf_size, "0x07 PROTECT2: %i \n\r", readRegister(AFE_BQ76920_I2C_ADDRESS, PROTECT2)); //expected 122
    EUSART_Write_Text(messageBuffer);
    snprintf(messageBuffer, messageBuf_size, "Current: %d \n\r", batCurrent);
    EUSART_Write_Text(messageBuffer);
    snprintf(messageBuffer, messageBuf_size, "cellval: %d SOC: %d \n\r", lookupTableSamsung_voltage[20],lookupTableSamsung_SOC[20]);
    EUSART_Write_Text(messageBuffer);
    snprintf(messageBuffer, messageBuf_size, "Temp: %d e-2\n\r", temperatureThermistor);
    EUSART_Write_Text(messageBuffer);
 }

void printotAFERegisters() {

    __delay_ms(5); //allow time for i2c communication to end

    EUSART_Write_Text("Printing out AFE register values...\n\r"); //newline
    EUSART_Write_Text("\n\r"); //newline
    snprintf(messageBuffer, messageBuf_size, "0x00 SYS_STAT: %i \n\r", readRegister(AFE_BQ76920_I2C_ADDRESS, SYS_STAT));
    EUSART_Write_Text(messageBuffer);
    snprintf(messageBuffer, messageBuf_size, "0x01 CELLBAL1: %i \n\r", readRegister(AFE_BQ76920_I2C_ADDRESS, CELLBAL1));
    EUSART_Write_Text(messageBuffer);
    snprintf(messageBuffer, messageBuf_size, "0x04 SYS_CTRL1: %i \n\r", readRegister(AFE_BQ76920_I2C_ADDRESS, SYS_CTRL1));
    EUSART_Write_Text(messageBuffer);
    snprintf(messageBuffer, messageBuf_size, "0x05 SYS_CTRL2: %i \n\r", readRegister(AFE_BQ76920_I2C_ADDRESS, SYS_CTRL2));
    EUSART_Write_Text(messageBuffer);
    snprintf(messageBuffer, messageBuf_size, "0x06 PROTECT1: %i \n\r", readRegister(AFE_BQ76920_I2C_ADDRESS, PROTECT1)); //expected 159
    EUSART_Write_Text(messageBuffer);
    snprintf(messageBuffer, messageBuf_size, "0x07 PROTECT2: %i \n\r", readRegister(AFE_BQ76920_I2C_ADDRESS, PROTECT2)); //expected 122
    EUSART_Write_Text(messageBuffer);
    snprintf(messageBuffer, messageBuf_size, "0x08 PROTECT3: %i \n\r", readRegister(AFE_BQ76920_I2C_ADDRESS, PROTECT3));
    EUSART_Write_Text(messageBuffer);
    snprintf(messageBuffer, messageBuf_size, "0x09 OV_TRIP: %i \n\r", readRegister(AFE_BQ76920_I2C_ADDRESS, OV_TRIP));
    EUSART_Write_Text(messageBuffer);
    snprintf(messageBuffer, messageBuf_size, "0x0A UV_TRIP: %i \n\r", readRegister(AFE_BQ76920_I2C_ADDRESS, UV_TRIP));
    EUSART_Write_Text(messageBuffer);
    snprintf(messageBuffer, messageBuf_size, "0x0B CC_CFG: %i \n\r", readRegister(AFE_BQ76920_I2C_ADDRESS, CC_CFG));
    EUSART_Write_Text(messageBuffer);
    snprintf(messageBuffer, messageBuf_size, "0x32 CC_HI: %i \n\r", readRegister(AFE_BQ76920_I2C_ADDRESS, CC_HI_BYTE));
    EUSART_Write_Text(messageBuffer);
    snprintf(messageBuffer, messageBuf_size, "0x33 CC_LO: %i \n\r", readRegister(AFE_BQ76920_I2C_ADDRESS, CC_LO_BYTE));
    EUSART_Write_Text(messageBuffer);
    EUSART_Write_Text("\n\r"); //newline
}
  