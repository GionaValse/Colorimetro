/* 
 * File:   uart.h
 * @brief Header file for UART management.
 *
 * This file contains the definitions and function prototypes for initializing and controlling UART communication.
 *
 * @date December 24, 2024
 */

#ifndef UART_H
#define	UART_H

/* uart4 pins */
#define write_UART4 U4TXREG // write port
#define read_UART4 U4RXREG // read port

#define avl_UART4_RX U4STAbits.URXDA // RX availability

/* public function */
void UART_Init(unsigned int baud);
void UART_PutChar(char c);
char UART_GetChar();
void UART_PutString(char szData[]);
unsigned char UART_GetString(char *pText);

/* private function */
void UART_ConfigurePins();
void UART_ConfigureUart(unsigned int baud);
void UART_ConfigureUartRXInt(unsigned int baud);

#endif	/* UART_H */

