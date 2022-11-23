/**
 * @file fifo.c
 * @author Eli Jelesko (ebj5hec@virginia.edu)
 * @brief Implements First In First Out (FIFO) data structures
 * @version 0.1
 * @date 2022-10-03
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "fifo.h"

/**
 * @brief Initializes a FIFO (starts empty)
 * 
 * @param p_fifo Pointer to the specified fifo
 */
void fifo8_init(fifo8_t* p_fifo)
{
    p_fifo->head = 0;
    p_fifo->tail = 0;
}

/**
 * @brief Pushes an element into the FIFO. If the FIFO is full, this will do nothing
 * 
 * @param p_fifo Pointer to the specified fifo
 * @param value The value to be put on the queue
 * @return Whether the push was successful
 */
bool fifo8_push(fifo8_t* p_fifo, FIFO8_TYPE value)
{
    // If the FIFO is full, return false
    if (fifo8_get_size(p_fifo) == FIFO8_SIZE)
    {
        return false;
    }
    else
    {
        // Put the value in
        p_fifo->fifo[p_fifo->head] = value;

        // Advance the head
        p_fifo->head += 1;
        if (p_fifo->head >= FIFO8_SIZE)
        {
            p_fifo->head = 0;
        }

        // Success
        return true;
    }
}

/**
 * @brief Removes the value at the front of the FIFO. If there is nothing in the FIFO, the storage pointer is evaulated to NULL
 * 
 * @param p_fifo Pointer to the specified fifo
 * @param p_value Pointer to where the value will be stored
 * @return Whether the pop was successful
 */
bool fifo8_pop(fifo8_t* p_fifo, FIFO8_TYPE* p_value)
{
    // If the FIFO is empty do nothing
    if (fifo8_is_empty(p_fifo))
    {
        return false;
    }
    else 
    {
        // Get the value
        *p_value = p_fifo->fifo[p_fifo->tail];

        // Advance the tail
        p_fifo->tail += 1;
        if (p_fifo->tail >= FIFO8_SIZE)
        {
            p_fifo->tail = 0;
        }
        
        // Success
        return true;
    }
}

/**
 * @brief Gives the number of elements currently in the FIFO
 * 
 * @param p_fifo Pointer to the specified fifo
 * @return The size of the specified fifo
 */
uint16_t fifo8_get_size(fifo8_t* p_fifo)
{
    // When (head==tail), the size is 0
    if (p_fifo->head >= p_fifo->tail) 
    {
        return p_fifo->head - p_fifo->tail;
    }
    else 
    {
        return FIFO8_SIZE - (p_fifo->tail - p_fifo->head) + 1;
    }
}

/**
 * @brief Checks if the FIFO is empty or not
 * 
 * @param p_fifo Pointer to the specified fifo
 * @return True if the FIFO is empty, false otherwise
 */
bool fifo8_is_empty(fifo8_t* p_fifo)
{
    return p_fifo->head == p_fifo->tail;
}

/**
 * @brief Clears the FIFO
 * 
 * @param p_fifo Pointer to the specified fifo
 * @return True always
 */
bool fifo8_clear(fifo8_t* p_fifo)
{
    p_fifo->head = p_fifo->tail;
    return true;
}

/* End fifo.c */
