/**
 * @file uart.c
 * @author Keenan Alchaar (ka5nt@virginia.edu) and Eli Jelesko (ebj5hec@virginia.edu)
 * @brief Code for the UART communications on the MSP432e401Y
 * @version 0.1
 * @date 2022-10-03
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "uart.h"
#include "gpio.h"
#include "utils.h"


// TODO: Make sure the elements are only 1 byte long!
static fifo_t uart_0_rx;
static fifo_t uart_0_tx;


static void copy_hardware_to_software(uint8_t uart_channel);
static void copy_software_to_hardware(uint8_t uart_channel);


void uart_init(uint8_t uart_channel) {
    // Currently only does UART0
    switch (uart_channel)
    {
        case UART_CHANNEL_0:
            // Initialize the FIFOS
            fifo_init(&uart_0_rx);
            fifo_init(&uart_0_tx);

            // Enable the UART clock gate control
            SYSCTL->RCGCUART |= SYSCTL_RCGCUART_R0;

            // Enable the GPIO pins
            utils_gpio_clock_enable(GPIOA);
            gpio_set_as_output(GPIOA, GPIO_PIN_0); // U0Rx
            gpio_set_as_output(GPIOA, GPIO_PIN_1); // U0Rx
            gpio_select_alternate(GPIOA, GPIO_PIN_0, 1); // 1st alternate
            gpio_select_alternate(GPIOA, GPIO_PIN_1, 1); // 1st alternate

            // Disable UART
            UART0->CTL &= ~UART_CTL_UARTEN;
            // Set baud clock source to PIOSC (16MHz)
            UART0->CC |= UART_CC_CS_PIOSC;
            // Configure for a 115200 baud rate
            // 16MHz / (16 * 115200) = 8.8056
            UART0->IBRD |= (8 << UART_IBRD_DIVINT_S); // Integer
            UART0->FBRD |= (52 << UART_FBRD_DIVFRAC_S); // Fractional
            // Enable FIFOs and set 8 bit word length
            UART0->LCRH |= (UART_LCRH_FEN | UART_LCRH_WLEN_8);
            // Set the clock source to PI0SC (16 MHz)
            UART0->CC |= UART_CC_CS_PIOSC;
            // Set the interrupt trigger levels
            // Triggers when both the Tx and Rx FIFOs are 1/8 full
            UART0->IFLS |= (UART_IFLS_RX1_8 | UART_IFLS_TX1_8);
            // Enable the FIFOs and Rx timeout interrupt
            UART0->IM |= (UART_IM_RXIM | UART_IM_TXIM | UART_IM_RTIM);
            // Set the UART0 interrupt to priority 2
            NVIC->IP[1] |= (2 << NVIC_PRI1_INT5_S);
            // Enable the UART0 interrupt
            NVIC->ISER[0] |= (1 << UART0_IRQn);

            // Enable UART
            UART0->CTL |= UART_CTL_UARTEN;
        break;

        default:

        break;
    }


}

bool uart_out_byte(uint8_t uart_channel, uint8_t byte)
{
    bool output = fifo_push(&uart_0_tx, byte);
    // If the Tx FIFO is empty copy to hardware
    if (UART0->FR & UART_FR_TXFE)
    {
        copy_software_to_hardware(UART_CHANNEL_0);
    }
    return output;
}

bool uart_read_byte(uint8_t uart_channel, uint8_t* byte)
{
    return fifo_pop(&uart_0_rx, byte);
}

static void copy_hardware_to_software(uint8_t uart_channel)
{
    uint8_t byte;
    // While there is data in the hardware buffer, copy it into the software buffer
    switch (uart_channel)
    {
        case UART_CHANNEL_0:
            // While the Rx FIFO is not empty and the software FIFO is not full, copy over
            while ((UART0->FR & UART_FR_RXFE == 0) && (fifo_get_size(&uart_0_rx) < FIFO_SIZE))
            {
                byte = (UART0->DR & UART_DR_DATA_M);
                fifo_push(&uart_0_rx, byte);
            }
        break;

        case UART_CHANNEL_1:

        break;

        case UART_CHANNEL_2:

        break;

        case UART_CHANNEL_3:

        break;

        case UART_CHANNEL_4:

        break;

        case UART_CHANNEL_5:

        break;

        case UART_CHANNEL_6:

        break;

        default:

        break;

    }
}
static void copy_software_to_hardware(uint8_t uart_channel)
{
    uint8_t byte;
    // While there is data in the software buffer, copy it into the hardware buffer
    switch (uart_channel)
    {
        case UART_CHANNEL_0:
            // While the Tx FIFO is not full and the software FIFO is not empty, copy over
            while (((UART0->FR & UART_FR_TXFF) == 0) && !fifo_is_empty(&uart_0_tx))
            {
                if (fifo_pop(&uart_0_tx, &byte))
                {
                    UART0->DR = byte;
                }
                else
                {
                    // Something went wrong reading from the software FIFO
                    break;
                }

            }
        break;

        case UART_CHANNEL_1:

        break;

        case UART_CHANNEL_2:

        break;

        case UART_CHANNEL_3:

        break;

        case UART_CHANNEL_4:

        break;

        case UART_CHANNEL_5:

        break;

        case UART_CHANNEL_6:

        break;

        default:

        break;

    }
}

__interrupt void UART0_IRQHandler(void)
{
    // The Tx interrupt occurred
    if (UART0->MIS & UART_MIS_TXMIS)
    {
        UART0->ICR |= UART_ICR_TXIC; // Clear the interrupt
        copy_software_to_hardware(UART_CHANNEL_0);
    }
    // The Rx interrupt occurred
    else if (UART0->MIS & UART_MIS_RXMIS)
    {
        UART0->ICR |= UART_ICR_RXIC; // Clear the interrupt
        copy_hardware_to_software(UART_CHANNEL_0);
    }
    // The Rx Timeout interrupt occurred
    else if (UART0->MIS & UART_MIS_RTMIS)
    {
        UART0->ICR |= UART_ICR_RTIC; // Clear the interrupt
        copy_hardware_to_software(UART_CHANNEL_0);
    }
    // Some other interrupt. Probably a fault of some kind
    else {
        // For debugging purposes
    }
}


/* End uart.c */
