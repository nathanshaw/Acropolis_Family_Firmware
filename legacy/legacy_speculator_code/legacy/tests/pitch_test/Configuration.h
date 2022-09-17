// this is the primary use configurable portion of the program

#ifndef CONFIGURATION_H
#define CONFIGURATION_H 
// for things which can be either on or off, the name is simply defined?

///////////////////////// General Settings /////////////////////////////////
#define SERIAL_ID 3

///////////////////////// Operating Modes //////////////////////////////////
#define CICADA_MODE 0
#define PITCH_MODE  1
#define FIRMWARE_MODE (PITCH_MODE)

///////////////////////// Lux    Settings /////////////////////////////////
#define LUX_CALIBRATION_TIME 4000
#define SMOOTH_LUX_READINGS true

#define LOW_LUX_THRESHOLD 16.0
// when a lux of this level is detected the LEDs will be driven with a brightness scaler of 1.0
#define MID_LUX_THRESHOLD 100
#define HIGH_LUX_THRESHOLD 450.0

// Neo Pixels
#define MIN_BRIGHTNESS 10
#define MAX_BRIGHTNESS 255

// on scale of 0-1.0 what is the min multiplier for lux sensor brightness adjustment
#define BRIGHTNESS_SCALER_MIN 0.5
#define BRIGHTNESS_SCALER_MAX 1.5

unsigned long lux_max_reading_delay = long(1000.0 * 60.0 * 6); // every 6 minutes
unsigned long lux_min_reading_delay = long(1000.0 * 60.0 * 1); // one minute

bool front_lux_active = true;
bool rear_lux_active = true;

////////////////////////////////////////////////////////////////////////////
///////////////////////// Datalog Settings /////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// record the run time // last value is number of minutes
#define RUNTIME_POLL_DELAY 1000*60*5
// when should the value log start // last value is number of minutes
#define LOG_START_DELAY 1000*60*1
// how long should the logging last? // last value is number of hours
#define LOG_TIME_FRAME 1000*60*60*0.5

///////////////////////// Jumper Settings /////////////////////////////////
// turn on/off reading jumpers in setup (if off take the default "true" values for jumper bools
#define JUMPERS_POPULATED 0

///////////////////////// Auto-Gain Settings /////////////////////////////////
// turn on/off auto gain. 0 is off, 1 is on
// #define AUTO_GAIN 1

// maximum amount of gain (as a proportion of the current gain) to be applied in the
// auto gain code. This value needs to be less than 1. 0.5 would mean that the gain can change
// by a factor of half its current gain. So, if the gain was 2.0 then it could be increased/decreased by 1.0
// with a max/min value of 1.0 / 3.0.
#define MIN_LED_ON_RATIO (0.3)
#define MAX_LED_ON_RATIO (0.95)

///////////////////////// Debuggings ////////////////////////////////////
#define PRINT_LUX_DEBUG       true
#define PRINT_LUX_READINGS    false
#define PRINT_BRIGHTNESS_SCALER_DEBUG false
// TODO
#define PRINT_SONG_DATA       false
// TODO
#define PRINT_CLICK_FEATURES  false
#define PRINT_CLICK_DEBUG     false
// TODO
#define PRINT_LED_VALUES      false
#define PRINT_AUTO_GAIN       false

#define PRINT_LED_DEBUG       false

#define PRINT_LOG_WRITE       false

#define EEPROM_WRITE_CHECK    false

// feature collector related
#define PRINT_RMS_VALS        false
#define PRINT_PEAK_VALS       false
#define PRINT_TONE_VALS       false
#define PRINT_FREQ_VALS       false
#define PRINT_FFT_VALS        false

/////////////////////////////////////////////////////////////////////////
//////////////////////// Firmware Controls //////////////////////////////
/////////////////////////////////////////////////////////////////////////
// set to true if you want to print out data stored in EEPROM on boot
#define PRINT_EEPROM_CONTENTS false

// turns on/off debug printing
#define DEBUG 0

// data logging related
#define AUDIO_USAGE_POLL_RATE 200000

// Audio
#define USB_OUTPUT 1
#define MAX_GAIN_ADJUSTMENT 0.10

const uint32_t auto_gain_frequency = 1000 * 60 * 10; // how often to calculate auto-gain (in ms)

// song gain
#define RMS_DELTA  0
#define PEAK_DELTA 1
#define ALL_FEATURES 10
//////// Song Settings

// TODO - add bin magnitude as a feature
// which audio feature to use to test
// "peak" will look at the audio "peak" value
// "rms" will look at the audio "rms" value
#define SONG_FEATURE PEAK_DELTA

// TODO need to determine what are good values for these
#define MIN_SONG_PEAK_AVG 0.005
#define MAX_SONG_PEAK_AVG 0.20

#define STARTING_SONG_GAIN 8.0

#define SONG_BQ1_THRESH 13500
#define SONG_BQ1_Q 0.85
#define SONG_BQ1_DB -12
#define SONG_BQ2_THRESH 14000
#define SONG_BQ2_Q 0.85
#define SONG_BQ2_DB -12

///////// Click Settings
// what feature will be used to determine if a click has been found
// "rms_delta" will use that feature along with CLICK_RMS_DELTA_THRESH
// "peak_delta" will use that feature along with CLICK_PEAK_DELTA_THRESH
// "all" will use all available features with their corresponding thresholds

#define CLICK_FEATURE RMS_DELTA
#define CLICK_RMS_DELTA_THRESH 0.03
#define CLICK_PEAK_DELTA_THRESH 0.03

// One click per ten minutes
#define MIN_CLICKS_PER_MINUTE 0.1
#define MAX_CLICKS_PER_MINUTE 40.0

#define STARTING_CLICK_GAIN 6.0

#define MIN_CLICK_GAIN 0.5
#define MAX_CLICK_GAIN 24.0

#define MIN_SONG_GAIN 0.5
#define MAX_SONG_GAIN 20

#define CLICK_BQ1_THRESH 1200
#define CLICK_BQ1_Q 0.95
#define CLICK_BQ1_DB -24
#define CLICK_BQ2_THRESH 2500
#define CLICK_BQ2_Q 0.95
#define CLICK_BQ2_DB -24

//////////////////// Leds

#endif // CONFIGURATION_H

#ifndef CONFIGURATION_ADV_H
#define CONFIGURATION_ADV_H

////////////////////////// Lux Sensors /////////////////////////////////////////
// TODO these need to change into variables which are set with the jumpers?

// turn this flag off if no I2C multiplexer is present
#define I2C_MULTI 1

// how long the lux sensors need the LEDs to be turned off in order to get an accurate reading
#define LUX_SHDN_LEN 40

///////////////////////// Audio //////////////////////////////////////////////////

#define FRONT_MICROPHONE_INSTALLED    true
#define REAR_MICROPHONE_INSTALLED     true

////////////////////////// Neo Pixels ///////////////////////////////////////////
#define UPDATE_ON_OFF_RATIOS true
// the amount of time that the LEDs need to be shutdown to allow lux sensors to get an accurate reading
#define FLASH_DEBOUNCE_TIME 200

// this file assigns pin values to different variables depending on the version of the board
/*
#ifndef __PINS_H__
#define __PINS_H__
*/
////////////////////////////////////////////////////////////////////////////
//////////////////////// Hardware Controls /////////////////////////////////
////////////////////////////////////////////////////////////////////////////

// how high the click flash timer will go up to
#define MAX_FLASH_TIME 200
// where the click flash timer will start
#define MIN_FLASH_TIME 40

// cicada (-) or bird song (+) mode
#define JMP1_PIN 12
bool cicada_mode = true;

// mono audio (-) or stereo (+)
#define JMP2_PIN 11
bool stereo_audio = true;
uint8_t num_channels = stereo_audio + 1;
#define FRONT_MIC true
#define REAR_MIC true

// one front lux (-) or two lux (+)
#define JMP3_PIN 14

// if false, a click detected on either side results in a LED flash on both sides
// if true, a click detected on one side will only result in a flash on that side
bool INDEPENDENT_CLICKS = true;

// independent lux readings (-) or average/combine lux readings (+)
#define JMP4_PIN 15
bool combine_lux_readings = false;

// auto gain adjust (-) or not (+)
#define JMP5_PIN 16
bool gain_adjust_active = true;

// data log not active (-) or active (+)
#define JMP6_PIN 17
bool data_logging_active = true;

// #endif // __PINS_H__
#define AUDIO_MEMORY 24

#define TCA_CHANNELS 2

#endif // CONFIGURATION_ADV_H
