#ifndef UART_H // May conflict??
#define UART_H

#include "msp.h"
#include <stdint.h>
#include "fifo.h"

#define UART_CHANNEL_0          (0)
#define UART_CHANNEL_1          (1)
#define UART_CHANNEL_2          (2)
#define UART_CHANNEL_3          (3)
#define UART_CHANNEL_4          (4)
#define UART_CHANNEL_5          (5)
#define UART_CHANNEL_6          (6)



void uart_init(uint8_t uart_channel);
bool uart_out_byte(uint8_t uart_channel, uint8_t byte);
bool uart_read_byte(uint8_t uart_channel, uint8_t* byte);


#endif /* UART_H */
