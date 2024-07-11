 /******************************************************************************
 *
 * Module: SYSTICK_EX_1
 *
 * File Name: main.c
 *
 * Description: Controlling LED with delay using SysTick
 *
 * Author: Yousif Elraey
 *
 *******************************************************************************/

#include "tm4c123gh6pm_registers.h"
#include "pins_macros.h"


/* Initializing RGB LED pin (PF1 for red, PF2 for blue, PF3 for green) */
void LED_Init(void)
{
    GPIO_PORTF_AFSEL_REG &= ~(1 << PF1);        /* Disable alternative function on PF1 */
    GPIO_PORTF_PCTL_REG  &= 0xFFFFFF0F;         /* Clear PMCx bits for PF1 to use it as GPIO pin */
    GPIO_PORTF_AMSEL_REG &= ~(1 << PF1);        /* Disable Analog on PF1 */
    GPIO_PORTF_DEN_REG   |= (1 << PF1);         /* Enable Digital I/O on PF1 */
    GPIO_PORTF_DIR_REG   |= (1 << PF1);         /* Configure PF1 as output pin */
    GPIO_PORTF_DATA_REG  &= ~(1 << PF1);        /* Clear bit 1 in Data register to turn off the led */
}

/* Initializing Switch pin (PF0* for SW2, PF1 for SW1) */
/* PF0 requires unlocking and commit */
void SW2_Init(void)
{
    GPIO_PORTF_LOCK_REG   = UNLOCK_ALL_PORT;     /* Unlock the GPIO_PORTF_CR_REG */
    GPIO_PORTF_CR_REG    |= (1 << PF0);         /* Enable changes on PF0 */

    GPIO_PORTF_AFSEL_REG &= ~(1 << PF0);        /* Disable alternative function on PF0 */
    GPIO_PORTF_PCTL_REG  &= 0xFFFFFFF0;         /* Clear PMCx bits for PF0 to use it as GPIO pin */
    GPIO_PORTF_AMSEL_REG &= ~(1 << PF0);        /* Disable Analog on PF0 */
    GPIO_PORTF_DEN_REG   |= (1 << PF0);         /* Enable Digital I/O on PF0 */
    GPIO_PORTF_DIR_REG   |= (1 << PF0);         /* Configure PF0 as input pin */
    GPIO_PORTF_PUR_REG   |= (1 << PF0);         /* Enable pull-up on PF0 */
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

    LED_Init();
    SW2_Init();

    Systick_Init();

    while(1)
    {
        /* Check the switch state */
        if(!(GPIO_PORTF_DATA_REG & (1<<PF0)))
        {
        GPIO_PORTF_DATA_REG |= (1<<PF1);  /* LED ON */
        /* wait until the COUNT flag = 1 which mean SysTick Timer reaches ZERO value ... COUNT flag is cleared after read the CTRL register value */
        while(!(SYSTICK_CTRL_REG & (1<<SYSTICK_COUNT_BIT)));

        GPIO_PORTF_DATA_REG &= ~(1<<PF1); /* LED OFF */
        /* wait until the COUNT flag = 1 which mean SysTick Timer reaches ZERO value ... COUNT flag is cleared after read the CTRL register value */
        while(!(SYSTICK_CTRL_REG & (1<<SYSTICK_COUNT_BIT)));
        }
        else
        {
        GPIO_PORTF_DATA_REG &= ~(1<<PF1); /* LED OFF */
        }
     }
}