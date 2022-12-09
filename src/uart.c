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

// TODO: Make sure the elements are only 1 byte long!

// Declare the uart fifos
fifo8_t fifo8s[NUMBER_OF_ACTIVE_UART_CHANNELS*2];
static fifo8_t* uart_0_rx = &fifo8s[UART0_RX_ID];
static fifo8_t* uart_0_tx = &fifo8s[UART0_TX_ID];
static fifo8_t* uart_1_rx = &fifo8s[UART1_RX_ID];
static fifo8_t* uart_1_tx = &fifo8s[UART1_TX_ID];
static fifo8_t* uart_2_rx = &fifo8s[UART2_RX_ID];
static fifo8_t* uart_2_tx = &fifo8s[UART2_TX_ID];
static fifo8_t* uart_3_rx = &fifo8s[UART3_RX_ID];
static fifo8_t* uart_3_tx = &fifo8s[UART3_TX_ID];
static fifo8_t* uart_6_rx = &fifo8s[UART6_RX_ID];
static fifo8_t* uart_6_tx = &fifo8s[UART6_TX_ID];

// Private functions
static void uart0_init(void);
static void uart1_init(void);
static void uart2_init(void);
static void uart3_init(void);
static void uart6_init(void);
static void uart_copy_hardware_to_software(uint8_t uart_channel);
static void uart_copy_software_to_hardware(uint8_t uart_channel);
static void uart_interrupt_activity(uint8_t uart_channel);

/**
 * @brief Configure UART on the specified channel
 * 
 * @param uart_channel One of UART_CHANNEL_X for X={0,1,2,3,6}
 */
void uart_init(uint8_t uart_channel)
{
    switch (uart_channel)
    {
        case UART_CHANNEL_0:
            uart0_init();
        break;

        case UART_CHANNEL_1:
            uart1_init();
        break;

        case UART_CHANNEL_2:
            uart2_init();
        break;

        case UART_CHANNEL_3:
            uart3_init();
        break;

        case UART_CHANNEL_6:
            uart6_init();
        break;

        default:
            // Invalid channel provided, do nothing
        break;
    }
}

/**
 * @brief Configure UART on channel 0
 */
void uart0_init(void)
{
    // Initialize the software FIFOS
    fifo8_init(uart_0_rx);
    fifo8_init(uart_0_tx);

    // Enable the UART clock gate control
    utils_uart_clock_enable(UART_CHANNEL_0);

    // Configure the transmit and receive GPIO
    utils_gpio_clock_enable(UART0_PORT);
    gpio_set_as_input(UART0_PORT, UART0_RX);
    gpio_set_as_output(UART0_PORT, UART0_TX);
    gpio_select_alternate_function(UART0_PORT, UART0_RX, 1);             // Select 1st alternate function for this GPIO pin
    gpio_select_alternate_function(UART0_PORT, UART0_TX, 1);             // Select 1st alternate function for this GPIO pin

    // Disable the UART module
    UART0->CTL &= ~UART_CTL_UARTEN;

    // Configure baude rate to be 9600 bits/sec
    UART0->IBRD |= (UART0_DIVINT  << UART_IBRD_DIVINT_S);        // Sets DIVINT to 104
    UART0->FBRD |= (UART0_DIVFRAC << UART_FBRD_DIVFRAC_S);       // Sets DIVFRAC to 11
    UART0->LCRH |= (UART_LCRH_FEN);                              // Enables the hardware FIFOs
    UART0->LCRH |= (UART_LCRH_WLEN_8);                           // Sets word length to 8 bits
    UART0->CC   |= (UART_CC_CS_PIOSC);                           // Sets baud rate generator to PIOSC (16 MHz)

    // Configure interrupts
    UART0->IFLS |= (UART_IFLS_RX1_8 | UART_IFLS_TX1_8);          // Sets Tx/Rx interrupt triggers to when FIFOs are 1/8 full
    UART0->IM   |= (UART_IM_RXIM | UART_IM_TXIM | UART_IM_RTIM); // Enable the Tx and Rx FIFOs, and Rx timeout interrupt
    utils_set_nvic(UART0_INTERRUPT_NUM, 0);                      // Configure the NVIC

    // Enable the UART module
    UART0->CTL  |= UART_CTL_UARTEN;
}

/**
 * @brief Configure UART on channel 1
 */
void uart1_init(void)
{
    // Initialize the software FIFOS
    fifo8_init(uart_1_rx);
    fifo8_init(uart_1_tx);

    // Enable the UART clock gate control
    utils_uart_clock_enable(UART_CHANNEL_1);

    // Configure the transmit and receive GPIO
    utils_gpio_clock_enable(UART1_PORT);
    gpio_set_as_input(UART1_PORT, UART1_RX);
    gpio_set_as_output(UART1_PORT, UART1_TX);
    gpio_select_alternate_function(UART1_PORT, UART1_RX, 1);             // Select 1st alternate function for this GPIO pin
    gpio_select_alternate_function(UART1_PORT, UART1_TX, 1);             // Select 1st alternate function for this GPIO pin

    // Disable the UART module
    UART1->CTL &= ~UART_CTL_UARTEN;

    // Configure baude rate to be 9600 bits/sec
    UART1->IBRD |= (UART1_DIVINT  << UART_IBRD_DIVINT_S);        // Sets DIVINT to 104
    UART1->FBRD |= (UART1_DIVFRAC << UART_FBRD_DIVFRAC_S);       // Sets DIVFRAC to 11
    UART1->LCRH |= (UART_LCRH_FEN);                              // Enables the hardware FIFOs
    UART1->LCRH |= (UART_LCRH_WLEN_8);                           // Sets word length to 8 bits
    UART1->CC   |= (UART_CC_CS_PIOSC);                           // Sets baud rate generator to PIOSC (16 MHz)

    // Configure interrupts
    UART1->IFLS |= (UART_IFLS_RX1_8 | UART_IFLS_TX1_8);          // Sets Tx/Rx interrupt triggers to when FIFOs are 1/8 full
    UART1->IM   |= (UART_IM_RXIM | UART_IM_TXIM | UART_IM_RTIM); // Enable the Tx and Rx FIFOs, and Rx timeout interrupt
    utils_set_nvic(UART1_INTERRUPT_NUM, 0);                      // Configure the NVIC

    // Enable the UART module
    UART1->CTL  |= UART_CTL_UARTEN;
}

/**
 * @brief Configure UART on channel 2
 */
void uart2_init(void)
{
    // Initialize the software FIFOS
    fifo8_init(uart_2_rx);
    fifo8_init(uart_2_tx);

    // Enable the UART clock gate control
    utils_uart_clock_enable(UART_CHANNEL_2);

    // Configure the transmit and receive GPIO
    utils_gpio_clock_enable(UART2_PORT);
    gpio_set_as_input(UART2_PORT, UART2_RX);
    gpio_set_as_output(UART2_PORT, UART2_TX);
    gpio_select_alternate_function(UART2_PORT, UART2_RX, 1);             // Select 1st alternate function for this GPIO pin
    gpio_select_alternate_function(UART2_PORT, UART2_TX, 1);             // Select 1st alternate function for this GPIO pin

    // Disable the UART module
    UART2->CTL &= ~UART_CTL_UARTEN;

    // Configure baude rate to be 9600 bits/sec
    UART2->IBRD |= (UART2_DIVINT  << UART_IBRD_DIVINT_S);        // Sets DIVINT to 104
    UART2->FBRD |= (UART2_DIVFRAC << UART_FBRD_DIVFRAC_S);       // Sets DIVFRAC to 11
    UART2->LCRH |= (UART_LCRH_FEN);                              // Enables the hardware FIFOs
    UART2->LCRH |= (UART_LCRH_WLEN_8);                           // Sets word length to 8 bits
    UART2->CC   |= (UART_CC_CS_PIOSC);                           // Sets baud rate generator to PIOSC (16 MHz)

    // Configure interrupts
    UART2->IFLS |= (UART_IFLS_RX1_8 | UART_IFLS_TX1_8);          // Sets Tx/Rx interrupt triggers to when FIFOs are 1/8 full
    UART2->IM   |= (UART_IM_RXIM | UART_IM_TXIM | UART_IM_RTIM); // Enable the Tx and Rx FIFOs, and Rx timeout interrupt
    utils_set_nvic(UART2_INTERRUPT_NUM, 0);                      // Configure the NVIC

    // Enable the UART module
    UART2->CTL  |= UART_CTL_UARTEN;
}

/**
 * @brief Configure UART on channel 3
 */
void uart3_init(void)
{
    // Initialize the software FIFOS
    fifo8_init(uart_3_rx);
    fifo8_init(uart_3_tx);

    // Enable the UART clock gate control
    utils_uart_clock_enable(UART_CHANNEL_3);

    // Configure the transmit and receive GPIO
    utils_gpio_clock_enable(UART3_PORT);
    gpio_set_as_input(UART3_PORT, UART3_RX);
    gpio_set_as_output(UART3_PORT, UART3_TX);
    gpio_select_alternate_function(UART3_PORT, UART3_RX, 1);             // Select 1st alternate function for this GPIO pin
    gpio_select_alternate_function(UART3_PORT, UART3_TX, 1);             // Select 1st alternate function for this GPIO pin

    // Disable the UART module
    UART3->CTL &= ~UART_CTL_UARTEN;

    // Configure baud rate to be 9600 bits/sec
    UART3->IBRD |= (UART3_DIVINT  << UART_IBRD_DIVINT_S);        // Sets DIVINT to 104
    UART3->FBRD |= (UART3_DIVFRAC << UART_FBRD_DIVFRAC_S);       // Sets DIVFRAC to 11
    UART3->LCRH |= (UART_LCRH_FEN);                              // Enables the hardware FIFOs
    UART3->LCRH |= (UART_LCRH_WLEN_8);                           // Sets word length to 8 bits
    UART3->CC   |= (UART_CC_CS_PIOSC);                           // Sets baud rate generator to PIOSC (16 MHz)

    // Configure interrupts
    UART3->IFLS |= (UART_IFLS_RX1_8 | UART_IFLS_TX1_8);          // Sets Tx/Rx interrupt triggers to when FIFOs are 1/8 full
    UART3->IM   |= (UART_IM_RXIM | UART_IM_TXIM | UART_IM_RTIM); // Enable the Tx and Rx FIFOs, and Rx timeout interrupt
    utils_set_nvic(UART3_INTERRUPT_NUM, 0);                      // Configure the NVIC

    // Enable the UART module
    UART3->CTL  |= UART_CTL_UARTEN;
}

/**
 * @brief Configure UART on channel 6
 */
void uart6_init(void)
{
    // Initialize the software FIFOS
    fifo8_init(uart_6_rx);
    fifo8_init(uart_6_tx);

    // Enable the UART clock gate control
    utils_uart_clock_enable(UART_CHANNEL_6);

    // Configure the transmit and receive GPIO
    utils_gpio_clock_enable(UART6_PORT);
    gpio_set_as_input(UART6_PORT, UART6_RX);
    gpio_set_as_output(UART6_PORT, UART6_TX);
    gpio_select_alternate_function(UART6_PORT, UART6_RX, 1);             // Select 1st alternate function for this GPIO pin
    gpio_select_alternate_function(UART6_PORT, UART6_TX, 1);             // Select 1st alternate function for this GPIO pin

    // Disable the UART module
    UART6->CTL &= ~UART_CTL_UARTEN;

    // Configure baude rate to be 9600 bits/sec
    UART6->IBRD |= (UART6_DIVINT  << UART_IBRD_DIVINT_S);        // Sets DIVINT to 104
    UART6->FBRD |= (UART6_DIVFRAC << UART_FBRD_DIVFRAC_S);       // Sets DIVFRAC to 11
    UART6->LCRH |= (UART_LCRH_FEN);                              // Enables the hardware FIFOs
    UART6->LCRH |= (UART_LCRH_WLEN_8);                           // Sets word length to 8 bits
    UART6->CC   |= (UART_CC_CS_PIOSC);                           // Sets baud rate generator to PIOSC (16 MHz)

    // Configure interrupts
    UART6->IFLS |= (UART_IFLS_RX1_8 | UART_IFLS_TX1_8);          // Sets Tx/Rx interrupt triggers to when FIFOs are 1/8 full
    UART6->IM   |= (UART_IM_RXIM | UART_IM_TXIM | UART_IM_RTIM); // Enable the Tx and Rx FIFOs, and Rx timeout interrupt
    utils_set_nvic(UART6_INTERRUPT_NUM, 0);                      // Configure the NVIC

    // Enable the UART module
    UART6->CTL  |= UART_CTL_UARTEN;
}

/**
 * @brief Sends a single byte (uint8_t) to the specified UART channel
 * 
 * @param uart_channel One of UART_CHANNEL_X for X={0,1,2,3,6}
 * @param byte The data to be sent
 * @return Whether transmission was successful
 */
bool uart_out_byte(uint8_t uart_channel, uint8_t data)
{
    bool status = true;
    fifo8_t* p_uart_tx_fifo;
    UART0_Type* p_uart_module;

    // Get pointers for the appropriate UART module
    switch (uart_channel)
    {
        case UART_CHANNEL_0:
            p_uart_tx_fifo = uart_0_tx;
            p_uart_module = UART0;
        break;

        case UART_CHANNEL_1:
            p_uart_tx_fifo = uart_1_tx;
            p_uart_module = UART1;
        break;

        case UART_CHANNEL_2:
            p_uart_tx_fifo = uart_2_tx;
            p_uart_module = UART2;
        break;

        case UART_CHANNEL_3:
            p_uart_tx_fifo = uart_3_tx;
            p_uart_module = UART3;
        break;

        case UART_CHANNEL_6:
            p_uart_tx_fifo = uart_6_tx;
            p_uart_module = UART6;
        break;

        default:
            // Invalid channel provided, do nothing
            status = false;
        break;
    }

    // If an invalid channel was provided, exit
    if (!status)
    {
        return false;
    }

    // Load the value to the software FIFO
    status = fifo8_push(p_uart_tx_fifo, data);

    // If the Tx FIFO is empty copy to hardware
    if (p_uart_module->FR & UART_FR_TXFE)
    {
        uart_copy_software_to_hardware(uart_channel);
    }

    return status;
}

/**
 * @brief Sends a string to the specified UART channel
 *
 * @param uart_channel One of UART_CHANNEL_X for X={0,1,2,3,6}
 * @param data Character buffer to be sent
 * @param size Number of characters to transmit (unless a null-terminator is reacher)
 * @return Whether transmission was successful
 */
bool uart_out_string(uint8_t uart_channel, char* data, uint8_t size)
{
    bool status = true;

    // Write one byte at a time
    int i = 0;
    for (i = 0; (i < size) && (status) && (data[i] != '\0'); i++)
    {
        status &= uart_out_byte(uart_channel, (uint8_t) data[i]);
    }


    return status;
}

/**
 * @brief Sends a signed 16 bit integer to the specified UART channel
 *
 * @param uart_channel One of UART_CHANNEL_X for X={0,1,2,3,6}
 * @param data The data to be sent
 * @return Whether transmission was successful
 */
bool uart_out_int16_t(uint8_t uart_channel, int16_t data)
{
    bool status = true;

    // Break the data into chunks and send each separately
    uint8_t data_chunk;
    uint8_t data_shift;
    int i = 1;
    for (i = 1; (i >= 0) && (status); i--)
    {
        data_shift = (8*i);
        data_chunk = (data >> data_shift);
        status &= uart_out_byte(uart_channel, data_chunk);
    }

    return status;
}

/**
 * @brief Sends an unsigned 32 bit integer to the specified UART channel
 *
 * @param uart_channel One of UART_CHANNEL_X for X={0,1,2,3,6}
 * @param data The data to be sent
 * @return Whether transmission was successful
 */
bool uart_out_uint32_t(uint8_t uart_channel, uint32_t data)
{
    bool status = true;

    // Break the data into chunks and send each separately
    uint8_t data_chunk;
    uint8_t data_shift;
    int i = 3;
    for (i = 3; (i >= 0) && (status); i--)
    {
        data_shift = (8*i);
        data_chunk = (data >> data_shift);
        status &= uart_out_byte(uart_channel, data_chunk);
    }

    return status;
}

/**
 * @brief Reads the oldest byte in the Rx FIFO. May not be the oldest byte received by the hardware
 *  Spins until something is received in the Rx FIFO
 * 
 * @param uart_channel One of UART_CHANNEL_X for X={0,1,2,3,6}
 * @param byte A pointer to where the byte will be stored
 * @return Whether reading was successful
 */
bool uart_read_byte(uint8_t uart_channel, uint8_t* byte)
{
    bool status = false;

    // Read the appropriate channel
    while (!status)
    {
        // Short circuit if a fault occurs
        if (sys_fault)
        {
            return false;
        }
        
        // Try to read a byte
        switch (uart_channel)
        {
            case UART_CHANNEL_0:
                status = fifo8_pop(uart_0_rx, byte);
            break;

            case UART_CHANNEL_1:
                status = fifo8_pop(uart_1_rx, byte);
            break;

            case UART_CHANNEL_2:
                status = fifo8_pop(uart_2_rx, byte);
            break;

            case UART_CHANNEL_3:
                status = fifo8_pop(uart_3_rx, byte);
            break;

            case UART_CHANNEL_6:
                status = fifo8_pop(uart_6_rx, byte);
            break;

            default:
                // Invalid channel provided, do nothing
                status = false;
            break;
        }
    }


    return status;
}

/**
 * @brief Uses UART to read data from any UART channel to the MSP
 *
 * @param uart_channel One of UART_CHANNEL_X for X={0,1,2,3,6}
 * @param data Character buffer to read into
 * @param size Number of characters to read (unless a null-terminator is reacher)
 * @return Whether transmission was successful
 */
bool uart_read_string(uint8_t uart_channel, char *data, uint8_t size)
{
    bool status = true;

    // Read one byte at a time
    uint8_t i = 0;
    for (i = 0; (i < size) && (status); i++)
    {
        status &= uart_read_byte(uart_channel, (uint8_t*) &data[i]);
    }

    return status;
}

/**
 * @brief Moves data from the hardware FIFO to our software one
 *
 * @param uart_channel One of UART_CHANNEL_X for X={0,1,2,3,6}
 */
static void uart_copy_hardware_to_software(uint8_t uart_channel)
{
    bool status = true;
    uint8_t byte;
    fifo8_t* p_uart_rx_fifo;
    UART0_Type* p_uart_module;

    // Get pointers for the appropriate UART module
    switch (uart_channel)
    {
        case UART_CHANNEL_0:
            p_uart_rx_fifo = uart_0_rx;
            p_uart_module = UART0;
        break;

        case UART_CHANNEL_1:
            p_uart_rx_fifo = uart_1_rx;
            p_uart_module = UART1;
        break;

        case UART_CHANNEL_2:
            p_uart_rx_fifo = uart_2_rx;
            p_uart_module = UART2;
        break;

        case UART_CHANNEL_3:
            p_uart_rx_fifo = uart_3_rx;
            p_uart_module = UART3;
        break;

        case UART_CHANNEL_6:
            p_uart_rx_fifo = uart_6_rx;
            p_uart_module = UART6;
        break;

        default:
            // Invalid channel provided, do nothing
            status = false;
        break;
    }

    // If an invalid channel was provided, exit
    if (!status)
    {
        return;
    }

    // While the Rx hardware FIFO is not empty and the software FIFO is not full, copy data over
    while (((p_uart_module->FR & UART_FR_RXFE) == 0) && (fifo8_get_size(p_uart_rx_fifo) < FIFO8_SIZE))
    {
        byte = (p_uart_module->DR & UART_DR_DATA_M);
        fifo8_push(p_uart_rx_fifo, byte);
    }
}

/**
 * @brief Moves data from our software FIFO to the hardware one
 * 
 * @param uart_channel One of UART_CHANNEL_X for X={0,1,2,3,6}
 */
static void uart_copy_software_to_hardware(uint8_t uart_channel)
{
    bool status = true;
    uint8_t byte;
    fifo8_t* p_uart_tx_fifo;
    UART0_Type* p_uart_module;

    // Get pointers for the appropriate UART module
    switch (uart_channel)
    {
        case UART_CHANNEL_0:
            p_uart_tx_fifo = uart_0_tx;
            p_uart_module = UART0;
        break;

        case UART_CHANNEL_1:
            p_uart_tx_fifo = uart_1_tx;
            p_uart_module = UART1;
        break;

        case UART_CHANNEL_2:
            p_uart_tx_fifo = uart_2_tx;
            p_uart_module = UART2;
        break;

        case UART_CHANNEL_3:
            p_uart_tx_fifo = uart_3_tx;
            p_uart_module = UART3;
        break;

        case UART_CHANNEL_6:
            p_uart_tx_fifo = uart_6_tx;
            p_uart_module = UART6;
        break;

        default:
            // Invalid channel provided, do nothing
            status = false;
        break;
    }

    // If an invalid channel was provided, exit
    if (!status)
    {
        return;
    }

    // While the Rx hardware FIFO is not empty and the software FIFO is not full, copy data over
    while (((p_uart_module->FR & UART_FR_TXFF) == 0) && !fifo8_is_empty(p_uart_tx_fifo))
    {
        if (fifo8_pop(p_uart_tx_fifo, &byte))
        {
            p_uart_module->DR = byte;
        }
        else
        {
            // For debugging purposes, something went wrong reading from the software FIFO
        }
    }
}

/**
 * @brief Clears the specified UART fifos
 * 
 * @param uart_channel One of UART_CHANNEL_X for X={0,1,2,3,6}
 */
void uart_reset(uint8_t uart_channel)
{
    switch (uart_channel)
    {
        case UART_CHANNEL_0:
            fifo8_clear(uart_0_rx);
            fifo8_clear(uart_0_tx);
        break;

        case UART_CHANNEL_1:
            fifo8_clear(uart_1_rx);
            fifo8_clear(uart_1_tx);
        break;

        case UART_CHANNEL_2:
            fifo8_clear(uart_2_rx);
            fifo8_clear(uart_2_tx);
        break;

        case UART_CHANNEL_3:
            fifo8_clear(uart_3_rx);
            fifo8_clear(uart_3_tx);
        break;

        case UART_CHANNEL_6:
            fifo8_clear(uart_6_rx);
            fifo8_clear(uart_6_tx);
        break;

        default:
            // Invalid channel provided, do nothing
        break;
    }
}

/* Interrupts */

/**
 * @brief Helper function to perform the interrupt activity for the specified UART channel and clear the intterrupt
 *
 * @param uart_channel One of UART_CHANNEL_X for X={0,...,7}
 */
static void uart_interrupt_activity(uint8_t uart_channel)
{
    bool status = true;
    UART0_Type* p_uart_module;

    // Get a pointer to the appropriate UART module
    switch (uart_channel)
    {
        case UART_CHANNEL_0:
            p_uart_module = UART0;
        break;

        case UART_CHANNEL_1:
            p_uart_module = UART1;
        break;

        case UART_CHANNEL_2:
            p_uart_module = UART2;
        break;

        case UART_CHANNEL_3:
            p_uart_module = UART3;
        break;

        case UART_CHANNEL_6:
            p_uart_module = UART6;
        break;

        default:
            // Invalid channel provided, do nothing
            status = false;
        break;
    }

    // If an invalid channel was provided, exit
    if (!status)
    {
        return;
    }

    // Check which interrupt occured
    if (p_uart_module->MIS & UART_MIS_TXMIS)        // Tx interrupt
    {
        p_uart_module->ICR |= UART_ICR_TXIC;        // Clear the interrupt
        uart_copy_software_to_hardware(uart_channel);
    }
    else if (p_uart_module->MIS & UART_MIS_RXMIS)   // Rx interrupt
    {
        p_uart_module->ICR |= UART_ICR_RXIC;        // Clear the interrupt
        uart_copy_hardware_to_software(uart_channel);
    }
    else if (p_uart_module->MIS & UART_MIS_RTMIS)   // Rx timeout interrupt
    {
        p_uart_module->ICR |= UART_ICR_RTIC;        // Clear the interrupt
        uart_copy_hardware_to_software(uart_channel);
    }
    else {                                          // Some other interrupt, possibly a fault
        // For debugging purposes
    }

}

/**
 * @brief Called when UART0 Tx or Rx FIFOs have <= 2 elements, or when Rx FIFO times out with <= 1 element
 */
__interrupt void UART0_HANDLER(void)
{
    // Perform the UART interrupt activity (also clears interrupt)
    uart_interrupt_activity(UART_CHANNEL_0);
}

/**
 * @brief Called when UART1 Tx or Rx FIFOs have <= 2 elements, or when Rx FIFO times out with <= 1 element
 */
__interrupt void UART1_HANDLER(void)
{
    // Perform the UART interrupt activity (also clears interrupt)
    uart_interrupt_activity(UART_CHANNEL_1);
}

/**
 * @brief Called when UART2 Tx or Rx FIFOs have <= 2 elements, or when Rx FIFO times out with <= 1 element
 */
__interrupt void UART2_HANDLER(void)
{
    // Perform the UART interrupt activity (also clears interrupt)
    uart_interrupt_activity(UART_CHANNEL_2);
}

/**
 * @brief Called when UART3 Tx or Rx FIFOs have <= 2 elements, or when Rx FIFO times out with <= 1 element
 */
__interrupt void UART3_HANDLER(void)
{
    // Perform the UART interrupt activity (also clears interrupt)
    uart_interrupt_activity(UART_CHANNEL_3);
}

/**
 * @brief Called when UART6 Tx or Rx FIFOs have <= 2 elements, or when Rx FIFO times out with <= 1 element
 */
__interrupt void UART6_HANDLER(void)
{
    // Perform the UART interrupt activity (also clears interrupt)
    uart_interrupt_activity(UART_CHANNEL_6);
}

/* End uart.c */
