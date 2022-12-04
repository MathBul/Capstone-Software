/**
 * @file sensornetwork.c
 * @author Nick Cooney (npc4crc@virginia.edu)
 * @brief Provides functions for the reed switch sensor network
 * @version 0.1
 * @date 2022-10-09
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef SENSORNETWORK_H_
#define SENSORNETWORK_H_

// Note on sensor network:
//  - Assumes a multiplexed crosspoint array
//  - Sends signals on the rows, reads on the columns

#include "msp.h"
#include "clock.h"
#include "gpio.h"
#include "utils.h"
#include <stdint.h>

// General sensor defines
#define NUMBER_OF_ROWS                      (8)
#define NUMBER_OF_COLS                      (8)
#define NUMBER_OF_SENSOR_ROW_SELECTS        (3)
#define NUMBER_OF_SENSOR_COL_SELECTS        (3)
#define SENSOR_NETWORK_TIMER                (TIMER6)
#define SENSOR_NETWORK_HANDLER              (TIMER6A_IRQHandler)

// Sensor rows
#define SENSOR_ROW_SELECT_0_PORT            (GPIOD)
#define SENSOR_ROW_SELECT_0_PIN             (GPIO_PIN_1)
#define SENSOR_ROW_SELECT_1_PORT            (GPIOD)
#define SENSOR_ROW_SELECT_1_PIN             (GPIO_PIN_0)
#define SENSOR_ROW_SELECT_2_PORT            (GPION)
#define SENSOR_ROW_SELECT_2_PIN             (GPIO_PIN_2)

// Sensor cols
#define SENSOR_COL_SELECT_0_PORT            (GPIOA)
#define SENSOR_COL_SELECT_0_PIN             (GPIO_PIN_7)
#define SENSOR_COL_SELECT_1_PORT            (GPIOP)
#define SENSOR_COL_SELECT_1_PIN             (GPIO_PIN_5)
#define SENSOR_COL_SELECT_2_PORT            (GPIOM)
#define SENSOR_COL_SELECT_2_PIN             (GPIO_PIN_7)

// Data line(s)
#define SENSOR_DATA_PORT                    (GPIOG)
#define SENSOR_DATA_PIN                     (GPIO_PIN_1)

// Sensor line struct (used for select and data lines)
typedef struct {
    GPIO_Type* line_port;     // Port for the sensor
    uint8_t    line_pin;      // Pin for the sensor
} sensornetwork_line_t;

// Local representation of the sensors
typedef struct {
    uint64_t current_inputs;
    uint64_t edges;
    uint64_t pos_transitions;
    uint64_t neg_transitions;
    uint64_t previous_inputs;
} sensornetwork_state_t;

// Virtual port for the sensor network
union utils_vport64_t sensor_vport;

// Public functions
void sensornetwork_init(void);
uint64_t sensornetwork_get_reading(void);

#endif /* SENSORNETWORK_H_ */
