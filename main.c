/* 
 * File:   main.c
 * Author: giona
 *
 * Created on December 16, 2024, 8:16 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <p32xxxx.h>

#include "audio.h"
#include "clm.h"
#include "config.h"
#include "gpio.h"
#include "lcd.h"
#include "spiflash.h"
#include "timer.h"
#include "uart.h"

// PB_CLOCK 40MHz
#pragma config JTAGEN = OFF     
#pragma config FWDTEN = OFF

// Device Config Bits in DEVCFG1
#pragma config FNOSC = FRCPLL
#pragma config FSOSCEN = OFF
#pragma config POSCMOD = XT
#pragma config OSCIOFNC = ON
#pragma config FPBDIV = DIV_2

// Device Config Bits in DEVCFG2
#pragma config FPLLIDIV = DIV_2
#pragma config FPLLMUL = MUL_20
#pragma config FPLLODIV = DIV_1

/* Global variables [START] */
unsigned char waitUser = 0;
unsigned char mode = 0;

unsigned char btncFlag = 0;

unsigned char uartFlag = 0;
unsigned int uartCount = 0;
unsigned char uartData[100];
/* Global variables[END] */

/* Interrupts [START] */
void __attribute__((interrupt(IPL6AUTO), vector(_UART_4_VECTOR)))
UART4MessageHandler() {
    uartData[uartCount++] = U4RXREG; // Add char in data
    uartFlag = 1; // New char arrived: flag on
	IFS2bits.U4RXIF = 0; // Clear the Uart4 interrupt flag
}

void __attribute__((interrupt(IPL7AUTO), vector(_EXTERNAL_4_VECTOR)))
BTNCClickHandler() {
    btncFlag = 1; // BTNC clicked: flag on
    IFS0bits.INT4IF = 0; // Clear the INT4 interrupt flag
}
/* Interrupts [END] */

/*
 * 
 */
int main(int argc, char** argv) {
    /* Initialize program [START] */
    AUDIO_Init();
    TIMER2_Init();
    UART_Init(9600);
    LCD_Init();
    CLM_Init();
    RGB_Init();
    BTNC_Init();
    SPIFLASH_Init();
    
    // Enable interrupts
    macro_enable_interrupts();
    
    char lcdData[20];
    unsigned short redCounter = 0;
    unsigned int colors[3];
    unsigned char checkRed = 0; // When a red is founded wait for a diff. color
    
    for (int i = 0; i < 3; i++) {
        colors[i] = 0;
    }
    
    if (CLM_GetID() != 0x44 || SPIFLASH_ReleasePowerDownGetDeviceID() != 0x15) {
        LCD_PutString("Errore periferiche");
        return (EXIT_FAILURE);
    }
    /* Initialize program [END] */
    
    while (1) {
        /* Interrupts logic [START] */
        if (uartFlag) {
            if (mode == 0)            
                uartManageData();
            uartFlag = 0;
        }
        
        if (btncFlag) {
            if (mode == 1) { // Get only in scan mode                
                /* Saving in flash memory [START] */
                SPIFALSH_Write2Byte(SPIFLASH_PROG_ADDR, redCounter);
                
                TIMER2_DelayMS(100);
                
                // Check if data are correctly writed
                unsigned short readValue = SPIFLASH_Read2Byte(SPIFLASH_PROG_ADDR);
                if (readValue != redCounter) {
                    char c[100];
                    snprintf(c, sizeof(c), "Errore nella scrittura della memoria flash: scritto %d, letto %d\n", redCounter, readValue);
                    UART_PutString(c);
                }
                
                redCounter = 0; // Reset red counter
                /* Saving in flash memory [END] */
                
                mode = 0;
            }
            
            btncFlag = 0; // Reset flag
        }
        /* Interrupts logic [END] */
        
        if (mode == 0 && !waitUser) { // Initial state, choice in terminal
            UART_PutString("Seleziona la funzionalita (inserisci il numero)\n");
            UART_PutString("1. avvio della scansione colorimetrica\n");
            UART_PutString("2. visualizza il n. di volte che e stato rilevato il colore rosso\n");
            UART_PutString("3. reset colori salvati\n");
            waitUser = 1;
        } else if (mode == 1) { // Scan mode
            /* Get & print color value [START] */
            CLM_GetColorData(colors);

            cmdLCD(0x80);

            snprintf(lcdData, sizeof(lcdData), "R: %d, G: %d            ", colors[0], colors[1]);
            LCD_PutString(lcdData);

            line2LCD();
            
            snprintf(lcdData, sizeof(lcdData), "B: %d            ", colors[2]);
            LCD_PutString(lcdData);
            /* Get & print color value [END] */
                        
            // Check if is red and increment var
            float gb = colors[1] + colors[2];
            if (gb > 0) {
                float redRatio = (float)colors[0] / gb; // r / (g + b) > 1 is red

                if (redRatio > 1 && !checkRed) {
                    // Count a new red if it is different from the last
                    checkRed = 1;
                    redCounter++;
                } else if (redRatio <= 1) {
                    checkRed = 0;
                }
            } else {
                // If g and b are 0
                checkRed = 0;
            }
        } else if (mode == 2) { // Show mode
            /* Show stored times [START] */
            unsigned short mem = SPIFLASH_Read2Byte(SPIFLASH_PROG_ADDR);
            
            char uartMemPrint[40];
            snprintf(uartMemPrint, sizeof(uartMemPrint), "Rosso visualizzato %d volte\n", mem);
            UART_PutString(uartMemPrint);
            
            RED_Pulse(mem);
            /* Show stored times [END] */
            
            mode = 0;
        } else if (mode == 3) { // Erease memory
            SPIFLASH_EraseAll();
            UART_PutString("Memoria cancellata!\n");
            mode = 0;
        }
    }

    return (EXIT_SUCCESS);
}

void clearArray(int *array, int size) {
    char c[10];
    for (int i = 0; i < size; i++) {        
        array[i] = 0; // Imposta ogni elemento a zero
    }
}

void uartManageData() {
    char newChar = uartData[uartCount - 1];

    if (newChar == 0x8) { // When a character is removed (backspace)
        if (uartCount > 1) {
            uartData[--uartCount] = 0; // Remove the cancelled character
            uartData[--uartCount] = 0; // Remove the backspace character
        } else if (uartCount == 1) {
            uartData[--uartCount] = 0; // Remove the backspace character if it's the only one
        }
    } else if (newChar == 0xA) { // Check the string when last char of data is enter (newline)
        uartData[--uartCount] = 0; // Remove the enter character
        
        // Remove invisible chars
        for (int i = 0; i < uartCount; i++) {
            if (!isprint(uartData[i])) {
                uartData[i] = 0;
            }
        }

        if (!strcmp(uartData, "1")) {
            UART_PutString("Scansione colori...\n");
            
            /* Scan beep [START] */
            // Start scan with a 0.5 second 10kHz beep
            AUDIO_BeepStart();
            TIMER2_DelayMS(500);
            AUDIO_BeepStop();
            /* Scan beep [END] */
            mode = 1;
        } else if (!strcmp(uartData, "2")) {
            mode = 2;
        } else if (!strcmp(uartData, "3")) {
            mode = 3;
        }

        clearArray(uartData, uartCount);
        uartCount = 0; // Reset index
        waitUser = 0;
    }
}
