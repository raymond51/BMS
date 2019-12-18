#include "BQ76920.h"


/*
 @brief: atempt to communicate with the AFE by sending i2c address, await for address response 
 */
uint8_t beginAFEcommunication(void){
    
   uint8_t errCode = 0;
    
  // initialize variables
  for (int i = 0; i < 4; i++) {
    cellVoltages[i] = 0;
  }
   
    // test communication
   I2C_writeRegister(AFE_BQ76920_I2C_ADDRESS,CC_CFG,0x19); // should be set to 0x19 according to datasheet
  
  return errCode;
  
}