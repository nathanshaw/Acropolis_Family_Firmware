#ifndef __LEGATUS_CONFIGURATION_H__
#define __LEGATUS_CONFIGURATION_H__

#if FIRMWARE_MODE == FEEDBACK_MODE
#define AUDIO_MEMORY              90
#else
#define AUDIO_MEMORY              32
#endif


#define NUM_AUDIO_FILES           19

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

// This value is used to calculate how long between unit actuations
uint32_t      PLAYBACK_INTERVAL = 0;
elapsedMillis last_playback_tmr;

#define PWR_KILL_PIN              8

#endif // __LEGATUS_CONFIGURATION_H__
