 /******************************************************************************
 *
 * Module: NVIC_EX_4
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

/* Initializing Global Variables */
uint8 count = 0;


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


void PLL_Init(void)
{
    /* Configure the system to use RCC2 for advanced features such as 400 MHz PLL and non-integer System Clock Divisor */
    SYSCTL_RCC2_REG |=  (1<<USERRCC2_BIT);

    /* Bypass PLL while initializing, Don’t use PLL while initialization */
    SYSCTL_RCC2_REG |=  (1<<BYPASS2_BIT);

    /* Select the crystal value and oscillator source */
    /* XTAL */
    SYSCTL_RCC_REG  &= ~(0x1F<<XTAL_BITS_POS);                                      /* Clear XTAL field -> shift (00001 1111) to XTAL bits then clear */
    SYSCTL_RCC_REG   =  (SYSCTL_RCC_REG & (XTAL_FREQ_16Mhz<<XTAL_BITS_POS));        /* Set the XTAL bits for 16 MHz crystal */

    /* OSC */
    SYSCTL_RCC2_REG &=  ~(0x07<<OSCSRC2_BITS_POS);                                   /* Clear OSCRC2 field -> shift (00000 0111) to XTAL bits then clear */
    SYSCTL_RCC2_REG |=  (SYSCTL_RCC2_REG & (OSCSRC2_MOSC<<OSCSRC2_BITS_POS));       /* Set the OSCSRC2 bits for main OSC */

    /* Activate PLL by clearing PWRDN2 */
    SYSCTL_RCC2_REG &= ~(1<<PWRDN2_BIT);

    /* Set the desired system divider and the system divider least significant bit */
    SYSCTL_RCC2_REG |=  (1<<DIV400_BIT);             /* use 400 MHz PLL */
    SYSCTL_RCC2_REG  =  (SYSCTL_RCC_REG & (SYSDIV2_BITS_POS<<DIV_VALUE));

    /* Wait for the PLL to lock by polling PLLLRIS bit */
    while(!(SYSCTL_RIS_REG & (1<<PLLRIS_BIT)));

    /* Enable use of PLL by clearing BYPASS2 */
    SYSCTL_RCC2_REG &= ~(1<<BYPASS2_BIT);
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
    if(count == 5)
    {
        GPIO_PORTF_DATA_REG &= ~(1<<PF1);       /* RED LED OFF */
        GPIO_PORTF_DATA_REG &= ~(1<<PF2);       /* BLUE LED OFF */
        GPIO_PORTF_DATA_REG &= ~(1<<PF3);       /* GREEN LED OFF */
    }
}


int main(void)
{
    /* Enable clock for PORTF and wait for clock to start */
    SYSCTL_RCGCGPIO_REG |= 0x20;
    while(!(SYSCTL_PRGPIO_REG & 0x20));

    LEDs_Init();

    PLL_Init();

    SysTick_Init();
    INT_NVIC_Init();

    while(1)
    {
        GPIO_PORTF_DATA_REG |= (1<<PF1);  /* RED LED ON */
        while(count < 5)
        {
            /* wait until the COUNT flag = 1 which mean SysTick Timer reaches ZERO value ... COUNT flag is cleared after read the CTRL register value */
            while(!(SYSTICK_CTRL_REG & (1<<SYSTICK_COUNT_BIT)));
            count++;
        }

        count = 0;
        GPIO_PORTF_DATA_REG |= (1<<PF2);  /* BLUE LED ON */
        while(count < 5)
        {
            /* wait until the COUNT flag = 1 which mean SysTick Timer reaches ZERO value ... COUNT flag is cleared after read the CTRL register value */
            while(!(SYSTICK_CTRL_REG & (1<<SYSTICK_COUNT_BIT)));
            count++;
        }

        count = 0;
        GPIO_PORTF_DATA_REG |= (1<<PF3);  /* GREEN LED ON */
        while(count < 5)
        {
            /* wait until the COUNT flag = 1 which mean SysTick Timer reaches ZERO value ... COUNT flag is cleared after read the CTRL register value */
            while(!(SYSTICK_CTRL_REG & (1<<SYSTICK_COUNT_BIT)));
            count++;
        }
     }
}
