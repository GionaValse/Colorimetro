/* 
 * File:   audio.h
 * @brief Header file for audio management.
 *
 * This file contains the definitions and function prototypes for initializing and controlling audio output.
 *
 * @date December 24, 2024
 */

#ifndef AUDIO_H
#define	AUDIO_H

/* audio pins */
#define tris_AUDIO_PWM TRISBbits.TRISB14
#define lat_AUDIO_PWM LATBbits.LATB14
#define ansel_AUDIO_PWM ANSELBbits.ANSB14
#define rp_AUDIO_PWM RPB14R
#define vector_TIMER3 12

/* public functions */
void AUDIO_Init();
void AUDIO_BeepStart();
void AUDIO_BeepStop();

/* private functions */
void AUDIO_InitPins();
void AUDIO_ConfigureOC();

#endif	/* AUDIO_H */

