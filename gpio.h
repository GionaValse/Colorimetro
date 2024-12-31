/* 
 * File:   gpio.h
 * @brief Header file for GPIO management.
 *
 * This file contains the definitions and function prototypes for initializing and controlling GPIO pins, including the button and RGB LED.
 *
 * @date December 17, 2024
 */

#ifndef RGBLED_H
#define	RGBLED_H

/* BTNC Pins */
#define tris_BTN_BTNC TRISFbits.TRISF0

#define prt_BTN_BTNC PORTFbits.RF0

/* RBGLed Pins */
#define tris_RGB_R TRISDbits.TRISD2
#define tris_RGB_G TRISDbits.TRISD12
#define tris_RGB_B TRISDbits.TRISD3

#define lat_RGB_R LATDbits.LATD2
#define lat_RGB_G LATDbits.LATD12
#define lat_RGB_B LATDbits.LATD3

/* public functions */
void BTNC_Init();

/* private functions */
void RGB_ConfigurePin();
void RGB_SetValue(char r, char g, char b);
void RED_Pulse(int times);

#endif	/* RGBLED_H */

