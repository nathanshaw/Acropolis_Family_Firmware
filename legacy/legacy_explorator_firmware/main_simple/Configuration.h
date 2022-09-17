#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include "Configuration_adv.h"

#define USB_OUTPUT       1

#define PRINT_BRIGHTNESS 0
#define PRINT_COLOR      0

// the direction of the motors, if one is spinning in the wrong way then change this value
#define M1_POLARITY           false
#define M2_POLARITY           false
#define M3_POLARITY           false

// this value can be used to determine how long and how fast to spin each motor...
#define M1_GEAR_RATIO         1
#define M2_GEAR_RATIO         1
#define M3_GEAR_RATIO         1

// the max and min values for the solenoids corrisponds to the lowest value which
// results in an audible sounds from the actuator
// the max is the value which produces the loudest sonic event from the solenoid
// any values higher than the max will not produce a louder sound
// These values should be tested and set within this file once determined in-situ

#define S1_MIN                10
#define S1_MAX                100
#define S2_MIN                10
#define S2_MAX                100
#define S3_MIN                10
#define S3_MAX                100
#define S4_MIN                10
#define S4_MAX                100
#define S5_MIN                10
#define S5_MAX                100
#define S6_MIN                10
#define S6_MAX                100
#define S7_MIN                10
#define S7_MAX                100
#define S8_MIN                10
#define S8_MAX                100
#define S9_MIN                10
#define S9_MAX                100

const int sol_min[9] = {S1_MIN, S2_MIN, S3_MIN, S4_MIN, S5_MIN, S6_MIN, S7_MIN, S8_MIN, S9_MIN};
const int sol_max[9] = {S1_MAX, S2_MAX, S3_MAX, S4_MAX, S5_MAX, S6_MAX, S7_MAX, S8_MAX, S9_MAX};

// if there are three motors put 3 here, if there are none then the code will not
// compile the motor libraries and other things.
#define MOTORS_ACTIVE         3

#define SHT_PRESENT           1
// this bool is set to true is a SHT sensor is detected and is giving good readings
bool SHT_ACTIVE           =   false;

#define LUX_PRESENT           0
bool LUX_ACTIVE =             false;

#define LEDS_ACTIVE           3

#define BELL_BOT              0
#define WOODPECKER_BOT        1
#define ACTUATOR_CONF         WOODPECKER_BOT

#define MAX_BRIGHTNESS        255

// Which pin on the Arduino is connected to the NeoPixels? 8 for old board
#define LED1_PIN 5
// note that if the second LED channel is used the teensy needs to be overclocked to 120 MHz
#define LED2_PIN 8
#define LED3_PIN 10

// what NEOpixel channel is the Eyestock connected to? (default is 0)
#define EYE_STOCK_LEDS         0

// How many NeoPixels are attached to the Arduino?

#define LED1_COUNT 10
#define LED2_COUNT 0
#define LED3_COUNT 0


// this is how long in MS to wait after the dampener is removed before striking with solenoid
#define DAMPENER_DELAY 10

#if PCB_REVISION == 0
#define SOL1_PIN 12
#define SOL2_PIN 11
#define SOL3_PIN 7
#define SOL4_PIN 6
#define SOL5_PIN 4
#define SOL6_PIN 3
#define NUM_SOLENOIDS 6
#define NUM_MOTORS    1

#define POT_PIN  14
#define NUM_POTS 1

#elif PCB_REVISION ==   1
// solenoid outputs
#define SOL1_PIN        3
#define SOL2_PIN        4
#define SOL3_PIN        6
#define SOL4_PIN        12
#define SOL5_PIN        11
#define SOL6_PIN        14
#define SOL7_PIN        15
#define SOL8_PIN        16
#define SOL9_PIN        7
#define NUM_SOLENOIDS   9

// motor outputs
#define NUM_MOTORS      3

#define M1_COAST        28
#define M1_SPEED        25
#define M1_DIR          26
#define M1_FAULT        27

#define M2_COAST        31
#define M2_SPEED        32
#define M2_DIR          29
#define M2_FAULT        30

#define M3_COAST        2
#define M3_SPEED        20
#define M3_DIR          21
#define M3_FAULT        22

#define MAX_BUTTONS     4
#define NUM_BUTTONS     3
#define BUT1            33
#define BUT2            A13
#define BUT3            A12
#define BUT4            17
const int but_pins [4] = {BUT1, BUT2, BUT3, BUT4};
bool but_active[4] = {true, true, true, true};

#define MAX_POTS        2
#define NUM_POTS        2
#define POT1            A11
#define POT2            A10
const int pot_pins[2] = {POT1, POT2};
bool pot_active[2] = {true, true};

//////////////////////////////////////////////////////////////////////////
///////////////////  Audio / Microphones  ////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#define MICROPHONE_PRESENT true
bool MICROPHONE_ACTIVE = false;

//////////////////////////////////////////////////////////////////////////
///////////////////  Solenoids/actuators  ////////////////////////////////
//////////////////////////////////////////////////////////////////////////
const int s_pins[] = {SOL1_PIN, SOL2_PIN, SOL3_PIN, SOL4_PIN, SOL5_PIN, SOL6_PIN, SOL7_PIN, SOL8_PIN, SOL9_PIN};
uint16_t sol_on_time[] = {30, 30, 30, 30, 30, 30, 30, 30, 30};
bool sol_state[] = {false, false, false, false, false, false, false, false, false}; // is the solenoid on or off
bool sol_active[] = {false, false, false, false, false, false, false, false, false};
unsigned long sol_timers[9];
elapsedMillis last_sol_action[9];

// the current speed of the motors (TODO - what is the range of this?)
int motor_speed[] = {0, 0, 0};
// if the motor drivers are recognised and the actuator configuration needs them, these valuees will be set to true
bool motor_active[] = {false, false, false};

// time in ms in which the solenoids need to cool down between individual actuations
#define SOL_COOLDOWN 10
// the number of active solenoid channels, 4 for boards 1.1 & 1.0 6 for rev 2.0
#define NUM_SOL_CHANNELS 9

#endif /// PCB_REVISION

#endif
