#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

// to get some definitions such as SPECULATOR
#include "Macros.h"
#include <ValueTrackerDouble.h>
// #include "../lib/ValueTracker/ValueTrackerDouble.h"

////////////////////////// Boot Tests ////////////////////////////////
#define TEST_SOLENOIDS           false
#define TEST_MOTORS              false
#define TEST_LEDS                false
// TODO
#define TEST_LUX                 false
#define TEST_TEMP                false
#define TEST_DOF                 false

#define DISABLE_USER_CONTROLS    true

//////////////////// Software Revision ////////////////////////////////
#define SV_MAJOR                  0
#define SV_MINOR                  1
#define SV_REVISION               12

/////////////////////// Artefact Genus ////////////////////////////////
// There are three artefact genera currently availible
// SPECULATOR, EXPLORATOR, and LEGATUS
// set ARTEFACT_GENUS to one of these types
/////////////////// TODO
#define ARTEFACT_GENUS            SPECUlATOR

////////////////////// Artefact Species //////////////////////////////////////
// Speculator Species include: SPEC_MINOR and SPEC_MAJOR
// Explorator Species include: EX_CHIPPER, EX_CHIRPER, EX_CLAPPER, EX_SPINNER, and EX_WINDER
// Legatus Species include: LEG_MAJOR

#if ARTEFACT_GENUS == EXPLORATOR
#define ARTEFACT_SPECIES                 EX_CHIRPER
#elif ARTEFACT_GENUS == LEGATUS 
#define ARTEFACT_SPECIES                 LEG_MAJOR
#elif ARTEFACT_GENUS == SPECULATOR
#define ARTEFACT_SPECIES                 SPEC_MINOR

#endif
#define ORB_BODY                  10
#define GND_BODY                  11


//////////////////////////////////////////////////////////////////////
////////////////////// Hardware Revision /////////////////////////////
//////////////////////////////////////////////////////////////////////
///////////////// Speculator  //////////////////////////////////////////////////////////
// There are different hardware revisions for different Artefacts
// Starting with the Speculator the 2.0, 2.1 and 3.0 are the primary revisions
// where the 2.0 and 2.1 are 10 LED 100 mm units, where the 2.1 has 6 jumpers and
// the 2.0 has no user controls. The 3.0 is the 80 mm units with 40 LEDs
//////////////// Explorator ////////////////////////////////////////////////////////////
// For the Explorator the 0.0 PCB is the yellow one with the temp/humid sensor on the
// main PCB (the bell bot is PCB revision 0.0, where revision 1.0 is the one with 3x motor
// drivers and 9 solenoid drivers, revision 2.0 is the small PCB used for the EX_WINDER
// revision 2.0 is the smaller PCB with two solenoid outputs and a single motor driver
// 0.1.4 = version used for the jan, 2021 southwest installations
//////////////// Legatus ///////////////////////////////////////////////////////////////
// There are three current Legatus firmware revisions:
// v0.1 - "square PCB"
// v1.0 - first circular PCB with the modular audio codecs and amplifiers
// v1.1 - second circular PCB with the jellybean audio amplifier and the audio codec
// v1.2 - second circular PCB which addressed shortcomings with first iteration
/////////////////////////////////////////////////////////////////////////////////////////
#if ARTEFACT_SPECIES == SPEC_MAJOR
#define HV_MAJOR                  2
#define HV_MINOR                  1
// some v2.0 PCBs are still operational 
// #define HV_MINOR                  0
#elif ARTEFACT_SPECIES == SPEC_MINOR
#define HV_MAJOR                  3
#define HV_MINOR                  0
#elif ARTEFACT_SPECIES == EX_CHIRPER
#define HV_MAJOR                  0
#define HV_MINOR                  0
#elif ARTEFACT_SPECIES == EX_CHIPPER
#define HV_MAJOR                  1
#define HV_MINOR                  0
#elif ARTEFACT_SPECIES == EX_CLAPPER
#define HV_MAJOR                  1
#define HV_MINOR                  0
#elif ARTEFACT_SPECIES == EX_SPINNER
#define HV_MAJOR                  1
#define HV_MINOR                  0
#elif ARTEFACT_SPECIES == EX_WINDER
#define HV_MAJOR                  2
#define HV_MINOR                  0
#elif ARTEFACT_SPECIES == LEG_MAJOR
#define HV_MAJOR                  1
#define HV_MINOR                  2
#else
#error ARTEFACT_SPECIES is not set to supported species
#endif

#if ARTEFACT_GENUS == LEGATUS
float USER_CONTROL_PLAYBACK_GAIN             = 0.0;
#endif

//////////////////////////////////////////////////////////////////////////////////////////
////////////////////// Behaviour Routine /////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/* This is where the run-time behaviour of the artefact is determined, each genus provides
    several Behaviour Routines to select from:

Speculator:
   CICADA_MODE
   PITCH_MODE
   TEST_MODE

Explorator:
    TODO

Legatus:
 MATCH_PITCH_ACTIVE = false;
 FEEDBACK_ACTIVE = false;
 FM_FEEDBACK_ACTIVE = true;
 PLAYBACK_ACTIVE = false;

*/

// TODO - these need to be replaced with Macros to fix the mode-selection for Legatus
int MATCH_PITCH_ACTIVE = false;
int FEEDBACK_ACTIVE = false;
int FM_FEEDBACK_ACTIVE = false;
int PLAYBACK_ACTIVE = true;

#if ARTEFACT_GENUS == SPECULATOR
#define BEHAVIOUR_ROUTINE             PITCH_MODE
#elif ARTEFACT_GENUS == LEGATUS
#define BEHAVIOUR_ROUTINE             MODULAR_LEGATUS_MODE
int ARTEFACT_BEHAVIOUR =          PLAYBACK_MODE;
// this needs to be a number which does not correspond to a mode so the audio system connects properly when needed.
int LAST_ARTEFACT_BEHAVIOR =      -1;
#else //
#define BEHAVIOUR_ROUTINE             PLAYBACK_MODE
#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////// Visual Feedback System //////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

///////////////////////// Brightness Scaling /////////////////////////////////////
/* There are several stages in which the brightness of LED feedback is scaled:

LED lighting events are determined by the artefact's species-specific firmware
lighting events are calculated in the HSB or RGBW colour space

lux_brightness_scaler ranges from TODO to TODO and is determine by ambient lighting conditions
user_brightness_scaler ranges from TODO to TODO and is determined by position of physical user controls

*/

// which pot will be used to control thebrightness overide
// if USER_BS_ACTIVE is set to true the user will scale the natural
// brightness levels (in pitch mode only) before being sent to the neopixel
// manager (which then might constrain according to Lux levels)
#if ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_WINDER
float user_brightness_scaler               = 1.0;
#elif ARTEFACT_GENUS == LEGATUS 
float user_brightness_scaler               = 0.0;
#else
float user_brightness_scaler               = 1.0;
#endif

///////////////////////// Saturation  /////////////////////////////////////
// TODO finish integrating this
/*
When the code is within the HSB colour space, there is an option to 
offset the saturation level according to this value
*/
float ADDED_SATURATION  = 0.4;


//////////////////////////////////////////////////////////
///////////////////////////////////////////////////////// 
// Including the Appropiate configuration files depending on
// the artefact type, no user adjustments are needed from this point onwards in the file
#if ARTEFACT_GENUS == SPECULATOR
#include "Configuration_Speculator.h"
#elif ARTEFACT_GENUS == EXPLORATOR
#include "Configuration_Explorator.h"
#elif ARTEFACT_GENUS == LEGATUS
#include "Configuration_Legatus.h"
#endif

// The hardware configuration file contains all the information for the PCB's
// it will set the right directives according to information in the
// speculator, explorator, and legatus configuration files
#include "Configuration_hardware.h"

#if ARTEFACT_GENUS == SPECULATOR
#define NUM_AMPLIFIERS                1
#define NUM_PEAK_ANAS                 1
#define NUM_RMS_ANAS                  0
#define NUM_FFT                       1
#define NUM_CHANNELS                  1
#else
#define NUM_AMPLIFIERS                1
#define NUM_PEAK_ANAS                 1
#define NUM_RMS_ANAS                  1
#define NUM_FFT                       1
#define NUM_CHANNELS                  1
#endif

///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// Boot Settings //////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

#if ARTEFACT_GENUS == SPECULATOR
// how long will the bot wait until starting the main loop
// this is useful for when it neeeds to bee put into an enclosure and then installed in the environment
uint32_t  BOOT_DELAY      =           (1000 * 60 * 2);
#elif ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_CLAPPER
uint32_t  BOOT_DELAY      =           (10000);
#elif ARTEFACT_GENUS == EXPLORATOR
uint32_t  BOOT_DELAY      =           (1000);
#else
uint32_t  BOOT_DELAY      =           (1000);
#endif
int BOOT_DELAY_ACTIVE    =           false;

///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// Audio Analysis /////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

// if set to true an audio USB object will be created so the audio can be debuged via Audacity
#define AUDIO_USB                       false

// Which Audio features will be activated?
#if ARTEFACT_GENUS == EXPLORATOR
#define PEAK_FEATURE_ACTIVE                    true
#define RMS_FEATURE_ACTIVE                     false
#elif ARTEFACT_GENUS == SPECULATOR
#define PEAK_FEATURE_ACTIVE                    true
#define RMS_FEATURE_ACTIVE                     false
#else
#define PEAK_FEATURE_ACTIVE                    true
#define RMS_FEATURE_ACTIVE                     true
#endif

///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// Ambient Light Sensing //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

// the options are to either generate a brightness scaler floating point value
// to which all color messages are multiplied against, adjust the maximum and minimum
// values allowed, or conduct both operations

// if LUX_MAPPING_SCHEMA is set to LUX_ADJUSTS_BS, the ambient light sensor's readings will generate a brightness-scaler that
// influences the brightness of all artefact visual feedback
#define LUX_ADJUSTS_BS                  0
//  if LUX_MAPPING_SCHEMA is set to LUX_ADJUSTS_MIN_MAX, the ambient light sensor's readings will adjust 
// the minimum and maximum display brightness values for all feedback
#define LUX_ADJUSTS_MIN_MAX             1
//  if LUX_MAPPING_SCHEMA is set to LUX_ADJUSTS_BS_AND_MIN_MAX, the ambient light sensor's readings will generate a brightness-scaler
// that influences the brightness of all artefact feedback AND adjusts 
// the minimum and maximum display brightness values for all feedback
#define LUX_ADJUSTS_BS_AND_MIN_MAX      2

// it is generally reccomdended that both options are left active for most species
// LUX_MAPPING_SCHEMA is a global varaible instead of a constant to allow 
// for user controls to toggle between different mapping options during runtime if wanted
uint8_t LUX_MAPPING_SCHEMA =            LUX_ADJUSTS_BS_AND_MIN_MAX;

////////////////////////////////// Lux Ambiant Lighting Thresholds /////////////////////
#if (ARTEFACT_GENUS == SPECULATOR) && HV_MAJOR == 2
#define NIGHT_LUX_THRESHOLD             0.25
// this is the threshold in which anything below will just be treated as the lowest reading
#define LOW_LUX_THRESHOLD               5.0
// when a lux of this level is detected the LEDs will be driven with a brightness scaler of 1.0
#define MID_LUX_THRESHOLD               20.0
#define HIGH_LUX_THRESHOLD              1200.0
#define EXTREME_LUX_THRESHOLD           5000.0

#elif (ARTEFACT_GENUS == SPECULATOR) && HV_MAJOR == 3
// the v3 hardware needs higher thresholds as it is brighter and thus needs to decrease its brightness sooner
#define NIGHT_LUX_THRESHOLD             1.0
// this is the threshold in which anything below will just be treated as the lowest reading
#define LOW_LUX_THRESHOLD               20.0
// when a lux of this level is detected the LEDs will be driven with a brightness scaler of 1.0
#define MID_LUX_THRESHOLD               500.0
#define HIGH_LUX_THRESHOLD              1000.0
#define EXTREME_LUX_THRESHOLD           5000.0

#elif (ARTEFACT_GENUS == EXPLORATOR)
#define NIGHT_LUX_THRESHOLD             1.0
// this is the threshold in which anything below will just be treated as the lowest reading
#define LOW_LUX_THRESHOLD               10.0
// when a lux of this level is detected the LEDs will be driven with a brightness scaler of 1.0
#define MID_LUX_THRESHOLD               350.0
#define HIGH_LUX_THRESHOLD              1200.0
#define EXTREME_LUX_THRESHOLD           5000.0

#elif ARTEFACT_GENUS == LEGATUS
#define NIGHT_LUX_THRESHOLD             1.0
// this is the threshold in which anything below will just be treated as the lowest reading
#define LOW_LUX_THRESHOLD               10.0
// when a lux of this level is detected the LEDs will be driven with a brightness scaler of 1.0
#define MID_LUX_THRESHOLD               350.0
#define HIGH_LUX_THRESHOLD              1200.0
#define EXTREME_LUX_THRESHOLD           5000.0

#endif // ARTEFACT_GENUS for Lux thresholds

////////////////////////////////////////////////////////////////
///////////////////////// Weather Manager //////////////////////
////////////////////////////////////////////////////////////////
#if ARTEFACT_GENUS == SPECULATOR && HV_MAJOR == 3
#define WEATHER_MANAGER_PRESENT true
#elif ARTEFACT_GENUS == SPECULATOR && HV_MAJOR == 1 && HV_MINOR == 1
#define WEATHER_MANAGER_PRESENT true
#elif ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_CHIPPER
#define WEATHER_MANAGER_PRESENT false
#elif ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_CHIRPER
#define WEATHER_MANAGER_PRESENT true
#elif ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_CLAPPER
#define WEATHER_MANAGER_PRESENT false
#elif ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_SPINNER
#define WEATHER_MANAGER_PRESENT true
#elif ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_WINDER
#define WEATHER_MANAGER_PRESENT true
#elif ARTEFACT_GENUS == LEGATUS
#define WEATHER_MANAGER_PRESENT true
#else
#define WEATHER_MANAGER_PRESENT false
#endif // if weather manager present?

///////////////////////////////////////////////////////////
////////////////// Weather Manager Effecting Feedback /////
///////////////////////////////////////////////////////////
bool HUMID_OFFSETS_FEEDBACK                  =  false;
#define HUMID_OFFSETS_HUE                       true
#define HUMID_OFFSETS_SAT                       true
#define HUMID_OFFSETS_BGT                       true
#define HUMID_OFFSET_MIN_VAL                    0.2
#define HUMID_OFFSET_MAX_VAL                    0.8
#define MIN_HUMID_OFFSET                        -0.1
#define MAX_HUMID_OFFSET                        0.1
#define HUMID_FEEDBACK_SCALING                  LINEAR_SCALING

bool TEMP_OFFSETS_FEEDBACK                   =  true;
#define TEMP_OFFSETS_HUE                       true
#define TEMP_OFFSETS_SAT                       true
#define TEMP_OFFSETS_BGT                       false
#define TEMP_OFFSET_MIN_VAL                     5
#define TEMP_OFFSET_MAX_VAL                     25
#define MIN_TEMP_OFFSET                         -0.3
#define MAX_TEMP_OFFSET                         0.3
#define TEMP_FEEDBACK_SCALING                  LINEAR_SCALING

bool HUMID_SCALES_FEEDBACK               =  true;
// these are the fo;r the humidity values, not the constraining
#define HUMID_SCALES_HUE                    true
#define HUMID_SCALES_SAT                    false
#define HUMID_SCALES_BGT                    false
#define HUMID_SCALE_MIN_THRESH              0.2
#define HUMID_SCALE_MAX_THRESH                0.8
#define HUMID_SCALE_AMOUNT                  0.5
// where the contrain will focus it's center
#define HUMID_SCALE_CENTER                  0.5

bool TEMP_SCALES_FEEDBACK               =  false;
// these are the fo;r the humidity values, not the constraining
#define TEMP_SCALES_HUE                    false
#define TEMP_SCALES_SAT                    false
#define TEMP_SCALES_BGT                    false
#define TEMP_SCALE_MIN_THRESH              0.2
#define TEMP_SCALE_MAX_THRESH                0.8
#define TEMP_SCALE_AMOUNT                  0.5
// where the contrain will focus it's center
#define TEMP_SCALE_CENTER                  0.5

/////////////////////////////// Lighting Conditions ///////////////////////////////////
// TODO this should be triggered by the light sensor and then determine the mapping
#define LC_NIIGHT                       0
#define LC_DIM                          1
#define LC_NORMAL                       2
#define LC_BRIGHT                       3
// TODO use this properly
uint8_t LIGHTING_CONDITION = LC_NORMAL;

///////////////////////////////// Lux Adjusts Min Max /////////////////////////////////
// These are determined by the LUX_READINGS, any values se here will just serve as a
// default value which will be overwriten once a lux reading is taken.
// PLEASE NOTE: the MAX_BRIGHTNEESS is for all three color channels not just a single channel
// so for instance if rgb = 150, 150, 150. that would be limited if the MAX_BRIGHTNESS IS 450
// that makes it so a MAX_BRIGHTNESS is actually 765
// lux_manager, luxmanager, lux manager
uint16_t  MIN_BRIGHTNESS =              0;

#if (ARTEFACT_GENUS == SPECULATOR) && (HV_MAJOR == 3)
// uint16_t  MAX_BRIGHTNESS =              765;
uint16_t  MAX_BRIGHTNESS =              765;
#else
uint16_t  MAX_BRIGHTNESS =              765;
#endif

///////////////////////////////// Lux Adjusts Brightness Scaler /////////////////////////////////
// on scale of 0-1.0 what is the min multiplier for the user defined brightness scaler
// 0.05 was too low, did not provide good enough feedback for the night time
#if (ARTEFACT_GENUS == SPECULATOR) || (ARTEFACT_GENUS == LEGATUS)
#define LUX_BS_MIN                      0.1
#define LUX_BS_MAX                      1.50
#else
#define LUX_BS_MIN                      0.75
#define LUX_BS_MAX                      1.50
#endif
/////////////////////////////// Update Regularity //////////////////////////
uint32_t lux_max_reading_delay =        1000 * 60 * 1;   // every 3 minute
uint32_t lux_min_reading_delay =        1000 * 2;       // ten seconds

#if ARTEFACT_GENUS == SPECULATOR && HV_MAJOR > 2
#define USER_BRIGHT_THRESH_OVERRIDE            true
#else
#define USER_BRIGHT_THRESH_OVERRIDE            false
#endif//HV_MAJOR

#if BEHAVIOUR_ROUTINE == CICADA_MODE
float user_brightness_cuttoff = 0.15;
#elif BEHAVIOUR_ROUTINE == PITCH_MODE && HV_MAJOR == 2
float user_brightness_cuttoff = 0.01;
#elif BEHAVIOUR_ROUTINE == PITCH_MODE && HV_MAJOR == 3
float user_brightness_cuttoff = 0.05;
#else 
float user_brightness_cuttoff = 0.0;
#endif//BEHAVIOUR_ROUTINE

///////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Neopixel LEDs /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
// this is needed for the forced lux
#define UPDATE_ON_OFF_RATIOS            false

/////////////////////////// Onsets //////////////////////
#if ARTEFACT_GENUS == SPECULATOR
#if BEHAVIOUR_ROUTINE == CICADA_MODE
#define ONSET_DETECTION_ACTIVE        true
#elif BEHAVIOUR_ROUTINE == PITCH_MODE
#define ONSET_DETECTION_ACTIVE        false
#endif// BEHAVIOUR_ROUTINE
#elif ARTEFACT_GENUS == EXPLORATOR
#define ONSET_DETECTION_ACTIVE        true
#else
#define ONSET_DETECTION_ACTIVE        false
#endif // ARTEFACT_GENUS

// when onset detection is active, this will be the threshold
double ONSET_THRESH =                   1.0;

// this color is used for flashes
#define ONSET_RED                       200
#define ONSET_GREEN                     200
#define ONSET_BLUE                      255

////////////////////////// LED MAPPING MODES ////////////////
#define LED_MAPPING_STANDARD      0
#define LED_MAPPING_INSIDE_OUT    1
#define LED_MAPPING_ROUND         2
#define LED_MAPPING_BOTTOM_UP     3
#define LED_MAPPING_CLOCK_HAND    4
#define LED_MAPPING_OUTSIDE_IN    5
#define LED_MAPPING_CLOCK_FILL    6
#define LED_MAPPING_CUSTOM        7

#if ARTEFACT_GENUS == SPECULATOR && HV_MAJOR == 3
// PLEASE NOTE - this must be set to standard if the FFT led mapping is used...
int LED_MAPPING_MODE = LED_MAPPING_INSIDE_OUT;
#elif COLOR_MAPPING_MODE == LED_MAPPING_FFT
int LED_MAPPING_MODE = LED_MAPPING_STANDARD;
#else
int LED_MAPPING_MODE = LED_MAPPING_STANDARD;
#endif //HV_MAJOR

#if ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_CLAPPER
// Which pin on the Arduino is connected to the NeoPixels? 8 for old board
#define LED1_PIN 8
// note that if the second LED channel is used the teensy needs to be overclocked to 120 MHz
#define LED2_PIN 5
#define LED3_PIN 10

#elif ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_CHIPPER
// Which pin on the Arduino is connected to the NeoPixels? 8 for old board
#define LED1_PIN 10
// note that if the second LED channel is used the teensy needs to be overclocked to 120 MHz
#define LED2_PIN 8
#define LED3_PIN 10

#elif ARTEFACT_GENUS == EXPLORATOR && HV_MAJOR == 2
#define LED1_PIN 5
#define LED2_PIN 10
#define LED3_PIN 25

#elif ARTEFACT_GENUS == LEGATUS && HV_MAJOR == 1 && HV_MINOR == 1
#define LED1_PIN 5
#define LED2_PIN 24
#define LED3_PIN 25

#else
#define LED1_PIN 5
// note that if the second LED channel is used the teensy needs to be overclocked to 120 MHz
#define LED2_PIN 8
#define LED3_PIN 10

#endif

// what the names of the neopixel strips will be
#if (ARTEFACT_GENUS == EXPLORATOR) && (ARTEFACT_SPECIES == EX_CHIRPER)
#define LED1_NAME      "small"
#define LED2_NAME      "medium"
#define LED3_NAME      "large"
#elif (ARTEFACT_GENUS == EXPLORATOR) && (ARTEFACT_SPECIES == EX_CHIPPER)
#define LED1_NAME      "Eye Stock"
#define LED2_NAME      "Pecker"
#define LED3_NAME      "N/A"
#elif (ARTEFACT_GENUS == EXPLORATOR) && (ARTEFACT_SPECIES == EX_CLAPPER)
#define LED1_NAME      "Inside Ring"
#define LED2_NAME      "N/A"
#define LED3_NAME      "N/A"
#elif (ARTEFACT_GENUS == EXPLORATOR) && (ARTEFACT_SPECIES == EX_WINDER)
#define LED1_NAME      "Main PCB"
#define LED2_NAME      "Sensor Stock"
#define LED3_NAME      "N/A"
#elif (ARTEFACT_GENUS == EXPLORATOR) && (ARTEFACT_SPECIES == EX_SPINNER)
#define LED1_NAME      "Bottom Ring"
#define LED2_NAME      "N/A"
#define LED3_NAME      "N/A"
#elif (ARTEFACT_GENUS == SPECULATOR)
#define LED1_NAME      "All"
#define LED2_NAME      "N/A"
#define LED3_NAME      "N/A"
#elif ARTEFACT_GENUS == LEGATUS
#define LED1_NAME      "All"
#define LED2_NAME      "N/A"
#define LED3_NAME      "N/A" 
#endif

// How many NeoPixels are attached to the Arduino?
// if no LEDs are connected to the line then set this to 0
#if (ARTEFACT_GENUS == SPECULATOR) && (HV_MAJOR < 3)
#define LED1_COUNT 16
#define LED2_COUNT 0
#define LED3_COUNT 0
#elif (ARTEFACT_GENUS == SPECULATOR) && (HV_MAJOR == 3)
#define LED1_COUNT 40
#define LED2_COUNT 0
#define LED3_COUNT 0
#elif (ARTEFACT_GENUS == EXPLORATOR) && (ARTEFACT_SPECIES == EX_CHIRPER)
#define LED1_COUNT 10
#define LED2_COUNT 10
#define LED3_COUNT 10
#elif (ARTEFACT_GENUS == EXPLORATOR) && (ARTEFACT_SPECIES == EX_CLAPPER)
#define LED1_COUNT 20
#define LED2_COUNT 0
#define LED3_COUNT 0
#elif (ARTEFACT_GENUS == EXPLORATOR) && (ARTEFACT_SPECIES == EX_CHIPPER)
#define LED1_COUNT 10
#define LED2_COUNT 0
#define LED3_COUNT 0
#elif (ARTEFACT_GENUS == EXPLORATOR) && (ARTEFACT_SPECIES == EX_SPINNER)
#define LED1_COUNT 10
#define LED2_COUNT 0
#define LED3_COUNT 0
#elif (ARTEFACT_GENUS == EXPLORATOR) && (ARTEFACT_SPECIES == EX_WINDER)
#define LED1_COUNT 20
#define LED2_COUNT 0
#define LED3_COUNT 0
#elif ARTEFACT_GENUS == LEGATUS
#define LED1_COUNT 20
#define LED2_COUNT 0
#define LED3_COUNT 0
#endif

#if LED1_COUNT > 0
#define LED1_ACTIVE     true
#else
#define LED1_ACTIVE     false
#endif //LED1_COUNT

#if LED2_COUNT > 0
#define LED2_ACTIVE     true
#else
#define LED2_ACTIVE     false
#endif //LED2_COUNT

#if LED3_COUNT > 0
#define LED3_ACTIVE     true
#else
#define LED3_ACTIVE     false
#endif //LED3_COUNT

#define LED_ACTIVE_CHANNELS (LED1_ACTIVE + LED2_ACTIVE + LED3_ACTIVE)

// if false, a onset detected on either side results in a LED flash on both sides
// if true, a onset detected on one side will only result in a flash on that side
int INDEPENDENT_ONSETS =               false; // WARNING NOT IMPLEMENTED - TODO
int FLASH_DOMINATES =                  false;

// if this is true then the brightness will b = (b + b) * b; in order to reduce its value, and make loud events even more noticable
int SQUARE_BRIGHTNESS =                false;

#if BEHAVIOUR_ROUTINE == CICADA_MODE
bool SATURATED_COLORS =                 false;
#else
bool SATURATED_COLORS =                 true;
#endif

// how high the onset flash timer will go up to
#define MAX_FLASH_TIME                  60
// where the onset flash timer will start
#define MIN_FLASH_TIME                  40
// the amount of time that the LEDs need to be shutdown to allow lux sensors to get an accurate reading
#define FLASH_DEBOUNCE_TIME             80

const bool led_channel_active[3] = {LED1_ACTIVE, LED2_ACTIVE, LED3_ACTIVE};
const uint8_t num_active_led_channels = LED1_ACTIVE + LED2_ACTIVE + LED3_ACTIVE;
const uint16_t max_led_count = max(max(LED1_COUNT, LED2_COUNT), LED3_COUNT);

// these should be 3
#if ARTEFACT_GENUS == SPECULATOR_GENUS || ARTEFACT_GENUS == LEGATUS_GENUS
byte drawingMemory[1][max_led_count * 3];       //  3 bytes per LED
DMAMEM byte displayMemory[1][max_led_count * 12]; // 12 bytes per LED
#else
byte drawingMemory[3][max_led_count * 3];       //  3 bytes per LED
DMAMEM byte displayMemory[3][max_led_count * 12]; // 12 bytes per LED
#endif

///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// Weather Manager ////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
#define WM_UPDATE_DELAY                     1000
// what is the threshold that will determine when Humidity shutddown is activated. .50 equates to 50%
#define HUMID_EXTREME_THRESH                95
// what temp is the thrsehold (in c)
// TODO what is a good value for this?
#define TEMP_EXTREME_THRESH                 60
// 0.2 equates to 20% of the threshold, so if the threshold is 100 and
// this value is 0.2 the Speculator will reactivate once the temperature
// drops to 80 degrees
#define TEMP_HISTERESIS                     0.15
#define WEATHER_MANAGER_UPDATE_RATE         3000

///////////////////////////////////////////////////////////////////////////////////////
/////////////////////////      FFTManager  ////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
// When calculating things such as which bin has the most energy and so on,
// what bin is considered the "1st?" and which one is the last?
// bin two covers 86 - 129 Hz
#define FFT_LOWEST_BIN              3
// this is 16340 Hz
#define FFT_HIGHEST_BIN             380
// todo this needs to be calculated better?
#define FFT_NUM_BINS                (FFT_HIGHEST_BIN - FFT_LOWEST_BIN)

// used the scaled FFT readings or the normal FFT readings, the scaled readings will eensure that
// all the bins of intrest will have their magnitudes add up to 1, thus is best used for determining the centroid within a sub frequency range (for instance 8k - 14k or something
#define SCALE_FFT_BIN_RANGE             false
#define FFT_FEATURES_ACTIVE             1

///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// User Controls //////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

// TODO - buttons should be linked to inturrupts

// in ms, how often will theUI controls update?
#if ((ARTEFACT_GENUS == SPECULATOR) && (HV_MAJOR == 2))
#define UI_POLLING_RATE                 1000
#elif ARTEFACT_GENUS == SPECULATOR && HV_MAJOR == 3
#define UI_POLLING_RATE                 500
#elif ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_CLAPPER
#define UI_POLLING_RATE                 600000
#elif ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_SPINNER
#define UI_POLLING_RATE                 20
#elif ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_WINDER
#define UI_POLLING_RATE                 20
#else
#define UI_POLLING_RATE                 500
#endif // UI_POLLING_RATE

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
///////////////////////////////// Datalogging //////////////////////////////////////////////////
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
#define DATALOG_ACTIVE                  false
#if DATALOG_ACTIVE
#define PRINT_EEPROM_ON_BOOT            true
#else
#define PRINT_EEPROM_ON_BOOT            false
#endif

#if DATALOG_ACTIVE == true
#include "Configuration_datalogging.h"
// perform a write check on everything that is written to EEPROM
#define EEPROM_WRITE_CHECK              true
#endif // DATALOG_ACTIVE

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
///////////////////////////////// Debug Printing ///////////////////////////////////////////////
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
#define P_LEDS_ON                       false
// print lux debug mostly prints info about when extreme lux is entered and
// other things in the lux manager, it is reccomended to leave this printing on
#define P_LED_ON_RATIO                  false
#define P_COLOR                         false
#define P_COLOR_WIPE                    false

////////////////////////////// weather manager ////////////////////////
#define P_WEATHER_MANAGER_READINGS      false

///////////////////////// NeoPixels and Colour ////////////////////////
#define P_HSB                           false
#define P_SMOOTH_HSB                    false
#define P_SATURATION                    true 
#define P_HUE                           true 
#define P_BRIGHTNESS                    true 

#define P_NEO_COLORS                    true

//////////////////////// LuxManager and Ambiant Lighting ///////////////
#define P_LUX_MANAGER_READINGS          false
#define P_EXTREME_LUX                   false
// sets p_lux_readings within the lux_manager but also the NeoPixelManager
#define P_LUMIN                         false
// sets p_lux within the lux_manager
#define P_LUX_READINGS                  false
// sets general debug printing for the lux_manager class
#define P_LUX_MANAGER_DEBUG             false

// sets print_brightness_scaler within the lux_manager
#define P_BS                            true

//
#define P_CALCULATE_BRIGHTNESS_LENGTH   false

// this is where the final brightness scaler is applied
#define P_PACK_COLORS                   false

#define P_SONG_GENERAL                  false
#define P_SONG_COLOR                    false

// basically do you want to print the number of song updates which occur every second?
#define P_NUM_SONG_UPDATES              false
#if P_NUM_SONG_UPDATES == 1
uint16_t song_updates = 0;
elapsedMillis song_update_timer = 0;
#endif // P_NUM_SONG_UPDATES

#define P_UPDATE_SONG_LENGTH                      false

//////////////////////////// Onset Functionality ///////////////////////
#define P_ONSET_FEATURES                          true 
#define P_ONSET                                   false

//////////////////////////// AutoGain //////////////////////////////////
#define P_AUTOGAIN                                false

// for calculating the dominate channel, it works best if the dominate channel is re-caculated every

/////////////////////////// Datalogging ////////////////////////////////
#define P_LOG_WRITE                               true
#define DLM_PRINT                                 false
// if LOOP_LENGTH is set to true the program will keep track of how long it takes to go through
// the main loop, it will also store the min and max loop length values as well as calculate
// what the average loop length is
// TODO - this needs to be implemented
#define P_LOOP_LENGTH                             false
///////////////////////// Audio Stuff ///////////////////////////////////////
#define P_RMS_VALS                                false
#define P_PEAK_VALS                               false
// #define P_TONE_VALS                     false
#define P_FREQ_VALS                               false
#define P_AUDIO_USAGE_MAX                         false
#define P_DOMINATE_CHANNEL                        false

// prints the max audio memory usage (to help calibrate how much is allocated to the system)
#if P_AUDIO_USAGE_MAX == true
elapsedMillis last_audio_usage_print;
#define AUDIO_USAGE_POLL_RATE                     5000
#endif

//////////////////////////// FFT ///////////////////////////////////////////
#define P_FFT_VALS                                false
// will print spectral flux if flux_active
#define P_FLUX_VALS                               false
#define P_ONSET_FLUX                              false
// will print centroid if centroid_active
#define P_CENTROID_VALS                           false
// will print highest energy bin in FFT
//////////////////// General Purpose //////////////////////////////////

// activates some printing which will print out how long different functions calls take
#define P_FUNCTION_TIMES                          false

//////////////////////////// EEPROM ///////////////////////////////////
// set to true if you want to print out data stored in EEPROM on boot
#define P_EEPROM_CONTENTS                         false

////////////////////////////////////////////////////////////////////////////
/////////////////////////// User Controls //////////////////////////////////
////////////////////////////////////////////////////////////////////////////

// will print readings from jumpers and pots
#define P_USER_CONTROLS                           false

///////////////////////// Solenoids //////////////////////////////////////////
#define P_SOLENOID_DEBUG                          false

//////////////////////////////////////////////////////////////////////////////
//////////////////////////// GLOBAL VARIABLES ////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

// for mapping the target color to real colors
#define RED_LOW                                   0
#define RED_HIGH                                  50

#define GREEN_LOW                                 150
#define GREEN_HIGH                                50

#define BLUE_LOW                                  55
#define BLUE_HIGH                                 255

// options are SPECTRAL_CENTROID or SPLIT_BAND
#define SPLIT_BAND                                10
#define COLOR_FEATURE                             SPLIT_BAND

//////////////////////////////// Global Variables /////////////////////////
double color_feature_min =                                1.00;
double color_feature_max =                                0.0;

elapsedMillis feature_reset_tmr;
elapsedMillis last_led_update_tmr;
const unsigned long led_refresh_rate =                    33; // for 30 updates a second
const unsigned long feature_reset_time =                  (1000 * 2.5);// every 2.5 minute?

double brightness_feature_min =                           1.0;
double brightness_feature_max =                           0.0;

double current_brightness =                               1.0;
double last_brightness =                                  1.0;

double current_color =                                    0.5;
double last_color =                                       0.5;

double last_feature;
double current_feature;

////////////////////////////////////////////////////////////////////////////
///////////////////////// Auto-Gain Settings ///////////////////////////////
////////////////////////////////////////////////////////////////////////////
// the autogain active flag currently also controls the dominate microphone selection process
// will autogain based on the LED ON/OFF time be active?
#if ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_CLAPPER
int AUTOGAIN_ACTIVE      =                               false;
#elif ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_SPINNER
int AUTOGAIN_ACTIVE      =                               false;
#elif ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_WINDER
int AUTOGAIN_ACTIVE      =                               false;
#elif ARTEFACT_GENUS == SPECULATOR
int AUTOGAIN_ACTIVE      =                               false;
#elif ARTEFACT_GENUS == LEGATUS
int AUTOGAIN_ACTIVE      =                               false;
#else
int AUTOGAIN_ACTIVE      =                               true;
#endif
// how much to adjust the gain in the autogain routine. A 1.0 corresponds to
// potentially doubling or halfing the gain. A 0.5 will potentially result in
// adding 50% to the gain or reducing it by 25%
#define AUTOGAIN_MAX_GAIN_ADJ                          (double)0.25

// maximum amount of gain (as a proportion of the current gain) to be applied in the
// auto gain code. This value needs to be less than 1. 0.5 would mean that the gain  can change
// by a factor of half its current gain. So, if the gain was 2.0 then it could be increased/decreased by 1.0
// with a max/min value of 1.0 / 3.0.
#define AUTOGAIN_ON_RATIO_TARGET                        0.5
#define AUTOGAIN_ON_RATIO_TOLERANCE                     0.3
#define AUTOGAIN_ON_RATIO_MIN                           0.05
#define AUTOGAIN_ON_RATIO_MAX                           0.8

#define AUTOGAIN_RMS_TARGET                             0.075
#define AUTOGAIN_RMS_TOLERANCE                          0.025
#define AUTOGAIN_RMS_MIN                                0.025
#define AUTOGAIN_RMS_MAX                                0.15

#define AUTOGAIN_PEAK_TARGET                            0.5
#define AUTOGAIN_PEAK_TOLERANCE                         0.2
#define AUTOGAIN_PEAK_MIN                               0.1
#define AUTOGAIN_PEAK_MAX                               0.9

// should be at least 60 seconds so enough data can build up for a meaningful update...
#define AUTOGAIN_INITIAL_DELAY                          (1000 * 30)
// how often to calculate auto-gain (in ms)
#define AUTOGAIN_UPDATE_RATE                            (1000 * 60 * 0.5)

#define AUTOGAIN_TRACK_ON_RATIO                         false
#define AUTOGAIN_TRACK_RMS                              true
#define AUTOGAIN_TRACK_PEAK                             false

// what is the minimum and maximum gains which will be allowed by the program
// this is really related to the autogain routine, this is here to prevent run-away
// and could potentially be used to identify broken microphones (if the mic needs a
// gain outside of these limits it is likely defective...)
// IMPORTANT - these values need to be floats
#define AUTOGAIN_MIN_GAIN                               (double)1.0
#define AUTOGAIN_MAX_GAIN                               (double)3000.0

#if ARTEFACT_GENUS == LEGATUS && BEHAVIOUR_ROUTINE == FM_FEEDBACK_MODE
const float min_playback_gain = 0.0001;
const float mid_playback_gain = 0.15;
const float max_playback_gain = 1.0;
#elif ARTEFACT_GENUS == LEGATUS
const float min_playback_gain = 0.02;
const float mid_playback_gain = 0.25;
const float max_playback_gain = 1.0;
#endif // playback gains for legatus

////////////////////////////// Dominate Channel /////////////////////
#define CALCULATE_DOMINATE_CHANNEL                      false

#if CALCULATE_DOMINATE_CHANNEL == true
uint64_t first_dominate_channel_calculation =           1000 * 60 * 2;  // starting two minutes into the program
uint32_t dominate_channel_update_rate =                 1000 * 60 * 10; // every ten minutes
elapsedMillis dominate_channel_update =                 0;
#endif
int dominate_channel =                                  0;

////////////////////////////////////////////////////////////////////////////
///////////////////////// FFT Manager //////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#if ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_CLAPPER
uint8_t num_fft_managers =                              1;
#elif ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_SPINNER
uint8_t num_fft_managers =                              1;
#elif ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_WINDER
uint8_t num_fft_managers =                              1;
#elif ARTEFACT_GENUS == SPECULATOR && HV_MAJOR == 2
uint8_t num_fft_managers =                              1;
#elif ARTEFACT_GENUS == SPECULATOR && HV_MAJOR == 3
uint8_t num_fft_managers =                              1;
#elif ARTEFACT_GENUS == LEGATUS 
uint8_t num_fft_managers =                              1;
#else
uint8_t num_fft_managers =                              2;
#endif

#if ARTEFACT_GENUS == SPECULATOR && BEHAVIOUR_ROUTINE == CICADA_MODE
#define CENTROID_FEATURE_MIN                            4000
#define CENTROID_FEATURE_MAX                            16000
#elif ARTEFACT_GENUS == SPECULATOR && BEHAVIOUR_ROUTINE == PITCH_MODE
#define CENTROID_FEATURE_MIN                            200
#define CENTROID_FEATURE_MAX                            20000
#elif ARTEFACT_GENUS == EXPLORATOR
#define CENTROID_FEATURE_MIN                            120
#define CENTROID_FEATURE_MAX                            20000
#elif ARTEFACT_GENUS == LEGATUS
#define CENTROID_FEATURE_MIN                            120
#define CENTROID_FEATURE_MAX                            24000
#endif

////////////////////////////////////////////////////////////////////////////
///////////////////////// User Controls ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
///////////////////////////// P1 - Brightness Overide ////////////////////////

const float min_user_brightness_scaler           = 0.01;
const float mid_user_brightness_scaler           = 0.7;
const float max_user_brightness_scaler           = 2.0;

const float min_user_brightness_cuttoff           = 0.0;
const float mid_user_brightness_cuttoff           = 0.3;
const float max_user_brightness_cuttoff           = 1.3;

// this will determine if the USER will have control over the brightness scaler
// at this point v2.1 does this via jumpers and v3.0 does this via a pot
// so the default value is true
#if ARTEFACT_GENUS == LEGATUS
#define USER_BS_ACTIVE                        false
#else
#define USER_BS_ACTIVE                        false
#endif

int but_test[NUM_BUTTONS];
float pot_test[NUM_POTS];

////////////////////////////////////////////////////////////////////////////
///////////////////////// Neo Manager //////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#if ARTEFACT_GENUS == SPECULATOR
#define NUM_NEOP_MANAGERS                               1
#elif ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_CLAPPER
#define NUM_NEOP_MANAGERS                               1
#elif ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_SPINNER
#define NUM_NEOP_MANAGERS                               1
#elif ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_WINDER
#define NUM_NEOP_MANAGERS                               1
#elif ARTEFACT_GENUS == EXPLORATOR
#define NUM_NEOP_MANAGERS                               3
#elif ARTEFACT_GENUS == LEGATUS         
#define NUM_NEOP_MANAGERS                               1
#endif

// what factor will the new values be scaled by compared to the old values
// when calculating the song brightness target vs current levels a 1.0 will turn off
// the low filtering so only the new values will be used while 0.5 will result in the
// average of the old and new value to be used, a higher value will be a quicker responce
// the max value is 1.0 and the min value is 0.0
#define HUE_LP_LEVEL                          0.02
#define SATURATION_LP_LEVEL                   0.02
#define BRIGHTNESS_LP_LEVEL                   0.02

// if > 0 then the brightness will be smoothed with a previous value
// thee higher the value the more it is smoothed
#define HUE_DECAY_DELAY      15000
#define HUE_DECAY_FACTOR     0.1

#define SAT_DECAY_DELAY      15000
#define SAT_DECAY_FACTOR     0.1

#define BGT_DECAY_DELAY      15000
#define BGT_DECAY_FACTOR     0.1

#define BGT_MIN_UPDATE_FACTOR BRIGHTNESS_LP_LEVEL
#define BGT_MAX_UPDATE_FACTOR BRIGHTNESS_LP_LEVEL

#define SAT_MIN_UPDATE_FACTOR SATURATION_LP_LEVEL
#define SAT_MAX_UPDATE_FACTOR SATURATION_LP_LEVEL

#define HUE_MIN_UPDATE_FACTOR HUE_LP_LEVEL
#define HUE_MAX_UPDATE_FACTOR HUE_LP_LEVEL

double hue = 0.5;
double brightness = 0.5;
double saturation = 0.5;// needs to start at 0.0 or else the min/max value tracker has issues

ValueTrackerDouble hue_tracker        = ValueTrackerDouble((String)"HUE", &hue, HUE_DECAY_FACTOR, HUE_DECAY_DELAY, HUE_LP_LEVEL);
ValueTrackerDouble saturation_tracker = ValueTrackerDouble((String)"SATURATION", &saturation, SAT_DECAY_FACTOR, SAT_DECAY_DELAY, SATURATION_LP_LEVEL);
ValueTrackerDouble brightness_tracker = ValueTrackerDouble((String)"BRIGHTNESS", &brightness, BGT_DECAY_FACTOR, BGT_DECAY_DELAY, BRIGHTNESS_LP_LEVEL);

////////////////////////////////////////////////////////////////////////////
// TODO -- need to implement the target hue saturation and brightness mapping schema
float target_hue = 1.0;
float target_saturation = 1.0;
float target_brightness = 1.0;
////////////////////////////////////////////////////////////////////////////
// WARNING!!!!!! DO NOT USE THIS FEATURE!!!!!
int USE_TARGET_BRIGHTNESS = false;
int USE_TARGET_HUE        = false;
int USE_TARGET_SATURATION = false;

////////////////////////////////////////////////////////////////////////////
///////////////////////// Audio System /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// for speculator cicada installations channel 1 is song and channel 2 is click?
// TODO - the input to the two different sides should be the dominate microphones

// audio usage loggings
elapsedMillis last_usage_print =              0;// for keeping track of audio memory usage

// this is dictated by user controls and is multiplied against the STARTING_GAIN to determine runtime gain
float USER_CONTROL_GAIN_ADJUST               = 1.0;

// TODO - implement this
#define SPH_MICROPHONE                        0
#define TDK_MICROPHONE                        1
#define MICROPHONE_TYPE                       TDK_MICROPHONE

#if ARTEFACT_GENUS == SPECULATOR && HV_MAJOR == 2
#define STARTING_GAIN                         12.0
#elif ARTEFACT_GENUS == SPECULATOR && HV_MAJOR == 3
// 30.0 is good for testing when no enclosure is present, but a higher value should be used when an enclosure is present
// 240.00 is good for the better mics?
#define STARTING_GAIN                         40.0
#elif ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_CLAPPER
#define STARTING_GAIN                         20.0
#elif ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_SPINNER
#define STARTING_GAIN                         20.0
#elif ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_WINDER
#define STARTING_GAIN                         480.0

#elif ARTEFACT_GENUS == EXPLORATOR
#define STARTING_GAIN                         240.0

#elif ARTEFACT_GENUS == LEGATUS
#define STARTING_GAIN                         6.0
#endif

#if ARTEFACT_GENUS == SPECULATOR
#define MAKEUP_GAIN                            2.0
#else
#define MAKEUP_GAIN                            2.0
#endif // makeup_gain

//////////////////////////////// STARTING GAIN //////////////////////////////////////////////
float starting_gain = STARTING_GAIN * ENCLOSURE_GAIN_FACTOR;
/////////////////////////////////////////////////////////////////////////////////////////////

#if ARTEFACT_GENUS == SPECULATOR && BEHAVIOUR_ROUTINE == CICADA_MODE
// SONG HP
#define LBQ1_THRESH         4000
#define LBQ1_Q              0.85
#define LBQ1_DB             -12
// SONG LP
#define LBQ2_THRESH         16000
#define LBQ2_Q              0.85
#define LBQ2_DB             -12
// CLICK HP
#define RBQ1_THRESH         1200
#define RBQ1_Q              0.85
#define RBQ1_DB             -12
// CLICK LP
#define RBQ2_THRESH         3000
#define RBQ2_Q              0.85
#define RBQ2_DB             -12

#elif ARTEFACT_GENUS == SPECULATOR && BEHAVIOUR_ROUTINE == PITCH_MODE
// Mix HP
#define LBQ1_THRESH         400
#define LBQ1_Q              1.0
#define LBQ1_DB             -12
// Mix LP
#define LBQ2_THRESH         20000
#define LBQ2_Q              1.0
#define LBQ2_DB             -12
// Should be Inactive
#define RBQ1_THRESH         400
#define RBQ1_Q              1.0
#define RBQ1_DB             -12
// Should be Inactive
#define RBQ2_THRESH         20000
#define RBQ2_Q              0.85
#define RBQ2_DB             -12
//////////////////
#elif ARTEFACT_GENUS == EXPLORATOR
// SONG HP
#define LBQ1_THRESH         400
#define LBQ1_Q              1.0
#define LBQ1_DB             -12

// SONG LP
#define LBQ2_THRESH         20000
#define LBQ2_Q              1.0
#define LBQ2_DB             -12

// Should be Inactive
#define RBQ1_THRESH         400
#define RBQ1_Q              1.0
#define RBQ1_DB             -12

// Should be Inactive
#define RBQ2_THRESH         20000
#define RBQ2_Q              1.0
#define RBQ2_DB             -12

//////////////////
#elif ARTEFACT_GENUS == LEGATUS
// Microphone HP
#define LBQ1_THRESH         400
#define LBQ1_Q              1.0
#define LBQ1_DB             -12
// Microphone LP
#define LBQ2_THRESH         20000
#define LBQ2_Q              1.0
#define LBQ2_DB             -12

// playback HP
#define RBQ1_THRESH         80
#define RBQ1_Q              1.0
#define RBQ1_DB             -12
// playback LQ
#define RBQ2_THRESH         18000
#define RBQ2_Q              1.0
#define RBQ2_DB             -12

#else
// SONG HP
#define LBQ1_THRESH         400
#define LBQ1_Q              1.0
#define LBQ1_DB             -12
// SONG LP
#define LBQ2_THRESH         20000
#define LBQ2_Q              1.0
#define LBQ2_DB             -12

// Should be Inactive
#define RBQ1_THRESH         400
#define RBQ1_Q              1.0
#define RBQ1_DB             -12
// Should be Inactive
#define RBQ2_THRESH         20000
#define RBQ2_Q              1.0
#define RBQ2_DB             -12
#endif
/////////////////////////////////////////////////////////////////////////
/////////////////////////      FFTManager  //////////////////////////////
/////////////////////////////////////////////////////////////////////////
// used the scaled FFT readings or the normal FFT readings, the scaled readings will eensure that
// all the bins of intrest will have their magnitudes add up to 1, thus is best used for determining the centroid within a sub frequency range (for instance 8k - 14k or something
#define SCALE_FFT_BIN_RANGE                     false
#define FFT_FEATURES_ACTIVE                     1

/////////////////////////////// Color Mapping /////////////////////////////////////
// when calculating the hue for the NeoPixel leds, what feature do you want to use?
// look under Audio Features for the Available Features
double hue_min =                                1.0;
double hue_max =                                0.0;

/////////////////////////////////////////////////////////////////////////
/////////////////////////////// Audio Features //////////////////////////
/////////////////////////////////////////////////////////////////////////
// all the different features that are available to use through the feature collector for
// various mappings.
#define FEATURE_RMS                         (1)
#define FEATURE_RMS_POS_DELTA               (11)
#define FEATURE_RMS_AVG                     (12)
#define FEATURE_PEAK                        (2)
#define FEATURE_PEAK_POS_DELTA              (22)
#define FEATURE_PEAK_AVG                    (23)
#define FEATURE_FREQ                        (3)
#define FEATURE_TONE                        (4)
#define FEATURE_FFT                         (5)
#define FEATURE_FFT_ENERGY                  (6)
#define FEATURE_FFT_RELATIVE_ENERGY         (7)
#define FEATURE_FFT_MAX_BIN                 (8)
#define FEATURE_FFT_BIN_RANGE               (9)
#define FEATURE_STRONG_FFT                  (10)
#define FEATURE_CENTROID                    (13)
#define FEATURE_FLUX                        (14)

// When the color mapping is using HSB, this will be where the features used are determined
#if ARTEFACT_GENUS == SPECULATOR
uint8_t HUE_FEATURE         =               FEATURE_CENTROID;
uint8_t SATURATION_FEATURE  =               FEATURE_FLUX;
uint8_t BRIGHTNESS_FEATURE  =               FEATURE_FFT_ENERGY;
#else
uint8_t HUE_FEATURE         =               FEATURE_CENTROID;
uint8_t SATURATION_FEATURE  =               (FEATURE_FFT_RELATIVE_ENERGY);
uint8_t BRIGHTNESS_FEATURE  =               (FEATURE_FFT_ENERGY);
#endif // ARTEFACT_GENUS == SPECULATOR

int REVERSE_SATURATION     =               true;
int REVERSE_BRIGHTNESS     =               false;
int REVERSE_HUE            =               false;

// These are different color mapping modes
// #define COLOR_MAPPING_FFT                     0
// originated with the pitch mode for the speculator
#define COLOR_MAPPING_HSB                     0
// the simple but effective explorator mapping strategy
#define COLOR_MAPPING_EXPLORATOR              1
// a RGB based color mapping which delegates the energy in a frequency
// range as determining the brightness of the red, green, and blue elements
#define COLOR_MAPPING_FFT                     2

// For the neopixels will the color mapping exist within the RGB or HSB domain?
#if ARTEFACT_GENUS == SPECULATOR
#define COLOR_MAP_MODE                    COLOR_MAPPING_HSB
#elif ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_CLAPPER
// int COLOR_MAP_MODE          =             COLOR_MAPPING_HSB;
int COLOR_MAP_MODE          =             COLOR_MAPPING_HSB;
#elif ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_SPINNER
int COLOR_MAP_MODE          =             COLOR_MAPPING_HSB;
#elif ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_WINDER
int COLOR_MAP_MODE          =             COLOR_MAPPING_HSB;
#else
int COLOR_MAP_MODE          =             COLOR_MAPPING_EXPLORATOR;
#endif

// for the COLOR_MAPPING_FFT mode
#define RED_LOW_FREQ                          100
#define RED_HIGH_FREQ                         400
#define GREEN_LOW_FREQ                        600
#define GREEN_HIGH_FREQ                       2400
#define BLUE_LOW_FREQ                         2000
#define BLUE_HIGH_FREQ                        8000

#define  MODE_SINGLE_RANGE                    0
#define  MODE_ALL_BINS                        1
#define  MODE_BIN_ENERGY                      2

// should the centroid value be smoothed?
#define SMOOTH_CENTROID                       true

#endif // __CONFIGURATION_H__
