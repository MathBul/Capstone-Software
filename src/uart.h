/**
 * @file uart.h
 * @author Keenan Alchaar (ka5nt@virginia.edu) and Eli Jelesko (ebj5hec@virginia.edu)
 * @brief Code for the UART communications on the MSP432e401Y
 * @version 0.1
 * @date 2022-10-03
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef UART_H 
#define UART_H

#include "msp.h"
#include <stdint.h>
#include "fifo.h"

// These are all the UART channels on the msp432e401y
#define UART_CHANNEL_0          (0)
#define UART_CHANNEL_1          (1)
#define UART_CHANNEL_2          (2)
#define UART_CHANNEL_3          (3)
#define UART_CHANNEL_4          (4)
#define UART_CHANNEL_5          (5)
#define UART_CHANNEL_6          (6)
#define UART_CHANNEL_6          (7)

/**
 * @brief Initializes the specified UART channel
 * 
 * @param uart_channel One of UART_CHANNEL_X
 */
void uart_init(uint8_t uart_channel);

/**
 * @brief Sends a single byte (uint8_t) to the specified UART channel
 * 
 * @param uart_channel One of UART_CHANNEL_X
 * @param byte The uint8_t to be sent
 * @return true The byte was sent
 * @return false The byte could not be sent
 */
bool uart_out_byte(uint8_t uart_channel, uint8_t byte);

/**
 * @brief Reads the last byte in the Rx FIFO. This may not nessisarily be the last byte received by the
 * hardware. This function spins until something is received. 
 * 
 * @param uart_channel One of UART_CHANNEL_X
 * @param byte A pointer to where the byte will be stored
 * @return true The byte was read
 * @return false The byte could not be read
 */
bool uart_read_byte(uint8_t uart_channel, uint8_t* byte);


#endif /* UART_H */
