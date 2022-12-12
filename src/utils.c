/**
 * @file utils.c
 * @author Nick Cooney (npc4crc@virginia.edu), Eli Jelesko (ebj5hec@virginia.edu), Keenan Alchaar (ka5nt@virginia.edu)
 * @brief Utility functions that may be used by more than one module
 * @version 0.1
 * @date 2022-10-02
 * 
 * @copyright Copyright (c) 2022
 */

#include "utils.h"

/* GPIO, Interrupt, and Empty */

/**
 * @brief Enable the specified GPIO port by assigning it a clock
 * 
 * @param port GPIO_Type for the port being used
 */
void utils_gpio_clock_enable(GPIO_Type* port)
{
    uint32_t clock_gate_control_bit = 0;

    // Determine the clock gate control bit from the port
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

    // Wait for the clock to be ready
    while (!(SYSCTL->PRGPIO & clock_gate_control_bit))
    {
    }
}

/**
 * @brief Enable the specified UART channel by assigning it a clock
 * 
 * @param uart_channel One of UART_CHANNEL_X for X={0,...,7}
 */
void utils_uart_clock_enable(uint8_t uart_channel)
{
    uint32_t clock_gate_control_bit = 0;

    // Determine the clock gate control bit from the port
    if (uart_channel == UART_CHANNEL_0)
    {
        clock_gate_control_bit = SYSCTL_RCGCUART_R0;
    }
    else if (uart_channel == UART_CHANNEL_1)
    {
        clock_gate_control_bit = SYSCTL_RCGCUART_R1;
    }
    else if (uart_channel == UART_CHANNEL_2)
    {
        clock_gate_control_bit = SYSCTL_RCGCUART_R2;
    }
    else if (uart_channel == UART_CHANNEL_3)
    {
        clock_gate_control_bit = SYSCTL_RCGCUART_R3;
    }
    else if (uart_channel == UART_CHANNEL_4)
    {
        clock_gate_control_bit = SYSCTL_RCGCUART_R4;
    }
    else if (uart_channel == UART_CHANNEL_5)
    {
        clock_gate_control_bit = SYSCTL_RCGCUART_R5;
    }
    else if (uart_channel == UART_CHANNEL_6)
    {
        clock_gate_control_bit = SYSCTL_RCGCUART_R6;
    }
    else if (uart_channel == UART_CHANNEL_7)
    {
        clock_gate_control_bit = SYSCTL_RCGCUART_R7;
    }

    // Enable clock gate control
    SYSCTL->RCGCUART |= clock_gate_control_bit;

    // Wait for the clock to be ready
    while (!(SYSCTL->PRUART & clock_gate_control_bit))
    {
    }
}

/**
 * @brief Enable the specified UART channel by assigning it a clock
 * 
 * @param timer One of TIMERX for X={0,...,7}
 */
void utils_timer_clock_enable(TIMER0_Type* timer)
{
    uint32_t clock_gate_control_bit = 0;

    // Determine the clock gate control bit from the port
    if (timer == TIMER0)
    {
        clock_gate_control_bit = SYSCTL_RCGCTIMER_R0;
    }
    else if (timer == TIMER1)
    {
        clock_gate_control_bit = SYSCTL_RCGCTIMER_R1;
    }
    else if (timer == TIMER2)
    {
        clock_gate_control_bit = SYSCTL_RCGCTIMER_R2;
    }
    else if (timer == TIMER3)
    {
        clock_gate_control_bit = SYSCTL_RCGCTIMER_R3;
    }
    else if (timer == TIMER4)
    {
        clock_gate_control_bit = SYSCTL_RCGCTIMER_R4;
    }
    else if (timer == TIMER5)
    {
        clock_gate_control_bit = SYSCTL_RCGCTIMER_R5;
    }
    else if (timer == TIMER6)
    {
        clock_gate_control_bit = SYSCTL_RCGCTIMER_R6;
    }
    else if (timer == TIMER7)
    {
        clock_gate_control_bit = SYSCTL_RCGCTIMER_R7;
    }

    // Enable clock gate control
    SYSCTL->RCGCTIMER |= clock_gate_control_bit;

    // Wait for the clock to be ready
    while (!(SYSCTL->PRTIMER & clock_gate_control_bit))
    {
    }
}

/**
 * @brief Delay the next operation with a for-loop (used to space-out UART communication during testing)
 *
 * @param ticks Number of clock ticks to delay
 */
void utils_delay(uint32_t ticks)
{
    int i;
    for (i = 0; i < ticks; i++) {}
}

/**
 * @brief Sets the correct ISER bit in the NVIC
 * 
 * @param interrupt_num Which interrupt is being configured, one of {TIMERXY_IRQn}
 * @param priority Desired interrupt priority
 */
void utils_set_nvic(uint8_t interrupt_num, uint8_t priority)
{
    // Determine the correct ISER register (32 interrupts per ISER)
    uint8_t enable_shift = (interrupt_num % 32);
    uint8_t iser_position = (interrupt_num - enable_shift)/32;

    // Set the values to the registers
    NVIC->ISER[iser_position] |= (1 << (enable_shift));
    NVIC->IP[interrupt_num] |= (priority << 5);
}

/**
 * @brief A general purpose empty function for a command {entry, action, exit} that does nothing
 */
void utils_empty_function(command_t* command)
{
    return;
}

/* Math and Bit Manipulation */

/**
 * @brief Gets the shift corresponding to the least significant bit (LSB) set in an 8-bit value
 * 
 * @param value The value to find the shift of
 * @return uint8_t The shift needed to reach the LSB
 */
uint8_t utils_bits8_get_lsb_shift(uint8_t value)
{
    // Assert at least one bit is set
    assert(value);
    
    // Shift until we get the lowest set bit
    uint8_t shift = 0;
    for (shift = 0; shift < 8; shift++)
    {
        if ((value & 1) == 1)
        {
            break;
        }
        value = (value >> 1);
    }

    // Return the number of shifts needed
    return shift;
}

/**
 * @brief Moves a bit in the specified byte to a new position. Useful for translating from physical ports to virtual ports
 *      Note: All but the remasked bit will be zeros
 * 
 * @param byte The byte being remasked
 * @param original_mask The original mask (where the bit was in the original byte)
 * @param new_mask The new mask (where the bit should be in the new byte)
 * @return uint8_t The remasked byte
 */
uint8_t utils_bits8_remask(uint8_t byte, uint8_t original_mask, uint8_t new_mask)
{
    // Determine tthe shifts needed to unmask and remask the bit
    uint8_t unmask_shift = utils_bits8_get_lsb_shift(original_mask);
    uint8_t remask_shift = utils_bits8_get_lsb_shift(new_mask);

    // Perform the shifts
    return (((byte & original_mask) >> unmask_shift) << remask_shift);
}

/**
 * @brief Restricts a specified  value to within a given range
 *
 * @param value The value to be bounded
 * @param lower_bound If (value < lower_bound), returns lower_bound
 * @param upper_bound If (value > upper_bound), returns upper_bound
 * @return uint16_t A value between lower_bound and upper_bound, inclusive 
*/
uint16_t utils_bound(uint16_t value, uint16_t lower_bound, uint16_t upper_bound)
{
    // Ensure the bounds are in the correct order
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

/* Fletcher-16 Checksum */

/**
 * @brief Computes the Fletcher-16 checksum given an array of chars
 *
 * @param data Pointer to first element of a char array to checksum
 * @param count The number of chars to consider in the calculation
 * @return The 16-bit Fletcher checksum of the array
 */
uint16_t utils_fl16_data_to_checksum(uint8_t *data, int count)
{    
    uint16_t sum1 = 0;
    uint16_t sum2 = 0;
    int i;

    // Perform two sums (up to mod 255)
    for (i = 0; i < count; i++)
    {
        // Conditional subtractiton is more efficient than a modulus operation
        sum1 += data[i];
        if (sum1 > 255)
        {
            sum1 -= 255;
        }

        // Conditional subtractiton is more efficient than a modulus operation
        sum2 += sum1;
        if (sum2 > 255)
        {
            sum2 -= 255;
        }
    }

    // Combine the sums
    return (sum2 << 8) | sum1;
}

/**
 * @brief Computes the Fletcher-16 checkbytes from its checksum
 *
 * @param checksum The checksum (returned from utils_fl16_data_to_csum())
 * @param check_bytes Char buffer to write the check bytes into
 * @return The checkbyte array
 */
void utils_fl16_checksum_to_checkbytes(uint16_t checksum, char check_bytes[2])
{
    uint16_t f0, f1, c0, c1;

    // Do checkbyte math
    f0 = (checksum & 0xFF);
    f1 = ((checksum >> 8) & 0xFF);
    c0 = (0xFF - ((f0 + f1) % 0xFF));
    c1 = (0xFF - ((f0 + c0) % 0xFF));
    
    // Store and return the checkbytes
    check_bytes[0] = c0;
    check_bytes[1] = c1;

}

/**
 * @brief Performs a Fletcher16 checksum and directly stores its checkbytes
 *
 * @param data Pointer to first element of a char array to checksum
 * @param count Number of chars to consider in the checksum calculation
 * @param check_bytes Char buffer to write the check bytes into
 * @return The checkbyte array
 */
void utils_fl16_data_to_checkbytes(uint8_t *data, int count, char check_bytes[2])
{
    // Get the checksum from the data
    uint16_t checksum = utils_fl16_data_to_checksum(data, count);

    // Get the check bytes from the checksum and write them to check_bytes
    utils_fl16_checksum_to_checkbytes(checksum, check_bytes);

}

/**
 * @brief Validates a messages checkbytes
 *
 * @param data Pointer to first element of a char array to checksum
 * @param count Number of chars to consider in the checksum calculation
 * @param actual_check_bytes Char buffer representing the "actual" checkbytes
 * @return Whether the message is valid
 */
bool utils_validate_transmission(uint8_t *data, int count, char actual_check_bytes[2])
{
    // Compute the expectted checkbytes
    char expected_check_bytes[2];
    utils_fl16_data_to_checkbytes(data, count, expected_check_bytes);

    // Compare to the actual checkbytes
    return ((expected_check_bytes[0] == actual_check_bytes[0]) && (expected_check_bytes[1] == actual_check_bytes[1]));
}

/* Chess Related */

/**
 * @brief Helper function to translate a rank/file to the index of that tile in a 64-bit representation
 *
 * @param file The column to select
 * @param rank The row to select
 * @return The tile index
 */
uint8_t utils_tile_to_index(chess_file_t file, chess_rank_t rank)
{
    uint8_t tile_index = 0;

    // Determine the index
    switch (file)
    {
        case A:
            tile_index += 0;
        break;

        case B:
            tile_index += 1;
        break;

        case C:
            tile_index += 2;
        break;

        case D:
            tile_index += 3;
        break;

        case E:
            tile_index += 4;
        break;

        case F:
            tile_index += 5;
        break;

        case G:
            tile_index += 6;
        break;

        case H:
            tile_index += 7;
        break;

        default:
            // Invalid file, do nothing
        break;
    }

    // Determine how much to shift for this rank
    switch (rank)
    {
        case FIRST:
            tile_index += 0;
        break;

        case SECOND:
            tile_index += 8;
        break;

        case THIRD:
            tile_index += 16;
        break;

        case FOURTH:
            tile_index += 24;
        break;

        case FIFTH:
            tile_index += 32;
        break;

        case SIXTH:
            tile_index += 40;
        break;

        case SEVENTH:
            tile_index += 48;
        break;

        case EIGHTH:
            tile_index += 56;
        break;

        default:
            // Invalid rank, do nothing
        break;
    }

    return tile_index;
}

/**
 * @brief Helper function to convert a board index into a rank
 * 
 * @param index One of {0,...,7} for the board row
 * @return The corresponding rank
 */
chess_rank_t utils_index_to_rank(uint8_t index)
{
    chess_rank_t output = RANK_ERROR;
    switch (index)
    {
        case 0:
            output = FIRST;
        break;
        
        case 1:
            output = SECOND;
        break;

        case 2:
            output = THIRD;
        break;
        
        case 3:
            output = FOURTH;
        break;
        
        case 4:
            output = FIFTH;
        break;
        
        case 5:
            output = SIXTH;
        break;
        
        case 6:
            output = SEVENTH;
        break;
        
        case 7:
            output = EIGHTH;
        break;

        default:
            output = RANK_ERROR;
        break;
    }

    return output;
}

/**
 * @brief Helper function to convert a board index into a file
 * 
 * @param index One of {0,...,7} for the board column
 * @return The corresponding file
 */
chess_file_t utils_index_to_file(uint8_t index)
{
    chess_file_t output = FILE_ERROR;
    switch (index)
    {
        case 0:
            output = A;
        break;
        
        case 1:
            output = B;
        break;

        case 2:
            output = C;
        break;
        
        case 3:
            output = D;
        break;
        
        case 4:
            output = E;
        break;
        
        case 5:
            output = F;
        break;
        
        case 6:
            output = G;
        break;
        
        case 7:
            output = H;
        break;

        default:
            output = FILE_ERROR;
        break;
    }

    return output;
}

/**
 * @brief Converts a given byte to a chess file
 * 
 * @param byte Represents the file (e.g., as a char)
 * @return One of the possible enum files
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
 * @param byte Represents the rank (e.g., as a char)
 * @return One of the possible enum ranks
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
 * @param byte Represents the move (e.g., as a char)
 * @return One of the possible enum moves
 */
chess_move_type_t utils_byte_to_move_type(uint8_t byte)
{
    chess_move_type_t type = IDLE;
    switch (byte)
    {
        case '_':
            type = MOVE;
        break;

        case 'Q':
            type = PROMOTION;
        break;

        case 'q':
            type = CAPTURE_PROMOTION;
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
 * @brief Converts a given byte to a chess piece
 * 
 * @param byte Represents the move (e.g., as a char)
 * @return One of the possible enum moves
 */
chess_piece_t utils_byte_to_piece_type(uint8_t byte)
{
    chess_piece_t piece = EMPTY_PIECE;
    switch (byte)
    {
        case 'p':
            piece = PAWN;
        break;

        case 'P':
            piece = PAWN;
        break;

        case 'q':
            piece = QUEEN;
        break;

        case 'Q':
            piece = QUEEN;
        break;

        case 'k':
            piece = KING;
        break;

        case 'K':
            piece = KING;
        break;

        case 'n':
            piece = KNIGHT;
        break;

        case 'N':
            piece = KNIGHT;
        break;

        case 'r':
            piece = ROOK;
        break;

        case 'R':
            piece = ROOK;
        break;

        case 'b':
            piece = BISHOP;
        break;

        case 'B':
            piece = BISHOP;
        break;

        default:
            piece = EMPTY_PIECE;
        break;
    }

    return piece;
}

/**
 * @brief Adds an offset to piece height based on position on board
 *
 * @param file The column
 * @param rank The row
 * @param piece The piece
 * @return The total vertical distance to travel
 */
int32_t utils_calculate_offset(int32_t pos_x, int32_t pos_y, int32_t pos_z)
{
    int offset = 0;

    // Add the offset
    switch (pos_x)
    {
        case A:
            offset -= 0;
        break;

        case B:
            offset -= 0;
        break;

        case C:
            offset -= 1;
        break;

        case D:
            offset -= 1;
        break;

        case E:
            offset -= 0;
        break;

        case F:
            offset -= 0;
        break;

        case G:
            offset -= 1;
        break;

        case H:
            offset -= 0;
        break;

        default:
            // Error, do nothing
        break;
    }

    switch (pos_y)
    {
        case FIRST:
            offset += 6;
        break;

        case SECOND:
            offset += 6;
        break;

        case THIRD:
            offset += 4;
        break;

        case FOURTH:
            offset += 2;
        break;

        case FIFTH:
            offset += 1;
        break;

        case SIXTH:
            offset += 1;
        break;

        case SEVENTH:
            offset += 0;
        break;

        case EIGHTH:
            offset += 0;
        break;

        default:
            // Error, do nothing
        break;
    }

    return ((int32_t) offset);

}

/* End utils.c */
