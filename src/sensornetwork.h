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

// Sensor cols
#define SENSOR_COL_SELECT_0_PORT            (GPIOD)
#define SENSOR_COL_SELECT_0_PIN             (GPIO_PIN_1)
#define SENSOR_COL_SELECT_1_PORT            (GPIOD)
#define SENSOR_COL_SELECT_1_PIN             (GPIO_PIN_0)
#define SENSOR_COL_SELECT_2_PORT            (GPION)
#define SENSOR_COL_SELECT_2_PIN             (GPIO_PIN_2)

// Sensor rows
#define SENSOR_ROW_DATA_1_PORT              (GPIOH)
#define SENSOR_ROW_DATA_1_PIN               (GPIO_PIN_2)
#define SENSOR_ROW_DATA_2_PORT              (GPIOL)
#define SENSOR_ROW_DATA_2_PIN               (GPIO_PIN_3)
#define SENSOR_ROW_DATA_3_PORT              (GPIOL)
#define SENSOR_ROW_DATA_3_PIN               (GPIO_PIN_2)
#define SENSOR_ROW_DATA_4_PORT              (GPIOH)
#define SENSOR_ROW_DATA_4_PIN               (GPIO_PIN_3)
#define SENSOR_ROW_DATA_5_PORT              (GPIOL)
#define SENSOR_ROW_DATA_5_PIN               (GPIO_PIN_4)
#define SENSOR_ROW_DATA_6_PORT              (GPIOL)
#define SENSOR_ROW_DATA_6_PIN               (GPIO_PIN_1)
#define SENSOR_ROW_DATA_7_PORT              (GPIOL)
#define SENSOR_ROW_DATA_7_PIN               (GPIO_PIN_0)
#define SENSOR_ROW_DATA_8_PORT              (GPIOL)
#define SENSOR_ROW_DATA_8_PIN               (GPIO_PIN_5)

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
