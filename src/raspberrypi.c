/**
 * @file raspberrypi.c
 * @author Nick Cooney (npc4crc@virginia.edu) and Keenan Alchaar (ka5nt@virginia.edu)
 * @brief Provides functions for interacting with a Raspberry Pi
 * @version 0.1
 * @date 2022-10-09
 * 
 * @copyright Copyright (c) 2022
 */

#include "raspberrypi.h"

/**
 * @brief Initialize the Raspberry Pi UART tx and rx lines
 */
void rpi_init()
{
    uart_init(RPI_UART_CHANNEL);
}

/**
 * @brief Uses UART to send data from the MSP432 to the Raspberry Pi
 *
 * @param data An array of characters to send
 * @return True if successful; false otherwise
 */
bool rpi_transmit(char* data, uint8_t num_chars)
{
    bool status;
    uint8_t i = 0;
    
    for (i = 0; i < num_chars; i++)
    {
        status = uart_out_byte(RPI_UART_CHANNEL, (uint8_t) data[i]);

        if (!status)
        {
            return false;
        }
    }
    // TODO: Checksum here?

    return true;
}

/**
 * @brief Uses UART to read data from the the Raspberry Pi to the MSP432
 *
 * @param data The character array to read the data into
 * @return True if successful; false otherwise
 */
bool rpi_receive(char *data, uint8_t num_chars)
{
    bool status;
    uint8_t i = 0;
    
    for (i = 0; i < num_chars; i++)
    {
        status = uart_read_byte(RPI_UART_CHANNEL, (uint8_t*) &data[i]);

        if (!status)
        {
            return false;
        }
    }

    return true;
    /*
     * Dev notes:
     * Order of operations: (if timeout at any point reset)
     * 1. Wait for start byte
     * 2. Once start sequence is received, read the order and assign to variables
     * 3. Calculate checksum
     * 4. If checksum passes, send an ack and return the move struct
     * 4b. If checksum fails, send a bad ack and goto step 1.
     */
}

/**
 * @brief Send a RESET instruction from the MSP432 to the Raspberry Pi
 */
void rpi_transmit_reset(void)
{
    uart_out_byte(RPI_UART_CHANNEL, (uint8_t) START_BYTE);
    uart_out_byte(RPI_UART_CHANNEL, (uint8_t) RESET_INSTR_AND_LEN);
}

/**
 * @brief Send a START_W or START_B instruction from the MSP432 to the Raspberry Pi
 * @param color A char representing the color the human player is playing as
 */
void rpi_transmit_start(char color)
{
    if (color == 'W')
    {
        uart_out_byte(RPI_UART_CHANNEL, (uint8_t) START_BYTE);
        uart_out_byte(RPI_UART_CHANNEL, (uint8_t) START_W_INSTR_AND_LEN);
    }
    else if (color == 'B')
    {
        uart_out_byte(RPI_UART_CHANNEL, (uint8_t) START_BYTE);
        uart_out_byte(RPI_UART_CHANNEL, (uint8_t) START_B_INSTR_AND_LEN);
    }
}

/**
 * @brief Send a HUMAN_MOVE instruction from the MSP432 to the Raspberry Pi
 * @param move A null-terminated C-string representing the move the player
 *             wishes to make in UCI notation
 */
void rpi_transmit_human_move(char *move)
{
    uart_out_byte(RPI_UART_CHANNEL, (uint8_t) START_BYTE);
    uart_out_byte(RPI_UART_CHANNEL, (uint8_t) HUMAN_MOVE_INSTR_AND_LEN);
    char *t;
    for (t = move; *t != '\0'; t++)
    {
        uart_out_byte(RPI_UART_CHANNEL, (uint8_t) *t);
        utils_delay(50000);
    }
}

/**
 * @brief Clears the tx and rx fifos for RPi communication
 */
void rpi_reset_uart()
{
    uart_reset(RPI_UART_CHANNEL);
}

// Chess
chess_file_t rpi_byte_to_file(uint8_t byte)
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
chess_rank_t rpi_byte_to_rank(uint8_t byte)
{
    chess_rank_t rank = RANK_ERROR;
    switch (byte)
    {
        case 1:
            rank = FIRST;
        break;
        case 2:
            rank = SECOND;
        break;
        case 3:
            rank = THIRD;
        break;
        case 4:
            rank = FOURTH;
        break;
        case 5:
            rank = FIFTH;
        break;
        case 6:
            rank = SIXTH;
        break;
        case 7:
            rank = SEVENTH;
        break;
        case 8:
            rank = EIGHTH;
        break;
        default:
            rank = RANK_ERROR;
        break;
    }

    return rank;
}

chess_move_type rpi_byte_to_move_type(uint8_t byte)
{
    chess_move_type type = IDLE;
    switch (byte)
    {
        case 'M':
            type = MOVE;
        break;

        case 'P':
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



// Command Functions
void rpi_entry(command_t* command);
void rpi_action(command_t* command);
void rpi_exit(command_t* command);
bool rpi_is_done(command_t* command);

/* End raspberrypi.c */
