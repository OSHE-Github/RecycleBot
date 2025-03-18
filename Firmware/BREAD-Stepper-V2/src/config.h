#ifndef CONFIG_H
#define CONFIG_H

#include "BREAD_CAN_Slices.h"

//???????????????????????
#define NUM_DCMT_SLICES 8
#define NUM_RLHT_SLICES 8
#define NUM_PHDO_SLICES 2
//???????????????????????

// ********************
// * Universal Macros *
// ********************

#define ESTOP1  36
#define SYNC    15

// Analog Feedback
#define AG_1    32
#define AG_2    33
#define AG_3    25
#define AG_4    26

// Stepper Motor Driver Pins
#define MS1     27 // +---------------+
#define MS2     14 // | Microstepping |
#define MS3     12 // +---------------+
#define ST_1    19 // +-----------+
#define DR_1    18 // |           |
#define ST_2    5  // |  Stepper  |
#define DR_2    17 // |   Motor   |
#define ST_3    16 // |  Step &   |
#define DR_3    4  // | Direction |
#define ST_4    0  // |           |
#define DR_4    2  // +-----------+

// ***************************
// * Loaf V2 Specific Macros *
// ***************************

// Misc Pins
#define ESTOP2      39
#define SLICE_LED   15

// CAN
#define SLICE_DATA_INTERVAL_MS 1000 // CAN Bus Data Interval
#define CAN_TX	35
#define CAN_RX  34

// ************************************
// * Loaf V1 (Legacy) Specific Macros *
// ************************************

// I2C
#define SDA     21
#define SCL     22

#endif
