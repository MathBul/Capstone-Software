/**
 * @file fifo.h
 * @author Eli Jelesko (ebj5hec@virginia.edu)
 * @brief Implements a First In First Out (FIFO) data structure
 * @version 0.1
 * @date 2022-10-03
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef FIFO_H_
#define FIFO_H_

#include <stdint.h>
#include <stdbool.h>
#include "testing.h"

#ifdef UART_DEBUG
#define FIFO_TYPE               uint8_t
#define FIFO_SIZE               (16) // Must be at least 1 and less than 65535
#endif

#ifdef STEPPER_DEBUG
#define FIFO_TYPE               uint32_t
#define FIFO_SIZE               (128) // Must be at least 1 and less than 65535
#endif

#ifdef CHESS_ROBOT_MAIN
#define FIFO_TYPE               uint8_t
#define FIFO_SIZE               (16) // Must be at least 1 and less than 65535
#endif

// Fifo data structure struct
typedef struct fifo_t {
    FIFO_TYPE fifo[FIFO_SIZE];
    uint16_t head;
    uint16_t tail;
} fifo_t;

// Fifo function definitions
void fifo_init(fifo_t* fifo);
bool fifo_push(fifo_t* fifo, FIFO_TYPE value);
bool fifo_pop(fifo_t* fifo, FIFO_TYPE* p_value);
uint16_t fifo_get_size(fifo_t* fifo);
bool fifo_is_empty(fifo_t* fifo);

#endif /* FIFO_H_ */
