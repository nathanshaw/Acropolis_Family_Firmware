
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
#define FFT_FEATURES_ACTIVE             1

////////////////////////////////////////////////////////////////////////////
#if ONSET_DETECTION_ACTIVE
//////////////////////////////// Onset ////////////////////////////////////
elapsedMillis onset_feature_reset_tmr;
const unsigned long onset_feature_reset_time = (1000 * 60 * 1);// every 5 minute?

double last_range_rms = 0.0;
double min_rms = 1.0;
double max_rms = 0.1;

double last_flux = 0.0;
double min_flux = 1.0;
double max_flux = 0.1;

double last_cent_degd = 0.0;
double min_cent_negd = 1.0;
double max_cent_negd = 0.0;

double min_onset_feature = 1.0;
double max_onset_feature = 0.0;

double onset_flux = 0.0;
double onset_rms = 0.0;
double onset_cent = 1000.0;
double onset_feature = 0.0;

ValueTrackerDouble onset_tracker = ValueTrackerDouble((String) "Onset Feature", &onset_feature, 0.1, 3000, 1.0);
ValueTrackerDouble cent_tracker = ValueTrackerDouble((String) "Cent", &onset_cent, 0.1, 3000, 1.0);
ValueTrackerDouble rms_tracker = ValueTrackerDouble((String) "RMS", &onset_rms, 0.1, 3000, 1.0);

#endif // ONSET_DETECTION_ACTIVE

#endif // __CONFIGURATION_AUDIO_SYSTEM_H__