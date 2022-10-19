/**
 * @file raspberrypi.h
 * @author Nick Cooney (npc4crc@virginia.edu) and Keenan Alchaar (ka5nt@virginia.edu)
 * @brief Provides functions for interacting with a Raspberry Pi
 * @version 0.1
 * @date 2022-10-09
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef RASPBERRYPI_H_
#define RASPBERRYPI_H_

#include "msp.h"
#include "gpio.h"
#include <stdint.h>
#include <stdbool.h>

// General Raspberry Pi defines
#define RPI_UART_CHANNEL                    (UART_CHANNEL_3)

// Public Raspberry Pi functions
void rpi_init();
bool rpi_transmit(char data);
bool rpi_receive(char *data);

// Raspberry Pi Instruction functions
void rpi_transmit_RESET();
void rpi_transmit_start(char color);
void rpi_transmit_HUMAN_MOVE(char *move);

#endif /* RASPBERRYPI_H_ */
