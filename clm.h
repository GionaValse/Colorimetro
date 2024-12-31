/* 
 * File:   clm.h
 * @brief Header file for colorimeter management.
 *
 * This file contains the definitions and function prototypes for initializing and controlling the colorimeter module.
 *
 * @date December 16, 2024
 */

#ifndef CLM_H
#define	CLM_H

#define clm_I2C_ADDR 0x29

/* register address */
#define clm_ID_ADDR 0x12

#define clm_CDATAL 0x14 // clear data low byte
#define clm_CDATAH 0x15 // clear data low byte
#define clm_RDATAL 0x16 // red data low byte
#define clm_RDATAH 0x17 // red data low byte
#define clm_GDATAL 0x18 // green data low byte
#define clm_GDATAH 0x19 // green data low byte
#define clm_BDATAL 0x1A // blue data low byte
#define clm_BDATAH 0x1B // blue data low byte

/* public functions */
void CLM_Init();
unsigned char CLM_GetID();
void CLM_GetColorData(unsigned int *colors);

#endif	/* COLORIMETER_H */

