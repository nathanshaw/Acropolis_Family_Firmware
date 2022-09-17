#ifndef __EXPLORATOR_CONFIGURATION_H__
#define __EXPLORATOR_CONFIGURATION_H__

// a higher level the more debug printing occurs
#define DEBUG 0
#define TEST_SOLENOIDS 0
#define TEST_MOTOR 0
#define TEST_NEOP 0
// todo make this bring up the stereo test...?
#define TEST_MICS 0
// todo implement this
#define TEST_TEMP_HUMIDITY 0
#define TEST_LDRS 0

#define PRINT_PEAK_VALS 0

// should the program datalog?
#define DATALOG 0
#define PRINT_EEPROM_ON_BOOT 0

///////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Hardware and Software Settings ////////////////////
///////////////////////////////////////////////////////////////////////////////////////
#define HV_MAJOR       0
#define HV_MINOR       0
#define SV_MAJOR       0
#define SV_MINOR       0
#define SV_REVISION    1
///////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Neopixel LEDs /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
// Which pin on the Arduino is connected to the NeoPixels? 8 for old board
#define LED1_PIN 5
// note that if the second LED channel is used the teensy needs to be overclocked to 120 MHz
#define LED2_PIN 8
#define LED3_PIN 10

// what the names of the neopixel strips will be
#define LED1_NAME      "small"
#define LED2_NAME      "medium"
#define LED3_NAME      "large"

// How many NeoPixels are attached to the Arduino?
// if no LEDs are connected to the line then set this to 0
#define LED1_COUNT 16
#define LED2_COUNT 16
#define LED3_COUNT 16

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

const uint16_t max_led_count = max(max(LED1_COUNT, LED2_COUNT), LED3_COUNT);

byte drawingMemory[3][max_led_count * 3];       //  3 bytes per LED
DMAMEM byte displayMemory[3][max_led_count * 12]; // 12 bytes per LED

const uint8_t num_active_led_channels = LED1_ACTIVE + LED2_ACTIVE + LED3_ACTIVE;

///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// H-Bridge Motors  ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

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

const bool active_motors[3] = {MOT1_ACTIVE, MOT2_ACTIVE, MOT3_ACTIVE};

#elif HV_MAJOR == 1
#define NUM_MOTORS            3

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

const bool active_motors[NUM_MOTORS] = {MOT1_ACTIVE, MOT2_ACTIVE, MOT3_ACTIVE};

#endif // HV_MAJOR

///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// Solenoid Actuators /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
// this is how long in MS to wait after the dampener is removed before striking with solenoid
#define DAMPENER_DELAY 10

#if HV_MAJOR == 0

#define NUM_SOLENOIDS 6
#define SOL1_PIN 12
#define SOL2_PIN 11
#define SOL3_PIN 7
#define SOL4_PIN 6
#define SOL5_PIN 4
#define SOL6_PIN 3

#define SOL1_ACTIVE true
#define SOL2_ACTIVE true
#define SOL3_ACTIVE true
#define SOL4_ACTIVE true
#define SOL5_ACTIVE true
#define SOL6_ACTIVE true

const int s_pins[] = {SOL1_PIN, SOL2_PIN, SOL3_PIN, SOL4_PIN, SOL5_PIN, SOL6_PIN};
uint16_t sol_on_time[] = {30, 30, 30, 30, 30, 30};
bool sol_state[] = {false, false, false, false, false, false}; // is the solenoid on or off

# elif HV_MAJOR == 1
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

#define SOL1_ACTIVE true
#define SOL2_ACTIVE true
#define SOL3_ACTIVE false
#define SOL4_ACTIVE true
#define SOL5_ACTIVE true
#define SOL6_ACTIVE false
#define SOL7_ACTIVE true
#define SOL8_ACTIVE true
#define SOL9_ACTIVE false

const int s_pins[] = {SOL1_PIN, SOL2_PIN, SOL4_PIN, SOL5_PIN, SOL7_PIN, SOL8_PIN};
uint16_t sol_on_time[] = {30, 30, 30, 30, 30, 30};
bool sol_state[] = {false, false, false, false, false, false}; // is the solenoid on or off

#endif // HV_MAJOR == 1
// TODO - something about grouping together the solenoids?

///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// User Controls //////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
// in ms, how often will theUI controls update?
#define UI_POLLING_RATE 60

// should the UIManager print when the user control elements are changed?
#define P_UIMANAGER     true

#if HV_MAJOR == 0
///////////////////// Buttons //////////////////////////////////////////////////
#define NUM_BUTTONS     0
#define BUT1_PIN        33
#define BUT2_PIN        A13
#define BUT3_PIN        A12
#define BUT4_PIN        17

// should the values received from the buttons be reversed?
#define BUT1_REVERSE    false
#define BUT2_REVERSE    false
#define BUT3_REVERSE    false
#define BUT4_REVERSE    false

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

///////////////////// Pots /////////////////////////////////////////////////////
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
# elif HV_MAJOR == 1

///////////////////// Buttons //////////////////////////////////////////////////
#define NUM_BUTTONS     4
#define BUT1_PIN        33
#define BUT2_PIN        A13
#define BUT3_PIN        A12
#define BUT4_PIN        17

// should the values received from the buttons be reversed?
#define BUT1_REVERSE    false
#define BUT2_REVERSE    false
#define BUT3_REVERSE    false
#define BUT4_REVERSE    false

#define BUT1_ACTIVE     true
#define BUT2_ACTIVE     true
#define BUT3_ACTIVE     true
#define BUT4_ACTIVE     true

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
#define POT2_NAME       "POT2"

#endif // HV_MAJOR

///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// Weather Manager ////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
#define WM_UPDATE_DELAY                     1000
// what is the threshold that will determine when Humidity shutddown is activated. .50 equates to 50%
#define HUMID_EXTREME_THRESH                95
// what temp is the thrsehold (in c)        
#define TEMP_EXTRME_THRESH                  50
// 0.2 equates to 20% of the threshold, so if the threshold is 100 and 
// this value is 0.2 the Speculator will reactivate once the temperature
// drops to 80 degrees
#define TEMP_HISTORESIS                     0.2

///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// Lux Manager ////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
#define LUX_ADJUSTS_BS                  0
#define LUX_ADJUSTS_MIN_MAX             1
uint8_t LUX_MAPPING_SCHEMA =            LUX_ADJUSTS_MIN_MAX;

// These are determined by the LUX_READINGS, any values se here will just serve as a
// default value which will be overwriten once a lux reading is taken. 
// PLEASE NOTE: the MAX_BRIGHTNEESS is for all three color channels not just a single channel
// so for instance if rgb = 150, 150, 150. that would be limited if the MAX_BRIGHTNESS IS 450
// that makes it so a MAX_BRIGHTNESS is actually 765
uint16_t  MIN_BRIGHTNESS =              0;
uint16_t  MAX_BRIGHTNESS =              765;

#define NIGHT_LUX_THRESHOLD             1.0
// this is the threshold in which anything below will just be treated as the lowest reading
#define LOW_LUX_THRESHOLD               10.0
// when a lux of this level is detected the LEDs will be driven with a brightness scaler of 1.0
#define MID_LUX_THRESHOLD               350.0
#define HIGH_LUX_THRESHOLD              1200.0
#if HV_MAJOR < 3
#define EXTREME_LUX_THRESHOLD           5000.0
#else
#define EXTREME_LUX_THRESHOLD           5000.0
#endif

// on scale of 0-1.0 what is the min multiplier for the user defined brightness scaler
#define BRIGHTNESS_SCALER_MIN           0.05
#define BRIGHTNESS_SCALER_MAX           3.00

uint32_t lux_max_reading_delay =        1000 * 60 * 1;   // every minute
uint32_t lux_min_reading_delay =        1000 * 10;       // ten seconds

/////////////////////////////////////////////////////////////////////////
/////////////////////////      FFTManager  //////////////////////////////
/////////////////////////////////////////////////////////////////////////
// When calculating things such as which bin has the most energy and so on,
// what bin is considered the "1st?" and which one is the last?
// bin two covers 86 - 129 Hz
#define FFT_LOWEST_BIN              3
// this is 16340 Hz
#define FFT_HIGHEST_BIN             380
// todo this needs to be calculated better?
#define FFT_NUM_BINS                377

// used the scaled FFT readings or the normal FFT readings, the scaled readings will eensure that
// all the bins of intrest will have their magnitudes add up to 1, thus is best used for determining the centroid within a sub frequency range (for instance 8k - 14k or something
#define SCALE_FFT_BIN_RANGE             false
#define FFT_FEATURES_ACTIVE                 1

#endif // __CONFIGURATION__H__
