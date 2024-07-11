 /******************************************************************************
 *
 * Module: GPIO_EX_1
 *
 * File Name: pins_macros.h
 *
 * Description:
 *
 * Author: Yousif Elraey
 *
 *******************************************************************************/

#ifndef PINS_MACROS_H_
#define PINS_MACROS_H_

/* Magic Numbers */
#define UNLOCK_ALL_PORT     0x4C4F434B      // value for unlocking ports (used with GPIO_PORTF_LOCK_REG)

/* PORTS */
/* PORT A */
#define PA0                     0
#define PA1                     1
#define PA2                     2
#define PA3                     3
#define PA4                     4
#define PA5                     5
#define PA6                     6
#define PA7                     7

/* PORT B */
#define PB0                     0
#define PB1                     1
#define PB2                     2
#define PB3                     3
#define PB4                     4
#define PB5                     5
#define PB6                     6
#define PB7                     7

/* PORT C */
#define PC0                     0
#define PC1                     1
#define PC2                     2
#define PC3                     3
#define PC4                     4
#define PC5                     5
#define PC6                     6
#define PC7                     7

/* PORT D */
#define PD0                     0
#define PD1                     1
#define PD2                     2
#define PD3                     3
#define PD4                     4
#define PD5                     5
#define PD6                     6
#define PD7                     7

/* PORT F */
#define PF0                     0
#define PF1                     1
#define PF2                     2
#define PF3                     3
#define PF4                     4

/* SysTick */
#define SYSTICK_COUNT_BIT       16

/* PLL */
/* RCC2 REG */
#define USERRCC2_BIT            31
#define BYPASS2_BIT             11
#define PWRDN2_BIT              13
#define DIV400_BIT              30
#define SYSDIV2_BITS_POS        22
#define DIV_VALUE               39            // Adjust according to wanted frequency (400MHz/DIV_VALUE) (DIV_VALUE = n - 1)
#define OSCSRC2_BITS_POS        4
#define OSCSRC2_MOSC            0x0           // Main OSC

/* RCC REG */
#define XTAL_BITS_POS           6
#define XTAL_FREQ_16MHz         0x15

/* RIS REG */
#define PLLRIS_BIT              6

#endif /* PINS_MACROS_H_ */
