/**
 * @file stepper_fifo.c
 * @author Eli Jelesko (ebj5hec@virginia.edu)
 * @brief Implements a First In First Out (FIFO) data structure for the stepper motors (32-bit)
 * @version 0.1
 * @date 2022-10-03
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "stepper_fifo.h"

/**
 * @brief Initializes the FIFO. Starts empty
 */
void stepper_fifo_init(stepper_fifo_t* fifo)
{
    fifo->head = 0;
    fifo->tail = 0;
}

/**
 * @brief Pushes an element into the FIFO. If the FIFO is full, this will do nothing
 * 
 * @param value The value to be put on the queue. 
 */
bool stepper_fifo_push(stepper_fifo_t* fifo, STEPPER_FIFO_TYPE value)
{
    // If the FIFO is full, return
    if (stepper_fifo_get_size(fifo) == STEPPER_FIFO_SIZE)
    {
        return false;
    }
    else
    {
        // Put the value in
        fifo->fifo[fifo->head] = value;

        // Advance the head
        fifo->head++;

        // Wrap
        if (fifo->head >= STEPPER_FIFO_SIZE)
        {
            fifo->head = 0;
        }

        // Success
        return true;
    }
}

/**
 * @brief Removes the value at the front of the FIFO. If there is nothing in the FIFO, 
 *  the storage pointer is evaulated to NULL
 * 
 * @param p_value A pointer to where the value will be stored
 */
bool stepper_fifo_pop(stepper_fifo_t* fifo, STEPPER_FIFO_TYPE* p_value)
{
    // If it's empty do nothing
    if (stepper_fifo_is_empty(fifo))
    {
        return false;
    }
    else 
    {
        // Get the value
        *p_value = fifo->fifo[fifo->tail];

        // Advance the tail
        fifo->tail++;
        if (fifo->tail >= STEPPER_FIFO_SIZE)
        {
            fifo->tail = 0;
        }
        
        // Success
        return true;
    }
}

/**
 * @brief Gives the number of elements currently in the FIFO
 */
uint16_t stepper_fifo_get_size(stepper_fifo_t* fifo)
{
    if (fifo->head >= fifo->tail) // When they are equal the size is 0
    {
        return fifo->head - fifo->tail;
    }
    else 
    {
        return STEPPER_FIFO_SIZE - (fifo->tail - fifo->head) + 1;
    }
}

/**
 * @brief Checks if the FIFO is empty or not
 * 
 * @param fifo The particular FIFO
 * @return true The FIFO is empty
 * @return false There are elements still in the FIFO
 */
bool stepper_fifo_is_empty(stepper_fifo_t* fifo)
{
    return fifo->head == fifo->tail;
}

/* End fifo.c */
