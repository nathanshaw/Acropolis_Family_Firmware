
#ifndef __CONFIGURATION_LED_FEEDBACK_H__
#define __CONFIGURATION_LED_FEEDBACK_H__

// to get some definitions such as SPECULATOR
#include <Macros.h>

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
float user_brightness_scaler               = 1.0;

///////////////////////// Saturation  /////////////////////////////////////
// TODO finish integrating this
/*
When the code is within the HSB colour space, there is an option to 
offset the saturation level according to this value
*/
float ADDED_SATURATION  = 0.4;

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


// if false, a onset detected on either side results in a LED flash on both sides
// if true, a onset detected on one side will only result in a flash on that side
int INDEPENDENT_ONSETS =               false; // WARNING NOT IMPLEMENTED - TODO
int FLASH_DOMINATES =                  false;

// if this is true then the brightness will b = (b + b) * b; in order to reduce its value, and make loud events even more noticable
int SQUARE_BRIGHTNESS =                false;

#if BEHAVIOUR_ROUTINE == B_TARGETED_FEEDBACK
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
/////////////////////////////////// Neopixel LEDs /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
// this is needed for the forced lux
#define UPDATE_ON_OFF_RATIOS            false

/////////////////////////// Onsets //////////////////////
#if ARTEFACT_GENUS == SPECULATOR
#if BEHAVIOUR_ROUTINE == B_TARGETED_FEEDBACK
#define ONSET_DETECTION_ACTIVE        true
#elif BEHAVIOUR_ROUTINE == B_ADAPTIVE_FEEDBACK
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

#endif // __CONFIGURATION_LED_FEEDBACK_H__