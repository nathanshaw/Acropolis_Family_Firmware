#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

// to get some definitions such as SPECULATOR
#include "Macros.h"
#include "ValueTrackerDouble.h"

////////////////////////// Boot Tests ////////////////////////////////
#define TEST_SOLENOIDS           false
#define TEST_MOTORS              false
#define TEST_LEDS                false

/////////////////////// Artefact Type ////////////////////////////////
// There are three artefact types currently availible
// SPECULATOR, EXPLORATOR, and LEGATUS
// set ARTEFACT_TYPE to one of these types
/////////////////// TODO
#define ARTEFACT_TYPE             SPECULATOR

// TODO finish integrating this
float ADDED_SATURATION  = 0.4;

////////////////////// Hardware Revision /////////////////////////////
// There are different hardware revisions for different Artefacts
// Starting with the Speculator the 2.0, 2.1 and 3.0 are the primary revisions
// where the 2.0 and 2.1 are 10 LED 100 mm units, where the 2.1 has 6 jumpers and
// the 2.0 has no user controls. The 3.0 is the 80 mm units with 40 LEDs
// For the Explorator the 0.0 PCB is the yellow one with the temp/humid sensor on the
// main PCB (the bell bot is PCB revision 0.0, where revision 1.0 is the one with 3x motor
// drivers and 9 solenoid drivers
// 0.1.4 = version used for the jan, 2021 southwest installations
#define HV_MAJOR                  3
#define HV_MINOR                  0

//////////////////// Software Revision ////////////////////////////////
#define SV_MAJOR                  0
#define SV_MINOR                  1
#define SV_REVISION               4

////////////////////// Body Type //////////////////////////////////////
// for the explorator there are two currently available body types
// bell bot, which features 3 desk bells with solenoid strikers and damperers
// woodpecker bot, which features a rotating wooden disk which is struck by a solenoid
#if ARTEFACT_TYPE == EXPLORATOR
#define WOODPECKER_BODY           0
#define BELL_BODY                 1
#define CLAPPER_BODY              2
#define BODY_TYPE                 WOODPECKER_BODY
#endif

////////////////////// Firmware Mode //////////////////////////////////////////////////////
// For the Speculator.....
// FIRMWARE MODE should be set to  CICADA_MODE, PITCH_MODE, or TEST_MODE
// depending on what functionality you want
// For the Explorator......
#define PLAYBACK_MODE             99
#define REACTIVE_MODE             100
#define ECHO_FEEDBACK_MODE        101

#if ARTEFACT_TYPE == SPECULATOR
#define FIRMWARE_MODE             PITCH_MODE
#else
#define FIRMWARE_MODE             ECHO_FEEDBACK_MODE
#endif

//////////////////////////////////////////////////////////
///////////////////////////////////////////////////////// 
// Including the Appropiate configuration files depending on
// the artefact type, no user adjustments are needed from this point onwards in the file
#if ARTEFACT_TYPE == SPECULATOR
#include "Configuration_Speculator.h"
#elif ARTEFACT_TYPE == EXPLORATOR
#include "Configuration_Explorator.h"
#elif ARTEFACT_TYPE == LEGATUS
#include "Configuration_Legatus.h"
#endif

// The hardware configuration file contains all the information for the PCB's
// it will set the right directives according to information in the
// speculator, explorator, and legatus configuration files
#include "Configuration_hardware.h"

#if ARTEFACT_TYPE == SPECULATOR && HV_MAJOR == 2
#define NUM_AMPLIFIERS                1
#define NUM_PEAK_ANAS                 1
#define NUM_RMS_ANAS                  1
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

#if ARTEFACT_TYPE == SPECULATOR
// how long will the bot wait until starting the main loop
// this is useful for when it neeeds to bee put into an enclosure and then installed in the environment
uint32_t  BOOT_DELAY      =           (1000 * 60 * 2);
#elif ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE == CLAPPER_BODY
uint32_t  BOOT_DELAY      =           (10000);
#elif ARTEFACT_TYPE == EXPLORATOR
uint32_t  BOOT_DELAY      =           (1000);
#else
uint32_t  BOOT_DELAY      =           (1000);
#endif
int BOOT_DELAY_ACTIVE    =           false;

///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// Audio System ///////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

// if set to true an audio USB object will be created so the audio can be debuged via Audacity
#define AUDIO_USB                       true

///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// Lux Manager ////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
// TODO - this setting is critial to how the LuxManager effects NeoPixel brightness
// the two options are to either generate a brightness scaler floating point value
// to which all color messages are multiplied against, or to adjust the maximum and minimum
// values allowed
#define LUX_ADJUSTS_BS                  1
#define LUX_ADJUSTS_MIN_MAX             1
#if ARTEFACT_TYPE == SPECULATOR
uint8_t LUX_MAPPING_SCHEMA =            LUX_ADJUSTS_MIN_MAX;
#else
uint8_t LUX_MAPPING_SCHEMA =            LUX_ADJUSTS_BS;
#endif

////////////////////////////////// Lux Ambiant Lighting Thresholds /////////////////////
#if (ARTEFACT_TYPE == SPECULATOR) && HV_MAJOR == 2
#define NIGHT_LUX_THRESHOLD             0.25
// this is the threshold in which anything below will just be treated as the lowest reading
#define LOW_LUX_THRESHOLD               5.0
// when a lux of this level is detected the LEDs will be driven with a brightness scaler of 1.0
#define MID_LUX_THRESHOLD               20.0
#define HIGH_LUX_THRESHOLD              1200.0
#define EXTREME_LUX_THRESHOLD           5000.0

#elif (ARTEFACT_TYPE == SPECULATOR) && HV_MAJOR == 3
// the v3 hardware needs higher thresholds as it is brighter and thus needs to decrease its brightness sooner
#define NIGHT_LUX_THRESHOLD             0.25
// this is the threshold in which anything below will just be treated as the lowest reading
#define LOW_LUX_THRESHOLD               5.0
// when a lux of this level is detected the LEDs will be driven with a brightness scaler of 1.0
#define MID_LUX_THRESHOLD               15.0
#define HIGH_LUX_THRESHOLD              3000.0
#define EXTREME_LUX_THRESHOLD           5000.0

#elif (ARTEFACT_TYPE == EXPLORATOR)
#define NIGHT_LUX_THRESHOLD             1.0
// this is the threshold in which anything below will just be treated as the lowest reading
#define LOW_LUX_THRESHOLD               10.0
// when a lux of this level is detected the LEDs will be driven with a brightness scaler of 1.0
#define MID_LUX_THRESHOLD               350.0
#define HIGH_LUX_THRESHOLD              1200.0
#define EXTREME_LUX_THRESHOLD           5000.0

#elif ARTEFACT_TYPE == LEGATUS
#define NIGHT_LUX_THRESHOLD             1.0
// this is the threshold in which anything below will just be treated as the lowest reading
#define LOW_LUX_THRESHOLD               10.0
// when a lux of this level is detected the LEDs will be driven with a brightness scaler of 1.0
#define MID_LUX_THRESHOLD               350.0
#define HIGH_LUX_THRESHOLD              1200.0
#define EXTREME_LUX_THRESHOLD           5000.0

#endif // ARTEFACT_TYPE for Lux thresholds

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

#if (ARTEFACT_TYPE == SPECULATOR) && (HV_MAJOR == 3)
uint16_t  MAX_BRIGHTNESS =              765;
#else
uint16_t  MAX_BRIGHTNESS =              765;
#endif

///////////////////////////////// Lux Adjusts Brightness Scaler /////////////////////////////////
// on scale of 0-1.0 what is the min multiplier for the user defined brightness scaler
// 0.05 was too low, did not provide good enough feedback for the night time
#if (ARTEFACT_TYPE == SPECULATOR) || (ARTEFACT_TYPE == LEGATUS)
#define LUX_BS_MIN                      0.75
#define LUX_BS_MAX                      2.50
#else
#define LUX_BS_MIN                      0.75
#define LUX_BS_MAX                      1.50
#endif
/////////////////////////////// Update Regularity //////////////////////////
uint32_t lux_max_reading_delay =        1000 * 60 * 3;   // every 3 minute
uint32_t lux_min_reading_delay =        1000 * 10;       // ten seconds

#if ARTEFACT_TYPE == SPECULATOR && HV_MAJOR > 2
#define USER_BRIGHT_THRESH_OVERRIDE            true
#else
#define USER_BRIGHT_THRESH_OVERRIDE            false
#endif//HV_MAJOR

#if FIRMWARE_MODE == CICADA_MODE
float BRIGHTNESS_CUTTOFF_THRESHOLD = 0.15;
#elif FIRMWARE_MODE == PITCH_MODE && HV_MAJOR == 2
float BRIGHTNESS_CUTTOFF_THRESHOLD = 0.01;
#elif FIRMWARE_MODE == PITCH_MODE && HV_MAJOR == 3
float BRIGHTNESS_CUTTOFF_THRESHOLD = 0.0;
#else 
float BRIGHTNESS_CUTTOFF_THRESHOLD = 0.0;
#endif//FIRMWARE_MODE

///////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Neopixel LEDs /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
// this is needed for the forced lux
#define UPDATE_ON_OFF_RATIOS            false

// when onset detection is active, this will be the threshold
double ONSET_THRESH =                   1.20;

// this color is used for flashes
#define ONSET_RED                       200
#define ONSET_GREEN                     200
#define ONSET_BLUE                      255

////////////////////////// LED MAPPING MODES ////////////////
#define LED_MAPPING_STANDARD      0
#define LED_MAPPING_CENTER_OUT    1
#define LED_MAPPING_ROUND         2
#define LED_MAPPING_BOTTOM_UP     3
#define LED_MAPPING_CLOCK_HAND    4
#define LED_MAPPING_OUTSIDE_IN    5
#define LED_MAPPING_CLOCK_FILL    6
#define LED_MAPPING_CUSTOM        7

#if ARTEFACT_TYPE == SPECULATOR && HV_MAJOR == 3
int LED_MAPPING_MODE = LED_MAPPING_CENTER_OUT;
#else
int LED_MAPPING_MODE = LED_MAPPING_STANDARD;
#endif //HV_MAJOR

#if ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE == CLAPPER_BODY
// Which pin on the Arduino is connected to the NeoPixels? 8 for old board
#define LED1_PIN 8
// note that if the second LED channel is used the teensy needs to be overclocked to 120 MHz
#define LED2_PIN 5
#define LED3_PIN 10
#else
#define LED1_PIN 5
// note that if the second LED channel is used the teensy needs to be overclocked to 120 MHz
#define LED2_PIN 8
#define LED3_PIN 10
#endif

// what the names of the neopixel strips will be
#if (ARTEFACT_TYPE == EXPLORATOR) && (BODY_TYPE == BELL_BODY)
#define LED1_NAME      "small"
#define LED2_NAME      "medium"
#define LED3_NAME      "large"
#elif (ARTEFACT_TYPE == EXPLORATOR) && (BODY_TYPE == WOODPECKER_BODY)
#define LED1_NAME      "Eye Stock"
#define LED2_NAME      "Pecker"
#define LED3_NAME      "N/A"
#elif (ARTEFACT_TYPE == EXPLORATOR) && (BODY_TYPE == CLAPPER_BODY)
#define LED1_NAME      "Inside Ring"
#define LED2_NAME      "N/A"
#define LED3_NAME      "N/A"
#elif (ARTEFACT_TYPE == SPECULATOR)
#define LED1_NAME      "All"
#define LED2_NAME      "N/A"
#define LED3_NAME      "N/A"
#elif ARTEFACT_TYPE == LEGATUS
#define LED1_NAME      "All"
#define LED2_NAME      "N/A"
#define LED3_NAME      "N/A" 
#endif

// How many NeoPixels are attached to the Arduino?
// if no LEDs are connected to the line then set this to 0
#if (ARTEFACT_TYPE == SPECULATOR) && (HV_MAJOR < 3)
#define LED1_COUNT 16
#define LED2_COUNT 0
#define LED3_COUNT 0
#elif (ARTEFACT_TYPE == SPECULATOR) && (HV_MAJOR == 3)
#define LED1_COUNT 40
#define LED2_COUNT 0
#define LED3_COUNT 0
#elif (ARTEFACT_TYPE == EXPLORATOR) && (BODY_TYPE == BELL_BODY)
#define LED1_COUNT 10
#define LED2_COUNT 10
#define LED3_COUNT 10
#elif (ARTEFACT_TYPE == EXPLORATOR) && (BODY_TYPE == CLAPPER_BODY)
#define LED1_COUNT 20
#define LED2_COUNT 0
#define LED3_COUNT 0
#elif (ARTEFACT_TYPE == EXPLORATOR) && (BODY_TYPE == WOODPECKER_BODY)
#define LED1_COUNT 10
#define LED2_COUNT 10
#define LED3_COUNT 10
#elif ARTEFACT_TYPE == LEGATUS
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

// if false, a onset detected on either side results in a LED flash on both sides
// if true, a onset detected on one side will only result in a flash on that side
int INDEPENDENT_FLASHES =              false; // WARNING NOT IMPLEMENTED - TODO

int FLASH_DOMINATES =                  false;

// if this is true then the brightness will b = (b + b) * b; in order to reduce its value, and make loud events even more noticable
int SQUARE_BRIGHTNESS =                false;

int SATURATED_COLORS =                 true;

// how high the onset flash timer will go up to
#define MAX_FLASH_TIME                  60
// where the onset flash timer will start
#define MIN_FLASH_TIME                  40
// the amount of time that the LEDs need to be shutdown to allow lux sensors to get an accurate reading
#define FLASH_DEBOUNCE_TIME             80

const bool led_channel_active[3] = {LED1_ACTIVE, LED2_ACTIVE, LED3_ACTIVE};
const uint8_t num_active_led_channels = LED1_ACTIVE + LED2_ACTIVE + LED3_ACTIVE;
const uint16_t max_led_count = max(max(LED1_COUNT, LED2_COUNT), LED3_COUNT);

byte drawingMemory[3][max_led_count * 3];       //  3 bytes per LED
DMAMEM byte displayMemory[3][max_led_count * 12]; // 12 bytes per LED

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

// in ms, how often will theUI controls update?
#if ((ARTEFACT_TYPE == SPECULATOR) && (HV_MAJOR == 2))
#define UI_POLLING_RATE                 1000
#elif ARTEFACT_TYPE == SPECULATOR && HV_MAJOR == 3
#define UI_POLLING_RATE                 500
#elif ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE == CLAPPER_BODY
#define UI_POLLING_RATE                 600000
#else
#define UI_POLLING_RATE                 500
#endif // UI_POLLING_RATE

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
///////////////////////////////// Datalogging //////////////////////////////////////////////////
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
#define DATALOG_ACTIVE                  false
#define PRINT_EEPROM_ON_BOOT            false

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
#define P_SATURATION                    false
#define P_HUE                           false
#define P_BRIGHTNESS                    false

#define P_NEO_COLORS                    false

//////////////////////// LuxManager and Ambiant Lighting ///////////////
#define P_LUX_MANAGER_READINGS          true
#define P_EXTREME_LUX                   true
// sets p_lux_readings within the lux_manager but also the NeoPixelManager
#define P_LUMIN                         true
// sets p_lux within the lux_manager
#define P_LUX_READINGS                  true
// sets general debug printing for the lux_manager class
#define P_LUX_MANAGER_DEBUG             false
// sets print_brightness_scaler within the lux_manager
#define P_BS                            false

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
#define P_ONSET_FEATURES                          false
#define P_ONSET                                   false

//////////////////////////// AutoGain //////////////////////////////////
#define P_AUTOGAIN                                false

// for calculating the dominate channel, it works best if the dominate channel is re-caculated every

/////////////////////////// Datalogging ////////////////////////////////
#define P_LOG_WRITE                               false
#define DLM_PRINT                                 false
// if LOOP_LENGTH is set to true the program will keep track of how long it takes to go through
// the main loop, it will also store the min and max loop length values as well as calculate
// what the average loop length is
#define P_LOOP_LENGTH                             false
///////////////////////// Audio Stuff ///////////////////////////////////////
#define P_RMS_VALS                                false
#define P_PEAK_VALS                               false
// #define P_TONE_VALS                     false
#define P_FREQ_VALS                               false
#define P_AUDIO_USAGE_MAX                         true
#define P_DOMINATE_CHANNEL                        true

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
#define P_USER_CONTROLS                           true

///////////////////////// Solenoids //////////////////////////////////////////
#define P_SOLENOID_DEBUG                          true

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
#if ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE == CLAPPER_BODY
int AUTOGAIN_ACTIVE      =                               false;
#elif ARTEFACT_TYPE == SPECULATOR
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
#if ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE == CLAPPER_BODY
uint8_t num_fft_managers =                              1;
#elif ARTEFACT_TYPE == SPECULATOR && HV_MAJOR == 2
uint8_t num_fft_managers =                              1;
#elif ARTEFACT_TYPE == SPECULATOR && HV_MAJOR == 3
uint8_t num_fft_managers =                              1;
#else
uint8_t num_fft_managers =                              2;
#endif

#if ARTEFACT_TYPE == SPECULATOR && FIRMWARE_MODE == CICADA_MODE
#define CENTROID_FEATURE_MIN                            4000
#define CENTROID_FEATURE_MAX                            16000
#elif ARTEFACT_TYPE == SPECULATOR && FIRMWARE_MODE == PITCH_MODE
#define CENTROID_FEATURE_MIN                            200
#define CENTROID_FEATURE_MAX                            20000
#elif ARTEFACT_TYPE == EXPLORATOR
#define CENTROID_FEATURE_MIN                            120
#define CENTROID_FEATURE_MAX                            20000
#elif ARTEFACT_TYPE == LEGATUS
#define CENTROID_FEATURE_MIN                            120
#define CENTROID_FEATURE_MAX                            24000
#endif

////////////////////////////////////////////////////////////////////////////
///////////////////////// User Controls ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
///////////////////////////// P1 - Brightness Overide ////////////////////////
// which pot will be used to control thebrightness overide
// if USER_BS_ACTIVE is set to true the user will scale the natural
// brightness levels (in pitch mode only) before being sent to the neopixel
// manager (which then might constrain according to Lux levels)
float user_brightness_scaler               = 1.0;

float min_user_brightness_scaler           = 0.05;
float mid_user_brightness_scaler           = 1.0;
float max_user_brightness_scaler           = 3.0;

float min_user_brightness_cuttoff           = 0.0;
float mid_user_brightness_cuttoff           = 0.5;
float max_user_brightness_cuttoff           = 1.0;

// this will determine if the USER will have control over the brightness scaler
// at this point v2.1 does this via jumpers and v3.0 does this via a pot
// so the default value is true
#define USER_BS_ACTIVE                        true

int but_test[NUM_BUTTONS];
float pot_test[NUM_POTS];

////////////////////////////////////////////////////////////////////////////
///////////////////////// Neo Manager //////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#if ARTEFACT_TYPE == SPECULATOR
#define NUM_NEOP_MANAGERS                               1
#elif ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE == CLAPPER_BODY
#define NUM_NEOP_MANAGERS                               1
#elif ARTEFACT_TYPE == EXPLORATOR
#define NUM_NEOP_MANAGERS                               3
#elif ARTEFACT_TYPE == LEGATUS         
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

// TODO -- need to implement the target hue saturation and brightness mapping schema
float target_hue = 1.0;
float target_saturation = 1.0;
float target_brightness = 1.0;

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
double USER_CONTROL_GAIN_ADJUST               = 1.0;

#if ARTEFACT_TYPE == SPECULATOR && HV_MAJOR == 2
#define STARTING_GAIN                         12.0
#elif ARTEFACT_TYPE == SPECULATOR && HV_MAJOR == 3
// 30.0 is good for testing when no enclosure is present, but a higher value should be used when an enclosure is present
#define STARTING_GAIN                         240.0
#elif ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE == CLAPPER_BODY
#define STARTING_GAIN                         40.0
#elif ARTEFACT_TYPE == EXPLORATOR
#define STARTING_GAIN                         240.0
#elif ARTEFACT_TYPE == LEGATUS
#define STARTING_GAIN                         240.0
#endif

#if ARTEFACT_TYPE == SPECULATOR && FIRMWARE_MODE == CICADA_MODE
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

#elif ARTEFACT_TYPE == SPECULATOR && FIRMWARE_MODE == PITCH_MODE
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
#define RBQ2_Q              0.85
#define RBQ2_DB             -12
//////////////////
#elif ARTEFACT_TYPE == EXPLORATOR
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
#if ARTEFACT_TYPE == SPECULATOR
uint8_t HUE_FEATURE         =               FEATURE_CENTROID;
uint8_t SATURATION_FEATURE  =               FEATURE_PEAK_AVG;
uint8_t BRIGHTNESS_FEATURE  =               FEATURE_FFT_ENERGY;
#else
uint8_t HUE_FEATURE         =               FEATURE_CENTROID;
uint8_t SATURATION_FEATURE  =               (FEATURE_FFT_RELATIVE_ENERGY);
uint8_t BRIGHTNESS_FEATURE  =               (FEATURE_FFT_ENERGY);
#endif // ARTEFACT_TYPE == SPECULATOR

int REVERSE_SATURATION     =               true;
int REVERSE_BRIGHTNESS     =               false;
int REVERSE_HUE            =               false;

// These are different color mapping modes
// #define COLOR_MAPPING_RGB                     0
// originated with the pitch mode for the speculator
#define COLOR_MAPPING_HSB                     0
// the simple but effective explorator mapping strategy
#define COLOR_MAPPING_EXPLORATOR              1

// For the neopixels will the color mapping exist within the RGB or HSB domain?
#if ARTEFACT_TYPE == SPECULATOR
int COLOR_MAP_MODE          =             COLOR_MAPPING_HSB;
#else
int COLOR_MAP_MODE          =             COLOR_MAPPING_EXPLORATOR;
#endif

#define  MODE_SINGLE_RANGE                    0
#define  MODE_ALL_BINS                        1
#define  MODE_BIN_ENERGY                      2

// should the centroid value be smoothed?
#define SMOOTH_CENTROID                       true

#endif // __CONFIGURATION_H__
