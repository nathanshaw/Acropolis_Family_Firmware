#ifndef __LEGATUS_CONFIGURATION_H__
#define __LEGATUS_CONFIGURATION_H__

#define AUDIO_MEMORY              24

#define NUM_AUDIO_FILES           6

const char *audio_file_names[] = {
  "SDTEST1.WAV",
  "SDTEST2.WAV",
  "SDTEST3.WAV",
  "SDTEST4.WAV",
  "SDTEST5.WAV",
  "SDTEST6.WAV"
};

// This value is used to calculate how long between unit actuations
uint32_t      PLAYBACK_INTERVAL = 0;
elapsedMillis last_playback_tmr;

#define PWR_KILL_PIN              8

#endif // __LEGATUS_CONFIGURATION_H__
