#include "config.h"
#include "gpio.h"
#include "timer.h"
#include <p32xxxx.h>

/***	BTNC_Init
**
**	Parameters:
**		
**
**	Return Value:
**		
**
**	Description:
**		This function initializes the button BTNC by configuring the pin as input and setting up the interrupt.
**      It assigns INT4 to the pin RF0 and configures the interrupt on the falling edge with a specified priority and sub-priority.
**      
**          
*/
void BTNC_Init() {
    tris_BTN_BTNC = 1; // Configures PF0 as input

    // Configure INT4R register
    INT4R = 0b0100; // Assign INT4 to pin RF0 (port F, pin 0)

    // Configure interrupt
    INTCONbits.INT4EP = 0; // Configure interrupt on falling edge
    IPC4bits.INT4IP = 7;   // Set interrupt priority for INT4
    IPC4bits.INT4IS = 3;   // Set interrupt sub-priority for INT4
    
    macro_enable_interrupts();
    
    IFS0bits.INT4IF = 0;   // Reset interrupt flag for INT4
    IEC0bits.INT4IE = 1;   // Enable interrupt for INT4
}

/***	RGB_Init
**
**	Parameters:
**		
**
**	Return Value:
**		
**
**	Description:
**		This function initializes the RGB LED by configuring the pins and setting the initial state to off.
**      
**          
*/
void RGB_Init() {
    RGB_ConfigurePin();
    
    lat_RGB_R = 0;
    lat_RGB_G = 0;
    lat_RGB_B = 0;
}

/***	RGB_ConfigurePin
**
**	Parameters:
**		
**
**	Return Value:
**		
**
**	Description:
**		This function configures the pins used for the RGB LED.
**      It sets the pins as digital outputs and disables analog mode for the relevant pins.
**      
**          
*/
void RGB_ConfigurePin() {
    tris_RGB_R = 0;
    tris_RGB_G = 0;
    tris_RGB_B = 0;
    
    // disable analog (set pins as digital)
    ANSELDbits.ANSD2 = 0;
    ANSELDbits.ANSD3 = 0;
}

/***	RGB_SetValue
**
**	Parameters:
**      char r - Red value (0 or 1).
**      char g - Green value (0 or 1).
**      char b - Blue value (0 or 1).
**
**	Return Value:
**		
**
**	Description:
**		This function sets the RGB LED to the specified values for red, green, and blue.
**      
**          
*/
void RGB_SetValue(char r, char g, char b) {
    lat_RGB_R = r;
    lat_RGB_G = g;
    lat_RGB_B = b;
}

/***	RED_Pulse
**
**	Parameters:
**      int times - Number of times to pulse the red LED.
**
**	Return Value:
**		
**
**	Description:
**		This function pulses the red LED on and off for the specified number of times.
**      Each pulse consists of turning the LED on for 250 milliseconds and then off for 250 milliseconds.
**      
**          
*/
void RED_Pulse(int times) {
    for (int i = 0; i < times; i++) {
        TIMER2_DelayMS(250);
        RGB_SetValue(1, 0, 0);
        TIMER2_DelayMS(250);
        RGB_SetValue(0, 0, 0);
    }
}
