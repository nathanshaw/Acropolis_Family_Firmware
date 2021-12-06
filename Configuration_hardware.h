#ifndef __CONFIGURATION_HARDWARE_H__
#define __CONFIGURATION_HARDWARE_H__
#include "PrintUtils.h"
#include "Macros.h"

#define FLIP_SWITCHES             true

/////////////////////// Enclosure Type //////////////////////////////
// Currently there are four the Speculator enclosures: 
// GROUND_ENCLOSURE, ORB_ENCLOSURE_WITH_HOLE
// ORB_ENCLOSURE_WITHOUT_HOLE, and NO_ENCLOSURE
// if it is unknown what enclosure will be paired with the PCB
// it is recommended to map ENCLOSURE_TYPE to a physical switch
uint8_t ENCLOSURE_TYPE =          NO_ENCLOSURE;

////////////////////////////////////////////////////////////
// different enclosures result in varying gain levels
// ENCLOSURE_GAIN_FACTOR is automatically calculated based on
// the ARTEFACT_TYPE and ENCLOSURE_TYPE
#if ARTEFACT_TYPE == SPECULATOR
#if ENCLOSURE_TYPE == ORB_ENCLOSURE_WITH_HOLE
double ENCLOSURE_GAIN_FACTOR =        1.0;
#elif ENCLOSURE_TYPE == ORB_ENCLOSURE_WITHOUT_HOLE
double ENCLOSURE_GAIN_FACTOR =        2.0;
#elif ENCLOSURE_TYPE == GROUND_ENCLOSURE
double ENCLOSURE_GAIN_FACTOR =        3.0;
#elif ENCLOSURE_TYPE == NO_ENCLOSURE
double ENCLOSURE_GAIN_FACTOR =        0.65;
#endif // ENCLOSURE_TYPE

#elif ARTEFACT_TYPE == EXPLORATOR
double ENCLOSURE_GAIN_FACTOR =        1.0;
#elif ARTEFACT_TYPE == LEGATUS
double ENCLOSURE_GAIN_FACTOR =        1.0;
#endif // ARTEFACT_TYPE

//////////////////////////////////////////////////////////////////
/////////////////////// Enclosure Finish /////////////////////////
//////////////////////////////////////////////////////////////////
////////////////// TODO - ensure this is fully implemented ///////
#define FINISH_NORMAL                  0
#define FINISH_FROSTED                 1

#define ENCLOSURE_FINISH               FINISH_NORMAL

#if ENCLOSURE_FINISH == FINISH_NORMAL
#define LUX_ENCLOSURE_SCALER           0.75
#elif ENCLOSURE_FINISH == FINISH_FROSTED
#define LUX_ENCLOSURE_SCALER           1.0
#endif 

//////////////////////////////////////////////////////////////////////
////////////////////// Microphones ///////////////////////////////////
//////////////////////////////////////////////////////////////////////
// TODO
#if ARTEFACT_TYPE == SPECULATOR
#define NUM_MICROPHONES                 2
#elif ARTEFACT_TYPE == EXPLORATOR
#define NUM_MICROPHONES                 1
#elif ARTEFACT_TYPE == LEGATUS
#define NUM_MICROPHONES                 2
#endif // microphones

//////////////////////////////////////////////////////////////////////
////////////////////// Lux Sensors ///////////////////////////////////
//////////////////////////////////////////////////////////////////////
#if ARTEFACT_TYPE == SPECULATOR
#define NUM_LUX_SENSORS           2
#elif ARTEFACT_TYPE == EXPLORATOR
#define NUM_LUX_SENSORS           2
#elif ARTEFACT_TYPE == LEGATUS
#define NUM_LUX_SENSORS           2
#endif

//////////////////////////////////////////////////////////////////////
////////////////////////// TCA Bus Expanders  ////////////////////////
//////////////////////////////////////////////////////////////////////
// I2C_MULTI should be 0 if no TCA I2C bus expander is present on the PCB
// I2C MULTI should be 1 if a TCA I2C bus expander is present
#if HV_MAJOR < 3 && ARTEFACT_TYPE == SPECULATOR
#define I2C_MULTI                 1
// the number of active channels on the TCA (can in theory support 8 sensors, etc.)
#define TCA_CHANNELS              2
#else
#define I2C_MULTI                 0
#define TCA_CHANNELS              0 
#endif

//////////////////////////////////////////////////////////////////////
////////////////////////////// User Controls /////////////////////////
//////////////////////////////////////////////////////////////////////
// how much play will the pots have before returning a value?
// good values should range between 0.002 and 0.005
#if ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE == MB_BODY
#define POT_PLAY        0.05
#else
#define POT_PLAY        0.006
#endif

// for the bell bot basically
#if (ARTEFACT_TYPE == EXPLORATOR) &&  (HV_MAJOR == 0)
///////////////////// Buttons /////////////////////////////////////////
#define NUM_BUTTONS     0
#define BUT1_PIN        33
#define BUT2_PIN        A13
#define BUT3_PIN        A12
#define BUT4_PIN        17

// should the values received from the buttons be reversed?
#define BUT1_LOW_VAL    0
#define BUT1_HIGH_VAL   1
#define BUT2_LOW_VAL    0
#define BUT2_HIGH_VAL   1
#define BUT3_LOW_VAL    0
#define BUT3_HIGH_VAL   1
#define BUT4_LOW_VAL    0
#define BUT4_HIGH_VAL   1

#define BUT1_LOW_CHANGES    true
#define BUT2_LOW_CHANGES    true
#define BUT3_LOW_CHANGES    true
#define BUT4_LOW_CHANGES    true

#define BUT1_ACTIVE     false
#define BUT2_ACTIVE     false
#define BUT3_ACTIVE     false
#define BUT4_ACTIVE     false

#define BUT1_PULLUP     false
#define BUT2_PULLUP     false
#define BUT3_PULLUP     false
#define BUT4_PULLUP     false

// what is the name of the button? should reflect what it does
#define BUT1_NAME       "N/A"
#define BUT2_NAME       "N/A"
#define BUT3_NAME       "N/A"
#define BUT4_NAME       "N/A"

///////////////////// Pots /////////////////////////////////////////////
#define NUM_POTS        1
#define POT1_PIN        A1
#define POT2_PIN        A10

#define POT1_ACTIVE     true
#define POT2_ACTIVE     false

// should the values received from the pots be reversed?
#define POT1_REVERSE    true
#define POT2_REVERSE    true

// how much play will the pots have before returning a value?
// good values should range between 0.002 and 0.005
#define POT1_PLAY       0.004
#define POT2_PLAY       0.004

// what is the name of the button? should reflect what it does
#define POT1_NAME       "POT1"
#define POT2_NAME       "N/A"

////////////////////////////////////////////////////////////////////////////////
// now lets set things up for the woodpecker bot
////////////////////////////////////////////////////////////////////////////////
#elif (ARTEFACT_TYPE == EXPLORATOR) && (HV_MAJOR == 1)

///////////////////// Buttons //////////////////////////////////////////////////
#define NUM_BUTTONS     4

#define BUT1_PIN        33
#define BUT2_PIN        A13
#define BUT3_PIN        A12
#define BUT4_PIN        17

// should the values received from the buttons be reversed?
#define BUT1_LOW_VAL    0
#define BUT1_HIGH_VAL   1
#define BUT2_LOW_VAL    0
#define BUT2_HIGH_VAL   1
#define BUT3_LOW_VAL    0
#define BUT3_HIGH_VAL   1
#define BUT4_LOW_VAL    0
#define BUT4_HIGH_VAL   1

#define BUT1_LOW_CHANGES    true
#define BUT2_LOW_CHANGES    true
#define BUT3_LOW_CHANGES    true
#define BUT4_LOW_CHANGES    true

#if BODY_TYPE == CLAPPER_BODY
#define BUT1_ACTIVE     false
#define BUT2_ACTIVE     false
#define BUT3_ACTIVE     false
#define BUT4_ACTIVE     false
#else // for the woodpecker and bellbot body
#define BUT1_ACTIVE     true
#define BUT2_ACTIVE     true
#define BUT3_ACTIVE     true
#define BUT4_ACTIVE     true
#endif

#define BUT1_PULLUP     false
#define BUT2_PULLUP     false
#define BUT3_PULLUP     false
#define BUT4_PULLUP     false

// what is the name of the button? should reflect what it does
#define BUT1_NAME       "BUT1"
#define BUT2_NAME       "BUT2"
#define BUT3_NAME       "BUT3"
#define BUT4_NAME       "BUT4"

///////////////////// Pots /////////////////////////////////////////////////////
#define NUM_POTS        2
#define POT1_PIN        A11
#define POT2_PIN        A10

// should the values received from the pots be reversed?
#define POT1_REVERSE    false
#define POT2_REVERSE    false

#if BODY_TYPE == CLAPPER_BODY
#define POT1_ACTIVE     false
#define POT2_ACTIVE     false
#else // for the woodpecker and bellbot body
#define POT1_ACTIVE     true
#define POT2_ACTIVE     true
#endif

// how much play will the pots have before returning a value?
// good values should range between 0.002 and 0.005
#define POT1_PLAY       0.004
#define POT2_PLAY       0.004

// what is the name of the button? should reflect what it does
#define POT1_NAME       "ACTIVITY_LEVEL"
#define POT2_NAME       "STRIKE_LENGTH"

// For the Explorator MB_BODY
#elif ARTEFACT_TYPE == EXPLORATOR && HV_MAJOR == 2
///////////////////// Buttons //////////////////////////////////////////////////
#define NUM_BUTTONS     2

#define BUT1_PIN        4
#define BUT2_PIN        6

// should the values received from the buttons be reversed?
#define BUT1_LOW_VAL    0
#define BUT1_HIGH_VAL   1

#define BUT2_LOW_VAL    0
#define BUT2_HIGH_VAL   1

#define BUT1_LOW_CHANGES    false
#define BUT2_LOW_CHANGES    false

#define BUT1_ACTIVE     true
#define BUT2_ACTIVE     true

#define BUT1_PULLUP     true
#define BUT2_PULLUP     true

// what is the name of the button? should reflect what it does
#define BUT1_NAME       "Wind Forward"
#define BUT2_NAME       "Wind Backward"

///////////////////// Pots /////////////////////////////////////////////////////
#define NUM_POTS        2
#define POT1_PIN        A3
#define POT2_PIN        A1

// should the values received from the pots be reversed?
#define POT1_REVERSE    false
#define POT2_REVERSE    false

#define POT1_ACTIVE     true
#define POT2_ACTIVE     true

// how much play will the pots have before returning a value?
// good values should range between 0.002 and 0.005
#define POT1_PLAY       0.004
#define POT2_PLAY       0.004

// what is the name of the button? should reflect what it does
#define POT1_NAME       "Activity Level"
#define POT2_NAME       "Winding Rate"

#elif (ARTEFACT_TYPE == SPECULATOR) && (HV_MAJOR == 2) && (HV_MINOR == 1)
// num buttons (how many buttons does the hardware support?)
#define NUM_BUTTONS       6

// pins        (what are the pins associated with the buttons?)
#define BUT1_PIN          12
#define BUT2_PIN          11
#define BUT3_PIN          14
#define BUT4_PIN          15
#define BUT5_PIN          16
#define BUT6_PIN          17

#define BUT1_LOW_CHANGES    true
#define BUT2_LOW_CHANGES    true
#define BUT3_LOW_CHANGES    true
#define BUT4_LOW_CHANGES    true
#define BUT5_LOW_CHANGES    true
#define BUT6_LOW_CHANGES    true

// reverse     (should the reading be reversed?)
#define BUT1_LOW_VAL    0
#define BUT1_HIGH_VAL   1

#define BUT2_LOW_VAL    0
#define BUT2_HIGH_VAL   1

#define BUT3_LOW_VAL    0
#define BUT3_HIGH_VAL   1

#define BUT4_LOW_VAL    0
#define BUT4_HIGH_VAL   1

#define BUT5_LOW_VAL    0
#define BUT5_HIGH_VAL   1

#define BUT6_LOW_VAL    0
#define BUT6_HIGH_VAL   1

// active      (is this button populated (available on the enclosure))
#define BUT1_ACTIVE     true
#define BUT2_ACTIVE     true
#define BUT3_ACTIVE     true
#define BUT4_ACTIVE     true
#define BUT5_ACTIVE     true
#define BUT6_ACTIVE     true

// pullup      (should the hardware pull-up be used)
#define BUT1_PULLUP     false
#define BUT2_PULLUP     false
#define BUT3_PULLUP     false
#define BUT4_PULLUP     false
#define BUT5_PULLUP     false
#define BUT6_PULLUP     false

// TODO name these properly
// name        (what is the name for the buttons)
#define BUT1_NAME       "COLOR_MAP_MODE"
#define BUT2_NAME       "SQUARE_BRIGHTNESS"
#define BUT3_NAME       "USE_TARGET_BRIGHTNESS"
#define BUT4_NAME       "REVERSE_HUE"
#define BUT5_NAME       "REVERSE_SATURATION"
#define BUT6_NAME       "BOOT_DELAY_ACTIVE"

// num pots    (how many pots does the hardware support?)
#define NUM_POTS        0
// pins        (what are the A pins associated with the pots?)
// reverse     (should the reading be reversed?)
// active      (is the pot populated (available on the enclosure)) 
// play        (what is the amount of play allowed? 0.002 - 0.005 is usally good
// name        (what is the name for the pot?)

#elif (ARTEFACT_TYPE == SPECULATOR) && (HV_MAJOR == 3) && (HV_MINOR == 0)
// num buttons (how many buttons does the hardware support?)
#define NUM_BUTTONS       10

// pins        (what are the pins associated with the buttons?)
#define BUT1_PIN          2
#define BUT2_PIN          3
#define BUT3_PIN          4
#define BUT4_PIN          6
#define BUT5_PIN          7
#define BUT6_PIN          8
#define BUT7_PIN          10
#define BUT8_PIN          11
#define BUT9_PIN          12
#define BUT10_PIN         14

// TODO what dooes this do?
#define BUT1_LOW_CHANGES    true
#define BUT2_LOW_CHANGES    true
#define BUT3_LOW_CHANGES    true
#define BUT4_LOW_CHANGES    true
#define BUT5_LOW_CHANGES    true
#define BUT6_LOW_CHANGES    true
#define BUT7_LOW_CHANGES    false
#define BUT8_LOW_CHANGES    false
#define BUT9_LOW_CHANGES    false
#define BUT10_LOW_CHANGES   true

#if FLIP_SWITCHES == true

// reverse     (should the reading be reversed?)
#define BUT1_LOW_VAL    1
#define BUT1_HIGH_VAL   0

#define BUT2_LOW_VAL    1
#define BUT2_HIGH_VAL   0

#define BUT3_LOW_VAL    1
#define BUT3_HIGH_VAL   0

#define BUT4_LOW_VAL    1
#define BUT4_HIGH_VAL   0

#define BUT5_LOW_VAL    1
#define BUT5_HIGH_VAL   0

#define BUT6_LOW_VAL    1
#define BUT6_HIGH_VAL   0

#define BUT7_HIGH_VAL    LED_MAPPING_STANDARD
#define BUT7_LOW_VAL     LED_MAPPING_CENTER_OUT

#define BUT8_HIGH_VAL    LED_MAPPING_STANDARD
#define BUT8_LOW_VAL     LED_MAPPING_OUTSIDE_IN

// other options include outside in
#define BUT9_HIGH_VAL    LED_MAPPING_STANDARD
#define BUT9_LOW_VAL     LED_MAPPING_CLOCK_FILL

#define BUT10_LOW_VAL    0
#define BUT10_HIGH_VAL   1

#else // flip switches

// reverse     (should the reading be reversed?)

#define BUT1_LOW_VAL    0
#define BUT1_HIGH_VAL   1

#define BUT2_LOW_VAL    0
#define BUT2_HIGH_VAL   1

#define BUT3_LOW_VAL    0
#define BUT3_HIGH_VAL   1

#define BUT4_LOW_VAL    0
#define BUT4_HIGH_VAL   1

#define BUT5_LOW_VAL    0
#define BUT5_HIGH_VAL   1

#define BUT6_LOW_VAL    0
#define BUT6_HIGH_VAL   1

#define BUT7_LOW_VAL    LED_MAPPING_STANDARD
#define BUT7_HIGH_VAL   LED_MAPPING_CENTER_OUT

#define BUT8_LOW_VAL    LED_MAPPING_STANDARD
#define BUT8_HIGH_VAL   LED_MAPPING_OUTSIDE_IN

// other options include outside in
#define BUT9_LOW_VAL    LED_MAPPING_STANDARD
#define BUT9_HIGH_VAL   LED_MAPPING_BOTTOM_UP

#define BUT10_LOW_VAL    1
#define BUT10_HIGH_VAL   0
#endif // flip switches

// active      (is this button populated (available on the enclosure))
#define BUT1_ACTIVE     true
#define BUT2_ACTIVE     true
#define BUT3_ACTIVE     true
#define BUT4_ACTIVE     true
#define BUT5_ACTIVE     true
#define BUT6_ACTIVE     true
#define BUT7_ACTIVE     true
#define BUT8_ACTIVE     true
#define BUT9_ACTIVE     true
#define BUT10_ACTIVE    true

// pullup      (should the hardware pull-up be used)
#define BUT1_PULLUP     false
#define BUT2_PULLUP     false
#define BUT3_PULLUP     false
#define BUT4_PULLUP     false
#define BUT5_PULLUP     false
#define BUT6_PULLUP     false
#define BUT7_PULLUP     false
#define BUT8_PULLUP     false
#define BUT9_PULLUP     false
#define BUT10_PULLUP    false

// TODO name these properly
// name        (what is the name for the buttons)
#define BUT1_NAME       "USE_TARGET_HUE"
#define BUT2_NAME       "SQUARE_BRIGHTNESS"
#define BUT3_NAME       "USE_TARGET_BRIGHTNESS"
#define BUT4_NAME       "REVERSE_HUE"
#define BUT5_NAME       "REVERSE_SATURATION"
#define BUT6_NAME       "REVERSE_BRIGHTNESS"
#define BUT7_NAME       "LED_MAPPING_MODE (CENTER_OUT)"
#define BUT8_NAME       "LED_MAPPING_MODE (OUTSIDE_IN)"
#define BUT9_NAME       "LED_MAPPING_MODE (CLOCK FACE)"
#define BUT10_NAME      "BOOT_DELAY_ACTIVE"

// num pots    (how many pots does the hardware support?)
#define NUM_POTS          4
// pins        (what are the A pins associated with the pots?)
#define POT1_PIN          22
#define POT2_PIN          21
#define POT3_PIN          20
#define POT4_PIN          17

/*
#define POT1_PIN          21
#define POT2_PIN          20
#define POT3_PIN          22
#define POT4_PIN          17
*/

// reverse     (should the reading be reversed?)
#define POT1_REVERSE    true 
#define POT2_REVERSE    true 
#define POT3_REVERSE    true
#define POT4_REVERSE    true 

// active      (is this button populated (available on the enclosure))
#define POT1_ACTIVE     true
#define POT2_ACTIVE     true
#define POT3_ACTIVE     true
#define POT4_ACTIVE     true

// play        (what is the amount of play allowed? 0.002 - 0.005 is usally good
#define POT1_PLAY       0.004
#define POT2_PLAY       0.004
#define POT3_PLAY       0.004
#define POT4_PLAY       0.004

// name        (what is the name for the pot?)
#define POT1_NAME       "user_brightness_scaler"
#define POT2_NAME       "ADDED_SATURATION"
#define POT3_NAME       "POT3"
#define POT4_NAME       "user_brightness_cuttoff"

#elif ARTEFACT_TYPE == LEGATUS && HV_MAJOR == 1 && HV_MINOR == 0

// num buttons (how many buttons does the hardware support?)
#define NUM_BUTTONS       6

// pins        (what are the pins associated with the buttons?)
#define BUT1_PIN          0
#define BUT2_PIN          1
#define BUT3_PIN          4
#define BUT4_PIN          6
#define BUT5_PIN          8
#define BUT6_PIN          15

// reverse     (should the reading be reversed?)
#define BUT1_LOW_VAL    0
#define BUT1_HIGH_VAL   1
#define BUT2_LOW_VAL    0
#define BUT2_HIGH_VAL   1
#define BUT3_LOW_VAL    0
#define BUT3_HIGH_VAL   1
#define BUT4_LOW_VAL    0
#define BUT4_HIGH_VAL   1
#define BUT5_LOW_VAL    0
#define BUT5_HIGH_VAL   1
#define BUT6_LOW_VAL    0
#define BUT6_HIGH_VAL   1

// active      (is this button populated (available on the enclosure))
#define BUT1_ACTIVE     true
#define BUT2_ACTIVE     true
#define BUT3_ACTIVE     true
#define BUT4_ACTIVE     true
#define BUT5_ACTIVE     true
#define BUT6_ACTIVE     true

// pullup      (should the hardware pull-up be used)
#define BUT1_PULLUP     true
#define BUT2_PULLUP     true
#define BUT3_PULLUP     true
#define BUT4_PULLUP     true
#define BUT5_PULLUP     true
#define BUT6_PULLUP     true

// TODO name these properly
// name        (what is the name for the buttons)
#define BUT1_NAME       "BUT1"
#define BUT2_NAME       "BUT2"
#define BUT3_NAME       "BUT3"
#define BUT4_NAME       "BUT4"
#define BUT5_NAME       "BUT5"
#define BUT6_NAME       "BUT6"

#define BUT1_LOW_CHANGES    true
#define BUT2_LOW_CHANGES    true
#define BUT3_LOW_CHANGES    true
#define BUT4_LOW_CHANGES    true
#define BUT5_LOW_CHANGES    true
#define BUT6_LOW_CHANGES    true

// num pots    (how many pots does the hardware support?)
#define NUM_POTS          2

// pins        (what are the A pins associated with the pots?)
#define POT1_PIN          A3
#define POT2_PIN          A7

// reverse     (should the reading be reversed?)
#define POT1_REVERSE      true
#define POT2_REVERSE      true

// active      (is this button populated (available on the enclosure))
#define POT1_ACTIVE       true
#define POT2_ACTIVE       true

// play        (what is the amount of play allowed? 0.002 - 0.005 is usally good
#define POT1_PLAY         0.004
#define POT2_PLAY         0.004

// name        (what is the name for the pot?)
#define POT1_NAME         "POT1"
#define POT2_NAME         "POT2"

#elif ARTEFACT_TYPE == LEGATUS && HV_MAJOR == 1 && HV_MINOR > 0

// num buttons (how many buttons does the hardware support?)
#define NUM_BUTTONS         6

// pins        (what are the pins associated with the buttons?)
#define BUT1_PIN            0
#define BUT2_PIN            1
#define BUT3_PIN            2
#define BUT4_PIN            3
#define BUT5_PIN            4
#define BUT6_PIN            6

// reverse     (should the reading be reversed?)
#define BUT1_LOW_VAL        1
#define BUT1_HIGH_VAL       0
#define BUT2_LOW_VAL        1
#define BUT2_HIGH_VAL       0
#define BUT3_LOW_VAL        1
#define BUT3_HIGH_VAL       0
#define BUT4_LOW_VAL        1
#define BUT4_HIGH_VAL       0
#define BUT5_LOW_VAL        1
#define BUT5_HIGH_VAL       0
#define BUT6_LOW_VAL        1
#define BUT6_HIGH_VAL       0

// active      (is this button populated (available on the enclosure))
#define BUT1_ACTIVE         true
#define BUT2_ACTIVE         true
#define BUT3_ACTIVE         true
#define BUT4_ACTIVE         true
#define BUT5_ACTIVE         true
#define BUT6_ACTIVE         true

// pullup      (should the hardware pull-up be used in the microcontroller)
#define BUT1_PULLUP         true
#define BUT2_PULLUP         true
#define BUT3_PULLUP         true
#define BUT4_PULLUP         true
#define BUT5_PULLUP         true
#define BUT6_PULLUP         true

// TODO properly explain this
#define BUT1_LOW_CHANGES    true
#define BUT2_LOW_CHANGES    true
#define BUT3_LOW_CHANGES    true
#define BUT4_LOW_CHANGES    true
#define BUT5_LOW_CHANGES    true
#define BUT6_LOW_CHANGES    true

// TODO name these properly
// name        (what is the name for the buttons)
#define BUT1_NAME           "BUT1"
#define BUT2_NAME           "BUT2"
#define BUT3_NAME           "BUT3"
#define BUT4_NAME           "BUT4"
#define BUT5_NAME           "BUT5"
#define BUT6_NAME           "BUT6"

// num pots    (how many pots does the hardware support?)
#define NUM_POTS            2

// pins        (what are the A pins associated with the pots?)
#define POT1_PIN            A3
#define POT2_PIN            A7

// reverse     (should the reading be reversed?)
#define POT1_REVERSE        true
#define POT2_REVERSE        true

// active      (is this button populated (available on the enclosure))
#define POT1_ACTIVE         true
#define POT2_ACTIVE         true

// play        (what is the amount of play allowed? 0.002 - 0.005 is usally good
#define POT1_PLAY           0.004
#define POT2_PLAY           0.004

// name        (what is the name for the pot?)
#define POT1_NAME           "Brightness"
#define POT2_NAME           "Amplitude"

#endif // ARTEFACT_TYPE and HV_MAJOR

// TODO - need  to move some of this to the EEPROM storage, and add a flag in the standard configuratui file to  either read that information or to write it
// how long does the microphone test routine last for in the feature collector testMicrophone() function
#define MICROPHONE_TEST_DURATION  1500
#define LUX_TEST_DURATION         1000

#endif // __HARDWARE_CONFIGURATION_H__

//////////////////////////////// SD Card Stuff /////////////////////
#if ARTEFACT_TYPE == LEGATUS && FIRMWARE_MODE == MODULAR_LEGATUS_MODE

#define SD_PRESENT                            true
#define SD_MOSI                               7
#define SD_MISO                               12
#define SD_CLK                                14
#define SD_CS                                 10

#else
#define SD_PRESENT                            false
#endif// ARTEFACT_TYPE == LEGATUS for SD card stuff

/////////////////////////////// DOF Sensors ////////////////////////
#if ARTEFACT_TYPE == LEGATUS

#define DOF_PRESENT                           true

#else 

#define DOF_PRESENT                           false

#endif // DOF sensor stuff
