#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

// to get some definitions such as SPECULATOR
#include <Macros.h>
#include <ValueTrackerDouble.h>
#include "Configuration_printing_and_tests.h"

/////////////////////// Artefact Genus ////////////////////////////////
// There are three artefact genera currently availible
// SPECULATOR, EXPLORATOR, and LEGATUS
// set ARTEFACT_GENUS to one of these types
/////////////////// TODO
#define ARTEFACT_GENUS            SPECULATOR

//////////////////////////////////////////////////////////////////////
////////////////////// Artefact Species //////////////////////////////
//////////////////////////////////////////////////////////////////////
// Speculator Species include: SPEC_MINOR and SPEC_MAJOR
// Explorator Species include: EX_CHIPPER, EX_CHIRPER, EX_CLAPPER, EX_SPINNER, and EX_WINDER
// Legatus Species include: LEG_MAJOR

#if ARTEFACT_GENUS == EXPLORATOR
#define ARTEFACT_SPECIES                 EX_WINDER
#elif ARTEFACT_GENUS == LEGATUS 
#define ARTEFACT_SPECIES                 LEG_MAJOR
#elif ARTEFACT_GENUS == SPECULATOR
#define ARTEFACT_SPECIES                 SPEC_MINOR
// #define ARTEFACT_SPECIES                 SPEC_MAJOR
#endif

//////////////////////////////////////////////////////////////////////
////////////////////// Behaviour Routine /////////////////////////////
//////////////////////////////////////////////////////////////////////
/* each species can operate using at least one behaviour routine
   the behaviour routine determines the artefact's feedback mapping and general operation
   outside of basic sensor polling 

   Speculator major and Speculator minor curenty have two available behaviour routines
   ------------------------------------------------------------------------------------
   B_TARGETED_FEEDBACK : "cicada" installation where feedback filtering is hard coded to target specific sounds
   B_ADAPTIVE_FEEDBACK : feedback filtering is generated during runtime by firmware to adapt to unknown sonic env.

   Each Explorator Species currently has one Behaviour routine
   ------------------------------------------------------------------------------------

   Legatus major currently support four operating modes
   ------------------------------------------------------------------------------------
   B_LEG_MATCH_PITCH 
   B_LEG_FEEDBACK
   B_LEG_FM_FEEDBACK
   B_LEG_SAMP_PLAYBACK
*/

// Speculator behaviours
#if ARTEFACT_GENUS == SPECULATOR
#define BEHAVIOUR_ROUTINE             B_ADAPTIVE_FEEDBACK
// WARNING - TARGETED FEEDBACK IS CURRENTLY BROKEN!!!!!
// #define BEHAVIOUR_ROUTINE          B_TARGETED_FEEDBACK
#if BEHAVIOUR_ROUTINE != B_TARGETED_FEEDBACK && BEHAVIOUR_ROUTINE != B_ADAPTIVE_FEEDBACK
#error "WRONG BEHAVIOUR_ROUTINE GIVEN, UNABLE TO COMPILE"
#endif // error for wrong behaviour
// Explorator behaviours
#elif ARTEFACT_GENUS == EXPLORATOR
#if ARTEFACT_SPECIES == EX_CHIRPER
#define BEHAVIOUR_ROUTINE             B_CHIRPER_BASIC
#elif ARTEFACT_SPECIES == EX_CHIPPER
#define BEHAVIOUR_ROUTINE             B_CHIPPER_BASIC
#elif ARTEFACT_SPECIES == EX_CLAPPER
#define BEHAVIOUR_ROUTINE             B_CLAPPER_BASIC
#elif ARTEFACT_SPECIES == EX_SPINNER
#define BEHAVIOUR_ROUTINE             B_SPINNER_BASIC
#elif ARTEFACT_SPECIES == EX_WINDER
#define BEHAVIOUR_ROUTINE             B_WINDER_BASIC
#endif // EXPLORATOR species behavour routines
// Legatus behaviours
#elif ARTEFACT_GENUS == LEGATUS

// TODO - warning this behaviour is currently buggy and needs fixing 
// #define BEHAVIOUR_ROUTINE         B_LEG_MATCH_PITCH
// #define BEHAVIOUR_ROUTINE           B_LEG_FEEDBACK
// #define BEHAVIOUR_ROUTINE         B_LEG_FM_FEEDBACK
#define BEHAVIOUR_ROUTINE         B_LEG_SAMP_PLAYBACK

// this variable is used to allow Legatus artefact's to switch between behaviour routines
// after initialisation
int ARTEFACT_BEHAVIOUR =          BEHAVIOUR_ROUTINE;
// this needs to be a number which does not correspond to a mode so the audio system connects properly when needed
int LAST_ARTEFACT_BEHAVIOR =      -1;
#endif // genearting the artefact behaviour based on genus and species

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
#include "Configuration_led_feedback.h"
#include "Configuration_user_controls.h"
#include "Configuration_audio_system.h"

///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// Ambient Light Sensing //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

// the options are to either generate a brightness scaler floating point value
// to which all color messages are multiplied against, adjust the maximum and minimum
// values allowed, or conduct both operations

// if LUX_MAPPING_SCHEMA is set to LUX_ADJUSTS_BS, the ambient light sensor's readings will generate a brightness-scaler that
// influences the brightness of all artefact visual feedback
//  if LUX_MAPPING_SCHEMA is set to LUX_ADJUSTS_MIN_MAX, the ambient light sensor's readings will adjust 
// the minimum and maximum display brightness values for all feedback
//  if LUX_MAPPING_SCHEMA is set to LUX_ADJUSTS_BS_AND_MIN_MAX, the ambient light sensor's readings will generate a brightness-scaler
// that influences the brightness of all artefact feedback AND adjusts 
// the minimum and maximum display brightness values for all feedback
// it is generally reccomdended that both options are left active for most species
// LUX_MAPPING_SCHEMA is a global varaible instead of a constant to allow 
// for user controls to toggle between different mapping options during runtime if wanted
// uint8_t LUX_MAPPING_SCHEMA =            LUX_ADJUSTS_BS_AND_MIN_MAX;
uint8_t LUX_MAPPING_SCHEMA =            LUX_ADJUSTS_BS;
// uint8_t LUX_MAPPING_SCHEMA =            LUX_ADJUSTS_MIN_MAX;

////////////////////////////////// Lux Ambiant Lighting Thresholds /////////////////////
#if ARTEFACT_SPECIES == SPEC_MAJOR
// lux below this threshold triggers a shutdown routines an a bs of 0.0
// lux between night and low result in a reading of min_bs
#define NIGHT_LUX_THRESHOLD             0.25
// readings between low and mid result in a scaling between min_bs and 1.0
#define LOW_LUX_THRESHOLD               10.0
// readings between mid and high result in a scaling between 1.0 and max_bs
#define MID_LUX_THRESHOLD               400.0
// readings between high and extreme high result in a scaling of max_bs
#define HIGH_LUX_THRESHOLD              1200.0
// readings above extreme result in a scaling of 0.0 and a shutdown routine
#define EXTREME_LUX_THRESHOLD           4000.0

#elif ARTEFACT_SPECIES == SPEC_MINOR
// the v3 hardware needs higher thresholds as it is brighter and thus needs to decrease its brightness sooner
#define NIGHT_LUX_THRESHOLD             0.5
// this is the threshold in which anything below will just be treated as the lowest reading
#define LOW_LUX_THRESHOLD               20.0
// when a lux of this level is detected the LEDs will be driven with a brightness scaler of 1.0
#define MID_LUX_THRESHOLD               500.0
#define HIGH_LUX_THRESHOLD              1500.0
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
#if ARTEFACT_GENUS == LEGATUS
#define LUX_BS_MIN                      0.1
#define LUX_BS_MAX                      1.50
#elif ARTEFACT_GENUS == SPECULATOR
#define LUX_BS_MIN                      0.01
#define LUX_BS_MAX                      2.50
#else
#define LUX_BS_MIN                      0.75
#define LUX_BS_MAX                      1.50
#endif
/////////////////////////////// Update Regularity //////////////////////////
uint32_t lux_max_reading_delay =        1000 * 10;   // every 10 seconds
uint32_t lux_min_reading_delay =        1000 * 1;    // one seconds

#if USER_CONTROLS_ACTIVE
#if ARTEFACT_GENUS == SPECULATOR && HV_MAJOR > 2
#define USER_BRIGHT_THRESH_OVERRIDE            true
#else
#define USER_BRIGHT_THRESH_OVERRIDE            false
#endif//HV_MAJOR

#if BEHAVIOUR_ROUTINE == B_TARGETED_FEEDBACK
float user_brightness_cuttoff = 0.00;
#elif BEHAVIOUR_ROUTINE == B_ADAPTIVE_FEEDBACK && HV_MAJOR == 2
float user_brightness_cuttoff = 0.00;
#elif BEHAVIOUR_ROUTINE == B_ADAPTIVE_FEEDBACK && HV_MAJOR == 3
float user_brightness_cuttoff = 0.00;
#else 
float user_brightness_cuttoff = 0.0;
#endif//BEHAVIOUR_ROUTINE
#endif // USER_CONTROLS_ACTIVE

/////////////////////////////// Lighting Conditions ///////////////////////////////////
// TODO this should be triggered by the light sensor and then determine the mapping
// TODO use this properly
// #define LC_NIGHT                        0
// #define LC_DIM                          1
// #define LC_NORMAL                       2
// #define LC_BRIGHT                       3
// uint8_t LIGHTING_CONDITION = LC_NORMAL;


////////////////////////////////////////////////////////////////
///////////////////////// Weather Manager //////////////////////
////////////////////////////////////////////////////////////////
#if ARTEFACT_SPECIES == SPEC_MINOR
#define WEATHER_MANAGER_ACTIVE true
#elif ARTEFACT_SPECIES == SPEC_MAJOR
#define WEATHER_MANAGER_ACTIVE false
#elif ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_CHIPPER
#define WEATHER_MANAGER_ACTIVE false
#elif ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_CHIRPER
#define WEATHER_MANAGER_ACTIVE true
#elif ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_CLAPPER
#define WEATHER_MANAGER_ACTIVE false
#elif ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_SPINNER
#define WEATHER_MANAGER_ACTIVE true
#elif ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_WINDER
#define WEATHER_MANAGER_ACTIVE true
#elif ARTEFACT_GENUS == LEGATUS
#define WEATHER_MANAGER_ACTIVE true
#else
#define WEATHER_MANAGER_ACTIVE false
#endif // if weather manager present?

///////////////////////////////////////////////////////////
////////////////// Weather Manager Effecting Feedback /////
///////////////////////////////////////////////////////////
bool HUMID_OFFSETS_FEEDBACK                  =  true;
#define HUMID_OFFSETS_HUE                       true
#define HUMID_OFFSETS_SAT                       false
#define HUMID_OFFSETS_BGT                       false
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

bool HUMID_SCALES_FEEDBACK               =  false;
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

#if ARTEFACT_GENUS == SPECULATOR && BEHAVIOUR_ROUTINE == B_TARGETED_FEEDBACK
#define CENTROID_FEATURE_MIN                            4000
#define CENTROID_FEATURE_MAX                            16000
#elif ARTEFACT_GENUS == SPECULATOR && BEHAVIOUR_ROUTINE == B_ADAPTIVE_FEEDBACK
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

// TODO - buttons should be linked to inturrupts

// in ms, how often will theUI controls update?
#if USER_CONTROLS_ACTIVE
#if ARTEFACT_SPECIES == SPECULATOR_MAJOR
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

// TODO - also check for if this scaling is actually mapped
const float min_user_brightness_scaler           = 0.01;
const float mid_user_brightness_scaler           = 0.7;
const float max_user_brightness_scaler           = 2.0;

const float min_user_brightness_cuttoff           = 0.0;
const float mid_user_brightness_cuttoff           = 0.3;
const float max_user_brightness_cuttoff           = 1.3;
#endif // USER_CONTROLS_ACTIVE

// this will determine if the USER will have control over the brightness scaler
// at this point v2.1 does this via jumpers and v3.0 does this via a pot
// so the default value is true
#if USER_CONTROLS_ACTIVE
int but_test[NUM_BUTTONS];
float pot_test[NUM_POTS];
#define USER_BS_ACTIVE                        false
#else
#define USER_BS_ACTIVE                        false
#endif // USER_CONTROLS_ACTIVE


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
#define SPH_MICROPHONE                        10
#define TDK_MICROPHONE                        1
#if ARTEFACT_SPECIES == SPECULATOR_MAJOR
#define MICROPHONE_TYPE                       SPH_MICROPHONE
#else
#define MICROPHONE_TYPE                       TDK_MICROPHONE
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// Starting Gain //////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
#if ARTEFACT_GENUS == SPECULATOR 
#if ARTEFACT_SPECIES == SPEC_MAJOR
// most Speculator Major artefacts use the old microphones that require huge amounts of gain...
#define STARTING_GAIN                         1.0
#elif ARTEFACT_SPECIES == SPEC_MINOR
// 30.0 is good for testing when no enclosure is present, but a higher value should be used when an enclosure is present
// 240.00 is good for the better mics?
#define STARTING_GAIN                         240.0
#endif // types of speculator species
#elif ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_CLAPPER
#define STARTING_GAIN                         20.0
#elif ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_SPINNER
#define STARTING_GAIN                         20.0
#elif ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_WINDER
#define STARTING_GAIN                         480.0

#elif ARTEFACT_GENUS == EXPLORATOR
#define STARTING_GAIN                         240.0

#elif ARTEFACT_GENUS == LEGATUS
#if ARTEFACT_BEHAVIOUR == B_LEG_FEEDBACK
#define STARTING_GAIN                         0.5
#else
#define STARTING_GAIN                         6.0
#endif
#endif // makeup_gain

// makeup gain, applied after the filtering stage to boost signal
#if ARTEFACT_GENUS == SPECULATOR
#define MAKEUP_GAIN                            2.0
#else
#define MAKEUP_GAIN                            2.0
#endif


//////////////////////////////// STARTING GAIN //////////////////////////////////////////////
float starting_gain = STARTING_GAIN * ENCLOSURE_GAIN_FACTOR;
/////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

#if ARTEFACT_GENUS == SPECULATOR && BEHAVIOUR_ROUTINE == B_TARGETED_FEEDBACK
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

#elif ARTEFACT_GENUS == SPECULATOR && BEHAVIOUR_ROUTINE == B_ADAPTIVE_FEEDBACK
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
#if BEHAVIOUR_ROUTINE == B_TARGETED_FEEDBACK
uint8_t HUE_FEATURE         =               FEATURE_CENTROID;
uint8_t SATURATION_FEATURE  =               FEATURE_FLUX;
uint8_t BRIGHTNESS_FEATURE  =               FEATURE_RMS;
#elif BEHAVIOUR_ROUTINE == B_ADAPTIVE_FEEDBACK
uint8_t HUE_FEATURE         =               FEATURE_CENTROID;
uint8_t SATURATION_FEATURE  =               FEATURE_FLUX;
uint8_t BRIGHTNESS_FEATURE  =               FEATURE_FFT_RELATIVE_ENERGY;
#endif // speculator behaviour routines
#else
uint8_t HUE_FEATURE         =               FEATURE_CENTROID;
uint8_t SATURATION_FEATURE  =               (FEATURE_FFT_RELATIVE_ENERGY);
uint8_t BRIGHTNESS_FEATURE  =               (FEATURE_FFT_ENERGY);
#endif // ARTEFACT_GENUS == SPECULATOR

int REVERSE_SATURATION     =               true;
int REVERSE_BRIGHTNESS     =               false;
int REVERSE_HUE            =               false;

// These are different color mapping modes
// originated with the pitch mode for the speculator
#define COLOR_MAPPING_HSB                     1
// the simple but effective explorator mapping strategy
#define COLOR_MAPPING_EXPLORATOR              0
// a RGB based color mapping which delegates the energy in a frequency
// range as determining the brightness of the red, green, and blue elements
#define COLOR_MAPPING_FFT                     2

// For the neopixels will the color mapping exist within the RGB or HSB domain?
#if ARTEFACT_GENUS == SPECULATOR
int color_map_mode          =             COLOR_MAPPING_HSB;
#elif ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_CLAPPER
// int color_map_mode          =             COLOR_MAPPING_HSB;
int color_map_mode          =             COLOR_MAPPING_HSB;
#elif ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_SPINNER
int color_map_mode          =             COLOR_MAPPING_HSB;
#elif ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_WINDER
int color_map_mode          =             COLOR_MAPPING_HSB;
#else
int color_map_mode          =             COLOR_MAPPING_EXPLORATOR;
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

//////////////////// Software Revision ////////////////////////////////
#define SV_MAJOR                  0
#define SV_MINOR                  1
#define SV_REVISION               12

#if P_FUNCTION_TIMES == true
elapsedMillis function_times = 0;
long loop_num = 0;
long loop_length = 15;
// for tracking percentage of loops within given bounds
long under_ten = 0;
long over_ninty_nine = 0;
long over_fourty = 0;
long over_twenty = 0;
long over_fifteen= 0;
long max_loop_length = 0;
long min_loop_length = 1000;

// how many times will the test be run before termination
#define loop_test_period 10000

ValueTrackerLong loop_length_value_tracker = ValueTrackerLong("loop_length", &loop_length, 0, 0, 0.0);

void updateFunctionTimeStats() {
   if (loop_num != 0) {
      loop_length = function_times;
      loop_length_value_tracker.update();
      if (loop_length < 10) {
         under_ten++;
      }
      if (loop_length > 99){
         over_ninty_nine++;
         over_fourty++;
         over_twenty++;
         over_fifteen++;
      }else if (loop_length > 40) {
         over_fourty++;
         over_twenty++;
         over_fifteen++;
      }else if (loop_length > 20) {
         over_twenty++;
         over_fifteen++;
      }else if (loop_length > 15) {
         over_fifteen++;
      }
   }
   else{loop_length_value_tracker.resetMinMax();}
   if (loop_length > max_loop_length){
      max_loop_length = loop_length;
   } else if (loop_length < min_loop_length) {
      min_loop_length = loop_length;
   }
    if (loop_num % 100 == 0) {
      loop_length_value_tracker.printStats();
      Serial.print("after a total of ");
      Serial.print(loop_num);
      Serial.println(" loops, the firmware identified ");
      Serial.print(over_ninty_nine);
      Serial.println(" loops over 99ms");
      Serial.print(over_fourty);
      Serial.println(" loops over 40ms");
      Serial.print(over_twenty);
      Serial.println(" loops over 20ms");
      Serial.print(over_fifteen);
      Serial.println(" loops over 15ms");
      Serial.print(under_ten);
      Serial.println(" loops under 10ms");
      Serial.print(max_loop_length);
      Serial.println(" was the longest identified loop length");
      Serial.print(min_loop_length);
      Serial.println(" was the shortest identified loop length");
      Serial.println("-----------------------------------");
      if (loop_num == loop_test_period){
         Serial.println("------------- TEST COMPLETE -----------------");
         Serial.println("|||||||||||||||||||||||||||||||||||||||||||||");
         delay(9999999);
      }
    }
    loop_num++;
    function_times = 0;
}


#endif // P_FUNCTION_TIMES

#endif // __CONFIGURATION_H__
