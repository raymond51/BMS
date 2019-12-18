#include "RGB.h"

/*
 @brief: set all the RGB pins high as RGB light in active low state
 */
void init_RGB() {
    // Set pin High
    LATAbits.LATA4 = 1;
    LATAbits.LATA5 = 1;
    LATEbits.LATE0 = 1;

}

/*
 @brief: output the desired single RGB color
 */
void RGB_color(int color) {

    switch (color) {
        case 0:
            LATAbits.LATA4 = 0;
            LATAbits.LATA5 = 1;
            LATEbits.LATE0 = 1;
            break;
        case 1:
            LATAbits.LATA4 = 1;
            LATAbits.LATA5 = 0;
            LATEbits.LATE0 = 1;
            break;
        case 2:
            LATAbits.LATA4 = 1;
            LATAbits.LATA5 = 1;
            LATEbits.LATE0 = 0;
            break;

    }

}

/*
 @brief: RGB color to indicate AFE connection status, called by interrupt to toggle
 */
void RGB_AWAIT_AFE_CONN() {

    if (toggleColor) {
        toggleColor = !toggleColor;
        RGB_color(RGB_RED);
    } else {
        toggleColor = !toggleColor;
        RGB_color(RGB_GREEN);
    }

}