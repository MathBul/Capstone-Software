#ifndef GPIO_H_
#define GPIO_H_

#include "msp432e401y.h"

// Functions for GPIO output
void gpioSetAsOutput(GPIO_Type* Port, uint8_t Pin, uint32_t sysctl);
void gpioSetOutputHigh(GPIO_Type* Port, uint8_t Pin);
void gpioSetOutputLow(GPIO_Type* Port, uint8_t Pin);
void gpioSetOutputToggle(GPIO_Type* Port, uint8_t Pin);

// Functions for GPIO input
void gpioSetAsInput(GPIO_Type* Port, uint8_t Pin, uint32_t sysctl);

#endif /* GPIO_H_ */
