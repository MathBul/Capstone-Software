#include "uart.h"

//#define UART_MAIN

void uart_init(void) {
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

#ifdef UART_MAIN
int main(void)
{
//    sysclock_initialize();
//    uart_init();
    int i = 0;
    SYSCTL->RCGCGPIO |= SYSCTL_RCGCGPIO_R12;
    GPION->DEN |= 1;
    GPION->DIR |= 1;
    while(1)
    {
        GPION->DATA |= 1;
    }
}
#endif
