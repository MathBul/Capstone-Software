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
uint16_t utils_fl16_data_to_csum(uint8_t *data, int count)
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
 * @brief Computes the Fletcher-16 checkbytes from its checksum
 *
 * @param uint16_t checksum The checksum (returned from fletcher_16())
 * @param char check_bytes[2] A char array to write the check bytes into
 *
 * @return char* The same array that was passed in as check_bytes
 */
char* utils_fl16_csum_to_cbytes(uint16_t checksum, char check_bytes[2])
{
    uint16_t f0, f1, c0, c1;
    f0 = checksum & 0xFF;
    f1 = (checksum >> 8) & 0xFF;
    c0 = 0xFF - ((f0 + f1) % 0xFF);
    c1 = 0xFF - ((f0 + c0) % 0xFF);
    check_bytes[0] = c0;
    check_bytes[1] = c1;
    return check_bytes;
}

/**
 * @brief Combines the other two Fletcher16 functions to get from a char
 *        array directly to its check bytes (most common use case)
 *
 * @param uint8_t *data The array of uint8_t's (chars) with which to compute the checksum
 * @param int count The number of chars in the array, or the number to consider in the
 *                  checksum calculation
 * @param char check_bytes[2] A char array to write the check bytes into
 *
 * @return char* The same array that was passed in as check_bytes
 */
char* utils_fl16_data_to_cbytes(uint8_t *data, int count, char check_bytes[2])
{
    // Get the checksum from the data
    uint16_t checksum = utils_fl16_data_to_csum(data, count);
    // Get the check bytes from the checksum and write them to check_bytes
    utils_fl16_csum_to_cbytes(checksum, check_bytes);

    return check_bytes;
}

/**
 * @brief Validates transmission integrity by comparing the received check bytes of
 *        a message to its manually-calculated check bytes.
 *
 * @param uint8_t *data The array of uint8_t's (chars) with which to manually
 *                      calculate the check bytes.
 * @param int count The number of chars to use in the array (this should be the
 *                  length of the message, minus its 2 checksum bytes)
 * @param char check_bytes[2] A char array representing the "actual" check bytes
 *
 * @return bool true if expected and actual checksums match, false if not
 */
bool utils_validate_transmission(uint8_t *data, int count, char check_bytes[2])
{
    char expected[2];
    utils_fl16_data_to_cbytes(data, count, expected);
    bool result0 = (expected[0] == check_bytes[0]);
    bool result1 = (expected[1] == check_bytes[1]);
    return (result0 && result1);
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
void utils_set_nvic(uint8_t interrupt_num, uint8_t priority)
{
    uint8_t enable_shift = (interrupt_num % 32);
    uint8_t iser_position = (interrupt_num - enable_shift)/32;

    NVIC->ISER[iser_position] |= (1 << (enable_shift));
    NVIC->IP[interrupt_num] |= (priority << 5);
}

/**
 * @brief A general purpose empty function for command {entry, action, exit} that do nothing
 */
void utils_empty_function()
{
    return;
}

/* Chess Related */

/**
 * @brief Converts a given byte to a chess file
 * 
 * @param byte Represents the file
 * @return chess_file_t Enum of possible files
 */
chess_file_t utils_byte_to_file(uint8_t byte)
{
    chess_file_t file = FILE_ERROR;
    switch (byte)
    {
        case 'a':
            file = A;
        break;
        case 'b':
            file = B;
        break;
        case 'c':
            file = C;
        break;
        case 'd':
            file = D;
        break;
        case 'e':
            file = E;
        break;
        case 'f':
            file = F;
        break;
        case 'g':
            file = G;
        break;
        case 'h':
            file = H;
        break;
        default:
            file = FILE_ERROR;
        break;
    }

    return file;
}

/**
 * @brief Converts a given byte to a chess rank
 * 
 * @param byte Represents the rank
 * @return chess_rank_t Enum of possible rank
 */
chess_rank_t utils_byte_to_rank(uint8_t byte)
{
    chess_rank_t rank = RANK_ERROR;
    switch (byte)
    {
        case '1':
            rank = FIRST;
        break;
        case '2':
            rank = SECOND;
        break;
        case '3':
            rank = THIRD;
        break;
        case '4':
            rank = FOURTH;
        break;
        case '5':
            rank = FIFTH;
        break;
        case '6':
            rank = SIXTH;
        break;
        case '7':
            rank = SEVENTH;
        break;
        case '8':
            rank = EIGHTH;
        break;
        default:
            rank = RANK_ERROR;
        break;
    }

    return rank;
}

/**
 * @brief Converts a given byte to a chess move
 * 
 * @param byte Represents the move
 * @return chess_move_type Enum of possible rank
 */
chess_move_type utils_byte_to_move_type(uint8_t byte)
{
    chess_move_type type = IDLE;
    switch (byte)
    {
        case '_':
            type = MOVE;
        break;

        case 'Q':
            type = PROMOTION;
        break;

        case 'C':
            type = CAPTURE;
        break;

        case 'c':
            type = CASTLING;
        break;

        case 'E':
            type = EN_PASSENT;
        break;

        default:
            type = IDLE;
        break;
    }

    return type;
}

/**
 * @brief Get the difference in board state between two bit boards
 * 
 * @param changes_in_presence The XOR of two bit boards
 * @param board_changes_t Represents the move
 * @return chess_move_type Struct storing the changes in board states
 */
void utils_get_board_changes(uint64_t changes_in_presence, board_changes_t *board_changes)
{
    uint8_t i = 0;

    // Find set bits (1's) in changes_in_presence
    for (i = 0; i < 64; i++)
    {
        // If a set bit is found in some bit position i, store its index
        if ((changes_in_presence >> i) & 0x01)
        {
            board_changes->num_changes += 1;

            // On the first set bit seen, store in index1
            if (board_changes->num_changes == 1)
            {
                board_changes->index1 = i;
            }
            else if (board_changes->num_changes == 2)
            {
                board_changes->index2 = i;
            }
            else if (board_changes->num_changes == 3)
            {
                board_changes->index3 = i;
            }
            else if (board_changes->num_changes == 4)
            {
                board_changes->index4 = i;
            }
        }
    }
}

/**
 * @brief Sets arr1 equal to arr2 (used with 8x8 board_pieces arrays)
 *
 * @param arr1 The board to modify
 * @param arr2 The board to be copied
 */
void utils_set_pieces_equal(char arr1[8][8], char arr2[8][8])
{
    int i;
    int j;
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            arr1[i][j] = arr2[i][j];
        }
    }
}

/* End utils.c */
