#include "algorithms.h"

void watchdog_timeout_shutdown() {
    //check if watchdog timer expired
    if (!STATUSbits.nTO) {
        RGB_color(RGB_RED);
         __delay_ms(2000); 
         currState = AFE_COMM_ERROR;
        //hutdown_BMS();
    }
}


/*
void shutdown_BMS(){
 LATAbits.LATA3 = 1; //unlatch the relay to shutdown bms system
}
*/

/*
 @brief: compares the read value during unloaded cell to determine the OCV SOC
 */
void calibrate_BATTSOC(void){

        //loop through all 5 cells
    for(int i = 0; i<MAX_NUMBER_OF_CELLS;i++){
        for(int j = 0; j < LUT_SIZE; j++ ){
            if(cellVoltages[i]>=lookupTableSamsung_voltage[j]){
                cellSOC[i] = lookupTableSamsung_SOC[j];
                cellCharge[i] = (lookupTableSamsung_SOC[j]/100.0) * samsung_cell_max_charge; 
                break;
            }
        }
    }
    
}

void coulomb_counter(void){
    for(int i = 0; i<MAX_NUMBER_OF_CELLS;i++){
       cellCharge[i] = cellCharge[i] - (batCurrent * (MEASUREMENT_DELAY/ 3600000.0));
       cellSOC[i] = ((cellCharge[i] * 100.0 )/samsung_cell_max_charge); 
     }
}

int AFE_Status(void){
    
    regSYS_STAT_t sys_stat;
    int error_flag = 0;
    sys_stat.regByte = readRegister(AFE_BQ76920_I2C_ADDRESS, SYS_STAT);
  
    //check if error bits triggered, check the bits at position
    // XR error - B00100000
    if (sys_stat.regByte & 0x20){
        XR_error = 1;
        error_flag=1;
    }
    // Alert error - B00010000
    if(sys_stat.regByte & 0x10){
        alert_error = 1;
        error_flag=1;
    }
    // UV error - B00001000
    if(sys_stat.regByte & 0x08 ){
        uv_error = 1;
        error_flag=1;
    }
    // OV error - B00000100
    if(sys_stat.regByte & 0x04 ){
        ov_error = 1;
        error_flag=1;
    }
    // SCD error - B00000010
   if(sys_stat.regByte & 0x02 ){
        scd_error = 1;
        error_flag=1;
    }
   // OCD error - B00000001
   if(sys_stat.regByte & 0x01){
        ocd_error = 1;
        error_flag=1;
    }
    
    return error_flag;
}

void AFE_FET_Status(void){
    uint8_t sys_ctrl2;
    sys_ctrl2 = readRegister(AFE_BQ76920_I2C_ADDRESS, SYS_CTRL2);
    chg_fet_enable = 0;
    dschg_fet_enable = 0;
 
   if(sys_ctrl2 & 0x01){
       chg_fet_enable = 1;
    }
 
   if(sys_ctrl2 & 0x02){
       dschg_fet_enable = 1;
    }
}