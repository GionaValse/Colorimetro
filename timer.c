#include <p32xxxx.h>
#include "timer.h"
#include "config.h"

/***	TIMER2_Init
**
**	Parameters:
**		
**
**	Return Value:
**		
**
**	Description:
**		This function initializes Timer 2 by configuring the necessary pins.
**      
**          
*/
void TIMER2_Init() {
    TIMER2_ConfigurePins();
}

/***	TIMER2_ConfigurePins
**
**	Parameters:
**		
**
**	Return Value:
**		
**
**	Description:
**		This function configures the settings for Timer 2.
**      It sets the timer to 16-bit mode, applies a prescaler of 64, and initializes the period register for a 1ms period.
**      
**          
*/
void TIMER2_ConfigurePins() {
    T2CONbits.ON = 0; // disable timer
    T2CONbits.T32 = MODE_16; // 16 bit mode
    T2CONbits.TCKPS = PRS_64; // prescaler set to 64
    T2CONbits.TGATE = 0;
    T2CONbits.TCS = 0;
    
    TMR2 = 0; // timer start value
    PR2 = 625; // Period register 1ms
            
    T2CONbits.ON = 1; // turn on timer
}

/***	TIMER2_DelayMS
**
**	Parameters:
**      unsigned int ms - The number of milliseconds to delay.
**
**	Return Value:
**		
**
**	Description:
**		This function creates a delay of the specified number of milliseconds using Timer 2.
**      It resets the timer and waits until it reaches the period value for each millisecond.
**      
**          
*/
void TIMER2_DelayMS(unsigned int ms) {
    for (int i = 0; i < ms; i++) {
        TMR2 = 0; // Reset Timer1 counter
        while (TMR2 < PR2 - 1); // Wait until Timer1 reaches the period value
    }
}
