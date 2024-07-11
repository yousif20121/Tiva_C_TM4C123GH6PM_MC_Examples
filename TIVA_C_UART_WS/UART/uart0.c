 /******************************************************************************
 *
 * Module: UART0
 *
 * File Name: uart0.c
 *
 * Description: Source file for the TM4C123GH6PM UART0 driver
 *
 * Author: Yousif Elraey
 *
 *******************************************************************************/

#include "uart0.h"
#include "tm4c123gh6pm_registers.h"

/*******************************************************************************
 *                         Private Functions Definitions                       *
 *******************************************************************************/


/*
 * Function Name:   GPIO_SetupUART0Pins
 *
 * Return Type:     static void
 *
 * Arguments:       void
 *
 * Description:     Function to initialize GPIO Pins to work in UART Mode
 *
 * Note:            This is a helper function used only in this file. It's used with PA0 (Rx) & PA1 (Tx)
 *
 * */
static void GPIO_SetupUART0Pins(void)
{
    SYSCTL_RCGCGPIO_REG  |= 0x01;         /* Enable clock for GPIO PORTA */
    while(!(SYSCTL_PRGPIO_REG & 0x01));   /* Wait until GPIO PORTA clock is activated and it is ready for access*/

    GPIO_PORTA_AMSEL_REG &= 0xFC;         /* Disable Analog on PA0 & PA1 */
    GPIO_PORTA_DIR_REG   &= 0xFE;         /* Configure PA0 as input pin */
    GPIO_PORTA_DIR_REG   |= 0x02;         /* Configure PA1 as output pin */
    GPIO_PORTA_AFSEL_REG |= 0x03;         /* Enable alternative function on PA0 & PA1 */
    /* Set PMCx bits for PA0 & PA1 with value 1 to use PA0 as UART0 RX pin and PA1 as UART0 Tx pin */
    GPIO_PORTA_PCTL_REG  = (GPIO_PORTA_PCTL_REG & 0xFFFFFF00) | 0x00000011;
    GPIO_PORTA_DEN_REG   |= 0x03;         /* Enable Digital I/O on PA0 & PA1 */
}


/*******************************************************************************
 *                         Public Functions Definitions                        *
 *******************************************************************************/


/*
 * Function Name:   UART0_Init
 *
 * Return Type:     void
 *
 * Arguments:       void
 *
 * Description:     Function to initialize UART0
 *
 * */
void UART0_Init(void)
{
    /* Setup UART0 pins PA0 --> U0RX & PA1 --> U0TX */
    GPIO_SetupUART0Pins();

    SYSCTL_RCGCUART_REG |= 0x01;          /* Enable clock for UART0 */
    while(!(SYSCTL_PRUART_REG & 0x01));   /* Wait until UART0 clock is activated and it is ready for access*/

    UART0_CTL_REG = 0;                    /* Disable UART0 at the beginning */

    UART0_CC_REG  = 0;                    /* Use System Clock*/

    /* To Configure UART0 with Baud Rate 9600 */
    UART0_IBRD_REG = 104;
    UART0_FBRD_REG = 11;

    /* UART Line Control Register Settings
     * BRK = 0 Normal Use
     * PEN = 0 Disable Parity
     * EPS = 0 No affect as the parity is disabled
     * STP2 = 0 1-stop bit at end of the frame
     * FEN = 0 FIFOs are disabled
     * WLEN = 0x3 8-bits data frame
     * SPS = 0 no stick parity
     */
    UART0_LCRH_REG = (UART_DATA_8BITS << UART_LCRH_WLEN_BITS_POS);

    /* UART Control Register Settings
     * RXE = 1 Enable UART Receive
     * TXE = 1 Enable UART Transmit
     * HSE = 0 The UART is clocked using the system clock divided by 16
     * UARTEN = 1 Enable UART
     */
    UART0_CTL_REG = UART_CTL_UARTEN_MASK | UART_CTL_TXE_MASK | UART_CTL_RXE_MASK;
}


/*
 * Function Name:   UART0_SendByte
 *
 * Return Type:     void
 *
 * Arguments:       • data -> constant unsigned char of 1 byte (8 bits)
 *
 * Description:     Function to send 1 byte through UART0
 *
 * */
void UART0_SendByte(const uint8 data)
{
    /* Make sure there's no data transmission is happening to begin transmission */
    while(!(UART0_FR_REG & UART_FR_TXFE_MASK));
    /* Write wanted data to uart0 data register*/
    UART0_DR_REG = data;
}


/*
 * Function Name:   UART0_ReceiveByte
 *
 * Return Type:     • data -> unsigned char of 1 byte (8 bits)
 *
 * Arguments:       void
 *
 * Description:     Function to receive 1 byte through UART0
 *
 * */
uint8 UART0_ReceiveByte(void)
{
    /* Make sure there's no data reception is happening to begin transmission */
    while(UART0_FR_REG & UART_FR_RXFE_MASK);
    /* Return the received data from data register */
    return UART0_DR_REG;
}


/*
 * Function Name:   UART0_SendString
 *
 * Return Type:     void
 *
 * Arguments:       • data -> constant unsigned char pointer that points to the first letter of the string/array
 *
 * Description:     Function to send a string of characters through UART0
 *
 * Note:            You must send '#' at the end of your transmission in order for the receive function to work properly
 *
 * */
void UART0_SendString(const uint8* dataP)
{
    /* Loop until you reach the end of the string indicated by NULL */
    while(*dataP != '\0')
    {
        /* Send a byte */
        UART0_SendByte(*dataP);
        /* Move pointer to next address */
        dataP++;
    }
}


/*
 * Function Name:   UART0_ReceiveString
 *
 * Return Type:     void
 *
 * Arguments:       • data -> constant unsigned char pointer that points to the first empty address of the array for the received data to be saved in
 *
 * Description:     Function to receive a string of characters through UART0 protocol
 *
 * */
void UART0_ReceiveString(uint8* dataP)
{
    /* Receive the first byte from the string to initialize the received string */
    *dataP = UART0_ReceiveByte();

    /* Loop till you receive '#' */
    while(*dataP != '#')
    {
        /* Move pointer to next address/element space */
        dataP++;
        /* Save received byte */
        *dataP = UART0_ReceiveByte();
    }

    /* Replace '#' with NULL to indicate the end of the string reception */
    *dataP = '\0';
}


/*
 * Function Name:   UART0_SendData
 *
 * Return Type:     void
 *
 * Arguments:       • data -> constant unsigned char pointer that points to the first position of the data/array
 *
 *                  • uSize -> unsigned long variable to indicate how many bytes to be transmitted
 *
 * Description:     Function to send data of known size (bytes) through UART0
 *
 *
 * */
void UART0_SendData(const uint8 *dataP, uint32 uSize)
{
    /* Loop until transmitted data is over (uSize = 0) */
    while(uSize--)
    {
        /* Send a byte */
        UART0_SendByte(*dataP);
        /* Move pointer to next address */
        dataP++;
    }
}


/*
 * Function Name:   UART0_ReceiveData
 *
 * Return Type:     void
 *
 * Arguments:       • data -> constant unsigned char pointer that points to the first empty address of the array for the received data to be saved in
 *
 *                  • uSize -> unsigned long variable to indicate how many bytes to be received
 *
 * Description:     Function to receive data of known size (bytes) through UART0 protocol
 *
 * */
void UART0_ReceiveData(uint8 *dataP, uint32 uSize)
{
    /* Loop until received data is over (uSize = 0) */
    while(uSize--)
    {
        /* receive a byte */
        *dataP = UART0_ReceiveByte();
        /* Move pointer to next address */
        dataP++;
    }
}




