#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

// to get some definitions such as SPECULATOR
#include "Macros.h"

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

////////////////////// Hardware Revision /////////////////////////////
// There are different hardware revisions for different Artefacts
// Starting with the Speculator the 2.0, 2.1 and 3.0 are the primary revisions
// where the 2.0 and 2.1 are 10 LED 100 mm units, where the 2.1 has 6 jumpers and
// the 2.0 has no user controls. The 3.0 is the 80 mm units with 40 LEDs
// For the Explorator the 0.0 PCB is the yellow one with the temp/humid sensor on the
// main PCB (the bell bot is PCB revision 0.0, where revision 1.0 is the one with 3x motor
// drivers and 9 solenoid drivers
#define HV_MAJOR       2
#define HV_MINOR       1

//////////////////// Software Revision ////////////////////////////////
#define SV_MAJOR       0
#define SV_MINOR       1
#define SV_REVISION    2

////////////////////// Body Type //////////////////////////////////////
// for the explorator there are two currently available body types
// bell bot, which features 3 desk bells with solenoid strikers and damperers
// woodpecker bot, which features a rotating wooden disk which is struck by a solenoid
#if ARTEFACT_TYPE == EXPLORATOR
#define WOODPECKER_BODY           0
#define BELL_BODY                 1
#define BODY_TYPE                 WOODPECKER_BODY
#endif

////////////////////// Firmware Mode //////////////////////////////////////////////////////
// For the Speculator.....
// FIRMWARE MODE should be set to  CICADA_MODE, PITCH_MODE, or TEST_MODE
// depending on what functionality you want
// For the Explorator......
#define PLAYBACK_MODE             99
#define REACTIVE_MODE             100

#if ARTEFACT_TYPE == SPECULATOR
#define FIRMWARE_MODE             PITCH_MODE
#else
#define FIRMWARE_MODE             PLAYBACK_MODE
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

///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// Boot Settings //////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

#if ARTEFACT_TYPE == SPECULATOR
// how long will the bot wait until starting the main loop
// this is useful for when it neeeds to bee put into an enclosure and then installed in the environment
uint32_t  BOOT_DELAY      =           (1000 * 60 * 2);
#elif ARTEFACT_TYPE == EXPLORATOR
uint32_t  BOOT_DELAY      =           (1000);
#endif
bool BOOT_DELAY_ACTIVE    =           false;

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
#define LUX_ADJUSTS_BS                  0
#define LUX_ADJUSTS_MIN_MAX             1
uint8_t LUX_MAPPING_SCHEMA =            LUX_ADJUSTS_BS;

////////////////////////////////// Lux Ambiant Lighting Thresholds /////////////////////
#if (ARTEFACT_TYPE == SPECULATOR) && HV_MAJOR == 2
#define NIGHT_LUX_THRESHOLD             1.0
// this is the threshold in which anything below will just be treated as the lowest reading
#define LOW_LUX_THRESHOLD               10.0
// when a lux of this level is detected the LEDs will be driven with a brightness scaler of 1.0
#define MID_LUX_THRESHOLD               350.0
#define HIGH_LUX_THRESHOLD              1200.0
#define EXTREME_LUX_THRESHOLD           5000.0

#elif (ARTEFACT_TYPE == SPECULATOR) && HV_MAJOR == 3
// the v3 hardware needs higher thresholds as it is brighter and thus needs to decrease its brightness sooner
#define NIGHT_LUX_THRESHOLD             10.0
// this is the threshold in which anything below will just be treated as the lowest reading
#define LOW_LUX_THRESHOLD               350.0
// when a lux of this level is detected the LEDs will be driven with a brightness scaler of 1.0
#define MID_LUX_THRESHOLD               1200.0
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
#endif // HV_MAJOR == 2

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
uint16_t  MAX_BRIGHTNESS =              510;
#else
uint16_t  MAX_BRIGHTNESS =              765;
#endif

///////////////////////////////// Lux Adjusts Brightness Scaler /////////////////////////////////
// on scale of 0-1.0 what is the min multiplier for the user defined brightness scaler
// 0.05 was too low, did not provide good enough feedback for the night time
#if (ARTEFACT_TYPE == SPECULATOR) || (ARTEFACT_TYPE == LEGATUS)
#define LUX_BS_MIN           0.1
#define LUX_BS_MAX           2.50
#else
#define LUX_BS_MIN           0.75
#define LUX_BS_MAX           1.50
#endif
/////////////////////////////// Update Regularity //////////////////////////
uint32_t lux_max_reading_delay =        1000 * 60 * 1;   // every minute
uint32_t lux_min_reading_delay =        1000 * 10;       // ten seconds
///////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Neopixel LEDs /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
// this is needed for the forced lux
#define UPDATE_ON_OFF_RATIOS            true

// this color is used for flashes
#define ONSET_RED                       200
#define ONSET_GREEN                     200
#define ONSET_BLUE                      255

#if (ARTEFACT_TYPE == SPECULATOR) && (HV_MAJOR > 2)
uint8_t LED_MAPPING_MODE = LED_MAPPING_CENTER_OUT;
#else
uint8_t LED_MAPPING_MODE = LED_MAPPING_STANDARD;
#endif //HV_MAJOR

// Which pin on the Arduino is connected to the NeoPixels? 8 for old board
#define LED1_PIN 5
// note that if the second LED channel is used the teensy needs to be overclocked to 120 MHz
#define LED2_PIN 8
#define LED3_PIN 10

// what the names of the neopixel strips will be
#if (ARTEFACT_TYPE == EXPLORATOR) && (BODY_TYPE == BELL_BODY)
#define LED1_NAME      "small"
#define LED2_NAME      "medium"
#define LED3_NAME      "large"
#elif (ARTEFACT_TYPE == EXPLORATOR) && (BODY_TYPE == WOODPECKER_BODY)
#define LED1_NAME      "Eye Stock"
#define LED2_NAME      "Pecker"
#define LED3_NAME      "N/A"
#elif (ARTEFACT_TYPE == SPECULATOR)
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
#elif (ARTEFACT_TYPE == EXPLORATOR) && (BODY_TYPE == WOODPECKER_BODY)
#define LED1_COUNT 10
#define LED2_COUNT 10
#define LED3_COUNT 1
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
bool INDEPENDENT_FLASHES =              false; // WARNING NOT IMPLEMENTED - TODO

bool FLASH_DOMINATES =                  false;

// if this is true then the brightness will b = (b + b) * b; in order to reduce its value, and make loud events even more noticable
bool SQUARE_BRIGHTNESS =                true;

bool SATURATED_COLORS =                 false;

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
#define FFT_FEATURES_ACTIVE                 1

///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// User Controls //////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

// in ms, how often will theUI controls update?
#if ((ARTEFACT_TYPE == SPECULATOR) && (HV_MAJOR == 2))
#define UI_POLLING_RATE               1000
#elif ARTEFACT_TYPE == SPECULATOR && HV_MAJOR == 3
#define UI_POLLING_RATE               60
#else
#define UI_POLLING_RATE               100
#endif // UI_POLLING_RATE

// should the UIManager print when the user control elements are changed?
#define P_UIMANAGER     true

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
#define P_LED_ON_RATIO                  true
#define P_COLOR                         false
#define P_COLOR_WIPE                    false
// for printing neopixelmanager onset (click) data
#define P_ONSET                         false

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
#define P_AUTOGAIN                               true

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
#define P_AUDIO_USAGE_MAX                         false
#define P_DOMINATE_CHANNEL                        true

// prints the max audio memory usage (to help calibrate how much is allocated to the system)
#define P_AUDIO_MEMORY_MAX                        false
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

//////////////////////////// Pots ////////////////////////////////////////////
// this wll allow the POT to overide the brightness at the end of color wipe
#define USER_BRIGHTNESS_OVERDRIVE                 true
#define P_POT_VALS                                false

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
#define AUTOGAIN_ACTIVE                                 true
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
#define CALCULATE_DOMINATE_CHANNEL                      true
#if CALCULATE_DOMINATE_CHANNEL == true
uint64_t first_dominate_channel_calculation =           1000 * 60 * 2;  // starting two minutes into the program
uint32_t dominate_channel_update_rate =                 1000 * 60 * 10; // every ten minutes
elapsedMillis dominate_channel_update =                 0;
#endif
int dominate_channel =                                  0;

////////////////////////////////////////////////////////////////////////////
///////////////////////// FFT Manager //////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
uint8_t num_fft_managers =                              2;

////////////////////////////////////////////////////////////////////////////
///////////////////////// Neo Manager //////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#if ARTEFACT_TYPE == SPECULATOR
#define NUM_NEOP_MANAGERS                               1
#elif ARTEFACT_TYPE == EXPLORATOR
#define NUM_NEOP_MANAGERS                               3
#endif
#endif // __CONFIGURATION_H__
