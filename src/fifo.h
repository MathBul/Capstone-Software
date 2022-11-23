/**
 * @file fifo.h
 * @author Eli Jelesko (ebj5hec@virginia.edu)
 * @brief Implements a First In First Out (FIFO) data structures
 * @version 0.1
 * @date 2022-10-03
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef FIFO_H_
#define FIFO_H_

#include <stdint.h>
#include <stdbool.h>

#define FIFO8_TYPE              uint8_t
#define FIFO8_SIZE              (64)        // Must be at  least 1 and less than 65535

// Fifo data structure
typedef struct fifo8_t {
    FIFO8_TYPE fifo[FIFO8_SIZE];
    uint16_t head;
    uint16_t tail;
} fifo8_t;

// Public functions for 8-bit FIFO
void fifo8_init(fifo8_t* fifo);
bool fifo8_push(fifo8_t* fifo, FIFO8_TYPE value);
bool fifo8_pop(fifo8_t* fifo, FIFO8_TYPE* p_value);
uint16_t fifo8_get_size(fifo8_t* fifo);
bool fifo8_is_empty(fifo8_t* fifo);
bool fifo8_clear(fifo8_t* fifo);

#endif /* FIFO_H_ */
