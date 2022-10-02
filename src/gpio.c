#include "gpio.h"


/* GPIO Output Functions */

/*
 * Sets the specified GPIO port/pin as an output, then resets the output value
 */
void gpioSetAsOutput(GPIO_Type* Port, uint8_t Pin)
{
    Port->DIR |= Pin;
    Port->DEN |= Pin;
    Port->DATA &= ~Pin;
//    MAP_SysCtlPeripheralEnable(sysctl);
}

/*
 * Sets the output of the specified GPIO port/pin as high
 */
void gpioSetOutputHigh(GPIO_Type* Port, uint8_t Pin)
{
    Port->DATA |= Pin;
}

/*
 * Sets the output of the specified GPIO port/pin as low
 */
void gpioSetOutputLow(GPIO_Type* Port, uint8_t Pin)
{
    Port->DATA &= ~Pin;
}

/*
 * Toggle the output of the specified GPIO port/pin
 */
void gpioSetOutputToggle(GPIO_Type* Port, uint8_t Pin)
{
    Port->DATA ^= Pin;
}

/* GPIO Input Functions */

/*
 * Sets the specified GPIO port/pin as an output, then resets the output value
 */
void gpioSetAsInput(GPIO_Type* Port, uint8_t Pin)
{
    Port->DIR &= ~Pin;
    Port->DEN |= Pin;
//    MAP_SysCtlPeripheralEnable(sysctl);
}

// End gpio.c
