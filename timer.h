/* 
 * File:   timer.h
 * @brief Header file for timer management.
 *
 * This file contains the definitions and function prototypes for initializing and controlling Timer 2.
 *
 * @date October 7, 2024
 */

#ifndef TIMER_H
#define	TIMER_H

/* timer2 interrupt vector */
#define vector_TIMER2 8

/* define all prescaler */
#define PRS_1   0
#define PRS_2   1
#define PRS_4   2
#define PRS_8   3
#define PRS_16  4
#define PRS_32  5
#define PRS_64  6
#define PRS_256 7

/* define all mode */
#define MODE_16 0
#define MODE_32 1

/* define public function */
void TIMER2_Init();
void TIMER2_DelayMS(unsigned int ms);

/* define private function */
void TIMER2_ConfigurePins();

#endif	/* TIMER_H */

