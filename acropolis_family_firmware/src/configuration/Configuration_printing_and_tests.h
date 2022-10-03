#ifndef __CONFIGURATION_PRINTING_AND_TESTS_H__
#define __CONFIGURATION_PRINTING_AND_TESTS_H__
/* This configuration file
 * contains all the Serial printing options along
 * with the options for initalising setup and boot tests
*/
#include <Macros.h>

// prints high-level data from the behaviourUpdateLoop
#define P_BEHAVIOUR_UPDATE       false
#define P_AUDIO_MEMORY_MAX       false

#if P_AUDIO_MEMORY_MAX
////////////////////////////////// RANDOM stuff ///////////////////////////////////
// this is a high value to force jumper readings in the setup() loop
elapsedMillis last_audio_use_print = 0;
#define AUDIO_USE_RESET_RATE         20000
#endif // P_AUDIO_MEMORY_MAX
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ////////////////////////// Boot Tests ///////////////////////////////////////////////////////
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
#define TEST_SOLENOIDS           false
#define TEST_MOTORS              false
#define TEST_LEDS                false
// TODO
#define TEST_LUX                 false
#define TEST_TEMP                false
#define TEST_DOF                 false

///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// Boot Delay /////////////////////////////////////////////
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
#define P_LUX_MANAGER_READINGS          false
#define P_EXTREME_LUX                   false

// sets p_lux_readings within the lux_manager but also the NeoPixelManager
#define P_LUMIN                         false
// sets p_lux within the lux_manager
#define P_LUX_READINGS                  false
// sets general debug printing for the lux_manager class
#define P_LUX_MANAGER_DEBUG             false

// sets print_brightness_scaler within the lux_manager
#define P_BS                            false

// TODO - what is this?
#define P_CALCULATE_BRIGHTNESS_LENGTH   false

// this is where the final brightness scaler is applied
#define P_PACK_COLORS                   false

// TODO - what is this?
#define P_SONG_GENERAL                  false
#define P_SONG_COLOR                    false

// basically do you want to print the number of song updates which occur every second?
#define P_NUM_SONG_UPDATES              false
#if P_NUM_SONG_UPDATES == 1
uint16_t song_updates = 0;
elapsedMillis song_update_timer = 0;
#endif // P_NUM_SONG_UPDATES

#define P_UPDATE_SONG_LENGTH                      false
#define P_UPDATE_PASSIVE_LEDS                     false

//////////////////////////// Onset Functionality ///////////////////////
#define P_ONSET_FEATURES                          false
#define P_ONSET                                   false

//////////////////////////// AutoGain //////////////////////////////////
#define P_AUTOGAIN                                false

// for calculating the dominate channel, it works best if the dominate channel is re-caculated every

/////////////////////////// Datalogging ////////////////////////////////
#define P_LOG_WRITE                               true
#define DLM_PRINT                                 false

///////////////////////////////////////////////////////////////////////////////
///////////////////////// Audio Analysis///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#define P_RMS_VALS                                false
#define P_PEAK_VALS                               false
// #define P_TONE_VALS                     false
#define P_FREQ_VALS                               false
#define P_AUDIO_USAGE_MAX                         false
#define P_DOMINATE_CHANNEL                        false

//////////////////////////// Audio Playback ///////////////////////////////////
#define P_AUDIO_PLAYBACK                          false
// prints feedback from the function of the same name used to calculate the
// vocalisation lock period which prevents the artefact from vocalising too often
#define P_CALCULATE_PLAYBACK_INTERVAL             false

// prints information within the function of the same name
#define P_UPDATE_LEGATUS_PASSIVE_LEDS             false

//////////////////////////// Audio Memory Usage ///////////////////////////////
// prints the max audio memory usage (to help calibrate how much is allocated to the system)
#if P_AUDIO_USAGE_MAX == true
// TODO - this needs to be fully implemented
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

///////////////////////////////////////////////////////////////////////////////
//////////////////// General Purpose //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// activates some printing which will print out how long different functions calls take
// TODO - this needs implementation to prove real-time operation

#define P_FUNCTION_TIMES                          true
#if P_FUNCTION_TIMES == false
#define P_SPECULATOR_LED_UPDATE_RATE              false
#else
#define P_SPECULATOR_LED_UPDATE_RATE              false
#endif // P_FUNCTION_TIMES

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

//////////////////////// Printing Legatus-specific operating routines ////////
#define P_MATCH_PITCH_VOCALISATION                false
#endif // _CONFIGURATION_PRINTING_AND_TESTS_H__