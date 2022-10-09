/**
 * @file stepper_fifo.h
 * @author Eli Jelesko (ebj5hec@virginia.edu)
 * @brief Implements a First In First Out (FIFO) data structure for the stepper motors (32-bit)
 * @version 0.1
 * @date 2022-10-03
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef STEPPER_FIFO_H_
#define STEPPER_FIFO_H_

#include <stdint.h>
#include <stdbool.h>

#define STEPPER_FIFO_TYPE          int32_t
#define STEPPER_FIFO_SIZE          (128) // Must be at least 1 and less than 65535

// Fifo data structure struct
typedef struct stepper_fifo_t {
    STEPPER_FIFO_TYPE fifo[STEPPER_FIFO_SIZE];
    uint16_t head;
    uint16_t tail;
} stepper_fifo_t;

// Fifo function definitions
void stepper_fifo_init(stepper_fifo_t* fifo);
bool stepper_fifo_push(stepper_fifo_t* fifo, STEPPER_FIFO_TYPE value);
bool stepper_fifo_pop(stepper_fifo_t* fifo, STEPPER_FIFO_TYPE* p_value);
uint16_t stepper_fifo_get_size(stepper_fifo_t* fifo);
bool stepper_fifo_is_empty(stepper_fifo_t* fifo);

#endif /* STEPPER_FIFO_H_ */
