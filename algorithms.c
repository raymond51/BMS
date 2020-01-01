#include "algorithms.h"

void watchdog_timeout_shutdown() {
    //check if watchdog timer expired
    if (!STATUSbits.nTO) {
        RGB_color(RGB_RED);
         __delay_ms(2000); 
         LATAbits.LATA3 = 1; //unlatch the relay  to shutdown bms system
    }
}