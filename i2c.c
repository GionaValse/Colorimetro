#include "config.h"
#include "i2c.h"
#include <p32xxxx.h>

/***	I2C_Init
**
**	Parameters:
**		unsigned int i2cFreq - I2C clock frequency (Hz).
**          for example 400000 value sent as parameter corresponds to 400 kHz
**
**	Return Value:
**
**	Description:
**		This function configures the I2C1 hardware interface of PIC32, according to the provided frequency.
**      In order to compute the baud rate value, it uses the peripheral bus frequency definition (PB_FRQ, located in config.h).
**      This is a low-level function called by ACL_Init(), so user should avoid calling it directly.
**          
*/
void I2C_Init(unsigned int i2cFreq) {
    // no need pin mapping
    // SCL1 pin setup
    TRISGbits.TRISG2 = 0; // as digital output
    
    // SDA1 pin setup
    TRISGbits.TRISG3 = 0; // as digital output
    
    I2C1CON = 0x0000;            //Clear the content of I2C1CON register 
    I2C1BRG = ((1 / (2 * i2cFreq) - I2C_TPGD) * PB_CLK) - 2;
    I2C1CONbits.ON = 1;     // Enable the I2C module
}

/***	I2C_MasterStart
**
**	Parameters:
**
**	Return Value:
**
**	Description:
**		This function sends the start bit to initiate communication on the I2C bus.
**      It waits until the start bit is sent before returning.
**      
**          
*/
void I2C_MasterStart() {
    I2C1CONbits.SEN = 1; // send the start bit
    while(I2C1CONbits.SEN); // wait for the start bit to be sent
}

/***	I2C_MasterRestart
**
**	Parameters:
**
**	Return Value:
**
**	Description:
**		This function sends a restart condition on the I2C bus.
**      It waits until the restart condition is sent before returning.
**      
**          
*/
void I2C_MasterRestart() {
    I2C1CONbits.RSEN = 1; // send a restart
    while(I2C1CONbits.RSEN) { ; } // wait for the restart to clear
}

/***	I2C_MasterSend
**
**	Parameters:
**      unsigned char byte - The byte to be sent to the slave device.
**
**	Return Value:
**
**	Description:
**		This function sends a byte to the slave device on the I2C bus.
**      It waits until the transmission is complete before returning.
**      
**          
*/
unsigned char I2C_MasterSend(unsigned char byte) {
    // send a byte to slave
    I2C1TRN = byte; // if an address, bit 0 = 0 for write, 1 for read
    while(I2C1STATbits.TRSTAT); // wait for the transmission to finish
    // if this is high, slave has not acknowledged
    I2C1STATbits.ACKSTAT;
}

/***	I2C_MasterReceive
**
**	Parameters:
**
**	Return Value:
**      unsigned char - The byte received from the slave device.
**
**	Description:
**		This function receives a byte from the slave device on the I2C bus.
**      It waits until the data is received before returning.
**      
**          
*/
unsigned char I2C_MasterReceive() {
    // receive a byte from the slave
    I2C1CONbits.RCEN = 1; // start receiving data
    while(!I2C1STATbits.RBF) { ; } // wait to receive the data
    return I2C1RCV; // read and return the data
}

/***	I2C_MasterACK
**
**	Parameters:
**      int val - The value to send as an acknowledgment (0 for ACK, 1 for NACK).
**
**	Return Value:
**
**	Description:
**		This function sends an acknowledgment (ACK) or not-acknowledgment (NACK) to the slave device.
**      It waits until the acknowledgment is sent before returning.
**      
**          
*/
void I2C_MasterACK(int val) {
    // sends ACK = 0 (slave should send another byte)
    // or NACK = 1 (no more bytes requested from slave)
    I2C1CONbits.ACKDT = val; // store ACK/NACK in ACKDT
    I2C1CONbits.ACKEN = 1; // send ACKDT
    while(I2C1CONbits.ACKEN); // wait for ACK/NACK to be sent
}

/***	I2C_MasterStop
**
**	Parameters:
**
**	Return Value:
**
**	Description:
**		This function sends a stop condition to end communication on the I2C bus.
**      It waits until the stop condition is sent before returning.
**      
**          
*/
void I2C_MasterStop() {
    // send a STOP:
    I2C1CONbits.PEN = 1; // comm is complete and master relinquishes bus
    while(I2C1CONbits.PEN); // wait for STOP to complete
}
