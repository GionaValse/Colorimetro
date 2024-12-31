/* 
 * File:   config.h
 * @brief Configuration file for system settings.
 *
 * This file contains the definitions and macros for configuring system settings such as peripheral bus clock and SPI flash parameters.
 *
 * @date December 16, 2024
 */

#ifndef CONFIG_H
#define	CONFIG_H

#define PB_CLK 40000000

#define SPIFLASH_PROG_SIZE  2
#define SPIFLASH_PROG_ADDR  0x100

#define macro_enable_interrupts() {\
    unsigned int val = 0;\
    asm volatile("mfc0 %0,$13":"=r"(val));\
    val |= 0x00800000;\
    asm volatile("mtc0 %0,$13" : "+r"(val));\
    INTCONbits.MVEC = 1;\
    __builtin_enable_interrupts();\
}

#endif	/* CONFIG_H */

