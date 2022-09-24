
#ifndef __CONFIGURATION_AUDIO_SYSTEM_H__
#define __CONFIGURATION_AUDIO_SYSTEM_H__
#include <Macros.h>

///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// Audio Analysis /////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

// if set to true an audio USB object will be created so the audio can be debuged via Audacity
// WARNING - THIS FEATURE IS NOT FULLY IMPLEMENTED YET
// INSTEAD, USE THE test files for microphones
#define AUDIO_USB                             false

// if stereo feedback is set to true than only audio from both channels will be used to calculate visual feedback brightness and color
// not generally recommended... - TODO - currently needs a fix
#define STEREO_FEEDBACK                       false
// MIX_AUDIO determines if, when there is no stereo feedback,  if the input from the front and rear microphones
// should be mixed together or if a dominate microphone will be chosen.
#define MIX_AUDIO                             false

#if ARTEFACT_GENUS == SPECULATOR
#if BEHAVIOUR_ROUTINE == B_TARGETED_FEEDBACK
#define NUM_AMPLIFIERS                1
#define NUM_PEAK_ANAS                 1
#define NUM_RMS_ANAS                  1
#define NUM_FFT                       1
#define NUM_CHANNELS                  1
#elif BEHAVIOUR_ROUTINE == B_ADAPTIVE_FEEDBACK
#define NUM_AMPLIFIERS                0
#define NUM_PEAK_ANAS                 1
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

/////////////////////////////////////////////////////////////////////////
//////////////////////// Audio Settings /////////////////////////////////
/////////////////////////////////////////////////////////////////////////

// how often does the feature collector update
//33 is 30 times a second
#define FC_UPDATE_RATE                        10

#if ARTEFACT_SPECIES == SPEC_MAJOR
#define AUDIO_MEMORY                          12
#elif ARTEFACT_SPECIES == SPEC_MINOR
#define AUDIO_MEMORY                          50
#endif

bool stereo_audio =                           true;


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
///////////////////////// Audio System /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// for speculator cicada installations channel 1 is song and channel 2 is click?
// TODO - the input to the two different sides should be the dominate microphones

// this is dictated by user controls and is multiplied against the STARTING_GAIN to determine runtime gain
float USER_CONTROL_GAIN_ADJUST               = 1.0;

// TODO - implement this
#define SPH_MICROPHONE                        10
#define TDK_MICROPHONE                        1
#if ARTEFACT_SPECIES == SPECULATOR_MAJOR
#define MICROPHONE_TYPE                       SPH_MICROPHONE
#else
#define MICROPHONE_TYPE                       TDK_MICROPHONE
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// Starting Gain //////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
#if ARTEFACT_GENUS == SPECULATOR 
#if ARTEFACT_SPECIES == SPEC_MAJOR
// most Speculator Major artefacts use the old microphones that require huge amounts of gain...
#define STARTING_GAIN                         1.0
#elif ARTEFACT_SPECIES == SPEC_MINOR
// 30.0 is good for testing when no enclosure is present, but a higher value should be used when an enclosure is present
// 240.00 is good for the better mics?
#define STARTING_GAIN                         72.0
#endif // types of speculator species
#elif ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_CLAPPER
#define STARTING_GAIN                         20.0
#elif ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_SPINNER
#define STARTING_GAIN                         20.0
#elif ARTEFACT_GENUS == EXPLORATOR && ARTEFACT_SPECIES == EX_WINDER
#define STARTING_GAIN                         480.0

#elif ARTEFACT_GENUS == EXPLORATOR
#define STARTING_GAIN                         240.0

#elif ARTEFACT_GENUS == LEGATUS
#if ARTEFACT_BEHAVIOUR == B_LEG_FEEDBACK
#define STARTING_GAIN                         0.5
#else
#define STARTING_GAIN                         6.0
#endif
#endif // makeup_gain

// makeup gain, applied after the filtering stage to boost signal
#if ARTEFACT_GENUS == SPECULATOR
#define MAKEUP_GAIN                            2.0
#else
#define MAKEUP_GAIN                            2.0
#endif


//////////////////////////////// STARTING GAIN //////////////////////////////////////////////
float starting_gain = STARTING_GAIN * ENCLOSURE_GAIN_FACTOR;
/////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

#if ARTEFACT_GENUS == SPECULATOR && BEHAVIOUR_ROUTINE == B_TARGETED_FEEDBACK
// SONG HP
#define LBQ1_THRESH         4000
#define LBQ1_Q              0.85
#define LBQ1_DB             -12
// SONG LP
#define LBQ2_THRESH         16000
#define LBQ2_Q              0.85
#define LBQ2_DB             -12
// CLICK HP
#define RBQ1_THRESH         1200
#define RBQ1_Q              0.85
#define RBQ1_DB             -12
// CLICK LP
#define RBQ2_THRESH         3000
#define RBQ2_Q              0.85
#define RBQ2_DB             -12

#elif ARTEFACT_GENUS == SPECULATOR && BEHAVIOUR_ROUTINE == B_ADAPTIVE_FEEDBACK
// Mix HP
#define LBQ1_THRESH         400
#define LBQ1_Q              1.0
#define LBQ1_DB             -12
// Mix LP
#define LBQ2_THRESH         20000
#define LBQ2_Q              1.0
#define LBQ2_DB             -12
// Should be Inactive
#define RBQ1_THRESH         400
#define RBQ1_Q              1.0
#define RBQ1_DB             -12
// Should be Inactive
#define RBQ2_THRESH         20000
#define RBQ2_Q              0.85
#define RBQ2_DB             -12
//////////////////
#elif ARTEFACT_GENUS == EXPLORATOR
// SONG HP
#define LBQ1_THRESH         400
#define LBQ1_Q              1.0
#define LBQ1_DB             -12

// SONG LP
#define LBQ2_THRESH         20000
#define LBQ2_Q              1.0
#define LBQ2_DB             -12

// Should be Inactive
#define RBQ1_THRESH         400
#define RBQ1_Q              1.0
#define RBQ1_DB             -12

// Should be Inactive
#define RBQ2_THRESH         20000
#define RBQ2_Q              1.0
#define RBQ2_DB             -12

//////////////////
#elif ARTEFACT_GENUS == LEGATUS
// Microphone HP
#define LBQ1_THRESH         400
#define LBQ1_Q              1.0
#define LBQ1_DB             -12
// Microphone LP
#define LBQ2_THRESH         20000
#define LBQ2_Q              1.0
#define LBQ2_DB             -12

// playback HP
#define RBQ1_THRESH         80
#define RBQ1_Q              1.0
#define RBQ1_DB             -12
// playback LQ
#define RBQ2_THRESH         18000
#define RBQ2_Q              1.0
#define RBQ2_DB             -12

#else
// SONG HP
#define LBQ1_THRESH         400
#define LBQ1_Q              1.0
#define LBQ1_DB             -12
// SONG LP
#define LBQ2_THRESH         20000
#define LBQ2_Q              1.0
#define LBQ2_DB             -12

// Should be Inactive
#define RBQ1_THRESH         400
#define RBQ1_Q              1.0
#define RBQ1_DB             -12
// Should be Inactive
#define RBQ2_THRESH         20000
#define RBQ2_Q              1.0
#define RBQ2_DB             -12
#endif
/////////////////////////////////////////////////////////////////////////
/////////////////////////      FFTManager  //////////////////////////////
/////////////////////////////////////////////////////////////////////////
// used the scaled FFT readings or the normal FFT readings, the scaled readings will eensure that
// all the bins of intrest will have their magnitudes add up to 1, thus is best used for determining the centroid within a sub frequency range (for instance 8k - 14k or something
#define SCALE_FFT_BIN_RANGE                     false
#define FFT_FEATURES_ACTIVE                     1


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