 /******************************************************************************
 *
 * Module: NVIC_EX_3
 *
 * File Name: main.c
 *
 * Description: Toggling LED with interrupts using SysTick
 *
 * Author: Yousif Elraey
 *
 *******************************************************************************/

#include "tm4c123gh6pm_registers.h"
#include "main.h"


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
    SYSTICK_RELOAD_REG  = 15999999/2;           /* Frequency - 1 (16MHz --> 1 second) */
    SYSTICK_CURRENT_REG = 0;                    /* Clear the current count */
    /* Configure the SysTick Control Register
         * Enable the SysTick Timer (ENABLE = 1)
         * Disable SysTick Interrupt (INTEN = 1)
         * Choose the clock source to be System Clock (CLK_SRC = 1) */
    SYSTICK_CTRL_REG   |= 0x07;
}

void INT_NVIC_Init(void)
{
    Enable_Exceptions();
    Enable_Faults();
    NVIC_SYSTEM_PRI3_REG &= ~(1<<SYSTICK_PRIORITY_BITS_POS);          /* Setting priority of PORTF to 0 (HIGHEST) */
    NVIC_EN0_REG   |=  (1<<GPIO_PORTF_EN0_BIT_POS);                   /* Enable Bit 30 (from Vector Table) in EN0 Register */
}

void SysTick_Handler(void)
{
    GPIO_PORTF_DATA_REG &= ~(1<<PF1); /* RED LED OFF */
}


int main(void)
{
    /* Enable clock for PORTF and wait for clock to start */
    SYSCTL_RCGCGPIO_REG |= 0x20;
    while(!(SYSCTL_PRGPIO_REG & 0x20));

    LEDs_Init();

    SysTick_Init();
    INT_NVIC_Init();

    while(1)
    {
        GPIO_PORTF_DATA_REG |= (1<<PF1);  /* RED LED ON */
        /* wait until the COUNT flag = 1 which mean SysTick Timer reaches ZERO value ... COUNT flag is cleared after read the CTRL register value */
        while(!(SYSTICK_CTRL_REG & (1<<SYSTICK_COUNT_BIT)));
     }
}
