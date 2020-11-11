/* This is the main configuration file for the Moth Framework
 * Using this file, along with the other configuration files you
 * Can cistomise how the firmware performs.
 */
#ifndef __SPECULATOR_CONFIGURATION_H__
#define __SPECULATOR_CONFIGURATION_H__ 

#include "Configuration_hardware.h"
#include <ValueTracker.h>
#include <Audio.h>
#include "Macros.h"
#include <PrintUtils.h>

////////////////////////////////////////////////////////////////////////////
/////////////////////////// MOST IMPORTANT /////////////////////////////////
////////////////////////////////////////////////////////////////////////////

// should correspond to the serial number on the PCB
#define SERIAL_ID                       13

// will a onset_detector be active?
#define ONSET_ACTIVE                    false

///////////////////////////// P1 - Brightness Overide ////////////////////////
// which pot will be used to control thebrightness overide
// if USER_BS_ACTIVE is set to true the user will scale the natural 
// brightness levels (in pitch mode only) before being sent to the neopixel
// manager (which then might constrain according to Lux levels)
float user_brightness_scaler               = 1.0;

float min_user_brightness_scaler           = 0.25;
float max_user_brightness_scaler           = 2.0;

// this will determine if the USER will have control over the brightness scaler
// at this point v2.1 does this via jumpers and v3.0 does this via a pot
// so the default value is true
#define USER_BS_ACTIVE                        true

#if HV_MAJOR > 2
#define BS_POT_NUM                            0
#define POT_BS_MAX                            10.0
#define POT_BS_MIN                            0.05

#define BC_POT_NUM                            3
// TOD add min/bax for the brightness cuttoff thresh

#endif//HV_MAJOR

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
double ONSET_THRESH =                         1.0;

///////////////////////////// P4 - Brightness Threshold ////////////////////////
// the fourth pot will be used to change the song thresehold
// 0.0 - 1.0 any target brightness calculated below this threshold will be discarded
// in favor of a brightness of 0.0, this should be controled using pins or a POT if available...
// if USER_ONSET_THRESH_OVERRIDE is set to true the user will determine the threshold level for onsets
// this is used to determine the threshold for onset events and is controlled by pot #3
#if HV_MAJOR > 2
#define USER_BRIGHT_THRESH_OVERRIDE            true
#else
#define USER_BRIGHT_THRESH_OVERRIDE            false
#endif//HV_MAJOR

#if FIRMWARE_MODE == CICADA_MODE
float BRIGHTNESS_CUTTOFF_THRESHOLD = 0.15;
#elif FIRMWARE_MODE == PITCH_MODE
float BRIGHTNESS_CUTTOFF_THRESHOLD = 0.01;
#elif FIRMWARE_MODE == PITCH_MODE && HV_MAJOR == 3
float BRIGHTNESS_CUTTOFF_THRESHOLD = 0.0;
#endif//FIRMWARE_MODE

// if > 0 then the brightness will be smoothed with a previous value
// thee higher the value the more it is smoothed
#define HUE_DECAY_DELAY      1500
#define HUE_DECAY_FACTOR     0.025

#define SAT_DECAY_DELAY      6000
#define SAT_DECAY_FACTOR     0.01

#define BGT_DECAY_DELAY       1500
#define BGT_DECAY_FACTOR     0.025

#define BGT_MIN_UPDATE_FACTOR 0.25
#define BGT_MAX_UPDATE_FACTOR 0.25

#define SAT_MIN_UPDATE_FACTOR 0.05
#define SAT_MAX_UPDATE_FACTOR 0.01

#define HUE_MIN_UPDATE_FACTOR 0.25
#define HUE_MAX_UPDATE_FACTOR 0.25

#define BT_POT_NUM                            3   
#define USER_CUTTOFF_MIN                      0.0   
#define USER_CUTTOFF_MAX                      0.5

////////////////////////////////////////////////////////////////////////////
/////////////////////////// System Performance /////////////////////////////
////////////////////////////////////////////////////////////////////////////
#define MAX_FPS                         60.0
elapsedMillis loop_tmr = 0; 
uint32_t loop_length = (uint32_t)((double)1000.0 / (double)MAX_FPS);

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

// this needs to be included after the firmware_mode line so everything loads properly
#if FIRMWARE_MODE == PITCH_MODE
  #define NUM_AUTOGAINS                 1
  #define NUM_FEATURE_COLLECTORS        1
  #define NUM_NEO_GROUPS                2
#elif FIRMWARE_MODE == CICADA_MODE
  #define NUM_AUTOGAINS                 0
  #define NUM_FEATURE_COLLECTORS        1
  #define NUM_NEO_GROUPS                2
#elif FIRMWARE_MODE == TEST_MODE
  #define NUM_AUTOGAINS                 0
  #define NUM_FEATURE_COLLECTORS        0
  #define NUM_NEO_GROUPS                2  
#endif

////////////////////////////////////////////////////////////////////////////
///////////////////////// NeoPixel Settings ////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define SONG_RED_LOW                    50
#define SONG_GREEN_LOW                  255
#define SONG_BLUE_LOW                   50

#define SONG_RED_HIGH                   255
#define SONG_GREEN_HIGH                 60
#define SONG_BLUE_HIGH                  60

/*//////////////////////////// LED MAPPING MODE //////////////////////////////////////////////
 * The options which are being considered are 
 * LED_MAPPING_STANDARD
 *  All leds display the same brightness in tandum
 * LED_MAPPING_BOTTOM_UP
 *  The LEDs facing the ground are mapped first while the LEDs on the TOP
 *  are filled last
 * LED_MAPPING_ROUND
 *  The LEDs are displayed like around a clock face moving in a counter clockwise or a clockwise direction
 * LED_MAPPING_CENTER_OUT        
 *  The LEDS in the center of the PCB are mapped first while the outside LEDs are mapped last
*/

/////////////////////////////////////////////////////////////////////////
//////////////////////// Audio Settings /////////////////////////////////
/////////////////////////////////////////////////////////////////////////
// if stereo feedback is set to true than only audio from both channels will be used to calculate visual feedback brightness and color
// not generally recommended...
#define STEREO_FEEDBACK                 false

// how often does the feature collector update
//33 is 30 times a second
#define FC_UPDATE_RATE                  10
#define AUDIO_MEMORY                    24

bool stereo_audio =                     true;

// these are the default values which set front_mic_active
// if the microphone test is run and it is found that one of the microphones is
// not working properly, then the variables will be switched to false
bool front_mic_active =                 true;
bool rear_mic_active =                  true;

// audio usage loggings
uint8_t audio_usage_max =               0;
elapsedMillis last_usage_print =        0;// for keeping track of audio memory usage

#if HV_MAJOR < 3
#define STARTING_GAIN                         480.0
#elif HV_MAJOR == 3
// 30.0 is good for testing when no enclosure is present, but a higher value should be used when an enclosure is present
#define STARTING_GAIN                         240.0
#endif

// this is dictated by user controls and is multiplied against the STARTING_GAIN to determine runtime gain
double USER_CONTROL_GAIN_ADJUST               = 1.0;

double front_gain =                           STARTING_GAIN;  
double rear_gain  =                           STARTING_GAIN;

/////////////////////////////////////////////////////////////////////////
/////////////////////////      Feature Collector     ////////////////////
/////////////////////////////////////////////////////////////////////////
// minimum amount of time between peak-log resets  which is allowed.
#define PEAK_LOG_RESET_MIN              2000
#define RMS_LOG_RESET_MIN               2000

// Which Audio features will be activated?
#define PEAK_FEATURE_ACTIVE                 1
#define RMS_FEATURE_ACTIVE                  1
// these features are not currently implemented
// #define TONE_FEATURE_ACTIVE                 1
// #define FREQ_FEATURE_ACTIVE                 1

/////////////////////////////////////////////////////////////////////////
/////////////////////////      FFTManager  //////////////////////////////
/////////////////////////////////////////////////////////////////////////
// used the scaled FFT readings or the normal FFT readings, the scaled readings will eensure that
// all the bins of intrest will have their magnitudes add up to 1, thus is best used for determining the centroid within a sub frequency range (for instance 8k - 14k or something
#define SCALE_FFT_BIN_RANGE             false
#define FFT_FEATURES_ACTIVE                 1

/////////////////////////////// Color Mapping /////////////////////////////////////
// when calculating the hue for the NeoPixel leds, what feature do you want to use?
// look under Audio Features for the Available Features
double hue_min = 1.0;
double hue_max = 0.0;

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
uint8_t HUE_FEATURE         =               FEATURE_CENTROID;
uint8_t BRIGHTNESS_FEATURE  =               (FEATURE_FFT_ENERGY);
uint8_t SATURATION_FEATURE  =               (FEATURE_FFT_RELATIVE_ENERGY);

bool REVERSE_SATURATION     =               true;
bool REVERSE_BRIGHTNESS     =               false;
bool REVERSE_HUE            =               false;

// what factor will the new values be scaled by compared to the old values 
// when calculating the song brightness target vs current levels a 1.0 will turn off
// the low filtering so only the new values will be used while 0.5 will result in the
// average of the old and new value to be used, a higher value will be a quicker responce
// the max value is 1.0 and the min value is 0.0
#define HUE_LP_LEVEL                          0.15
#define SATURATION_LP_LEVEL                   0.15
#define BRIGHTNESS_LP_LEVEL                   0.15

// These are different color mapping modes
// #define COLOR_MAPPING_RGB                     0
// originated with the pitch mode for the speculator
#define COLOR_MAPPING_HSB                     false
// the simple but effective explorator mapping strategy
#define COLOR_MAPPING_EXPLORATOR              true

// For the neopixels will the color mapping exist within the RGB or HSB domain?
bool COLOR_MAP_MODE          =             COLOR_MAPPING_EXPLORATOR;

#define  MODE_SINGLE_RANGE                    0
#define  MODE_ALL_BINS                        1
#define  MODE_BIN_ENERGY                      2

// should the centroid value be smoothed?
#define SMOOTH_CENTROID                       true

///////////////////////////////////////////////////////////// SONG /////////////////////////////////////////////////////////////////
// TODO - add bin magnitude as a feature
// which audio feature to use to test
// "peak" will look at the audio "peak" value
// "rms" will look at the audio "rms" value
int SONG_FEATURE =                            PEAK_RAW;
int SONG_COLOR_FEATURE =                      SPECTRAL_CENTROID;

// these were 0.25
#define PEAK_THRESH                      0.15 
#define RMS_THRESH                       0.15

// TODO need to determine what are good values for these
#define MIN_SONG_PEAK_AVG                     0.002
#define MAX_SONG_PEAK_AVG                     0.2

// this is the gain that the input amps will be set at at the start of the installation
// #define STARTING_SONG_GAIN              8.0
// #define STARTING_ONSET_GAIN             16.0



#define SONG_BQ1_THRESH                       4000
#define SONG_BQ1_Q                            0.85
#define SONG_BQ1_DB                           -12

#define SONG_BQ2_THRESH                       16000
#define SONG_BQ2_Q                            0.85
#define SONG_BQ2_DB                           -12

// #define MIN_SONG_GAIN                        (STARTING_SONG_GAIN * 0.5)
// #define MAX_SONG_GAIN                        (STARTING_SONG_GAIN * 1.5)

///////// Onset Settings
// what feature will be used to determine if a onset has been found
// "rms_delta" will use that feature along with ONSET_RMS_DELTA_THRESH
// "peak_delta" will use that feature along with ONSET_PEAK_DELTA_THRESH
// "all" will use all available features with their corresponding thresholds
#define CALCULATE_FLUX                        true
#define CALCULATE_CENTROID                    true
#define CALCULATE_ROFF                        false

int ONSET_FEATURE =                           PEAK_DELTA;
#define ONSET_RMS_DELTA_THRESH                0.05
#define ONSET_PEAK_DELTA_THRESH               0.17
#define ONSET_SPECTRAL_FLUX_THRESH            100.0
#define SPECTRAL_FLUX_SCALER                  1

// these values are used to ensure that the light do not turn on when there are low 
// amplitudes in the sonic environment
#define RMS_LOW_THRESH                        0.1
#define PEAK_LOW_THRESH                       0.15
/*
 * TODO
 */
// one onset per minute
#define MIN_FPM_THRESH                  1 
#define LOW_FPM_THRESH                  1.0
#define HIGH_FPM_THRESH                 40
#define MAX_FPM_THRESH                  60.0

/*
 *  TODO
 */
#define ON_RATIO_FEEDBACK               1
#define MIN_ON_RATIO_THRESH             0.05
#define LOW_ON_RATIO_THRESH             0.40
#define HIGH_ON_RATIO_THRESH            0.60
#define MAX_ON_RATIO_THRESH             0.90

/*
 * TODO
 */
#define ONSET_BQ1_THRESH                1200
#define ONSET_BQ1_Q                     0.85
#define ONSET_BQ1_DB                    -12
#define ONSET_BQ2_THRESH                3000
#define ONSET_BQ2_Q                     0.85
#define ONSET_BQ2_DB                    -12

//////////////////////////////// Global Variables /////////////////////////
bool but_test[10];
float pot_test[4];

double hue = 1.0;
double brightness = 1.0;
double saturation = 0.0;// needs to start at 0.0 or else the min/max value tracker has issues

ValueTrackerDouble hue_tracker        = ValueTrackerDouble((String)"HUE", &hue, HUE_DECAY_FACTOR, HUE_DECAY_DELAY, HUE_LP_LEVEL);
ValueTrackerDouble saturation_tracker = ValueTrackerDouble((String)"SATURATION", &saturation, SAT_DECAY_FACTOR, SAT_DECAY_DELAY, SATURATION_LP_LEVEL);
ValueTrackerDouble brightness_tracker = ValueTrackerDouble((String)"BRIGHTNESS", &brightness, BGT_DECAY_FACTOR, BGT_DECAY_DELAY, BRIGHTNESS_LP_LEVEL);

// TODO -- need to implement the target hue saturation and brightness mapping schema
float target_hue = 1.0;
float target_saturation = 1.0;
bool USE_TARGET_BRIGHTNESS = false;

float target_brightness = 1.0;

//////////////////////////////// Onset ////////////////////////////////////
elapsedMillis onset_feature_reset_tmr;
const unsigned long onset_feature_reset_time = (1000 * 60 * 1);// every 5 minute?

double last_range_rms = 0.0;
double min_rrms = 1.0;
double max_rrms = 0.1;

double last_flux = 0.0;
double min_flux = 1.0;
double max_flux = 0.1;

double last_cent_degd = 0.0;
double min_cent_negd = 1.0;
double max_cent_negd = 0.0;

double min_onset_feature = 1.0;
double max_onset_feature = 0.0;

double onset_flux = 0.0;
double onset_rrms = 0.0;
double onset_cent = 0.0;

////////////////////////////////// RANDOM stuff ///////////////////////////////////
// this is a high value to force jumper readings in the setup() loop
elapsedMillis last_jumper_read = 100000;
#if P_FUNCTION_TIMES == true
elapsedMillis function_times = 0;
#endif // P_FUNCTION_TIMES


#endif // CONFIGURATION_H
