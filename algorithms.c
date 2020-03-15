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