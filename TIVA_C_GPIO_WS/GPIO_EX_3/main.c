 /******************************************************************************
 *
 * Module: GPIO_EX_3
 *
 * File Name: main.c
 *
 * Description: Controlling RGB LED with push button on Tiva-C Launch pad
 *
 * Author: Yousif Elraey
 *
 *******************************************************************************/

#include "tm4c123gh6pm_registers.h"
#include "pins_macros.h"

/* Initializing RGB LEDs pins' (PF1 for red, PF2 for blue, PF3 for green) */
void LEDs_Init(void)
{
    GPIO_PORTF_AFSEL_REG &= 0xF1;               /* Disable alternative function on PF1 */
    GPIO_PORTF_PCTL_REG  &= 0xFFFF000F;         /* Clear PMCx bits for PF1 to use it as GPIO pin */
    GPIO_PORTF_AMSEL_REG &= 0xF1;               /* Disable Analog on PF1 */
    GPIO_PORTF_DEN_REG   |= 0x0E;               /* Enable Digital I/O on PF1 */
    GPIO_PORTF_DIR_REG   |= 0x0E;               /* Configure PF1 as output pin */
    GPIO_PORTF_DATA_REG  &= 0xF1;               /* Clear bit 1 in Data register to turn off the led */
}

void SysTick_Init(void)
{
    SYSTICK_CTRL_REG    = 0;                    /* To clear from previous use */
    SYSTICK_RELOAD_REG  = 15999999;             /* Frequency - 1 */
    SYSTICK_CURRENT_REG = 0;                    /* Clear the current count */
    SYSTICK_CTRL_REG   |= 0x05;                 /* ENABLE & CLC_SRC */
}


int main(void)
{
    /* Enable clock for PORTF and wait for clock to start */
    SYSCTL_RCGCGPIO_REG |= 0x20;
    while(!(SYSCTL_PRGPIO_REG & 0x20));

    LEDs_Init();
    SysTick_Init();

    while(1)
    {
    GPIO_PORTF_DATA_REG |= (1 << PF1);      // Red LED on
    while(!(SYSTICK_CTRL_REG & (1<<16)));

    GPIO_PORTF_DATA_REG |= (1 << PF2);      // Blue LED on
    while(!(SYSTICK_CTRL_REG & (1<<16)));

    GPIO_PORTF_DATA_REG |= (1 << PF3);      // Green LED on
    while(!(SYSTICK_CTRL_REG & (1<<16)));
    }

}
