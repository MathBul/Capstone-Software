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
#include "clock.h"
#include "gpio.h"
#include "utils.h"
#include <stdint.h>

// General sensor defines
#define NUMBER_OF_ROWS_COLS                 (8)
#define NUMBER_OF_SENSOR_ROW_SELECTS        (3)
#define NUMBER_OF_SENSOR_COL_SELECTS        (3)
#define NUMBER_OF_DATA_LINES                (1)
#define SENSOR_NETWORK_TIMER                (TIMER6)
#define SENSOR_NETWORK_HANDLER              (TIMER6A_IRQHandler)

// Sensor rows
#define SENSOR_ROW_SELECT_1_PORT            (GPIOD)
#define SENSOR_ROW_SELECT_1_PIN             (GPIO_PIN_1)
#define SENSOR_ROW_SELECT_2_PORT            (GPIOD)
#define SENSOR_ROW_SELECT_2_PIN             (GPIO_PIN_0)
#define SENSOR_ROW_SELECT_3_PORT            (GPION)
#define SENSOR_ROW_SELECT_3_PIN             (GPIO_PIN_2)

// Sensor cols
#define SENSOR_COL_SELECT_1_PORT            (GPIOM)
#define SENSOR_COL_SELECT_1_PIN             (GPIO_PIN_7)
#define SENSOR_COL_SELECT_2_PORT            (GPIOP)
#define SENSOR_COL_SELECT_2_PIN             (GPIO_PIN_5)
#define SENSOR_COL_SELECT_3_PORT            (GPIOA)
#define SENSOR_COL_SELECT_3_PIN             (GPIO_PIN_7)

// Data line(s)
#define SENSOR_DATA_PORT                    (GPIOG)
#define SENSOR_DATA_PIN                     (GPIO_PIN_1)

// Tile macros
#define TILE_A1                            (BITS64_MASK(0))
#define TILE_B1                            (BITS64_MASK(1))
#define TILE_C1                            (BITS64_MASK(2))
#define TILE_D1                            (BITS64_MASK(3))
#define TILE_E1                            (BITS64_MASK(4))
#define TILE_F1                            (BITS64_MASK(5))
#define TILE_G1                            (BITS64_MASK(6))
#define TILE_H1                            (BITS64_MASK(7))
#define TILE_A2                            (BITS64_MASK(8))
#define TILE_B2                            (BITS64_MASK(9))
#define TILE_C2                            (BITS64_MASK(10))
#define TILE_D2                            (BITS64_MASK(11))
#define TILE_E2                            (BITS64_MASK(12))
#define TILE_F2                            (BITS64_MASK(13))
#define TILE_G2                            (BITS64_MASK(14))
#define TILE_H2                            (BITS64_MASK(15))
#define TILE_A3                            (BITS64_MASK(16))
#define TILE_B3                            (BITS64_MASK(17))
#define TILE_C3                            (BITS64_MASK(18))
#define TILE_D3                            (BITS64_MASK(19))
#define TILE_E3                            (BITS64_MASK(20))
#define TILE_F3                            (BITS64_MASK(21))
#define TILE_G3                            (BITS64_MASK(22))
#define TILE_H3                            (BITS64_MASK(23))
#define TILE_A4                            (BITS64_MASK(24))
#define TILE_B4                            (BITS64_MASK(25))
#define TILE_C4                            (BITS64_MASK(26))
#define TILE_D4                            (BITS64_MASK(27))
#define TILE_E4                            (BITS64_MASK(28))
#define TILE_F4                            (BITS64_MASK(29))
#define TILE_G4                            (BITS64_MASK(30))
#define TILE_H4                            (BITS64_MASK(31))
#define TILE_A5                            (BITS64_MASK(32))
#define TILE_B5                            (BITS64_MASK(33))
#define TILE_C5                            (BITS64_MASK(34))
#define TILE_D5                            (BITS64_MASK(35))
#define TILE_E5                            (BITS64_MASK(36))
#define TILE_F5                            (BITS64_MASK(37))
#define TILE_G5                            (BITS64_MASK(38))
#define TILE_H5                            (BITS64_MASK(39))
#define TILE_A6                            (BITS64_MASK(40))
#define TILE_B6                            (BITS64_MASK(41))
#define TILE_C6                            (BITS64_MASK(42))
#define TILE_D6                            (BITS64_MASK(43))
#define TILE_E6                            (BITS64_MASK(44))
#define TILE_F6                            (BITS64_MASK(45))
#define TILE_G6                            (BITS64_MASK(46))
#define TILE_H6                            (BITS64_MASK(47))
#define TILE_A7                            (BITS64_MASK(48))
#define TILE_B7                            (BITS64_MASK(49))
#define TILE_C7                            (BITS64_MASK(50))
#define TILE_D7                            (BITS64_MASK(51))
#define TILE_E7                            (BITS64_MASK(52))
#define TILE_F7                            (BITS64_MASK(53))
#define TILE_G7                            (BITS64_MASK(54))
#define TILE_H7                            (BITS64_MASK(55))
#define TILE_A8                            (BITS64_MASK(56))
#define TILE_B8                            (BITS64_MASK(57))
#define TILE_C8                            (BITS64_MASK(58))
#define TILE_D8                            (BITS64_MASK(59))
#define TILE_E8                            (BITS64_MASK(60))
#define TILE_F8                            (BITS64_MASK(61))
#define TILE_G8                            (BITS64_MASK(62))
#define TILE_H8                            (BITS64_MASK(63))

// Select line struct
typedef struct {
    GPIO_Type* line_port;     // Port for the sensor
    uint8_t    line_pin;      // Pin for the sensor
} sensor_line_t;

// Local representation of the sensors
typedef struct {
    uint64_t current_inputs;
    uint64_t edges;
    uint64_t pos_transitions;
    uint64_t neg_transitions;
    uint64_t previous_inputs;
} sensor_state_t;

// Virtual port for the sensor network
union utils_vport64_t sensor_vport;

// Public functions
void sensors_init();
uint64_t sensor_get_reading();

#endif /* SENSORNETWORK_H_ */
