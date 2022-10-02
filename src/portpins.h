#ifndef PORTPINS_H_
#define PORTPINS_H_

#include "msp432e401y.h"
#include "driverlib/sysctl.h"

// Stepper 1 (x-axis)
#define STEPPER_1_ENABLE_PORT               GPIOD
#define STEPPER_1_ENABLE_PIN                (1 << 2)
#define STEPPER_1_ENABLE_SYSCTL             SYSCTL_PERIPH_GPIOD
#define STEPPER_1_MS1_PORT                  GPIOD
#define STEPPER_1_MS1_PIN                   (1 << 0)
#define STEPPER_1_MS1_SYSCTL                SYSCTL_PERIPH_GPIOD
#define STEPPER_1_MS2_PORT                  GPIOP
#define STEPPER_1_MS2_PIN                   (1 << 1)
#define STEPPER_1_MS2_SYSCTL                SYSCTL_PERIPH_GPIOP
#define STEPPER_1_MS3_PORT                  GPIOQ
#define STEPPER_1_MS3_PIN                   (1 << 0)
#define STEPPER_1_MS3_SYSCTL                SYSCTL_PERIPH_GPIOQ
#define STEPPER_1_STEP_PORT                 GPIOP
#define STEPPER_1_STEP_PIN                  (1 << 4)
#define STEPPER_1_STEP_SYSCTL               SYSCTL_PERIPH_GPIOP
#define STEPPER_1_DIR_PORT                  GPION
#define STEPPER_1_DIR_PIN                   (1 << 5)
#define STEPPER_1_DIR_SYSCTL                SYSCTL_PERIPH_GPION


#endif /* PORTPINS_H_ */
