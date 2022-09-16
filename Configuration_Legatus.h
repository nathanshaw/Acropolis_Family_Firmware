#ifndef __LEGATUS_CONFIGURATION_H__
#define __LEGATUS_CONFIGURATION_H__

#ifndef __MACROS_H__
#include "Macros.h"
#endif

// should correspond to the serial number on the PCB
#define SERIAL_ID                 13

////////////////////////////////////////
#if FIRMWARE_MODE == FEEDBACK_MODE
#define AUDIO_MEMORY              45
#else // FIRMWARE_MODE == FEEDBACK_MODE
#define AUDIO_MEMORY              16
#endif // FIRMWARE_MODE == FEEDBACK_MODE
////////////////////////////////////////

#define NUM_AUDIO_FILES           19

#define USE_RAW_AUDIO_PLAYER      false

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
  "CHAIN1.raw",
  "CICADA1.raw",
  "CICADA2.raw",
  "CICADA3.raw",
  "CICADA4.raw",
  "CICADA5.raw",
  "FROGS1.raw",
  "FROGS2.raw",
  "FROGS3.raw",
  "CHICK1.raw",
  "CODE1.raw",
  "PRESS1.raw",
  "PRESS2.raw",
  "FIRE1.raw",
  "FIRE2.raw",
  "FIRE3.raw",
  "SEWING1.raw",
  "SEWING2.raw",
  "SEWING3.raw"
};
#endif // USE_RAW_AUDIO_PLAYER == false
////////////////////////////////////////

// This value is used to calculate how long between unit actuations
uint32_t      PLAYBACK_INTERVAL = 0;
elapsedMillis last_playback_tmr;

#define PWR_KILL_PIN              8

#endif // __LEGATUS_CONFIGURATION_H__
