/* 
 * File:   lcd.h
 * @brief Header file for LCD management.
 *
 * This file contains the definitions and function prototypes for initializing and controlling an LCD.
 *
 * @date October 30, 2024
 */

#ifndef LCD_H
#define	LCD_H

/* lcd pins */
#define tris_LCD_DISP_RS TRISBbits.TRISB15
#define lat_LCD_DISP_RS LATBbits.LATB15
#define ansel_LCD_DISP_RS ANSELBbits.ANSB15
#define rp_LCD_DISP_RS RPB15R

#define tris_LCD_DISP_RW TRISDbits.TRISD5
#define lat_LCD_DISP_RW LATDbits.LATD5
#define rp_LCD_DISP_RW RPD5R

#define tris_LCD_DISP_EN TRISDbits.TRISD4
#define lat_LCD_DISP_EN LATDbits.LATD4
#define rp_LCD_DISP_EN RPD4R

#define ansel_LCD_DB2 ANSELEbits.ANSE2
#define ansel_LCD_DB4 ANSELEbits.ANSE4
#define ansel_LCD_DB5 ANSELEbits.ANSE5
#define ansel_LCD_DB6 ANSELEbits.ANSE6
#define ansel_LCD_DB7 ANSELEbits.ANSE7

/* lcd utils */
#define LCDDATA 1 // RS = 1 ; access data register
#define LCDCMD 0 // RS = 0 ; access command register
#define PMDATA PMDIN // PMP data buffer

/* lcd macro functions */
#define busyLCD() LCD_Read(LCDCMD) & 0x80
#define putLCD(d) LCD_Write(LCDDATA, (d))
#define cmdLCD(c) LCD_Write(LCDCMD, (c))
#define homeLCD() LCD_Write(LCDCMD, 2)
#define line2LCD() LCD_Write(LCDCMD, 0x80 | 0x40)
#define clrLCD() LCD_Write(LCDCMD, 1)

/* public functions */
void LCD_Init();
char LCD_Read(int addr);
void LCD_Write(int addr, char c);
void LCD_PutString(char *s);

/* private functions */
void LCD_ConfigurePins();
void LCD_ConfigurePMP();

#endif	/* LCD_H */

