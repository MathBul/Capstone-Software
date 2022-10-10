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

// Assumes a multiplexed crosspoint array
//  Sends signals on the rows, reads on the columns

#include "msp.h"
#include "gpio.h"
#include <stdint.h>

// General sensor defines
#define NUMBER_OF_SENSOR_ROW_SELECTS        (3)
#define NUMBER_OF_SENSOR_COL_SELECTS        (3)

// Sensor rows
#define SENSOR_ROW_SELECT_1_PORT            (GPIOD)         // TODO: Update when known
#define SENSOR_ROW_SELECT_1_PIN             (GPIO_PIN_0)    // TODO: Update when known
#define SENSOR_ROW_SELECT_2_PORT            (GPIOD)         // TODO: Update when known
#define SENSOR_ROW_SELECT_2_PIN             (GPIO_PIN_1)    // TODO: Update when known
#define SENSOR_ROW_SELECT_3_PORT            (GPIOD)         // TODO: Update when known
#define SENSOR_ROW_SELECT_3_PIN             (GPIO_PIN_2)    // TODO: Update when known
#define SENSOR_ROW_DATA_PORT                (GPIOD)         // TODO: Update when known
#define SENSOR_ROW_DATA_PIN                 (GPIO_PIN_3)    // TODO: Update when known

// Sensor cols
#define SENSOR_COL_SELECT_1_PORT            (GPIOP)         // TODO: Update when known
#define SENSOR_COL_SELECT_1_PIN             (GPIO_PIN_0)    // TODO: Update when known
#define SENSOR_COL_SELECT_2_PORT            (GPIOP)         // TODO: Update when known
#define SENSOR_COL_SELECT_2_PIN             (GPIO_PIN_1)    // TODO: Update when known
#define SENSOR_COL_SELECT_3_PORT            (GPIOP)         // TODO: Update when known
#define SENSOR_COL_SELECT_3_PIN             (GPIO_PIN_2)    // TODO: Update when known
#define SENSOR_COL_DATA_PORT                (GPIOP)         // TODO: Update when known
#define SENSOR_COL_DATA_PIN                 (GPIO_PIN_3)    // TODO: Update when known

// Status of a given tile (piece present/absent)
typedef enum {
    present, absent
} sensor_status_t;

// Location of a sensor in terms of chess tiles
typedef enum {
    row_1, row_2, row_3, row_4, row_5, row_6, row_7, row_8,
    col_a, col_b, col_c, col_d, col_e, col_f, col_g, col_h
} sensor_pos_t;

// Sensor cluster struct
typedef struct {
    GPIO_Type* line_port;     // Port for the sensor
    uint8_t    line_pin;      // Pin for the sensor
} sensor_line_t;

// Public functions
void sensors_init();
sensor_status_t sensors_read_tile(sensor_pos_t row, sensor_pos_t col);

#endif /* SENSORNETWORK_H_ */
