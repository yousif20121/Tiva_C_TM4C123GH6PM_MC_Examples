 /******************************************************************************
 *
 * Module: NVIC_EX_6
 *
 * File Name: main.c
 *
 * Description: Switch from unprivileged to privileged access using SVC (Software Interrupt)
 *
 * Author: Yousif Elraey
 *
 *******************************************************************************/

#include "tm4c123gh6pm_registers.h"
#include "main.h"

/* MACROs */
#define SVC_PRIORITY_MASK            0x1FFFFFFF
#define SVC_PRIORITY                 0
#define SVC_PRIORITY_BITS_POS        29

/* Trigger SVC Exception ... This Macro use the SVC instruction to make SW Interrupt */
#define Trigger_SVC_Exception() __asm(" SVC #0 ")

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

void SVC_Init(void)
{
    /* Assign priority level 0 to the SVC Interrupt */
    NVIC_SYSTEM_PRI2_REG = (NVIC_SYSTEM_PRI2_REG & SVC_PRIORITY_MASK) | (SVC_PRIORITY << SVC_PRIORITY_BITS_POS);
}


void SVC_Handler(void)
{
    uint8 SVC_Num = 0;              /* First local variable stored in stack */
    __asm(" LDR R3, [SP, #32] ");   /* Extract the stacked PC register value which vectoring to this handler and add it to R3 */
    __asm(" LDRB R3, [R3, #-2] ");  /* Extract the SVC number value, it is exist in the first byte of address PC-2 */
    __asm(" STR R3, [SP] ");        /* Load the R3 value to SVC_Num variable as SP is pointing to SVC_Num location in the stack memory */
    switch(SVC_Num)
    {
    case 0:
        __asm(" MOV R3, #0 ");      /* Load the R3 register with value 0 */
        __asm(" MSR CONTROL, R3 "); /* Clear the PRIV Bit(Bit 0) ... This will switch to Privileged access level */
        break;
    case 1:
    case 2:
    case 3:
    case 255:
        break;
    default:
        break;
    }
}


int main(void)
{
    INT_NVIC_Init();

    SVC_Init();

    Switch_To_Unprivileged();

    Trigger_SVC_Exception();

    SysTick_Init();

    while(1)
    {}
}
