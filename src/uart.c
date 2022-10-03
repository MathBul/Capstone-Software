/**
 * @file uart.c
 * @author Keenan Alchaar (ka5nt@virginia.edu) and Eli Jelesko (ebj5hec@virginia.edu)
 * @brief Code for the UART communications on the MSP432e401Y
 * @version 0.1
 * @date 2022-10-03
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "uart.h"
#include <stdint.h>

#define UART_CHANNEL_0          (0)
#define UART_CHANNEL_1          (1)
#define UART_CHANNEL_2          (2)
#define UART_CHANNEL_3          (3)
#define UART_CHANNEL_4          (4)
#define UART_CHANNEL_5          (5)
#define UART_CHANNEL_6          (6)





void uart_init(uint8_t uart_channel) {
    // Enable UART module 6 (pins P0 and P1)
    SYSCTL->RCGCUART |= SYSCTL_RCGCUART_R6;
    // Run port P = R13 (13th bit)
    SYSCTL->RCGCGPIO |= SYSCTL_RCGCGPIO_R13;
    // Do not use analog function (set bits to use digital function)
    GPIOP->AFSEL |= (0x02); // 0010b
    GPIOP->DEN |= (0x02); // 0010b
    GPIOP->PCTL |= (0x11); // 0001 0001b
    // Disable UART
    UART6->CTL &= ~(0x01);
    // Integer baud rate divisor
    UART6->IBRD &= ~(0xFFFF);
    UART6->IBRD |= (0x0A); // 10
    // Fractional baud rate divisor
    UART6->FBRD &= ~(0xFFFF);
    UART6->FBRD |= (0x36); // 54
    // Write serial parameters
    UART6->LCRH = (0x60);
    // Set baud clock source to MOSC
    UART6->CC |= (0x03);
    // Enable UART
    UART6->CTL |= UART_CTL_UARTEN;
}

/* End uart.c */
