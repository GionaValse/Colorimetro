#include "audio.h"
#include "config.h"
#include <p32xxxx.h>

unsigned int TMR_FREQ = 10000; // 10kHz

/***	AUDIO_Init
**
**	Parameters:
**		
**
**	Return Value:
**		
**
**	Description:
**		This function initializes the audio module by configuring the pins and the Output Compare (OC) module.
**      
**          
*/
void AUDIO_Init() {
    AUDIO_InitPins();
    AUDIO_ConfigureOC();
}

/***	AUDIO_InitPins
**
**	Parameters:
**		
**
**	Return Value:
**		
**
**	Description:
**		This function configures the pins used for audio output.
**      It sets the relevant pins as digital outputs and assigns them to the OC1 module.
**      
**          
*/
void AUDIO_InitPins() {
    TRISBbits.TRISB14 = 0;
    ANSELBbits.ANSB14 = 0;
    RPB14R = 0x0C; // OC1
}

/***	AUDIO_ConfigureOC
**
**	Parameters:
**		
**
**	Return Value:
**		
**
**	Description:
**		This function configures the Output Compare (OC) module for audio output.
**      It sets up Timer3 and the OC1 module for PWM operation.
**      
**          
*/
void AUDIO_ConfigureOC() {
    T3CONbits.TCKPS = 0;     //1:1 prescale value
    PR3 = (PB_CLK / TMR_FREQ) * 1 - 1; // (PR+1) = (PBCLK / Freq_pwm) * Presc 
    TMR3 = 0; // initial TMR3 count is 0
   
    OC1CONbits.ON = 0; // Turn off OC1 while doing setup.
    OC1CONbits.OCM = 6; //0b110 PWM mode on OC1; Fault pin is disabled
    OC1CONbits.OCTSEL = 1; // Timer3 is the clock source for this Output Compare module
   
    OC1RS = 0; // duty cycle = OC1RS/(PR3+1) = 0%
    OC1R = 0; // init before turning OC1 on

    T3CONbits.ON = 1; // turn on Timer3
    OC1CONbits.ON = 1; // turn on OC1
}

/***	AUDIO_BeepStart
**
**	Parameters:
**		
**
**	Return Value:
**		
**
**	Description:
**		This function starts the audio beep by setting the duty cycle of the OC1 module to 50%.
**      
**          
*/
void AUDIO_BeepStart() {
    OC1RS = (PR3 + 1) * 0.5; // duty-cycle 50%
}

/***	AUDIO_BeepStop
**
**	Parameters:
**		
**
**	Return Value:
**		
**
**	Description:
**		This function stops the audio beep by setting the duty cycle of the OC1 module to 0%.
**      
**          
*/
void AUDIO_BeepStop() {
    OC1RS = 0;
}
