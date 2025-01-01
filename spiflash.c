#include "spiflash.h"
#include "config.h"
#include "timer.h"
#include "uart.h"
#include <p32xxxx.h>

unsigned char rd[10], wr[10];

/***	SPIFLASH_Init
**
**	Parameters:
**		
**
**	Return Value:
**		
**
**	Description:
**		This function initializes the hardware involved in the SPIFLASH module: 
**      The following digital pins are configured as digital outputs (SPIFLASH_CE, SPIFLASH_SCK, SPIFLASH_SI).
**      The following digital pins are configured as digital inputs (SPIFLASH_SO).
**      The SPIFLASH_SI and SPIFLASH_SO are mapped over the SPI1 interface.
**      The SPI1 module of PIC32 is configured to work at 1 Mhz, polarity 0 and edge 1.
**      
**          
*/
void SPIFLASH_Init()
{
    SPIFLASH_ConfigurePins();
    SPIFLASH_ConfigureSPI(1250000, 0, 1); // 1.25MHz -> 15
}

/***	SPIFLASH_ConfigureSPI
**
**	Parameters:
**		unsigned int spiFreq - SPI clock frequency (Hz).
**                             for example 1000000 corresponds to 1 MHz
**		unsigned char pol - SPI Clock Polarity, similar to CKP field of SPIxCON
**                  1 = Idle state for clock is a high level; active state is a low level
**                  0 = Idle state for clock is a low level; active state is a high level
**		unsigned char edge - SPI Clock Edge, similar to CKE field of SPIxCON
**                  1 = Serial output data changes on transition from active clock state to Idle clock state (see CKP bit)
**                  0 = Serial output data changes on transition from Idle clock state to active clock state (see CKP bit)
**
**	Return Value:
**		
**
**	Description:
**		This function configures the SPI1 hardware interface of PIC32, according to the provided parameters.
**      In order to compute the baud rate value, it uses the peripheral bus frequency definition (PB_FRQ, located in config.h)
**      
**          
*/
void SPIFLASH_ConfigureSPI(unsigned int spiFreq, unsigned char pol, unsigned char edge)
{
    // configures SPI1
    SPI1BRG = PB_CLK / (2 * spiFreq) - 1;
    SPI1CONbits.CKP = pol;    // SPI Clock Polarity
    SPI1CONbits.CKE = edge;   // SPI Clock Edge  
    SPI1CONbits.SMP = 0;      // SPI Data Input Sample Phase 
    SPI1CONbits.MSTEN = 1;    // Master
    SPI1CONbits.MODE16 = 0;   // 8 bit transfer
    SPI1CONbits.MODE32 = 0;   // 8 bit transfer
    SPI1CON2bits.AUDEN = 0;   // Audio protocol is disabled
    SPI1CONbits.ON = 1;       // enable SPI
}

/***	SPIFLASH_ConfigurePins
**
**	Parameters:
**		
**
**	Return Value:
**		
**
**	Description:
**		This function configures the digital pins involved in the SPIFLASH module: 
**      The following digital pins are configured as digital outputs: SPIFLASH_CE, SPIFLASH_SCK, SPIFLASH_SI
**      The following digital pins are configured as digital inputs: SPIFLASH_SO.
**      The SPIFLASH_SI and SPIFLASH_SO are mapped over the SPI1 interface.
**      The function uses pin related definitions from config.h file.
**      
**          
*/
void SPIFLASH_ConfigurePins()
{
    // Configure SPIFLASH signals as digital outputs.
    tris_SPIFLASH_CS = 0;
    tris_SPIFLASH_SCK = 0;
    tris_SPIFLASH_SI = 0;
    
    // Configure SPIFLASH signals as digital inputs.
    tris_SPIFLASH_SO = 1;
    
    // configure remapable pins
    rp_SPIFLASH_SI = 0x08; // RPF2R = 1000 = SDO1 
    rp_SPIFLASH_SO = 0x0F;// SDI1R = 1111 = RPF7

}

/***	SPIFLASH_RawTransferByte
**
**	Parameters:
**		unsigned char bVal  - the byte to be transmitted over SPI
**
**	Return Value:
**		unsigned char       - the byte received over SPI
**
**	Description:
**		This function implements basic byte transfer over SPI1. 
**      It transmits the parameter bVal and returns the received byte.
**      This function does not handle Slave Select (SS) pin, 
**      use SPIFLASH_TransferBytes for SPI transfer.
**      
**          
*/
unsigned char SPIFLASH_RawTransferByte(unsigned char bVal)
{
    while(!SPI1STATbits.SPITBE);	// wait for TX buffer to be empty
    SPI1BUF = bVal;
    while(!SPI1STATbits.SPIRBF);	// wait for RX buffer to be empty
    return SPI1BUF;
}

/***	SPIFLASH_TransferBytes
**
**	Parameters:
**      int bytesNumber         - Number of bytes to be transfered.
**      unsigned char *pbRdData - Pointer to a buffer storing the received bytes.
**      unsigned char *pbWrData - Pointer to a buffer storing the bytes to be transmitted.
**
**	Return Value:
**
**	Description:
**		This function implements transfer of a number of bytes over SPI1. 
**      It transmits the bytes from pbWrData and receives the bytes in pbRdData.
**      This function properly handles Slave Select (SS) pin.
**      
**          
*/
void SPIFLASH_TransferBytes(unsigned char bytesNumber, unsigned char *pbRdData, unsigned char *pbWrData)
{
    int i;
    lat_SPIFLASH_CS = 0; // Activate SS
    for(i = 0; i< bytesNumber; i++)
    {
        pbRdData[i] = SPIFLASH_RawTransferByte(pbWrData[i]);
    }
    lat_SPIFLASH_CS = 1; // Deactivate SS
}

/***	SPIFLASH_ReleasePowerDownGetDeviceID
**
**	Parameters:
**
**	Return Value:
**      unsigned char   - the Device ID provided by Deep-Power-Down / Device ID command
**
**	Description:
**		This function implements the Release from Deep-Power-Down / Device ID command. 
**      The obtained the Device ID is returned.
**      
**          
*/
unsigned char SPIFLASH_ReleasePowerDownGetDeviceID()
{
    wr[0] = SPIFLASH_CMD_PWRDWN_DEVID;
    wr[1] = 0;
    wr[2] = 0;
    wr[3] = 0;
    wr[4] = 0;
    SPIFLASH_TransferBytes(5, rd, wr);
    return rd[4];
}

/***	SPIFLASH_SendOneByteCmd
**
**	Parameters:
**      unsigned char bCmd      - the command ID 
**
**	Return Value:
**
**	Description:
**		This function sends one byte command over SPI. 
**      It properly handles Slave Select (SS) pin and transfers the command ID.
**      
**          
*/
void SPIFLASH_SendOneByteCmd(unsigned char bCmd)
{
    lat_SPIFLASH_CS = 0; // Activate SS
    SPIFLASH_RawTransferByte(bCmd);
    lat_SPIFLASH_CS = 1; // Deactivate SS
}

/***	SPIFLASH_GetStatus
**
**	Parameters:
**
**	Return Value:
**      unsigned char bCmd      - the Status Register
**
**	Description:
**		This function reads the Status Register and returns it. 
**      
**          
*/
unsigned char SPIFLASH_GetStatus()
{
    unsigned char bResult;
    lat_SPIFLASH_CS = 0; // Activate SS
    SPIFLASH_RawTransferByte(SPIFLASH_CMD_RDSR);
    bResult = SPIFLASH_RawTransferByte(0);
    lat_SPIFLASH_CS = 1; // Deactivate SS
    return bResult;
}

/***	SPIFLASH_WaitUntilNoBusy
**
**	Parameters:
**
**	Return Value:
**      
**
**	Description:
**		This function reads the Status Register and waits until Busy flag is not set. 
**      
**          
*/
void SPIFLASH_WaitUntilNoBusy()
{
    unsigned char status;
    while(SPIFLASH_GetStatus() & SPIFLASH_STATUS_BUSY);
}

/***	SPIFLASH_WriteEnable
**
**	Parameters:
**
**	Return Value:
**      
**
**	Description:
**		This function calls the Write Enable command. 
**      
**          
*/
void SPIFLASH_WriteEnable()
{
    SPIFLASH_SendOneByteCmd(SPIFLASH_CMD_WREN);
    SPIFLASH_WaitUntilNoBusy();
}

/***	SPIFLASH_WriteDisable
**
**	Parameters:
**
**	Return Value:
**      
**
**	Description:
**		This function calls the Write Disable command. 
**      
**          
*/
void SPIFLASH_WriteDisable()
{
    SPIFLASH_SendOneByteCmd(SPIFLASH_CMD_WRDI);
    SPIFLASH_WaitUntilNoBusy();
}

/***	SPIFLASH_EraseAll
**
**	Parameters:
**         
**
**	Return Value:
**      
**
**	Description:
**		This functions performs a chip erase: erases all memory within the device.
**      
**          
*/
void SPIFLASH_EraseAll()
{
    SPIFLASH_WaitUntilNoBusy();
    SPIFLASH_WriteEnable();
    SPIFLASH_SendOneByteCmd(SPIFLASH_CMD_ERASE_ALL);
    SPIFLASH_WaitUntilNoBusy();
}

/***	SPIFLASH_ProgramPage
**
**	Parameters:
**      unsigned int addr       - The memory address where data will be written
**      unsigned char *pBuf     - Pointer to a buffer storing the bytes to be written. 
**      int len                 - Number of bytes to be written.
 **
**	Return Value:
**      
**
**	Description:
**		This functions calls the Page Program command: 
**      it allows from one byte to 256 bytes (a page) of data to be programmed 
**      at previously erased memory locations.
**      
**          
*/
void SPIFLASH_ProgramPage(unsigned int addr, unsigned char *pBuf, unsigned int len)
{
    int i;
    SPIFLASH_WaitUntilNoBusy();
    SPIFLASH_WriteEnable();
    
    lat_SPIFLASH_CS = 0; // Activate SS
    
    SPIFLASH_RawTransferByte(SPIFLASH_CMD_PROGRAMPAGE);
    SPIFLASH_RawTransferByte(addr >> 16);
    SPIFLASH_RawTransferByte(addr >> 8);
    SPIFLASH_RawTransferByte(addr & 0xFF);
    for(i = 0; i< len; i++)
    {
        SPIFLASH_RawTransferByte(pBuf[i]);
    }
    lat_SPIFLASH_CS = 1; // Deactivate SS
    SPIFLASH_WaitUntilNoBusy();
}

/***	SPIFLASH_ProgramPage
**
**	Parameters:
**      unsigned int addr       - The memory address where data will be written
**      unsigned char *pBuf     - Pointer to a buffer storing the bytes to be written. 
**
**	Return Value:
**      
**
**	Description:
**		This functions calls the Page Program command: 
**      it allows 2 bytes of data to be programmed 
**      at previously erased memory locations.
**      
**          
*/
void SPIFALSH_Write2Byte(unsigned int addr, unsigned short buff) {
    unsigned char pBuff[2];
    
    pBuff[0] = buff;
    pBuff[1] = (buff >> 8);
    
    SPIFLASH_ProgramPage(addr, pBuff, 2);
}

/***	SPIFLASH_Read
**
**	Parameters:
**      unsigned int addr       - The memory address fromm where the data will be read
**      unsigned char *pBuf     - Pointer to a buffer storing the read bytes. 
**      int len                 - Number of bytes to be read.
 **
**	Return Value:
**      
**
**	Description:
**		This functions calls the Read Data command:
**      it allows one or more data bytes to be sequentially read from the memory. 
**      
**          
*/
void SPIFLASH_Read(unsigned int addr, unsigned char *pBuf, unsigned int len)
{
    int i;
    
    lat_SPIFLASH_CS = 0; // Activate SS
    
    SPIFLASH_RawTransferByte(SPIFLASH_CMD_READ);
    SPIFLASH_RawTransferByte(addr >> 16);
    SPIFLASH_RawTransferByte(addr >> 8);
    SPIFLASH_RawTransferByte(addr & 0xFF);
    for(i = 0; i< len; i++)
    {
        pBuf[i] = SPIFLASH_RawTransferByte(0);
    }
    lat_SPIFLASH_CS = 1; // Deactivate SS
}

/***	SPIFLASH_Read2Byte
**
**	Parameters:
**      unsigned int addr       - The memory address fromm where the data will be read
**
**	Return Value:
**      
**
**	Description:
**		This functions calls the Read Data command:
**      it allows 2 data bytes to be sequentially read from the memory. 
**      
**          
*/
unsigned short SPIFLASH_Read2Byte(unsigned int addr) {
    unsigned char buff[2];
    SPIFLASH_Read(addr, buff, 2);
    return (buff[1] << 8) | buff[0];
}

/***	SPIFLASH_Close
**
**	Parameters:
** 
**
**	Return Value:
**      
**
**	Description:
**		This functions releases the hardware involved in SPIFLASH library: 
**      it turns off the SPI1 interface.
**      
**          
*/
void SPIFLASH_Close()
{
    SPI1CONbits.ON = 0; // disable SPI
}