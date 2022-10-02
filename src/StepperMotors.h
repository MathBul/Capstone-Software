#ifndef STEPPER_MOTORS_H_
#define STEPPER_MOTORS_H_

// Information on whether the stepper motor is enabled or disabled
typedef enum {
    Disabled, Enabled
} StepperMotorState;

// Information on whether the stepper motor is rotating clockise or counterclockwise
typedef enum {
    Clockwise, Counterclockwise
} StepperMotorDirection;

// Information on which column the arm is current above
typedef enum {
    ColA, ColB, ColC, ColD, ColE, ColF, ColG, ColH
} ArmXPosition;

// Information on which row the arm is currently above
typedef enum {
    Row1, Row2, Row3, Row4, Row5, Row6, Row7, Row8
} ArmYPosition;

// Stepper motor structure
typedef struct {
    uint8_t                 MotorID;                // ID for this stepper motor
    GPIO_Type*              DirPort;                // Port used to set the direction
    uint8_t                 DirPin;                 // Pin used to set the direction
    GPIO_Type*              StepPort;               // Port used to step the motor
    uint8_t                 StepPin;                // Pin used to step the motor
    GPIO_Type*              EnablePort;             // Port used to enable/disable
    uint8_t                 EnablePin;              // Pin used to enable/disable the motor
    StepperMotorState       CurrentState;           // Whether the motor is enabled/disabled
    ArmXPosition            CurrentXPosition;       // The current x-position of the motor in terms of board tiles
    ArmYPosition            CurrentYPosition;       // The current y-position of the motor in terms of board tiles
    ArmXPosition            DesiredXPosition;       // The desired x-position of the motor in terms of board tiles
    ArmYPosition            DesiredYPosition;       // The desired y-position of the motor in terms of board tiles
    uint32_t                HalfStepsToDesired;     // The number of steps required to reach the desired position
} StepperMotorType;

// Stepper motor function declarations
void initializeStepperXGPIO(void);
void initializeStepperMotor(StepperMotorType *StepperMotor, uint8_t MotorID);
void toggleDirection(StepperMotorType *StepperMotor);
void halfStep(StepperMotorType *StepperMotor);
void setDirectionClockwise(StepperMotorType *StepperMotor);
void setDirectionCounterclockwise(StepperMotorType *StepperMotor);
void goToPosition(StepperMotorType *StepperMotor, ArmXPosition desiredXPosition, ArmYPosition desiredYPosition, uint8_t tempNumTiles, uint8_t forward);

#endif /* STEPPER_MOTORS_H_ */
