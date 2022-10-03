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

static FIFO_TYPE fifo[FIFO_SIZE];
static uint16_t head;
static uint16_t tail;
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
void fifo_init() 
{
    head = 0;
    tail = 0;
}
/**
 * @brief Pushes an element into the FIFO. If the FIFO is full, this will do nothing. 
 * @param value The value to be put on the queue. 
 */
bool fifo_push(FIFO_TYPE value) 
{
    // If the FIFO is full, return
    if (head == tail - 1 || (head == FIFO_SIZE - 1 && tail == 0)) 
    {
        return false;
    }
    else
    {
        // Advance the head
        head++;
        // Wrap
        if (head >= FIFO_SIZE)
        {
            head = 0;
        }

        // Put the value in
        fifo[head] = value;

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
bool fifo_pop(FIFO_TYPE* p_value) 
{
    // If it's empty do nothing
    if (head == tail)
    {
        return false;
    }
    else 
    {
        // Get the value
        *p_value = fifo[tail];

        // Advance the tail
        tail++;
        if (tail >= FIFO_SIZE)
        {
            tail = 0;
        }
        
        // Success
        return true;
    }
}
/**
 * @brief Gives the number of elements currently in the FIFO
 * 
 */
uint16_t fifo_get_size()
{
    if (head >= tail) // When they are equal the size is 0
    {
        return (head - tail) / (sizeof(FIFO_TYPE));
    }
    else 
    {
        return FIFO_SIZE - ((tail - head) / sizeof(FIFO_TYPE));
    }
}