/* 
 * File:   i2c.h
 * @brief Header file for I2C communication.
 *
 * This file contains the definitions and function prototypes for initializing and controlling I2C communication.
 *
 * @date December 16, 2024
 */

#ifndef I2C_H
#define	I2C_H

#define I2C_WAIT_TIMEOUT 0x0FFF
#define I2C_TPGD 0.000000104

#define i2c_MASTER_WRITE 0
#define i2c_MASTER_READ 1

/* public functions */
void I2C_Init(unsigned int i2cFreq);

void I2C_MasterStart();
void I2C_MasterRestart();
unsigned char I2C_MasterSend(unsigned char byte);
unsigned char I2C_MasterReceive();
void I2C_MasterACK(int val);
void I2C_MasterStop();

#endif	/* I2C_H */

