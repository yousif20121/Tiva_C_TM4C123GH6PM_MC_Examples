 /******************************************************************************
 *
 * Module: NVIC_EX_5
 *
 * File Name: main.c
 *
 * Description: Generating a hard fault by accessing SysTick in unprivileged mode
 *
 * Author: Yousif Elraey
 *
 *******************************************************************************/

#include "tm4c123gh6pm_registers.h"
#include "main.h"

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

/* Set PRIV Bit(Bit 0) in the CONTROL register to switch to Unprivileged access level */
void Switch_To_Unprivileged(void)
{
    __asm(" MOV R0, #1 ");              /* Set the R0 register value to 1 */
    __asm(" MSR CONTROL, R0 ");         /* Set the PRIV Bit(Bit 0) to 1 ... This will switch to Unprivileged access level */
}


int main(void)
{
    INT_NVIC_Init();

    Switch_To_Unprivileged();

    SysTick_Init();

    while(1)
    {}
}
