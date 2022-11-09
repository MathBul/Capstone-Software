/**
 * @file uart_fifo.h
 * @author Eli Jelesko (ebj5hec@virginia.edu)
 * @brief Implements a First In First Out (FIFO) data structure for UART (8-bit)
 * @version 0.1
 * @date 2022-10-03
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef UART_FIFO_H_
#define UART_FIFO_H_

#include <stdint.h>
#include <stdbool.h>

#define UART_FIFO_TYPE          uint8_t
#define UART_FIFO_SIZE          (64) // Must be at least 1 and less than 65535

// Fifo data structure struct
typedef struct uart_fifo_t {
    UART_FIFO_TYPE fifo[UART_FIFO_SIZE];
    uint16_t head;
    uint16_t tail;
} uart_fifo_t;

// Fifo function definitions
void uart_fifo_init(uart_fifo_t* fifo);
bool uart_fifo_push(uart_fifo_t* fifo, UART_FIFO_TYPE value);
bool uart_fifo_pop(uart_fifo_t* fifo, UART_FIFO_TYPE* p_value);
uint16_t uart_fifo_get_size(uart_fifo_t* fifo);
bool uart_fifo_is_empty(uart_fifo_t* fifo);
bool uart_fifo_clear(uart_fifo_t* fifo);

#endif /* UART_FIFO_H_ */
