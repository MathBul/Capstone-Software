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

#include <stdint.h>
#include <stdbool.h>

#define FIFO_TYPE               uint8_t
#define FIFO_SIZE               (16) // Must be at least 1 and less than 65535

typedef struct fifo_t {
    FIFO_TYPE fifo[FIFO_SIZE];
    uint16_t head;
    uint16_t tail;
} fifo_t;

/**
 * @brief Initializes the FIFO. Starts empty.
 * 
 */
void fifo_init(fifo_t* fifo);
/**
 * @brief Pushes an element into the FIFO. If the FIFO is full, this will do nothing. 
 * @param value The value to be put on the queue.
 * @return True if the value was added, false otherwise 
 */
bool fifo_push(fifo_t* fifo, FIFO_TYPE value);
/**
 * @brief Removes the value at the front of the FIFO. If there is nothing in the FIFO, the 
 * storage pointer is evaulated to NULL
 * @param p_value A pointer to where the value will be stored
 * @return True if the value was popped, false otherwise
 */
bool fifo_pop(fifo_t* fifo, FIFO_TYPE* p_value);
/**
 * @brief Gives the number of elements currently in the FIFO
 * 
 */
uint16_t fifo_get_size(fifo_t* fifo);

bool fifo_is_empty(fifo_t* fifo);
