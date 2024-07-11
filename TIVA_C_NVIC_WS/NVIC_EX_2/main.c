 /******************************************************************************
 *
 * Module: NVIC_EX_1
 *
 * File Name: main.c
 *
 * Description: Controlling 3 LEDs through edge triggered interrupt
 *
 * Author: Yousif Elraey
 *
 *******************************************************************************/

#include "main.h"
#include "tm4c123gh6pm_registers.h"

volatile uint8 INT_FLAG = 0;

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

void INT_NVIC_Init(void)
{
    Enable_Exceptions();
    Enable_Faults();
    NVIC_PRI17_REG &= ~(1<<GPIO_PORTF_PRIORITY_BITS_POS);             /* Setting priority of PORTF to 0 (HIGHEST) */
    NVIC_EN0_REG   |=  (1<<GPIO_PORTF_EN0_BIT_POS);                   /* Enable Bit 30 (from Vector Table) in EN0 Register */
}

void INT_GPIO_CFG(void)
{
    GPIO_PORTF_IS_REG  &= ~(1<<PF4);
    GPIO_PORTF_IBE_REG &= ~(1<<PF4);
    GPIO_PORTF_IEV_REG &= ~(1<<PF4);        /* Detecting falling edge */
    GPIO_PORTF_IM_REG  |=  (1<<PF4);
    GPIO_PORTF_ICR_REG &= ~(1<<PF4);        /* Clearing interrupt flag from previous use */
}

void SW1_IntHandler(void)
{
    INT_FLAG = 1;
    GPIO_PORTF_ICR_REG |= (1<<PF4);         /* Clearing interrupt flag for future use */
}

int main(void)
{
    /* Enable clock for PORTF and wait for clock to start */
    SYSCTL_RCGCGPIO_REG |= 0x20;
    while(!(SYSCTL_PRGPIO_REG & 0x20));


    /* Initialize the SW1 and Blue LED as GPIO Pins */
    SW1_Init();
    LEDs_Init();

    /* Initialize NVIC and configure GPIO pin */
    INT_NVIC_Init();
    INT_GPIO_CFG();

    while(1)
    {
        if(INT_FLAG == 1)
        {
            GPIO_PORTF_DATA_REG |= (1<<PF1);
            GPIO_PORTF_DATA_REG |= (1<<PF2);
            GPIO_PORTF_DATA_REG |= (1<<PF3);
            INT_FLAG = 0;
        }
        else
        {
            GPIO_PORTF_DATA_REG &= ~(1<<PF1);
            GPIO_PORTF_DATA_REG &= ~(1<<PF2);
            GPIO_PORTF_DATA_REG &= ~(1<<PF3);
        }
    }
}
