/**
 * @file raspberrypi.c
 * @author Nick Cooney (npc4crc@virginia.edu)
 * @brief Provides functions for interacting with a Raspberry Pi
 * @version 0.1
 * @date 2022-10-09
 * 
 * @copyright Copyright (c) 2022
 */

#include "raspberrypi.h"
#include "uart.h"
#include "utils.h"

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
 * @param data The character to send
 * @return True if successful; false otherwise
 */
bool rpi_transmit(char data)
{
    return uart_out_byte(RPI_UART_CHANNEL, (uint8_t) data);
}

/**
 * @brief Uses UART to read data from the the Raspberry Pi to the MSP432
 *
 * @param data The character to read the data into
 * @return True if successful; false otherwise
 */
bool rpi_receive(char *data)
{
    return uart_read_byte(RPI_UART_CHANNEL, (uint8_t*) data);
}

void rpi_transmit_RESET()
{
    uart_out_byte(RPI_UART_CHANNEL, (uint8_t) START_BYTE);
    uart_out_byte(RPI_UART_CHANNEL, (uint8_t) RESET_INSTR_AND_LEN);
}

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

void rpi_transmit_HUMAN_MOVE(char *move)
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

/* End raspberrypi.c */
