#ifndef __CONFIGURATION_LEGATUS_H__
#define __CONFIGURATION_LEGATUS_H__

#include <Macros.h>

float USER_CONTROL_PLAYBACK_GAIN             = 1.0;


// should correspond to the serial number on the PCB
#define SERIAL_ID                 13

////////////////////////////////////////
#if BEHAVIOUR_ROUTINE == B_LEG_FEEDBACK
#define AUDIO_MEMORY              45
#elif BEHAVIOUR_ROUTINE == B_LEG_ECHO_CHAMBER
#define AUDIO_MEMORY              55
#elif BEHAVIOUR_ROUTINE == B_LEG_SAMP_PLAYBACK
#define AUDIO_MEMORY              32
#else // BEHAVIOUR_ROUTINE == B_LEG_FEEDBACK
#define AUDIO_MEMORY              50
#endif // BEHAVIOUR_ROUTINE == B_LEG_FEEDBACK
////////////////////////////////////////

// TODO - this should be dynamically calculated
#if BEHAVIOUR_ROUTINE == B_LEG_SAMP_PLAYBACK
#define NUM_AUDIO_FILES           19
#else
#define NUM_AUDIO_FILES           1
#endif

#if BEHAVIOUR_ROUTINE == B_LEG_ECHO_CHAMBER
#define USE_RAW_AUDIO_PLAYER      true
#elif BEHAVIOUR_ROUTINE == B_LEG_SAMP_PLAYBACK
// as we are not playing back recordings we have captured ourselves
// we use the .WAV file playback
#define USE_RAW_AUDIO_PLAYER      false
#else
#define USE_RAW_AUDIO_PLAYER      false
#endif // determine if RAW_AUDIO_PLAYER is used


////////////////////////////////////////
#if USE_RAW_AUDIO_PLAYER == false
const char *audio_file_names[] = {
  "CHAIN1.WAV",
  "CICADA1.WAV",
  "CICADA2.WAV",
  "CICADA3.WAV",
  "CICADA4.WAV",
  "CICADA5.WAV",
  "FROGS1.WAV",
  "FROGS2.WAV",
  "FROGS3.WAV",
  "CHICK1.WAV",
  "CODE1.WAV",
  "PRESS1.WAV",
  "PRESS2.WAV",
  "FIRE1.WAV",
  "FIRE2.WAV",
  "FIRE3.WAV",
  "SEWING1.WAV",
  "SEWING2.WAV",
  "SEWING3.WAV"
};
#else // USE_RAW_AUDIO_PLAYER == false
const char *audio_file_names[] = {
  "CHAIN1.RAW",
  "CICADA1.RAW",
  "CICADA2.RAW",
  "CICADA3.RAW",
  "CICADA4.RAW",
  "CICADA5.RAW",
  "FROGS1.RAW",
  "FROGS2.RAW",
  "FROGS3.RAW",
  "CHICK1.RAW",
  "CODE1.RAW",
  "PRESS1.RAW",
  "PRESS2.RAW",
  "FIRE1.RAW",
  "FIRE2.RAW",
  "FIRE3.RAW",
  "SEWING1.RAW",
  "SEWING2.RAW",
  "SEWING3.RAW"
};
#endif // USE_RAW_AUDIO_PLAYER == false
////////////////////////////////////////

// This value is used to calculate how long between unit actuations
uint32_t      PLAYBACK_INTERVAL = 0;
// This timer is reset every time the artefact finishes vocalising
elapsedMillis last_playback_tmr;

#if BEHAVIOUR_ROUTINE == B_LEG_FM_FEEDBACK
#define MIN_PLAYBACK_GAIN       0.0001
#define MID_PLAYBACK_GAIN       0.05
#define MAX_PLAYBACK_GAIN       0.5
#define SD_PRESENT              false
#elif BEHAVIOUR_ROUTINE == B_LEG_SAMP_PLAYBACK
#define SD_PRESENT              true
#define MIN_PLAYBACK_GAIN       0.0001
#define MID_PLAYBACK_GAIN       0.05
#define MAX_PLAYBACK_GAIN       0.5
#elif BEHAVIOUR_ROUTINE == B_LEG_FEEDBACK
#define SD_PRESENT              true
#define MIN_PLAYBACK_GAIN       0.0001
#define MID_PLAYBACK_GAIN       0.01
#define MAX_PLAYBACK_GAIN       0.05
#elif BEHAVIOUR_ROUTINE == B_LEG_MATCH_PITCH
#define SD_PRESENT              false
#define MIN_PLAYBACK_GAIN       0.0001
#define MID_PLAYBACK_GAIN       0.05
#define MAX_PLAYBACK_GAIN       0.2
#elif BEHAVIOUR_ROUTINE == B_LEG_ECHO_CHAMBER
#define SD_PRESENT              true
#define MIN_PLAYBACK_GAIN       0.0001
#define MID_PLAYBACK_GAIN       0.05
#define MAX_PLAYBACK_GAIN       0.2
#endif // playback gains for legatus

// TODO - what is this, is this the pin that turns off the artefact?
#define PWR_KILL_PIN              8

////////////////////// Audio Recording Settings //////////////////////////////
#define AUDIO_REC_MAX_LENGTH      (1000 * 60)
elapsedMillis last_state_change;
// how long does legatus pause before playing back recordings or vocalising
// TODO - this variable should be determined by the current env. conditions.
long reflection_time = AUDIO_REC_MAX_LENGTH * 0.5;

#endif // __CONFIGURATION_LEGATUS_H__
