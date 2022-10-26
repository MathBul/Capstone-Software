/**
 * @file uart_fifo.c
 * @author Eli Jelesko (ebj5hec@virginia.edu)
 * @brief Implements a First In First Out (FIFO) data structure for UART (8-bit)
 * @version 0.1
 * @date 2022-10-03
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "uart_fifo.h"

/**
 * @brief Initializes the FIFO. Starts empty
 */
void uart_fifo_init(uart_fifo_t* fifo)
{
    fifo->head = 0;
    fifo->tail = 0;
}

/**
 * @brief Pushes an element into the FIFO. If the FIFO is full, this will do nothing
 * 
 * @param value The value to be put on the queue. 
 */
bool uart_fifo_push(uart_fifo_t* fifo, UART_FIFO_TYPE value)
{
    // If the FIFO is full, return
    if (uart_fifo_get_size(fifo) == UART_FIFO_SIZE)
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
        if (fifo->head >= UART_FIFO_SIZE)
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
bool uart_fifo_pop(uart_fifo_t* fifo, UART_FIFO_TYPE* p_value)
{
    // If it's empty do nothing
    if (uart_fifo_is_empty(fifo))
    {
        return false;
    }
    else 
    {
        // Get the value
        *p_value = fifo->fifo[fifo->tail];

        // Advance the tail
        fifo->tail++;
        if (fifo->tail >= UART_FIFO_SIZE)
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
uint16_t uart_fifo_get_size(uart_fifo_t* fifo)
{
    if (fifo->head >= fifo->tail) // When they are equal the size is 0
    {
        return fifo->head - fifo->tail;
    }
    else 
    {
        return UART_FIFO_SIZE - (fifo->tail - fifo->head) + 1;
    }
}

/**
 * @brief Checks if the FIFO is empty or not
 * 
 * @param fifo The particular FIFO
 * @return true The FIFO is empty
 * @return false There are elements still in the FIFO
 */
bool uart_fifo_is_empty(uart_fifo_t* fifo)
{
    return fifo->head == fifo->tail;
}

/**
 * @brief Clears the FIFO
 * 
 * @param fifo The particular FIFO
 * @return true
 */
bool uart_fifo_clear(uart_fifo_t* fifo)
{
    fifo->head = fifo->tail;
    return true;
}

/* End fifo.c */
