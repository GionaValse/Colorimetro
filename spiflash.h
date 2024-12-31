/* 
 * File:   spiflash.h
 * @brief Header file for SPI flash memory management.
 *
 * This file contains the definitions and function prototypes for initializing and controlling SPI flash memory.
 *
 * @date December 24, 2024
 */

#ifndef SPIFLASH_H
#define	SPIFLASH_H

/* spi pins */
#define tris_SPIFLASH_CS TRISFbits.TRISF8
#define lat_SPIFLASH_CS LATFbits.LATF8

#define tris_SPIFLASH_SCK TRISFbits.TRISF6
#define lat_SPIFLASH_SCK LATFbits.LATF6

#define tris_SPIFLASH_SO TRISFbits.TRISF7
#define rp_SPIFLASH_SO SDI1R

#define tris_SPIFLASH_SI TRISFbits.TRISF2
#define rp_SPIFLASH_SI RPF2R

/* flash cmd */
#define SPIFLASH_CMD_PROGRAMPAGE        0x02    // SPI Flash opcode: Page program
#define SPIFLASH_CMD_READ               0x03    // SPI Flash opcode: Read up up to 25MHz
#define SPIFLASH_CMD_READ_FAST			0x0B    // SPI Flash opcode: Read up to 50MHz with 1 dummy byte
#define SPIFLASH_CMD_ERASE_ALL			0x60    // SPI Flash opcode: Entire chip erase
#define SPIFLASH_CMD_WRITE				0x02    // SPI Flash opcode: Write one byte (or a page of up to 256 bytes, depending on device)
#define SPIFLASH_CMD_WRITE_WORD_STREAM	0xAD    // SPI Flash opcode: Write continuous stream of 16-bit words (AAI mode); available on SST25VF016B (but not on SST25VF010A)
#define SPIFLASH_CMD_WRITE_BYTE_STREAM	0xAF    // SPI Flash opcode: Write continuous stream of bytes (AAI mode); available on SST25VF010A (but not on SST25VF016B)
#define SPIFLASH_CMD_RDSR				0x05    // SPI Flash opcode: Read Status Register
#define SPIFLASH_CMD_EWSR				0x50    // SPI Flash opcode: Enable Write Status Register
#define SPIFLASH_CMD_WRSR				0x01    // SPI Flash opcode: Write Status Register
#define SPIFLASH_CMD_WREN				0x06    // SPI Flash opcode: Write Enable
#define SPIFLASH_CMD_WRDI				0x04    // SPI Flash opcode: Write Disable / End AAI mode
#define SPIFLASH_CMD_RDID				0x90    // SPI Flash opcode: Read ID
#define SPIFLASH_CMD_JEDEC_ID			0x9F    // SPI Flash opcode: Read JEDEC ID
#define SPIFLASH_CMD_EBSY				0x70    // SPI Flash opcode: Enable write BUSY status on SO pin
#define SPIFLASH_CMD_DBSY				0x80    // SPI Flash opcode: Disable write BUSY status on SO pin
#define SPIFLASH_CMD_PWRDWN_DEVID       0xAB    // SPI Flash opcode: Release from Deep-Power-Down / Device ID

#define SPIFLASH_STATUS_BUSY            0x01    // Busy bit of SR1

/* public functions */
void SPIFLASH_Init();
void SPIFLASH_EraseAll();
unsigned char SPIFLASH_ReleasePowerDownGetDeviceID();
void SPIFLASH_ProgramPage(unsigned int addr, unsigned char *pBuf, unsigned int len);
void SPIFLASH_Read(unsigned int addr, unsigned char *pBuf, unsigned int len);
void SPIFLASH_Close();
void SPIFALSH_Write2Byte(unsigned int addr, unsigned short buff);
unsigned short SPIFLASH_Read2Byte(unsigned int addr);

/* private functions */
void SPIFLASH_ConfigurePins();
void SPIFLASH_ConfigureSPI(unsigned int spiFreq, unsigned char pol, unsigned char edge);
void SPIFLASH_SendOneByteCmd(unsigned char bCmd);
unsigned char SPIFLASH_GetStatus();
void SPIFLASH_WaitUntilNoBusy();
void SPIFLASH_WriteEnable();
void SPIFLASH_WriteDisable();
void SPIFLASH_ConfigurePins();
unsigned char SPIFLASH_TransferByte(unsigned char bVal);
void SPIFLASH_TransferBytes(unsigned char bytesNumber, unsigned char *pbRdData, unsigned char *pbWrData);

#endif	/* SPIFLASH_H */

