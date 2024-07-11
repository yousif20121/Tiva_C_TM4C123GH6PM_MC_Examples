 /******************************************************************************
 *
 * Module: SYSTICK_EX_1
 *
 * File Name: main.c
 *
 * Description: Controlling RGB LEDs with delay using SysTick
 *
 * Author: Yousif Elraey
 *
 *******************************************************************************/

#include "tm4c123gh6pm_registers.h"
#include "pins_macros.h"


/* Initializing RGB LED pin (PF1 for red, PF2 for blue, PF3 for green) */
void LEDs_Init(void)
{
    GPIO_PORTF_AFSEL_REG &= 0xF1;         /* Disable alternative function on PF1, PF2 & PF3 */
    GPIO_PORTF_PCTL_REG  &= 0xF1;         /* Clear PMCx bits for PF1, PF2 & PF3 to use it as GPIO pin */
    GPIO_PORTF_AMSEL_REG &= 0xF1;         /* Disable Analog on PF1, PF2 & PF3 */
    GPIO_PORTF_DEN_REG   |= 0x0E;         /* Enable Digital I/O on PF1, PF2 & PF3 */
    GPIO_PORTF_DIR_REG   |= 0x0E;         /* Configure PF1, PF2 & PF3 as output pin */
    GPIO_PORTF_DATA_REG  &= 0xF1;         /* Clear bit 1 in Data register to turn off the leds */
}


void SysTick_Init(void)
{
    SYSTICK_CTRL_REG    = 0;                    /* To clear from previous use */
    SYSTICK_RELOAD_REG  = 15999999;             /* Frequency - 1 */
    SYSTICK_CURRENT_REG = 0;                    /* Clear the current count */
    /* Configure the SysTick Control Register
         * Enable the SysTick Timer (ENABLE = 1)
         * Disable SysTick Interrupt (INTEN = 0)
         * Choose the clock source to be System Clock (CLK_SRC = 1) */
    SYSTICK_CTRL_REG   |= 0x05;
}


int main(void)
{
    /* Enable clock for PORTF and wait for clock to start */
    SYSCTL_RCGCGPIO_REG |= 0x20;
    while(!(SYSCTL_PRGPIO_REG & 0x20));

    LEDs_Init();

    Systick_Init();

    while(1)
    {
        GPIO_PORTF_DATA_REG |= (1<<PF1);  /* RED LED ON */
        /* wait until the COUNT flag = 1 which mean SysTick Timer reaches ZERO value ... COUNT flag is cleared after read the CTRL register value */
        while(!(SYSTICK_CTRL_REG & (1<<SYSTICK_COUNT_BIT)));
        GPIO_PORTF_DATA_REG &= ~(1<<PF1); /* RED LED OFF */

        GPIO_PORTF_DATA_REG |= (1<<PF2);  /* BLUE LED ON */
        /* wait until the COUNT flag = 1 which mean SysTick Timer reaches ZERO value ... COUNT flag is cleared after read the CTRL register value */
        while(!(SYSTICK_CTRL_REG & (1<<SYSTICK_COUNT_BIT)));
        GPIO_PORTF_DATA_REG &= ~(1<<PF2); /* BLUE LED OFF */

        GPIO_PORTF_DATA_REG |= (1<<PF3);  /* BLUE LED ON */
        /* wait until the COUNT flag = 1 which mean SysTick Timer reaches ZERO value ... COUNT flag is cleared after read the CTRL register value */
        while(!(SYSTICK_CTRL_REG & (1<<SYSTICK_COUNT_BIT)));
        GPIO_PORTF_DATA_REG &= ~(1<<PF3); /* BLUE LED OFF */
     }
}
