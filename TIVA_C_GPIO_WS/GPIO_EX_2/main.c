 /******************************************************************************
 *
 * Module: GPIO_EX_2
 *
 * File Name: main.c
 *
 * Description: Controlling Blue LED with push button on Tiva-C Launch pad
 *
 * Author: Yousif Elraey
 *
 *******************************************************************************/

#include "tm4c123gh6pm_registers.h"
#include "pins_macros.h"

/* Initializing flag for LED status */
uint8 flag = 0;

/* Initializing RGB LED pin (PF1 for red, PF2 for blue, PF3 for green) */
void LED_Init(void)
{
    GPIO_PORTF_AFSEL_REG &= ~(1 << PF2);        /* Disable alternative function on PF2 */
    GPIO_PORTF_PCTL_REG  &= 0xFFFFF0FF;         /* Clear PMCx bits for PF2 to use it as GPIO pin */
    GPIO_PORTF_AMSEL_REG &= ~(1 << PF2);        /* Disable Analog on PF2 */
    GPIO_PORTF_DEN_REG   |= (1 << PF2);         /* Enable Digital I/O on PF2 */
    GPIO_PORTF_DIR_REG   |= (1 << PF2);         /* Configure PF2 as output pin */
    GPIO_PORTF_DATA_REG  &= ~(1 << PF2);        /* Clear bit 1 in Data register to turn off the led */
}

/* Initializing Switch pin (PF0* for SW2, PF4 for SW1) */
void SW1_Init(void)
{
    GPIO_PORTF_AFSEL_REG &= ~(1 << PF4);        /* Disable alternative function on PF4 */
    GPIO_PORTF_PCTL_REG  &= 0xFFF0FFFF;         /* Clear PMCx bits for PF4 to use it as GPIO pin */
    GPIO_PORTF_AMSEL_REG &= ~(1 << PF4);        /* Disable Analog on PF4 */
    GPIO_PORTF_DEN_REG   |= (1 << PF4);         /* Enable Digital I/O on PF4 */
    GPIO_PORTF_DIR_REG   |= (1 << PF4);         /* Configure PF4 as input pin */
    GPIO_PORTF_PUR_REG   |= (1 << PF4);         /* Enable pull-up on PF4 */
}


int main(void)
{
    /* Enable clock for PORTF and wait for clock to start */
    SYSCTL_RCGCGPIO_REG |= 0x20;
    while(!(SYSCTL_PRGPIO_REG & 0x20));

    LED_Init();
    SW1_Init();

    while(1)
    {
        if(!(GPIO_PORTF_DATA_REG & (1 << PF4)) && (flag == 0))
        {
            GPIO_PORTF_DATA_REG |= (1 << PF2);          // LED on
            flag = 1;
        }
        else
        {
            GPIO_PORTF_DATA_REG &= ~(1 << PF2);         // LED off
            flag = 0;
        }

    }

}
