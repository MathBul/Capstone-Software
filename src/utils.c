/**
 * @file utils.c
 * @author Nick Cooney (npc4crc@virginia.edu) and Eli Jelesko (ebj5hec@virginia.edu)
 * @brief Utility functions that may be used by more than one module
 * @version 0.1
 * @date 2022-10-02
 * 
 * @copyright Copyright (c) 2022
 */

#include "utils.h"

bool utils_sys_fault = false;

/**
 * @brief Enable the specified GPIO port by assigning it a clock
 * 
 * @param port GPIO_Type for the port being used
 */
void utils_gpio_clock_enable(GPIO_Type* port)
{
    uint32_t clock_gate_control_bit = 0;

    // Set the clock gate control bit based on the port
    if (port == GPIOA)
    {
        clock_gate_control_bit = SYSCTL_RCGCGPIO_R0;
    }
    else if (port == GPIOB)
    {
        clock_gate_control_bit = SYSCTL_RCGCGPIO_R1;
    }
    else if (port == GPIOC)
    {
        clock_gate_control_bit = SYSCTL_RCGCGPIO_R2;
    }
    else if (port == GPIOD)
    {
        clock_gate_control_bit = SYSCTL_RCGCGPIO_R3;
    }
    else if (port == GPIOE)
    {
        clock_gate_control_bit = SYSCTL_RCGCGPIO_R4;
    }
    else if (port == GPIOF)
    {
        clock_gate_control_bit = SYSCTL_RCGCGPIO_R5;
    }
    else if (port == GPIOG)
    {
        clock_gate_control_bit = SYSCTL_RCGCGPIO_R6;
    }
    else if (port == GPIOH)
    {
        clock_gate_control_bit = SYSCTL_RCGCGPIO_R7;
    }
    else if (port == GPIOJ)
    {
        clock_gate_control_bit = SYSCTL_RCGCGPIO_R8;
    }
    else if (port == GPIOK)
    {
        clock_gate_control_bit = SYSCTL_RCGCGPIO_R9;
    }
    else if (port == GPIOL)
    {
        clock_gate_control_bit = SYSCTL_RCGCGPIO_R10;
    }
    else if (port == GPIOM)
    {
        clock_gate_control_bit = SYSCTL_RCGCGPIO_R11;
    }
    else if (port == GPION)
    {
        clock_gate_control_bit = SYSCTL_RCGCGPIO_R12;
    }
    else if (port == GPIOP)
    {
        clock_gate_control_bit = SYSCTL_RCGCGPIO_R13;
    }
    else if (port == GPIOQ)
    {
        clock_gate_control_bit = SYSCTL_RCGCGPIO_R14;
    }

    // Enable clock gate control
    SYSCTL->RCGCGPIO |= clock_gate_control_bit;

    // Wait for the port clock to be ready
    while (!(SYSCTL->PRGPIO & clock_gate_control_bit))
    {
    }
}

/**
 * @brief Gets the shift of the lowest set bit in an 8-bit mask
 * 
 * @param mask The mask to find the shift of
 * @return uint8_t The shift value
 */
uint8_t utils_bits8_mask_to_shift(uint8_t mask)
{
    // Assert at least one bit is set
    assert(mask);
    
    // Shift until we get the lowest set bit
    uint8_t shift = 0;
    for (shift = 0; shift < 8; shift++)
    {
        if ((mask & 1) == 1)
        {
            break;
        }
        mask = mask >> 1;
    }
    return shift;
}

/**
 * @brief Moves a bit from the specified byte to a new position
 *  Useful for translating data from a physical port to virtual port
 * 
 * @param byte The byte being remasked
 * @param original_mask The original mask (where the bit was in the original byte)
 * @param new_mask The new mask (where the bit should be in the new byte)
 * @return uint8_t The remasked byte
 */
uint8_t utils_bits8_remask(uint8_t byte, uint8_t original_mask, uint8_t new_mask)
{
    uint8_t unmask_shift = utils_bits8_mask_to_shift(original_mask);
    uint8_t remask_shift = utils_bits8_mask_to_shift(new_mask);

    return (((byte & original_mask) >> unmask_shift) << remask_shift);
}

/**
 * @brief Restricts a given value to within a given range
 *
 * @param value The value to be bounded
 * @param lower bound If value < lower_bound, the function will return lower_bound
 * @param upper_bound If value < lower_bound, the function will return lower_bound
 * @return uint16_t A value between lower_bound and upper_bound
 */
uint16_t utils_bound(uint16_t value, uint16_t lower_bound, uint16_t upper_bound)
{
    // Make sure the bounds are actually in the correct order
    if (lower_bound > upper_bound)
    {
        uint16_t temp = lower_bound;
        lower_bound = upper_bound;
        upper_bound = temp;
    }

    // Restrict the value
    if (value < lower_bound)
    {
        return lower_bound;
    }
    if (value > upper_bound)
    {
        return upper_bound;
    }
    return value;
}

/**
 * @brief Computes the Fletcher-16 checksum given an array of chars
 *
 * @param uint8_t *data The array of uint8_t's (chars) with which to compute the checksum
 * @param int count The number of chars in the array, or the number to consider in the
 *                  checksum calculation
 * @return uint16_t The 16-bit Fletcher checksum of the given array
 */
uint16_t fletcher_16(uint8_t *data, int count)
{
   uint16_t sum1 = 0;
   uint16_t sum2 = 0;
   int i;

   for (i = 0; i < count; i++)
   {
      sum1 = (sum1 + data[i]) % 255;
      sum2 = (sum2 + sum1) % 255;
   }

   return (sum2 << 8) | sum1;
}



/**
 * @brief Delay the next operation with a for-loop (used for spacing out UART communication)
 *
 * @param ticks the number of clock ticks to delay for
 */
void utils_delay(uint32_t ticks)
{
    int i;
    for (i = 0; i < ticks; i++) {}
}

/**
 * @brief Sets the correct ISER bit in the NVIC
 */
void utils_set_nvic(uint8_t interrupt_num)
{
    uint8_t interrupt_shift = (interrupt_num % 32);
    uint8_t iser_position = (interrupt_num - interrupt_shift)/32;
    NVIC->ISER[iser_position] |= (1 << (interrupt_shift));
}

/* End utils.c */
