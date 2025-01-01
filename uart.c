#include "config.h"
#include "uart.h"
#include <p32xxxx.h>

// https://www.ascii-code.com/ASCII

/* external var */
extern unsigned char uartFlag;
extern unsigned int uartCount;

/***	UART_Init
**
**	Parameters:
**      unsigned int baud - The baud rate for UART communication.
**
**	Return Value:
**
**	Description:
**		This function initializes the UART module by configuring the pins, UART settings, and UART RX interrupt.
**      It then turns on the UART module.
**      
**          
*/
void UART_Init(unsigned int baud) {
    UART_ConfigurePins();
    UART_ConfigureUart(baud); // Configure UART4 base
    UART_ConfigureUartRXInt(baud); // Configure UART4 interrupt
    
    U4MODEbits.ON = 1; // Turn on after config
}

/***	UART_ConfigurePins
**
**	Parameters:
**
**	Return Value:
**
**	Description:
**		This function configures the pins used for UART communication.
**      It sets the TX pin as a digital output and maps it to U4TX, and sets the RX pin as a digital input and maps it to U4RX.
**      
**          
*/
void UART_ConfigurePins() {
    // Configure TX
    TRISFbits.TRISF12 = 0; // digital output
    RPF12R = 2; // 0010 U4TX = Mapping U4TX to RPF12
    
    // Configure RX
    TRISFbits.TRISF13 = 1; // digital input
    U4RXR = 9; // 1001 RF13 = Mapping U4RX to RPF13
}

/***	UART_ConfigureUart
**
**	Parameters:
**      unsigned int baud - The baud rate for UART communication.
**
**	Return Value:
**
**	Description:
**		This function configures the UART module with the specified baud rate.
**      It sets various UART mode and status bits, and calculates the baud rate generator value.
**      
**          
*/
void UART_ConfigureUart(unsigned int baud) {
    U4MODEbits.ON = 0; // Turn off uart before config
    U4MODEbits.SIDL = 0;
    U4MODEbits.IREN = 0;
    U4MODEbits.RTSMD = 0;
    U4MODEbits.UEN0 = 0;
    U4MODEbits.UEN1 = 0;
    U4MODEbits.WAKE = 0;
    U4MODEbits.LPBACK = 0;
    U4MODEbits.ABAUD = 0;
    U4MODEbits.RXINV = 0;
    U4MODEbits.PDSEL1 = 0;
    U4MODEbits.PDSEL0 = 0;
    U4MODEbits.STSEL = 0;
    
    U4MODEbits.BRGH = 0;
    
    /* calculate brg */
    U4BRG = (int) (((float) PB_CLK / (16 * baud) - 1) + 0.5); // add 0.5 to round
    
    U4STAbits.UTXEN = 1;
    U4STAbits.URXEN = 1;
}

/***	UART_ConfigureUartRXInt
**
**	Parameters:
**      unsigned int baud - The baud rate for UART communication.
**
**	Return Value:
**
**	Description:
**		This function configures the UART RX interrupt with the specified baud rate.
**      It sets the interrupt priority and sub-priority, enables interrupts, and clears the UART4 interrupt flag.
**      
**          
*/
void UART_ConfigureUartRXInt(unsigned int baud) {    
    /* Set priority and subpriority */
    IPC9bits.U4IP = 6;
    IPC9bits.U4IS = 3;

    macro_enable_interrupts();  // enable interrupts
    
	IFS2bits.U4RXIF = 0;    // Clear the Uart4 interrupt flag.
    IEC2bits.U4RXIE = 1;    // enable RX interrupt
}

/***	UART_PutChar
**
**	Parameters:
**      char c - The character to be sent.
**
**	Return Value:
**
**	Description:
**		This function sends a single character over UART.
**      It waits until the transmit buffer is not full before sending the character.
**      
**          
*/
void UART_PutChar(char c) {
    while (U4STAbits.UTXBF);
    write_UART4 = c;
}

/***	UART_GetChar
**
**	Parameters:
**
**	Return Value:
**      char - The character received from UART.
**
**	Description:
**		This function receives a single character from UART.
**      It waits until data is available in the receive buffer before returning the character.
**      
**          
*/
char UART_GetChar() {
    while (!U4STAbits.URXDA);
    return U4RXREG;
}

/***	UART_PutString
**
**	Parameters:
**      char szData[] - The string to be sent.
**
**	Return Value:
**
**	Description:
**		This function sends a string over UART.
**      It sends each character of the string one by one.
**      
**          
*/
void UART_PutString(char szData[]) {
    char *pData = szData;
    
    while (*pData) {
        UART_PutChar((*(pData++)));
    }
}

/***	UART_GetString
**
**	Parameters:
**      char *pText - Pointer to the buffer where the received string will be stored.
**
**	Return Value:
**      unsigned char - The received character.
**
**	Description:
**		This function receives a string from UART.
**      It stores the received characters in the provided buffer until a carriage return ('\r') is received.
**      The function sets a flag when the string is completely received.
**      
**          
*/
unsigned char UART_GetString(char *pText) {
    unsigned char c;
    
    while(!uartFlag) {
        c = UART_GetChar();
        pText[uartCount++] = c;
        
        if (uartCount > 0 && c == 0xd) { // 0xd = \r
            pText[uartCount - 1] = 0;
            uartCount = 0;
            uartFlag = 1;   // flag goes to 1 when string is received completely. Remember to reset the flag in main.c 
        }       
    }
}
