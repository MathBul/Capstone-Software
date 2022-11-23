/**
 * @file uart.h
 * @author Keenan Alchaar (ka5nt@virginia.edu) and Eli Jelesko (ebj5hec@virginia.edu)
 * @brief Code for the UART communications on the MSP432E401Y
 * @version 0.1
 * @date 2022-10-03
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef UART_H 
#define UART_H

// Note on UART:
//  - Communication is done with receive (Rx) and transmit (Tx) hardware FIFOs
//  - To read and write, date is moved to/from software FIFOs
// 
// Baude rate math:
//  - Baude_Rate_Divisor = Baude_Rate_Generator / (Clock_Div * Baude_Rate)
//  - All channels use PIOSC (16MHz) as Baude_Rate_Generator
//  - All channels have the default Clock_Div of 16
//  - Baude_Rate is set using DIVINT and DIVFRAC
//  - DIVINT  = floor(Baude_Rate_Divisor)
//  - DIVFRAC = ceil[(Baude_Rate_Divisor - DIVINT) * 64 + 0.5]
//      - Ex: Baude_Rate=115200 <=> DIVINT=8  , DIVFRAC=52
//      - Ex: Baude_Rate=9600   <=> DIVINT=104, DIVFRAC=11

#include "msp.h"
#include <stdint.h>
#include "fifo.h"
#include "gpio.h"
#include "utils.h"

// General UART macros
#define NUMBER_OF_ACTIVE_UART_CHANNELS      (5)
#define UART_CHANNEL_0                      (0)
#define UART_CHANNEL_1                      (1)
#define UART_CHANNEL_2                      (2)
#define UART_CHANNEL_3                      (3)
#define UART_CHANNEL_4                      (4)
#define UART_CHANNEL_5                      (5)
#define UART_CHANNEL_6                      (6)
#define UART_CHANNEL_7                      (7)

// UART0 macros
#define UART0_PORT                          (GPIOA)
#define UART0_RX                            (GPIO_PIN_0)
#define UART0_TX                            (GPIO_PIN_1)
#define UART0_INTERRUPT_NUM                 (UART0_IRQn)
#define UART0_HANDLER                       (UART0_IRQHandler)
#define UART0_DIVINT                        (104)
#define UART0_DIVFRAC                       (11)
#define UART0_RX_ID                         (0)
#define UART0_TX_ID                         (1)

// UART1 macros
#define UART1_PORT                          (GPIOB)
#define UART1_RX                            (GPIO_PIN_0)
#define UART1_TX                            (GPIO_PIN_1)
#define UART1_INTERRUPT_NUM                 (UART1_IRQn)
#define UART1_HANDLER                       (UART1_IRQHandler)
#define UART1_DIVINT                        (104)
#define UART1_DIVFRAC                       (11)
#define UART1_RX_ID                         (2)
#define UART1_TX_ID                         (3)

// UART2 macros
#define UART2_PORT                          (GPIOD)
#define UART2_RX                            (GPIO_PIN_4)
#define UART2_TX                            (GPIO_PIN_5)
#define UART2_INTERRUPT_NUM                 (UART2_IRQn)
#define UART2_HANDLER                       (UART2_IRQHandler)
#define UART2_DIVINT                        (104)
#define UART2_DIVFRAC                       (11)
#define UART2_RX_ID                         (4)
#define UART2_TX_ID                         (5)

// UART3 macros
#define UART3_PORT                          (GPIOA)
#define UART3_RX                            (GPIO_PIN_4)
#define UART3_TX                            (GPIO_PIN_5)
#define UART3_INTERRUPT_NUM                 (UART3_IRQn)
#define UART3_HANDLER                       (UART3_IRQHandler)
#define UART3_DIVINT                        (104)
#define UART3_DIVFRAC                       (11)
#define UART3_RX_ID                         (6)
#define UART3_TX_ID                         (7)

// UART6 macros
#define UART6_PORT                          (GPIOP)
#define UART6_RX                            (GPIO_PIN_0)
#define UART6_TX                            (GPIO_PIN_1)
#define UART6_INTERRUPT_NUM                 (UART6_IRQn)
#define UART6_HANDLER                       (UART6_IRQHandler)
#define UART6_DIVINT                        (104)
#define UART6_DIVFRAC                       (11)
#define UART6_RX_ID                         (8)
#define UART6_TX_ID                         (9)

// Public functions
void uart_init(uint8_t uart_channel);
bool uart_out_byte(uint8_t uart_channel, uint8_t byte);
bool uart_read_byte(uint8_t uart_channel, uint8_t* byte);
bool uart_read_string(uint8_t uart_channel, char *data, uint8_t num_chars);
bool uart_out_string(uint8_t uart_channel, char* string, uint8_t size);
bool uart_out_int16_t(uint8_t uart_channel, int16_t value);
bool uart_out_uint32_t(uint8_t uart_channel, uint32_t value);
void uart_reset(uint8_t uart_channel);

#endif /* UART_H */
