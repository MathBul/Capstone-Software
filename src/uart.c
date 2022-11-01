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
static uart_fifo_t uart_0_rx;
static uart_fifo_t uart_0_tx;
static uart_fifo_t uart_2_rx;
static uart_fifo_t uart_2_tx;
static uart_fifo_t uart_3_rx;
static uart_fifo_t uart_3_tx;
static uart_fifo_t uart_6_rx;
static uart_fifo_t uart_6_tx;

// Private functions declarations
static void copy_hardware_to_software(uint8_t uart_channel);
static void copy_software_to_hardware(uint8_t uart_channel);

/**
 * @brief Initializes the specified UART channel
 * 
 * @param uart_channel One of UART_CHANNEL_X
 */
void uart_init(uint8_t uart_channel) {
    // Currently only does UART0
    switch (uart_channel)
    {
        case UART_CHANNEL_0:
            // Initialize the FIFOS
            uart_fifo_init(&uart_0_rx);
            uart_fifo_init(&uart_0_tx);

            // Enable the UART clock gate control
            SYSCTL->RCGCUART |= SYSCTL_RCGCUART_R0;

            // Wait for the clock to enable
            while (!(SYSCTL->PRUART & SYSCTL_RCGCUART_R0))
            {
            }

            // Enable the GPIO pins
            utils_gpio_clock_enable(GPIOA);
            gpio_set_as_input(GPIOA, GPIO_PIN_0); // U0Rx
            gpio_set_as_output(GPIOA, GPIO_PIN_1); // U0Tx
            gpio_select_alternate(GPIOA, GPIO_PIN_0, 1); // 1st alternate
            gpio_select_alternate(GPIOA, GPIO_PIN_1, 1); // 1st alternate

            // Disable UART
            UART0->CTL &= ~UART_CTL_UARTEN;

            // Set baud clock source to PIOSC (16MHz)
            // Configure for a 9600 baud rate
            // 16MHz / (16 * 9600) = 104.16667
            UART0->IBRD |= (104 << UART_IBRD_DIVINT_S); // Integer
            UART0->FBRD |= (11 << UART_FBRD_DIVFRAC_S); // Fractional

            // Enable FIFOs and set 8 bit word length
            UART0->LCRH |= (UART_LCRH_FEN | UART_LCRH_WLEN_8);

            // Set the baud clock source to PIOSC (16 MHz)
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

        case UART_CHANNEL_2:
            // Initialize the FIFOS
            uart_fifo_init(&uart_2_rx);
            uart_fifo_init(&uart_2_tx);

            // Enable the UART clock gate control
            SYSCTL->RCGCUART |= SYSCTL_RCGCUART_R2;

            // Wait for the clock to enable
            while (!(SYSCTL->PRUART & SYSCTL_RCGCUART_R2))
            {
            }

            // Enable the GPIO pins
            utils_gpio_clock_enable(GPIOD);
            gpio_set_as_input(GPIOD, GPIO_PIN_4); // U2Rx
            gpio_set_as_output(GPIOD, GPIO_PIN_5); // U2Tx
            gpio_select_alternate(GPIOD, GPIO_PIN_4, 1); // 1st alternate
            gpio_select_alternate(GPIOD, GPIO_PIN_5, 1); // 1st alternate

            // Disable UART
            UART2->CTL &= ~UART_CTL_UARTEN;

            // Configure for a 9600 baud rate
            // 16MHz / (16 * 9600) = 104.16667
            UART2->IBRD |= (104 << UART_IBRD_DIVINT_S); // Integer
            UART2->FBRD |= (11 << UART_FBRD_DIVFRAC_S); // Fractional

            // Enable FIFOs and set 8 bit word length
            UART2->LCRH |= (UART_LCRH_FEN | UART_LCRH_WLEN_8);

            // Set the baud clock source to PIOSC (16 MHz)
            UART2->CC |= UART_CC_CS_PIOSC;

            // Set the interrupt trigger levels
            // Triggers when both the Tx and Rx FIFOs are 1/8 full
            UART2->IFLS |= (UART_IFLS_RX1_8 | UART_IFLS_TX1_8);

            // Enable the FIFOs and Rx timeout interrupt
            UART2->IM |= (UART_IM_RXIM | UART_IM_TXIM | UART_IM_RTIM);

            // Set the UART2 interrupt (#33) to priority 2 (IP[14] services interrupts 32-35)
            NVIC->IP[8] |= ((uint32_t) 2 << NVIC_PRI8_INT33_S);

            // Enable the UART2 interrupt (ISER[1] services interrupts 32-63)
            NVIC->ISER[1] |= ((uint32_t) 1 << (UART2_IRQn - 32));

            // Enable UART
            UART2->CTL |= UART_CTL_UARTEN;
            break;

        case UART_CHANNEL_3:
            // Initialize the FIFOS
            uart_fifo_init(&uart_3_rx);
            uart_fifo_init(&uart_3_tx);

            // Enable the UART clock gate control
            SYSCTL->RCGCUART |= SYSCTL_RCGCUART_R3;

            // Wait for the clock to enable
            while (!(SYSCTL->PRUART & SYSCTL_RCGCUART_R3))
            {
            }

            // Enable the GPIO pins
            utils_gpio_clock_enable(GPIOA);
            gpio_set_as_input(GPIOA, GPIO_PIN_4); // U3Rx
            gpio_set_as_output(GPIOA, GPIO_PIN_5); // U3Tx
            gpio_select_alternate(GPIOA, GPIO_PIN_4, 1); // 1st alternate
            gpio_select_alternate(GPIOA, GPIO_PIN_5, 1); // 1st alternate

            // Disable UART
            UART3->CTL &= ~UART_CTL_UARTEN;

            // Configure for a 9600 baud rate
            // 16MHz / (16 * 9600) = 104.16667
            UART3->IBRD |= (104 << UART_IBRD_DIVINT_S); // Integer
            UART3->FBRD |= (11 << UART_FBRD_DIVFRAC_S); // Fractional

            // Enable FIFOs and set 8 bit word length
            UART3->LCRH |= (UART_LCRH_FEN | UART_LCRH_WLEN_8);

            // Set the baud clock source to PI0SC (16 MHz)
            UART3->CC |= UART_CC_CS_PIOSC;

            // Set the interrupt trigger levels
            // Triggers when both the Tx and Rx FIFOs are 1/8 full

            UART3->IFLS |= (UART_IFLS_RX1_8 | UART_IFLS_TX1_8);

            // Enable the FIFOs and Rx timeout interrupt
            UART3->IM |= (UART_IM_RXIM | UART_IM_TXIM | UART_IM_RTIM);

            // Set the UART3 interrupt (#56) to priority 2 (IP[14] services interrupts 56-59)
            NVIC->IP[14] |= ((uint32_t) 2 << NVIC_PRI14_INTA_S);

            // Enable the UART3 interrupt (ISER[1] services interrupts 32-63)
            NVIC->ISER[1] |= (1 << (UART3_IRQn - 32));

            // Enable UART
            UART3->CTL |= UART_CTL_UARTEN;
            break;

        case UART_CHANNEL_6:
            // Initialize the FIFOS
            uart_fifo_init(&uart_6_rx);
            uart_fifo_init(&uart_6_tx);

            // Enable the UART clock gate control
            SYSCTL->RCGCUART |= SYSCTL_RCGCUART_R6;

            // Wait for the clock to enable
            while (!(SYSCTL->PRUART & SYSCTL_RCGCUART_R6))
            {
            }

            // Enable the GPIO pins
            utils_gpio_clock_enable(GPIOP);
            gpio_set_as_input(GPIOP, GPIO_PIN_0); // U6Rx
            gpio_set_as_output(GPIOP, GPIO_PIN_1); // U6Tx
            gpio_select_alternate(GPIOP, GPIO_PIN_0, 1); // 1st alternate
            gpio_select_alternate(GPIOP, GPIO_PIN_1, 1); // 1st alternate

            // Disable UART
            UART6->CTL &= ~UART_CTL_UARTEN;

            // Set baud clock source to PIOSC (16MHz)
            // Configure for a 9600 baud rate
            // 16MHz / (16 * 9600) = 104.16667
            UART6->IBRD |= (104 << UART_IBRD_DIVINT_S); // Integer
            UART6->FBRD |= (11 << UART_FBRD_DIVFRAC_S); // Fractional

            // Enable FIFOs and set 8 bit word length
            UART6->LCRH |= (UART_LCRH_FEN | UART_LCRH_WLEN_8);

            // Set the baud clock source to PI0SC (16 MHz)
            UART6->CC |= UART_CC_CS_PIOSC;

            // Set the interrupt trigger levels
            // Triggers when both the Tx and Rx FIFOs are 1/8 full
            UART6->IFLS |= (UART_IFLS_RX1_8 | UART_IFLS_TX1_8);

            // Enable the FIFOs and Rx timeout interrupt
            UART6->IM |= (UART_IM_RXIM | UART_IM_TXIM | UART_IM_RTIM);

            // Set the UART6 interrupt (#59) to priority 2 (IP[14] services interrupts 56-59)
            NVIC->IP[14] |= ((uint32_t) 2 << NVIC_PRI14_INTD_S);

            // Enable the UART6 interrupt (ISER[1] services interrupts 32-63)
            NVIC->ISER[1] |= ((uint32_t) 1 << (UART6_IRQn - 32));

            // Enable UART
            UART6->CTL |= UART_CTL_UARTEN;
            break;

        default:

        break;
    }


}

/**
 * @brief Sends a single byte (uint8_t) to the specified UART channel
 * 
 * @param uart_channel One of UART_CHANNEL_X
 * @param byte The uint8_t to be sent
 * @return true The byte was sent
 * @return false The byte could not be sent
 */
bool uart_out_byte(uint8_t uart_channel, uint8_t byte)
{
    bool output;
    switch (uart_channel)
    {
        case UART_CHANNEL_0:
            output = uart_fifo_push(&uart_0_tx, byte);

            // If the Tx FIFO is empty copy to hardware
            if (UART0->FR & UART_FR_TXFE)
            {
                copy_software_to_hardware(UART_CHANNEL_0);
            }
        break;
        case UART_CHANNEL_2:
            output = uart_fifo_push(&uart_2_tx, byte);

            // If the Tx FIFO is empty copy to hardware
            if (UART2->FR & UART_FR_TXFE)
            {
                copy_software_to_hardware(UART_CHANNEL_2);
            }
        break;
        case UART_CHANNEL_3:
            output = uart_fifo_push(&uart_3_tx, byte);

            // If the Tx FIFO is empty copy to hardware
            if (UART3->FR & UART_FR_TXFE)
            {
                copy_software_to_hardware(UART_CHANNEL_3);
            }
        break;
        case UART_CHANNEL_6:
            output = uart_fifo_push(&uart_6_tx, byte);

            // If the Tx FIFO is empty copy to hardware
            if (UART6->FR & UART_FR_TXFE)
            {
                copy_software_to_hardware(UART_CHANNEL_6);
            }
        break;
        default:
            output = false;
        break;
    }

    return output;
}

/**
 * @brief Sends a string to the specified UART channel
 *
 * @param uart_channel One of UART_CHANNEL_X
 * @param string The string to be sent
 * @return true The string was sent
 * @return false The string could not be sent
 */
bool uart_out_string(uint8_t uart_channel, char* string)
{
    bool output = true;
    char* val = &string[0];
    switch (uart_channel)
    {
        case UART_CHANNEL_0:
            // Put all of the characters onto the software buffer
            while (*(val) != '\0')
            {
                output &= uart_fifo_push(&uart_0_tx, (uint8_t) *val);
                val++;
            }

            // If the Tx FIFO is empty copy to hardware
            if (UART0->FR & UART_FR_TXFE)
            {
                copy_software_to_hardware(UART_CHANNEL_0);
            }
        break;
        case UART_CHANNEL_2:
            // Put all of the characters onto the software buffer
            while (*(val) != '\0')
            {
                output &= uart_fifo_push(&uart_2_tx, (uint8_t) *val);
                val++;
            }

            // If the Tx FIFO is empty copy to hardware
            if (UART2->FR & UART_FR_TXFE)
            {
                copy_software_to_hardware(UART_CHANNEL_2);
            }
        break;
        case UART_CHANNEL_3:
            // Put all of the characters onto the software buffer
            while (*(val) != '\0')
            {
                output &= uart_fifo_push(&uart_3_tx, (uint8_t) *val);
                val++;
            }

            // If the Tx FIFO is empty copy to hardware
            if (UART3->FR & UART_FR_TXFE)
            {
                copy_software_to_hardware(UART_CHANNEL_3);
            }
        break;
        case UART_CHANNEL_6:
            // Put all of the characters onto the software buffer
            while (*(val) != '\0')
            {
                output &= uart_fifo_push(&uart_6_tx, (uint8_t) *val);
                val++;
            }

            // If the Tx FIFO is empty copy to hardware
            if (UART6->FR & UART_FR_TXFE)
            {
                copy_software_to_hardware(UART_CHANNEL_6);
            }
        break;
        default:
            output = false;
        break;
    }
    // Delay a bit so this isn't spamable
    utils_delay(200000);

    return output;
}


/**
 * @brief Reads the last byte in the Rx FIFO. This may not necessarily be the last byte received by the
 * hardware. This function spins until something is received. 
 * 
 * @param uart_channel One of UART_CHANNEL_X
 * @param byte A pointer to where the byte will be stored
 * @return true The byte was read
 * @return false The byte could not be read
 */
bool uart_read_byte(uint8_t uart_channel, uint8_t* byte)
{
    bool output = false;
    switch (uart_channel)
    {
        case UART_CHANNEL_0:
            output = uart_fifo_pop(&uart_0_rx, byte);
        break;

        case UART_CHANNEL_2:
            output = uart_fifo_pop(&uart_2_rx, byte);
        break;

        case UART_CHANNEL_3:
            output = uart_fifo_pop(&uart_3_rx, byte);
        break;

        case UART_CHANNEL_6:
            output = uart_fifo_pop(&uart_6_rx, byte);
        break;

        default:
            output = false;
        break;
    }

    return output;
}

/**
 * @brief Moves the data from the hardware FIFO to our software one. 
 * 
 * @param uart_channel One of UART_CHANNEL_X
 */
static void copy_hardware_to_software(uint8_t uart_channel)
{
    uint8_t byte;

    // While there is data in the hardware buffer, copy it into the software buffer
    switch (uart_channel)
    {
        case UART_CHANNEL_0:
            // While the Rx FIFO is not empty and the software FIFO is not full, copy over
            while (((UART0->FR & UART_FR_RXFE) == 0) && (uart_fifo_get_size(&uart_0_rx) < UART_FIFO_SIZE))
            {
                byte = (UART0->DR & UART_DR_DATA_M);
                uart_fifo_push(&uart_0_rx, byte);
            }
        break;

        case UART_CHANNEL_1:

        break;

        case UART_CHANNEL_2:
            // While the Rx FIFO is not empty and the software FIFO is not full, copy over
            while (((UART2->FR & UART_FR_RXFE) == 0) && (uart_fifo_get_size(&uart_2_rx) < UART_FIFO_SIZE))
            {
                byte = (UART2->DR & UART_DR_DATA_M);
                uart_fifo_push(&uart_2_rx, byte);
            }
        break;

        case UART_CHANNEL_3:
            // While the Rx FIFO is not empty and the software FIFO is not full, copy over
            while (((UART3->FR & UART_FR_RXFE) == 0) && (uart_fifo_get_size(&uart_3_rx) < UART_FIFO_SIZE))
            {
                byte = (UART3->DR & UART_DR_DATA_M);
                uart_fifo_push(&uart_3_rx, byte);
            }
        break;

        case UART_CHANNEL_4:

        break;

        case UART_CHANNEL_5:

        break;

        case UART_CHANNEL_6:
            // While the Rx FIFO is not empty and the software FIFO is not full, copy over
            while (((UART6->FR & UART_FR_RXFE) == 0) && (uart_fifo_get_size(&uart_6_rx) < UART_FIFO_SIZE))
            {
                byte = (UART6->DR & UART_DR_DATA_M);
                uart_fifo_push(&uart_6_rx, byte);
            }
        break;

        default:

        break;

    }
}

/**
 * @brief Moves the data from our software FIFO to the hardware
 * 
 * @param uart_channel One of UART_CHANNEL_X
 */
static void copy_software_to_hardware(uint8_t uart_channel)
{
    uint8_t byte;
    // While there is data in the software buffer, copy it into the hardware buffer
    switch (uart_channel)
    {
        case UART_CHANNEL_0:
            // While the Tx FIFO is not full and the software FIFO is not empty, copy over
            while (((UART0->FR & UART_FR_TXFF) == 0) && !uart_fifo_is_empty(&uart_0_tx))
            {
                if (uart_fifo_pop(&uart_0_tx, &byte))
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
            // While the Tx FIFO is not full and the software FIFO is not empty, copy over
            while (((UART2->FR & UART_FR_TXFF) == 0) && !uart_fifo_is_empty(&uart_2_tx))
            {
                if (uart_fifo_pop(&uart_2_tx, &byte))
                {
                    UART2->DR = byte;
                }
                else
                {
                    // Something went wrong reading from the software FIFO
                    break;
                }

            }
        break;

        case UART_CHANNEL_3:
            // While the Tx FIFO is not full and the software FIFO is not empty, copy over
            while (((UART3->FR & UART_FR_TXFF) == 0) && !uart_fifo_is_empty(&uart_3_tx))
            {
                if (uart_fifo_pop(&uart_3_tx, &byte))
                {
                    UART3->DR = byte;
                }
                else
                {
                    // Something went wrong reading from the software FIFO
                    break;
                }

            }
        break;

        case UART_CHANNEL_4:

        break;

        case UART_CHANNEL_5:

        break;

        case UART_CHANNEL_6:
            // While the Tx FIFO is not full and the software FIFO is not empty, copy over
            while (((UART6->FR & UART_FR_TXFF) == 0) && !uart_fifo_is_empty(&uart_6_tx))
            {
                if (uart_fifo_pop(&uart_6_tx, &byte))
                {
                    UART6->DR = byte;
                }
                else
                {
                    // Something went wrong reading from the software FIFO
                    break;
                }

            }
        break;

        default:

        break;

    }
}

/**
 * @brief Clears the specified UART fifo
 * 
 * @param uart_channel One of UART_CHANNEL_X
 */
void uart_reset(uint8_t uart_channel)
{
    switch (uart_channel)
    {
        case UART_CHANNEL_0:
            uart_fifo_clear(&uart_0_rx);
            uart_fifo_clear(&uart_0_tx);
            break;
        case UART_CHANNEL_2:
            uart_fifo_clear(&uart_2_rx);
            uart_fifo_clear(&uart_2_tx);
            break;
        case UART_CHANNEL_3:
            uart_fifo_clear(&uart_3_rx);
            uart_fifo_clear(&uart_3_tx);
            break;
        case UART_CHANNEL_6:
            uart_fifo_clear(&uart_6_rx);
            uart_fifo_clear(&uart_6_tx);
            break;
        default:
            break;
    }
}

/**
 * @brief Called when the Tx FIFO has 2 or fewer elements, when the Rx FIFO has 2 or more elements,
 * or when there is 1 or fewer elements in the Rx FIFO and it times out.
 * 
 */
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

/**
 * @brief Called when the UART2 Tx FIFO has 2 or fewer elements, when the Rx FIFO has 2 or more elements,
 * or when there is 1 or fewer elements in the Rx FIFO and it times out.
 *
 */
__interrupt void UART2_IRQHandler(void)
{
    // The Tx interrupt occurred
    if (UART2->MIS & UART_MIS_TXMIS)
    {
        UART2->ICR |= UART_ICR_TXIC; // Clear the interrupt
        copy_software_to_hardware(UART_CHANNEL_2);
    }
    // The Rx interrupt occurred
    else if (UART2->MIS & UART_MIS_RXMIS)
    {
        UART2->ICR |= UART_ICR_RXIC; // Clear the interrupt
        copy_hardware_to_software(UART_CHANNEL_2);
    }
    // The Rx Timeout interrupt occurred
    else if (UART2->MIS & UART_MIS_RTMIS)
    {
        UART2->ICR |= UART_ICR_RTIC; // Clear the interrupt
        copy_hardware_to_software(UART_CHANNEL_2);
    }
    // Some other interrupt. Probably a fault of some kind
    else {
        // For debugging purposes
    }
}

/**
 * @brief Called when the UART2 Tx FIFO has 2 or fewer elements, when the Rx FIFO has 2 or more elements,
 * or when there is 1 or fewer elements in the Rx FIFO and it times out.
 *
 */
__interrupt void UART3_IRQHandler(void)
{
    // The Tx interrupt occurred
    if (UART3->MIS & UART_MIS_TXMIS)
    {
        UART3->ICR |= UART_ICR_TXIC; // Clear the interrupt
        copy_software_to_hardware(UART_CHANNEL_3);
    }
    // The Rx interrupt occurred
    else if (UART3->MIS & UART_MIS_RXMIS)
    {
        UART3->ICR |= UART_ICR_RXIC; // Clear the interrupt
        copy_hardware_to_software(UART_CHANNEL_3);
    }
    // The Rx Timeout interrupt occurred
    else if (UART3->MIS & UART_MIS_RTMIS)
    {
        UART3->ICR |= UART_ICR_RTIC; // Clear the interrupt
        copy_hardware_to_software(UART_CHANNEL_3);
    }
    // Some other interrupt. Probably a fault of some kind
    else {
        // For debugging purposes
    }
}

/**
 * @brief Called when the UART6 Tx FIFO has 2 or fewer elements, when the Rx FIFO has 2 or more elements,
 * or when there is 1 or fewer elements in the Rx FIFO and it times out.
 *
 */
__interrupt void UART6_IRQHandler(void)
{
    // The Tx interrupt occurred
    if (UART6->MIS & UART_MIS_TXMIS)
    {
        UART6->ICR |= UART_ICR_TXIC; // Clear the interrupt
        copy_software_to_hardware(UART_CHANNEL_6);
    }
    // The Rx interrupt occurred
    else if (UART6->MIS & UART_MIS_RXMIS)
    {
        UART6->ICR |= UART_ICR_RXIC; // Clear the interrupt
        copy_hardware_to_software(UART_CHANNEL_6);
    }
    // The Rx Timeout interrupt occurred
    else if (UART6->MIS & UART_MIS_RTMIS)
    {
        UART6->ICR |= UART_ICR_RTIC; // Clear the interrupt
        copy_hardware_to_software(UART_CHANNEL_6);
    }
    // Some other interrupt. Probably a fault of some kind
    else {
        // For debugging purposes
    }
}


/* End uart.c */
