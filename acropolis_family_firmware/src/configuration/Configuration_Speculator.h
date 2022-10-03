#ifndef __CONFIGURATION_SPECULATOR_H__
#define __CONFIGURATION_SPECULATOR_H__
/* This is the main configuration file for the Moth Framework
   Using this file, along with the other configuration files you
   Can cistomise how the firmware performs.
*/

#include "Configuration_hardware.h"
#include <ValueTrackerLong.h>
#include <Audio.h>
#include <Macros.h>
#include "../lib/PrintUtils/PrintUtils.h"

////////////////////////////////////////////////////////////////////////////
/////////////////////////// MOST IMPORTANT /////////////////////////////////
////////////////////////////////////////////////////////////////////////////

// should correspond to the serial number on the PCB
#define SERIAL_ID                       13

// will a onset_detector be active?
#define ONSET_ACTIVE                    false

///////////////////////////// P3 - Onset Threshold ///////////////////////////
// if USER_ONSET_THRESH_OVERRIDE is set to true the user will determine the
// threshold level for onsets this is used to determine the threshold for onset
// events and is controlled by pot #3
#if HV_MAJOR > 2
#define USER_ONSET_THRESH_OVERRIDE            true
#else
#define USER_ONSET_THRESH_OVERRIDE            false
#endif//HV_MAJOR
// the third pot will be used for changing the onset threshold
#define OT_POT_NUM                            2
#define USER_OT_MIN                           0.1
#define USER_OT_MAX                           2.5


///////////////////////////// P4 - Brightness Threshold ////////////////////////
// the fourth pot will be used to change the song thresehold
// 0.0 - 1.0 any target brightness calculated below this threshold will be discarded
// in favor of a brightness of 0.0, this should be controled using pins or a POT if available...
// if USER_ONSET_THRESH_OVERRIDE is set to true the user will determine the threshold level for onsets
// this is used to determine the threshold for onset events and is controlled by pot #3

#define BT_POT_NUM                            3
#define USER_CUTTOFF_MIN                      0.0
#define USER_CUTTOFF_MAX                      0.5

////////////////////////////////////////////////////////////////////////////
/////////////////////////// System Performance /////////////////////////////
////////////////////////////////////////////////////////////////////////////
#define MAX_FPS                         60.0


////////////////////////////////////////////////////////////////////////////
/////////////////////////// jumper Pins ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// it only takes about 1/2 a ms to update the user controls
// it is very useful for troubleshooting to set these values lower
// TODO - makes a flag for troubleshooting
// TODO - make a mode for night time cricket stuff
#if HV_NUMBER == 3
#define USER_CONTROL_POLL_RATE         100
#elif HV_NUMBER < 3
#define USER_CONTROL_POLL_RATE         1000
#endif // HV_NUMBER

////////////////////////////////////////////////////////////////////////////
///////////////////////// General Settings /////////////////////////////////
////////////////////////////////////////////////////////////////////////////

bool gain_adjust_active =                false;

// WARNING NOT IMPLEMENTED - TODO
#define DEACTIVATE_UNDER_EXTREME_LUX     false

// this needs to be included after the BEHAVIOUR_ROUTINE line so everything loads properly
#if BEHAVIOUR_ROUTINE == B_ADAPTIVE_FEEDBACK
#define NUM_AUTOGAINS                 1
#define NUM_FEATURE_COLLECTORS        1
#define NUM_NEO_GROUPS                1
#elif BEHAVIOUR_ROUTINE == B_TARGETED_FEEDBACK
#define NUM_AUTOGAINS                 0
#define NUM_FEATURE_COLLECTORS        1
#define NUM_NEO_GROUPS                1
#elif BEHAVIOUR_ROUTINE == B_HARDWARE_TEST
#define NUM_AUTOGAINS                 0
#define NUM_FEATURE_COLLECTORS        0
#define NUM_NEO_GROUPS                1
#endif

////////////////////////////////////////////////////////////////////////////
///////////////////////// NeoPixel Settings ////////////////////////////////
////////////////////////////////////////////////////////////////////////////

// The values set in this field corresponds to the RGB colour
// sent to NeoPixels when the brightness is maximum, but
// the detected cicada song pitch is at its lowest value
#define SONG_RED_LOW                    50
#define SONG_GREEN_LOW                  255
#define SONG_BLUE_LOW                   50

// The values set in this field corresponds to the RGB colour
// sent to NeoPixels when the brightness is maximum, and
// the detected cicada song pitch is at its highest value
#define SONG_RED_HIGH                   255
#define SONG_GREEN_HIGH                 60
#define SONG_BLUE_HIGH                  60

/*//////////////////////////// LED MAPPING MODE //////////////////////////////////////////////
   The options which are being considered are
   LED_MAPPING_STANDARD
    All leds display the same brightness in tandum
   LED_MAPPING_BOTTOM_UP
    The LEDs facing the ground are mapped first while the LEDs on the TOP
    are filled last
   LED_MAPPING_ROUND
    The LEDs are displayed like around a clock face moving in a counter clockwise or a clockwise direction
   LED_MAPPING_INSIDE_OUT
    The LEDS in the center of the PCB are mapped first while the outside LEDs are mapped last
*/

/////////////////////////////////////////////////////////////////////////
/////////////////////////      Feature Collector     ////////////////////
/////////////////////////////////////////////////////////////////////////
// minimum amount of time between peak-log resets  which is allowed.
#define PEAK_LOG_RESET_MIN                    2000
#define RMS_LOG_RESET_MIN                     2000

// these features are not currently implemented
// #define TONE_FEATURE_ACTIVE                 1
// #define FREQ_FEATURE_ACTIVE                 1

///////////////////////////////////////////////////////////// SONG /////////////////////////////////////////////////////////////////
// TODO - add bin magnitude as a feature
// which audio feature to use to test
// "peak" will look at the audio "peak" value
// "rms" will look at the audio "rms" value
int SONG_FEATURE =                            PEAK_RAW;
int SONG_COLOR_FEATURE =                      SPECTRAL_CENTROID;

///////// Onset Settings
// what feature will be used to determine if a onset has been found
// "rms_delta" will use that feature along with ONSET_RMS_DELTA_THRESH
// "peak_delta" will use that feature along with ONSET_PEAK_DELTA_THRESH
// "all" will use all available features with their corresponding thresholds
int ONSET_FEATURE =                           PEAK_DELTA;
#define ONSET_RMS_DELTA_THRESH                0.05
#define ONSET_PEAK_DELTA_THRESH               0.17
#define ONSET_SPECTRAL_FLUX_THRESH            100.0
#define SPECTRAL_FLUX_SCALER                  1

/*
   TODO
*/
// one onset per minute
#define MIN_FPM_THRESH                  1
#define LOW_FPM_THRESH                  1.0
#define HIGH_FPM_THRESH                 40
#define MAX_FPM_THRESH                  60.0
/*
    TODO
*/
#define ON_RATIO_FEEDBACK               1
#define MIN_ON_RATIO_THRESH             0.05
#define LOW_ON_RATIO_THRESH             0.40
#define HIGH_ON_RATIO_THRESH            0.60
#define MAX_ON_RATIO_THRESH             0.90

/*
   TODO
*/





#endif // CONFIGURATION_H
