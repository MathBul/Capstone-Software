#ifndef PORTPINS_H_
#define PORTPINS_H_

#include "msp432e401y.h"

// Stepper 1 (x-axis)
#define STEPPER_X_ENABLE_PORT               GPIOD
#define STEPPER_X_ENABLE_PIN                (1 << 2)
#define STEPPER_X_MS1_PORT                  GPIOD
#define STEPPER_X_MS1_PIN                   (1 << 0)
#define STEPPER_X_MS2_PORT                  GPIOP
#define STEPPER_X_MS2_PIN                   (1 << 1)
#define STEPPER_X_MS3_PORT                  GPIOQ
#define STEPPER_X_MS3_PIN                   (1 << 0)
#define STEPPER_X_STEP_PORT                 GPIOP
#define STEPPER_X_STEP_PIN                  (1 << 4)
#define STEPPER_X_DIR_PORT                  GPION
#define STEPPER_X_DIR_PIN                   (1 << 5)

#endif /* PORTPINS_H_ */
