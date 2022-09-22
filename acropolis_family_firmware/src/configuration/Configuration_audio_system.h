
#ifndef __CONFIGURATION_AUDIO_SYSTEM_H__
#define __CONFIGURATION_AUDIO_SYSTEM_H__
#include <Macros.h>

///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// Audio Analysis /////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

// if set to true an audio USB object will be created so the audio can be debuged via Audacity
#define AUDIO_USB                     false

#if ARTEFACT_GENUS == SPECULATOR
#if BEHAVIOUR_ROUTINE == B_TARGETED_FEEDBACK
#define NUM_AMPLIFIERS                1
#define NUM_PEAK_ANAS                 1
#define NUM_RMS_ANAS                  1
#define NUM_FFT                       1
#define NUM_CHANNELS                  1
#elif BEHAVIOUR_ROUTINE == B_ADAPTIVE_FEEDBACK
#define NUM_AMPLIFIERS                2
#define NUM_PEAK_ANAS                 0
#define NUM_RMS_ANAS                  0
#define NUM_FFT                       1
#define NUM_CHANNELS                  1
#endif // specualtor behaviours
#elif ARTEFACT_GENUS == EXPLORATOR
#define NUM_AMPLIFIERS                1
#define NUM_PEAK_ANAS                 1
#define NUM_RMS_ANAS                  0
#define NUM_FFT                       1
#define NUM_CHANNELS                  1
#elif ARTEFACT_GENUS == LEGATUS
#define NUM_AMPLIFIERS                1
#define NUM_PEAK_ANAS                 1
#define NUM_RMS_ANAS                  1
#define NUM_FFT                       1
#define NUM_CHANNELS                  1
#endif

#endif // __CONFIGURATION_AUDIO_SYSTEM_H__