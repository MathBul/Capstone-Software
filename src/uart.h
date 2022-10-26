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
#include "uart_fifo.h"
#include <stdint.h>

// These are all the UART channels on the msp432e401y
#define UART_CHANNEL_0                      (0)
#define UART_CHANNEL_1                      (1)
#define UART_CHANNEL_2                      (2)
#define UART_CHANNEL_3                      (3)
#define UART_CHANNEL_4                      (4)
#define UART_CHANNEL_5                      (5)
#define UART_CHANNEL_6                      (6)
#define UART_CHANNEL_7                      (7)

// UART function declarations
void uart_init(uint8_t uart_channel);
bool uart_out_byte(uint8_t uart_channel, uint8_t byte);
bool uart_read_byte(uint8_t uart_channel, uint8_t* byte);
void uart_reset(uint8_t uart_channel);

#endif /* UART_H */
