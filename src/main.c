// Project-specific source code
#include "steppermotors.h"
#include "portpins.h"
#include "defines.h"

// Driver library and standard includes necessary for base functionality
//#include "driverlib/sysctl.h"
#include "driverlib/rom_map.h"
#include <stdint.h>

StepperMotorType StepperMotors[NUMBER_OF_STEPPER_MOTORS];
StepperMotorType* StepperMotorX = &StepperMotors[0];
uint8_t counter = 0;
uint8_t parity = 1;

int main(void)
{
    // Configure the system clock to 120MHZ
    uint32_t systemClock;
    systemClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

    // Enable the clock on the necessary ports (TODO: Add to GPIO methods)
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOQ);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    while(!(SysCtlPeripheralReady(SYSCTL_PERIPH_GPION)))
    {
    }

    // Initialize the stepper motor(s)
    initializeStepperMotor(StepperMotorX, STEPPER_1_ID);
    goToPosition(StepperMotorX, ColA, Row1, 0, 1);

    // Enable the SysTick timer to generate an interrupt every [1/12?] second
    MAP_SysTickPeriodSet(systemClock/1000);
    MAP_SysTickIntEnable();
    MAP_SysTickEnable();

    // This program drivers the arm back and forth, spreading farther from the center each time
    while(1)
    {
        if ((StepperMotorX->CurrentState == Disabled) && (counter < 5)) {
            goToPosition(StepperMotorX, ColA, Row1, counter, parity);
            counter += 1;
            parity ^= 1;
        }
    }
}

/*
 * Interrupt handler to check if the desired number of steps are complete
 */
void STEPPER_HANDLER(void)
{
    if (StepperMotorX->HalfStepsToDesired > 0) {
        halfStep(StepperMotorX);
        StepperMotorX->HalfStepsToDesired -= 1;
    } else {
        StepperMotorX->CurrentState = Disabled;
    }
}

// End main.c
