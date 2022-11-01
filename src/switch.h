/**
 * @file switch.h
 * @author Nick Cooney (npc4crc@virginia.edu)
 * @brief Provides functions for all peripheral switches (limit, buttons, rocker, etc.)
 * @version 0.1
 * @date 2022-10-09
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef SWITCHES_H_
#define SWITCHES_H_

// Note on use: 
//  - Assumes all switches are on the same physical port
//  - Creates a virtual port to access the physical port via imaging
//  - The SWITCH_HANDLER reads the virtual port and maps it to a local bitfield
//  - To move switches to different GPIO, change the GPIO macros below, no other changes required
// ISR defined in gantry.c

#include "msp.h"
#include "gpio.h"
#include "utils.h"
#include "clock.h"
#include <stdint.h>

// Switch GPIO macros
#define SWITCH_PORT                         (GPIOD)         // TODO: Update when known
#define BUTTON_RESET_PIN                    (GPIO_PIN_0)    // TODO: Update when known
#define BUTTON_END_TURN_PIN                 (GPIO_PIN_1)    // TODO: Update when known
#define BUTTON_ESTOP_PIN                    (GPIO_PIN_2)    // TODO: Update when known
#define BUTTON_HOME_PIN                     (GPIO_PIN_3)    // TODO: Update when known
#define LIMIT_X_PIN                         (GPIO_PIN_4)    // TODO: Update when known
#define LIMIT_Y_PIN                         (GPIO_PIN_5)    // TODO: Update when known
#define LIMIT_Z_PIN                         (GPIO_PIN_6)    // TODO: Update when known
#define ROCKER_COLOR_PIN                    (GPIO_PIN_7)    // TODO: Update when known
#define BUTTON_PIN_MASK                     (BUTTON_RESET_PIN | BUTTON_END_TURN_PIN | BUTTON_ESTOP_PIN | BUTTON_HOME_PIN)
#define LIMIT_PIN_MASK                      (LIMIT_X_PIN | LIMIT_Y_PIN | LIMIT_Z_PIN)
#define ROCKER_PIN_MASK                     (ROCKER_COLOR_PIN)

// General switch macros
#define SWITCH_TIMER                        (TIMER3)
#define SWITCH_HANDLER                      (TIMER3A_IRQHandler)
#define SWITCH_MASK                         (BUTTON_PIN_MASK | LIMIT_PIN_MASK | ROCKER_PIN_MASK)

// Button macros
#define BUTTON_START_RESET                  (BITS8_MASK(0))
#define BUTTON_ESTOP                        (BITS8_MASK(1))
#define BUTTON_HOME                         (BITS8_MASK(2))
#define BUTTON_END_TURN                     (BITS8_MASK(3))

// Limit switch macros
#define LIMIT_X                             (BITS8_MASK(4))
#define LIMIT_Y                             (BITS8_MASK(5))
#define LIMIT_Z                             (BITS8_MASK(6))

// Rocker macros
#define ROCKER_COLOR                        (BITS8_MASK(7))

typedef struct port_map_t {
    uint8_t reserved_0 : 1;         // PA.0
    uint8_t reserved_1 : 1;         // PA.1
    uint8_t reserved_2 : 1;         // PA.2
    uint8_t reserved_3 : 1;         // PA.3
    uint8_t reserved_4 : 1;         // PA.4
    uint8_t reserved_5 : 1;         // PA.5
    uint8_t reserved_6 : 1;         // PA.6
    uint8_t reserved_7 : 1;         // PA.7

    uint8_t reserved_8 : 1;         // PB.0
    uint8_t reserved_9 : 1;         // PB.1
    uint8_t reserved_10 : 1;        // PB.2
    uint8_t reserved_11 : 1;        // PB.3
    uint8_t reserved_12 : 1;        // PB.4
    uint8_t reserved_13 : 1;        // PB.5
    uint8_t reserved_14 : 1;        // PB.6
    uint8_t reserved_15 : 1;        // PB.7

    uint8_t reserved_16 : 1;        // PC.0
    uint8_t reserved_17 : 1;        // PC.1
    uint8_t reserved_18 : 1;        // PC.2
    uint8_t reserved_19 : 1;        // PC.3
    uint8_t reserved_20 : 1;        // PC.4
    uint8_t reserved_21 : 1;        // PC.5
    uint8_t reserved_22 : 1;        // PC.6
    uint8_t reserved_23 : 1;        // PC.7

    uint8_t reserved_24 : 1;        // PD.0
    uint8_t reserved_25 : 1;        // PD.1
    uint8_t reserved_26 : 1;        // PD.2
    uint8_t reserved_27 : 1;        // PD.3
    uint8_t reserved_28 : 1;        // PD.4
    uint8_t reserved_29 : 1;        // PD.5
    uint8_t reserved_30 : 1;        // PD.6
    uint8_t reserved_31 : 1;        // PD.7

    uint8_t reserved_32 : 1;        // PE.0
    uint8_t reserved_33 : 1;        // PE.1
    uint8_t reserved_34 : 1;        // PE.2
    uint8_t reserved_35 : 1;        // PE.3
    uint8_t reserved_36 : 1;        // PE.4
    uint8_t reserved_37 : 1;        // PE.5
    uint8_t reserved_38 : 1;        // PE.6
    uint8_t reserved_39 : 1;        // PE.7

    uint8_t reserved_40 : 1;        // PF.0
    uint8_t reserved_41 : 1;        // PF.1
    uint8_t reserved_42 : 1;        // PF.2
    uint8_t reserved_43 : 1;        // PF.3
    uint8_t reserved_44 : 1;        // PF.4
    uint8_t reserved_45 : 1;        // PF.5
    uint8_t reserved_46 : 1;        // PF.6
    uint8_t reserved_47 : 1;        // PF.7

    uint8_t reserved_48 : 1;        // PG.0
    uint8_t reserved_49 : 1;        // PG.1
    uint8_t reserved_50 : 1;        // PG.2
    uint8_t reserved_51 : 1;        // PG.3
    uint8_t reserved_52 : 1;        // PG.4
    uint8_t reserved_53 : 1;        // PG.5
    uint8_t reserved_54 : 1;        // PG.6
    uint8_t reserved_55 : 1;        // PG.7

    uint8_t reserved_56 : 1;        // PH.0
    uint8_t reserved_57 : 1;        // PH.1
    uint8_t reserved_58 : 1;        // PH.2
    uint8_t reserved_59 : 1;        // PH.3
    uint8_t reserved_60 : 1;        // PH.4
    uint8_t reserved_61 : 1;        // PH.5
    uint8_t reserved_62 : 1;        // PH.6
    uint8_t reserved_63 : 1;        // PH.7

    uint8_t reserved_64 : 1;        // PJ.0
    uint8_t reserved_65 : 1;        // PJ.1
    uint8_t reserved_66 : 1;        // PJ.2
    uint8_t reserved_67 : 1;        // PJ.3
    uint8_t reserved_68 : 1;        // PJ.4
    uint8_t reserved_69 : 1;        // PJ.5
    uint8_t reserved_70 : 1;        // PJ.6
    uint8_t reserved_71 : 1;        // PJ.7

    uint8_t reserved_72 : 1;        // PK.0
    uint8_t reserved_73 : 1;        // PK.1
    uint8_t reserved_74 : 1;        // PK.2
    uint8_t reserved_75 : 1;        // PK.3
    uint8_t reserved_76 : 1;        // PK.4
    uint8_t reserved_77 : 1;        // PK.5
    uint8_t reserved_78 : 1;        // PK.6
    uint8_t reserved_79 : 1;        // PK.7

    uint8_t reserved_80 : 1;        // PL.0
    uint8_t reserved_81 : 1;        // PL.1
    uint8_t reserved_82 : 1;        // PL.2
    uint8_t reserved_83 : 1;        // PL.3
    uint8_t reserved_84 : 1;        // PL.4
    uint8_t reserved_85 : 1;        // PL.5
    uint8_t reserved_86 : 1;        // PL.6
    uint8_t reserved_87 : 1;        // PL.7

    uint8_t reserved_88 : 1;        // PM.0
    uint8_t reserved_89 : 1;        // PM.1
    uint8_t reserved_90 : 1;        // PM.2
    uint8_t reserved_91 : 1;        // PM.3
    uint8_t reserved_92 : 1;        // PM.4
    uint8_t reserved_93 : 1;        // PM.5
    uint8_t reserved_94 : 1;        // PM.6
    uint8_t reserved_95 : 1;        // PM.7

    uint8_t reserved_96 : 1;        // PN.0
    uint8_t reserved_97 : 1;        // PN.1
    uint8_t reserved_98 : 1;        // PN.2
    uint8_t reserved_99 : 1;        // PN.3
    uint8_t reserved_100 : 1;       // PN.4
    uint8_t reserved_101 : 1;       // PN.5
    uint8_t reserved_102 : 1;       // PN.6
    uint8_t reserved_103 : 1;       // PN.7

    uint8_t reserved_104 : 1;       // PP.0
    uint8_t reserved_105 : 1;       // PP.1
    uint8_t reserved_106 : 1;       // PP.2
    uint8_t reserved_107 : 1;       // PP.3
    uint8_t reserved_108 : 1;       // PP.4
    uint8_t reserved_109 : 1;       // PP.5
    uint8_t reserved_110 : 1;       // PP.6
    uint8_t reserved_111 : 1;       // PP.7

    uint8_t reserved_112 : 1;       // PQ.0
    uint8_t reserved_113 : 1;       // PQ.1
    uint8_t reserved_114 : 1;       // PQ.2
    uint8_t reserved_115 : 1;       // PQ.3
    uint8_t reserved_116 : 1;       // PQ.4
    uint8_t reserved_117 : 1;       // PQ.5
    uint8_t reserved_118 : 1;       // PQ.6
    uint8_t reserved_119 : 1;       // PQ.7
}port_map_t;

typedef struct image_t {
    uint64_t alpha : 64;
    uint64_t beta : 56;
} image_t;

// Storage for the switches

typedef union switches_t {
    port_map_t port_map;
    image_t image;
} switches_t;

// Local representation of the switches
typedef struct {
    uint8_t current_inputs;
    uint8_t edges;
    uint8_t pos_transitions;
    uint8_t neg_transitions;
    uint8_t previous_inputs;
    char    color_latch;
} switch_state_t;

// Virtual port for the switches
union utils_vport_t switch_vport;

// Public functions
void switch_init();

#endif /* SWITCHES_H_ */
