/**
 * @file fifo.c
 * @author Eli Jelesko (ebj5hec@virginia.edu)
 * @brief Implements a First In First Out (FIFO) data structure
 * @version 0.1
 * @date 2022-10-03
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "fifo.h"

/*
Dev notes:
Implemented as a ring buffer
Tail is the end of the buffer, head is the start
So when head = tail - 1, it's full
when they're equal it's empty

*/

/**
 * @brief Initializes the FIFO. Starts empty.
 * 
 */
void fifo_init(fifo_t* fifo)
{
    fifo->head = 0;
    fifo->tail = 0;
}
/**
 * @brief Pushes an element into the FIFO. If the FIFO is full, this will do nothing. 
 * @param value The value to be put on the queue. 
 */
bool fifo_push(fifo_t* fifo, FIFO_TYPE value)
{
    // If the FIFO is full, return
    if (fifo_get_size(fifo) == FIFO_SIZE)
    {
        return false;
    }
    else
    {
        // Advance the head
        fifo->head++;
        // Wrap
        if (fifo->head >= FIFO_SIZE)
        {
            fifo->head = 0;
        }

        // Put the value in
        fifo->fifo[fifo->head] = value;

        // Success
        return true;
    }
}
/**
 * @brief Removes the value at the front of the FIFO. If there is nothing in the FIFO, the 
 * storage pointer is evaulated to NULL
 * @param p_value A pointer to where the value will be stored
 * 
 */
bool fifo_pop(fifo_t* fifo, FIFO_TYPE* p_value)
{
    // If it's empty do nothing
    if (fifo_is_empty(fifo))
    {
        return false;
    }
    else 
    {
        // Get the value
        *p_value = fifo->fifo[fifo->tail];

        // Advance the tail
        fifo->tail++;
        if (fifo->tail >= FIFO_SIZE)
        {
            fifo->tail = 0;
        }
        
        // Success
        return true;
    }
}
/**
 * @brief Gives the number of elements currently in the FIFO
 * 
 */
uint16_t fifo_get_size(fifo_t* fifo)
{
    if (fifo->head >= fifo->tail) // When they are equal the size is 0
    {
        return fifo->head - fifo->tail;
    }
    else 
    {
        return FIFO_SIZE - (fifo->tail - fifo->head) + 1;
    }
}

bool fifo_is_empty(fifo_t* fifo)
{
    return fifo->head == fifo->tail;
}
