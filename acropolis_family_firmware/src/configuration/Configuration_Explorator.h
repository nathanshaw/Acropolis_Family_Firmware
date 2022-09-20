#ifndef __CONFIGURATION_EXPLORATOR_H__
#define __CONFIGURATION_EXPLORATOR_H__

// should correspond to the serial number on the PCB
#define SERIAL_ID                       13

///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// H-Bridge Motors  ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

// for encoders
#define COUNTS_PER_REV        12

/////////////////// for the bell bot which should not use any of the H-bridge motors
#if HV_MAJOR == 0
#define NUM_MOTORS            0

#define MOT1_ACTIVE           false
#define MOT1_DIR_PIN          26
#define MOT1_PWM_PIN          25
#define MOT1_EN_PIN           28
#define MOT1_FAULT_PIN        27

#define MOT2_ACTIVE           false
#define MOT2_DIR_PIN          29
#define MOT2_PWM_PIN          32
#define MOT2_EN_PIN           31
#define MOT2_FAULT_PIN        30

#define MOT3_ACTIVE           false
#define MOT3_DIR_PIN          21
#define MOT3_PWM_PIN          20
#define MOT3_EN_PIN           2
#define MOT3_FAULT_PIN        22

// the direction of the motors, if one is spinning in the wrong way then change this value
#define M1_POLARITY           false
#define M2_POLARITY           false
#define M3_POLARITY           false

// this value can be used to determine how long and how fast to spin each motor...
#define M1_GEAR_RATIO         1
#define M2_GEAR_RATIO         1
#define M3_GEAR_RATIO         1

///////////////////////// for the woodpecker bot
#elif HV_MAJOR == 1 && ARTEFACT_SPECIES  == EX_CHIPPER
#define NUM_MOTORS            1

#define MOT1_ACTIVE           true
#define MOT1_DIR_PIN          26
#define MOT1_PWM_PIN          25
#define MOT1_EN_PIN           28
#define MOT1_FAULT_PIN        27

#define MOT2_ACTIVE           false
#define MOT2_DIR_PIN          29
#define MOT2_PWM_PIN          32
#define MOT2_EN_PIN           31
#define MOT2_FAULT_PIN        30

#define MOT3_ACTIVE           false
#define MOT3_DIR_PIN          21
#define MOT3_PWM_PIN          20
#define MOT3_EN_PIN           2
#define MOT3_FAULT_PIN        22

// the direction of the motors, if one is spinning in the wrong way then change this value
#define M1_POLARITY           false
#define M2_POLARITY           false
#define M3_POLARITY           false

// this value can be used to determine how long and how fast to spin each motor...
#define M1_GEAR_RATIO         1
#define M2_GEAR_RATIO         67
#define M3_GEAR_RATIO         1

#elif HV_MAJOR == 1 && ARTEFACT_SPECIES  == EX_SPINNER
#define NUM_MOTORS            1

#define MOT1_ACTIVE           true
#define MOT1_DIR_PIN          26
#define MOT1_PWM_PIN          25
#define MOT1_EN_PIN           28
#define MOT1_FAULT_PIN        27

#define MOT2_ACTIVE           false
#define MOT2_DIR_PIN          29
#define MOT2_PWM_PIN          32
#define MOT2_EN_PIN           31
#define MOT2_FAULT_PIN        30

#define MOT3_ACTIVE           false
#define MOT3_DIR_PIN          21
#define MOT3_PWM_PIN          20
#define MOT3_EN_PIN           2
#define MOT3_FAULT_PIN        22

// the direction of the motors, if one is spinning in the wrong way then change this value
#define M1_POLARITY           false
#define M2_POLARITY           false
#define M3_POLARITY           false

// this value can be used to determine how long and how fast to spin each motor...
#define M1_GEAR_RATIO         67
#define M2_GEAR_RATIO         1
#define M3_GEAR_RATIO         1

#elif HV_MAJOR == 2 && ARTEFACT_SPECIES == EX_WINDER
#define NUM_MOTORS            1

#define MOT1_ACTIVE           true
#define MOT1_DIR_PIN          20
#define MOT1_PWM_PIN          21
#define MOT1_EN_PIN           14
#define MOT1_FAULT_PIN        16

// these are unused pins on the rear of the board
#define MOT2_ACTIVE           false
#define MOT2_DIR_PIN          29
#define MOT2_PWM_PIN          32
#define MOT2_EN_PIN           31
#define MOT2_FAULT_PIN        30

// these are unused pins on the rear of the board
#define MOT3_ACTIVE           false
#define MOT3_DIR_PIN          24
#define MOT3_PWM_PIN          25
#define MOT3_EN_PIN           26
#define MOT3_FAULT_PIN        27

// the direction of the motors, if one is spinning in the wrong way then change this value
#define M1_POLARITY           false
#define M2_POLARITY           false
#define M3_POLARITY           false

// this value can be used to determine how long and how fast to spin each motor...
#define M1_GEAR_RATIO         125
#define M2_GEAR_RATIO         1
#define M3_GEAR_RATIO         1

#endif // HV_MAJOR

#if NUM_MOTORS   == 3
const bool active_motors[3] = {MOT1_ACTIVE, MOT2_ACTIVE, MOT3_ACTIVE};
#elif NUM_MOTORS == 1
const bool active_motors[1] = {MOT1_ACTIVE};
#endif

///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// Solenoid Actuators /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
// this is how long in MS to wait after the dampener is removed before striking with solenoid
#define DAMPENER_DELAY 10

/////////////////////////////// For the Bell Bot //////////////////////////////////////
#if HV_MAJOR == 0 && ARTEFACT_SPECIES == EX_CHIRPER
#define NUM_SOLENOIDS 6
#define SOL1_PIN 12
#define SOL2_PIN 11
#define SOL3_PIN 7
#define SOL4_PIN 6
#define SOL5_PIN 4
#define SOL6_PIN 3

#elif HV_MAJOR == 1 
#define NUM_SOLENOIDS 9
#define SOL1_PIN 3
#define SOL2_PIN 4
#define SOL3_PIN 6
#define SOL4_PIN 12
#define SOL5_PIN 11
#define SOL6_PIN 14
#define SOL7_PIN 15
#define SOL8_PIN 16
#define SOL9_PIN 7

#elif HV_MAJOR == 2
#define NUM_SOLENOIDS 2
#define SOL1_PIN 3
#define SOL2_PIN 7

#endif

#if ARTEFACT_SPECIES == EX_CHIRPER
#define SOL1_ACTIVE true
#define SOL2_ACTIVE true
#define SOL3_ACTIVE true
#define SOL4_ACTIVE true
#define SOL5_ACTIVE true
#define SOL6_ACTIVE true

#elif HV_MAJOR == 1 && ARTEFACT_SPECIES == EX_CHIPPER
// todo these need to be chosen by the body not the HV
#define SOL1_ACTIVE true
#define SOL2_ACTIVE false
#define SOL3_ACTIVE false
#define SOL4_ACTIVE false
#define SOL5_ACTIVE false
#define SOL6_ACTIVE false
#define SOL7_ACTIVE false
#define SOL8_ACTIVE false
#define SOL9_ACTIVE false

#elif HV_MAJOR == 1 && ARTEFACT_SPECIES == EX_CLAPPER 
// todo these need to be chosen by the body not the HV
#define SOL1_ACTIVE false
#define SOL2_ACTIVE false
#define SOL3_ACTIVE true
#define SOL4_ACTIVE false
#define SOL5_ACTIVE false
#define SOL6_ACTIVE false
#define SOL7_ACTIVE false
#define SOL8_ACTIVE true
#define SOL9_ACTIVE false

#elif HV_MAJOR == 1 && ARTEFACT_SPECIES == EX_SPINNER 
// todo these need to be chosen by the body not the HV
#define SOL1_ACTIVE false
#define SOL2_ACTIVE false
#define SOL3_ACTIVE false
#define SOL4_ACTIVE false
#define SOL5_ACTIVE false
#define SOL6_ACTIVE false
#define SOL7_ACTIVE false
#define SOL8_ACTIVE false
#define SOL9_ACTIVE false

#elif HV_MAJOR == 2 && ARTEFACT_SPECIES == EX_WINDER
#define SOL1_ACTIVE false
#define SOL2_ACTIVE false

#endif // HV_MAJOR == 1


#if NUM_SOLENOIDS == 9
const int s_pins[] = {SOL1_PIN, SOL2_PIN, SOL3_PIN, SOL4_PIN, SOL5_PIN, SOL6_PIN, SOL7_PIN, SOL8_PIN, SOL9_PIN};
uint16_t sol_on_time[] = {30, 30, 30, 30, 30, 30, 30, 30, 30};
bool sol_state[] = {false, false, false, false, false, false, false, false, false}; // is the solenoid on or off
bool sol_active[] = {SOL1_ACTIVE, SOL2_ACTIVE, SOL3_ACTIVE, SOL4_ACTIVE, SOL5_ACTIVE, SOL6_ACTIVE, SOL7_ACTIVE, SOL8_ACTIVE, SOL9_ACTIVE};
const uint8_t num_active_solenoids = SOL1_ACTIVE + SOL2_ACTIVE + SOL3_ACTIVE + SOL4_ACTIVE + SOL5_ACTIVE + SOL6_ACTIVE + SOL7_ACTIVE + SOL8_ACTIVE + SOL9_ACTIVE;
unsigned long sol_timers[9];
elapsedMillis last_sol_action[9];

// the max and min values for the solenoids corrisponds to the lowest value which
// results in an audible sounds from the actuator
// the max is the value which produces the loudest sonic event from the solenoid
// any values higher than the max will not produce a louder sound
// These values should be tested and set within this file once determined in-situ
// between is the min time allowed between actuations for that solenoid
#define S1_MIN                24
#define S1_MAX                100
#define S1_BETWEEN            20

#define S2_MIN                10
#define S2_MAX                100
#define S2_BETWEEN            20

#define S3_MIN                10
#define S3_MAX                100
#define S3_BETWEEN            20

#define S4_MIN                10
#define S4_MAX                100
#define S4_BETWEEN            20

#define S5_MIN                10
#define S5_MAX                100
#define S5_BETWEEN            20

#define S6_MIN                10
#define S6_MAX                100
#define S6_BETWEEN            20

#define S7_MIN                10
#define S7_MAX                100
#define S7_BETWEEN            20

#define S8_MIN                10
#define S8_MAX                100
#define S8_BETWEEN            20

#define S9_MIN                10
#define S9_MAX                100
#define S9_BETWEEN            20

const int sol_min[9] = {S1_MIN, S2_MIN, S3_MIN, S4_MIN, S5_MIN, S6_MIN, S7_MIN, S8_MIN, S9_MIN};
const int sol_max[9] = {S1_MAX, S2_MAX, S3_MAX, S4_MAX, S5_MAX, S6_MAX, S7_MAX, S8_MAX, S9_MAX};

#elif NUM_SOLENOIDS == 6
const int s_pins[] = {SOL1_PIN, SOL2_PIN, SOL3_PIN, SOL4_PIN, SOL5_PIN, SOL6_PIN};
uint16_t sol_on_time[] = {30, 30, 30, 30, 30, 30};
bool sol_state[] = {false, false, false, false, false, false}; // is the solenoid on or off
bool sol_active[] = {true, true, true, true, true, true}; // is the solenoid on or off
const uint8_t num_active_solenoids = SOL1_ACTIVE + SOL2_ACTIVE + SOL3_ACTIVE + SOL4_ACTIVE + SOL5_ACTIVE + SOL6_ACTIVE;
unsigned long sol_timers[6];
elapsedMillis last_sol_action[6];

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

const int sol_min[6] = {S1_MIN, S2_MIN, S3_MIN, S4_MIN, S5_MIN, S6_MIN};
const int sol_max[6] = {S1_MAX, S2_MAX, S3_MAX, S4_MAX, S5_MAX, S6_MAX};

#elif NUM_SOLENOIDS == 2
const int s_pins[] = {SOL1_PIN, SOL2_PIN};
uint16_t sol_on_time[] = {30, 30};
bool sol_state[] = {false, false}; // is the solenoid on or off
bool sol_active[] = {SOL1_ACTIVE, SOL2_ACTIVE}; // is the solenoid on or off
const uint8_t num_active_solenoids = SOL1_ACTIVE + SOL2_ACTIVE;
unsigned long sol_timers[2];
elapsedMillis last_sol_action[2];

// the max and min values for the solenoids corrisponds to the lowest value which
// results in an audible sounds from the actuator
// the max is the value which produces the loudest sonic event from the solenoid
// any values higher than the max will not produce a louder sound
// These values should be tested and set within this file once determined in-situ

#define S1_MIN                10
#define S1_MAX                100
#define S2_MIN                10
#define S2_MAX                100

const int sol_min[2] = {S1_MIN, S2_MIN};
const int sol_max[2] = {S1_MAX, S2_MAX};

#endif


// TODO - something about grouping together the solenoids?
// to keep track of if the different solenoid outputs are active or not

//////////////////////////// Global Actuation Settings and Values ////////////////////

// time in ms in which the solenoids need to cool down between individual actuations
#define SOL_COOLDOWN 50

// This value is used to calculate how long between unit actuations
uint32_t ACTUATION_DELAY = 0;
elapsedMillis last_playback_tmr;

double peak_val = 0.0;
double last_peak = 0.0;

///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Rhythm detection stuff ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
// the maximum number of rhythms to store
#define MAX_RHYTHMS 10
// the maximum number of notes which can be stored in a single rhythm
#define MAX_NOTES 16

uint8_t active_rhythm; // what rhythm number is to be played next
uint8_t active_notes; // what note number from within the current rhythm will be played next

elapsedMillis last_onset; // when was the last onset detected?
elapsedMillis last_vocalisation; // how long it has been since the bot vocalised last

unsigned long min_inter_note_rhythm = 100; // the shortest amount of time between onsets
unsigned long max_inter_note_rhythm = 1000; // the longest amount of time between onsets

float WINDING_RATE =              1.0;
#define MAX_WINDING_RATE          2.0
#define MID_WINDING_RATE          1.0
#define MIN_WINDING_RATE          0.5

// 90 and -35
#define BASE_FORWARD_RATE         225
#define BASE_BACKWARD_RATE        -125

#define MIN_FORWARD_RATE          BASE_FORWARD_RATE / 2
#define MAX_FORWARD_RATE          450

#define MIN_BACKWARD_RATE         (BASE_BACKWARD_RATE + 18)
#define MAX_BACKWARD_RATE         -250

// when set to true through the user interface theses bools
// trigger manual winding of the motor
int WIND_FORWARD = 0;
int WIND_BACKWARD = 0;

void detectOnset() {
  // given the current audio features determine if an onset has occured
}

#define AUDIO_MEMORY 18

// int but_test[4];
// float pot_test[4];


#define MIN_ACTIVITY_LEVEL    0.25
#define MID_ACTIVITY_LEVEL    1.0
#define MAX_ACTIVITY_LEVEL    3.0
// the user control scaler to determine how often the artefact with actuate
float ACTIVITY_LEVEL        = MID_ACTIVITY_LEVEL;

float MOTOR_MOVEMENT = 0.0;
float STRIKE_LENGTH  = 30.0;

#define PWR_KILL_PIN              25

#if ARTEFACT_SPECIES == EX_WINDER
int motor_speed = 0;
int target_motor_speed = 0;
int next_motor_speed = 0;
int motor_time = 0;
int next_motor_time = 0;
const int max_motor_speed = 150;
const int min_motor_speed = 0;

elapsedMillis last_winding;
elapsedMillis last_enc_change;
const long winding_interval = 15000;
int last_pos = 0;
#endif

#endif // __CONFIGURATION_EXPLORATOR_H__
