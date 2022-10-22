/**
 * @file command_queue.c
 * @author Eli Jelesko (ebj5hec@virginia.edu)
 * @brief Implements a First-In, First-Out (queue) data structure for commands
 * @version 0.1
 * @date 2022-10-22
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "command_queue.h"

static command_t queue[COMMAND_QUEUE_SIZE];
static uint16_t head;
static uint16_t tail;

/**
 * @brief Initializes the queue. Starts empty
 */
void command_queue_init(void)
{
    head = 0;
    tail = 0;
}

/**
 * @brief Pushes an element into the queue. If the queue is full, this will do nothing
 * 
 * @param value The value to be put on the queue. 
 */
bool command_queue_push(command_t value)
{
    // If the queue is full, return
    if (command_queue_get_size() == COMMAND_QUEUE_SIZE)
    {
        return false;
    }
    else
    {
        // Put the value in
        queue[head] = value;

        // Advance the head
        head++;

        // Wrap
        if (head >= COMMAND_QUEUE_SIZE)
        {
            head = 0;
        }

        // Success
        return true;
    }
}

/**
 * @brief Removes the value at the front of the queue. If there is nothing in the queue,
 *  the storage pointer is evaulated to NULL
 * 
 * @param p_value A pointer to where the value will be stored
 */
bool command_queue_pop(command_t* p_value)
{
    // If it's empty do nothing
    if (command_queue_is_empty())
    {
        return false;
    }
    else 
    {
        // Get the value
        *p_value = queue[tail];

        // Advance the tail
        tail++;
        if (tail >= COMMAND_QUEUE_SIZE)
        {
            tail = 0;
        }
        
        // Success
        return true;
    }
}

/**
 * @brief Gives the number of elements currently in the queue
 */
uint16_t command_queue_get_size(void)
{
    if (head >= tail) // When they are equal the size is 0
    {
        return head - tail;
    }
    else 
    {
        return COMMAND_QUEUE_SIZE - (tail - head) + 1;
    }
}

/**
 * @brief Checks if the queue is empty or not
 * 
 * @return true The queue is empty
 * @return false There are elements still in the queue
 */
bool command_queue_is_empty(void)
{
    return head == tail;
}

/* End command_queue.c */
