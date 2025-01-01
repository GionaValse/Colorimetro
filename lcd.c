#include "lcd.h"
#include "config.h"
#include "timer.h"
#include <p32xxxx.h>

/***	LCD_Init
**
**	Parameters:
**		
**
**	Return Value:
**		
**
**	Description:
**		This function initializes the LCD by configuring the pins and PMP.
**      It sets up the display with an 8-bit interface, 2 lines, and 5x7 dots.
**      It also clears the display and sets the cursor to increment mode.
**      
**          
*/
void LCD_Init() {
    LCD_ConfigurePins();
    LCD_ConfigurePMP();
    
    // Display setup
    TIMER2_DelayMS(30);
    PMADDR = LCDCMD; // command register (ADDR = 0)
    PMDATA = 0x38; // 8-bit interface, 2 lines, 5x7
    TIMER2_DelayMS(1); // > 48us
    PMDATA = 0x0c; // ON, no cursor, no blink
    TIMER2_DelayMS(1); // > 48us
    PMDATA = 0x01; // clear display
    TIMER2_DelayMS(2); // > 1.6ms
    PMDATA = 0x06; // increment cursor, no shift
    TIMER2_DelayMS(2); // > 1.6ms
}

/***	LCD_ConfigurePins
**
**	Parameters:
**		
**
**	Return Value:
**		
**
**	Description:
**		This function configures the control and data pins for the LCD.
**      It sets the control pins as digital outputs and disables analog mode for data pins.
**      
**          
*/
void LCD_ConfigurePins() {
    // set control pins as digital outputs.
    tris_LCD_DISP_RS = 0;
    tris_LCD_DISP_RW = 0;
    tris_LCD_DISP_EN = 0;
    
    // disable analog (set pins as digital))
    ansel_LCD_DISP_RS = 0;
    
    // default (IO) function for remapable pins
    rp_LCD_DISP_RS = 0;
    rp_LCD_DISP_RW = 0;
    rp_LCD_DISP_EN = 0;
    
    // make data pins digital (disable analog)
    ansel_LCD_DB2 = 0;
    ansel_LCD_DB4 = 0;
    ansel_LCD_DB5 = 0;
    ansel_LCD_DB6 = 0;
    ansel_LCD_DB7 = 0;
}

/***	LCD_ConfigurePMP
**
**	Parameters:
**		
**
**	Return Value:
**		
**
**	Description:
**		This function initializes the Parallel Master Port (PMP) for LCD communication.
**      It sets the PMP control and mode registers for proper operation.
**      
**          
*/
void LCD_ConfigurePMP() {
    // PMP initialization
    PMCON = 0x83BF; // Enable the PMP, long waits
    PMMODE = 0x3FF; // Master Mode 1
    PMAEN = 0x0001; // PMA0 enabled
}

/***	LCD_Read
**
**	Parameters:
**      int addr - The address to read from (command or data register).
**
**	Return Value:
**      char - The data read from the specified address.
**
**	Description:
**		This function reads a byte from the specified address of the LCD.
**      It waits for the PMP to be available before initiating the read cycle.
**      
**          
*/
char LCD_Read(int addr) {
    int dummy;
    while (PMMODEbits.BUSY); // Wait for PMP available
    
    PMADDR = addr; // select the command address
    dummy = PMADDR; // init read cycle, dummy read
    while (PMMODEbits.BUSY); // Wait for PMP available
    
    return PMDATA; // read the status register
}

/***	LCD_Write
**
**	Parameters:
**      int addr - The address to write to (command or data register).
**      char c   - The data to write.
**
**	Return Value:
**		
**
**	Description:
**		This function writes a byte to the specified address of the LCD.
**      It waits for the LCD to be ready and the PMP to be available before writing.
**      
**          
*/
void LCD_Write(int addr, char c) {
    while(busyLCD()); // check busy flag of LCD
    while(PMMODEbits.BUSY); // wait for PMP available
    PMADDR = addr; //RS (0 or 1) for LCD
    PMDATA = c;
    
    TIMER2_DelayMS(1); // Wait 1 milliseconds for the next operation
}

/***	LCD_PutString
**
**	Parameters:
**      char *s - Pointer to the string to be written to the LCD.
**
**	Return Value:
**		
**
**	Description:
**		This function writes a string to the LCD.
**      It sends each character of the string to the LCD one by one.
**      
**          
*/
void LCD_PutString(char *s) {
    while (*s) {
        putLCD(*s++); // Put and read the next char
    }
}