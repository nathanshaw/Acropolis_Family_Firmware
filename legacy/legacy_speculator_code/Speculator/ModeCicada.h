#ifndef __MODE_CICADA_H__
#define __MODE_CICADA_H__
/*
#include <WS2812Serial.h>
// #include <DLManager.h>
#include "Configuration.h"
#include <NeopixelManager.h>
#include <LuxManager.h>
// #include <UIManager.h>

#if HV_MAJOR > 2
#include <WeatherManager.h>
#endif // HV_MAJOR

#include <Audio.h>
#include <AudioEngine.h>

//////////////////////////////// Global Objects /////////////////////////
WS2812Serial leds(NUM_LED, LED_DISPLAY_MEMORY, LED_DRAWING_MEMORY, LED_PIN, WS2812_GRB);

NeoGroup neos = NeoGroup(&leds, 0, NUM_LED - 1, "All Neos", MIN_FLASH_TIME, MAX_FLASH_TIME);

// lux managers to keep track of the VEML readings
LuxManager lux_manager = LuxManager(lux_min_reading_delay, lux_max_reading_delay, LUX_MAPPING_SCHEMA);
// weather manager keeps track of temp and humidity sensors.

#if HV_MAJOR > 2
WeatherManager weather_manager = WeatherManager(HUMID_EXTREME_THRESH, TEMP_EXTREME_THRESH, TEMP_HISTORESIS);
#endif // HV_MAJOR

FeatureCollector feature_collector = FeatureCollector("All");

// DLManager datalog_manager = DLManager((String)"Datalog Manager");
FFTManager1024 fft_manager = FFTManager1024("Input FFT");

// AutoGain auto_gain = AutoGain(&feature_collector, &fft_manager, MIN_GAIN, MAX_GAIN, MAX_GAIN_ADJ);

// UIManager ui_manager = UIManager(UI_POLLING_RATE, POT_PLAY, P_UIMANAGER);

ValueTrackerFloat hue_tracker        = ValueTrackerFloat(&hue, HUE_DECAY_FACTOR, HUE_DECAY_DELAY, hsb_lp_level);
ValueTrackerFloat saturation_tracker = ValueTrackerFloat(&saturation, SAT_DECAY_FACTOR, SAT_DECAY_DELAY, hsb_lp_level);
ValueTrackerFloat brightness_tracker = ValueTrackerFloat(&brightness, BGT_DECAY_FACTOR, BGT_DECAY_DELAY, hsb_lp_level);

////////////////////////// Audio Objects //////////////////////////////////////////
AudioInputI2S            i2s1;           //xy=55,291.8571424484253
AudioFilterBiquad        biquad2;        //xy=217.00389099121094,302.0039281845093
AudioFilterBiquad        biquad1;        //xy=219.00390625,270.00391578674316
AudioAmplifier           amp2;           //xy=378.79129791259766,302.57704162597656
AudioAmplifier           amp1;           //xy=380.2198715209961,264.0055875778198
AudioAnalyzePeak         peak2;          //xy=517.0039100646973,316.003924369812
AudioAnalyzePeak         peak1;          //xy=521.00390625,221.0039176940918
AudioAnalyzeFFT1024      input_fft;      //xy=521.3627586364746,251.71987438201904
AudioAnalyzeRMS          rms1;            //xy=650.0000076293945,151.00000190734863
AudioAnalyzeRMS          rms2;            //xy=650.0000076293945,151.00000190734863

AudioConnection          patchCord10(amp1, rms1);
AudioConnection          patchCord11(amp2, rms2);

#if AUDIO_USB > 0
AudioOutputUSB           usb1;           //xy=519.142822265625,284.71433544158936
AudioConnection          patchCord5(amp2, 0, usb1, 1);
AudioConnection          patchCord7(amp1, 0, usb1, 0);
#endif // DEBUG_USB_AUDIO

AudioConnection          patchCord1(i2s1, 0, biquad1, 0);
AudioConnection          patchCord2(i2s1, 1, biquad2, 0);
AudioConnection          patchCord3(biquad2, amp2);
AudioConnection          patchCord4(biquad1, amp1);
AudioConnection          patchCord6(amp2, peak2);
AudioConnection          patchCord8(amp1, input_fft);
AudioConnection          patchCord9(amp1, peak1);
////////////////////////////////////////////////////////////////////////////////////

double current_brightness;
double brightness_feature_min;
double brightness_feature_max;
double color_feature_min;
double color_feature_max;
double last_brightness;

double calculateSongBrightness() {
  // how much energy is stored in the range of 4000 - 16000 compared to  the entire spectrum?
  // take the average of two-octave chunks from 1-4k and 250 - 1k
#if P_CALCULATE_BRIGHTNESS_LENGTH > 0
  uint32_t _start = millis();
#endif // calculate_brightness_length

  // 250 - 1k Hz is idx 5-23
  // 1k -4k Hz is idx 23-93
  // 4k - 16k Hz is idx 93 - 372
  double remaining_spect_avg = 0.5 * (fft_manager.getFFTRangeByIdx(5, 23) + fft_manager.getFFTRangeByIdx(23, 93));
  double target_brightness = fft_manager.getFFTRangeByIdx(93, 372) - remaining_spect_avg;
  if (target_brightness < 0.0) {
    target_brightness = 0.0;
    brightness_feature_min = 0.0;
  }

  if (target_brightness < brightness_feature_min) {
    if (P_BRIGHTNESS) {
      dprint(P_BRIGHTNESS, "target_B is less than feature_min: ");
      dprint(P_BRIGHTNESS, target_brightness, 5);
      dprint(P_BRIGHTNESS, " < ");
      dprint(P_BRIGHTNESS, brightness_feature_min, 5);
    }
    brightness_feature_min = (target_brightness * 0.15) + (brightness_feature_min * 0.85);
    if ( P_BRIGHTNESS) {
      dprint(P_BRIGHTNESS, " updated brightness_min and target_brightness to: ");
      dprintln(P_BRIGHTNESS, brightness_feature_min, 5);
    }
    target_brightness = brightness_feature_min;
  }

  if (target_brightness > brightness_feature_max) {
    dprint(P_BRIGHTNESS, "target_B is more than feature_max: ");
    dprint(P_BRIGHTNESS, target_brightness, 5);
    dprint(P_BRIGHTNESS, " > ");
    dprintln(P_BRIGHTNESS, brightness_feature_max, 5);

    brightness_feature_max = (target_brightness * BRIGHTNESS_LP_LEVEL) + (brightness_feature_max * (1.0 - BRIGHTNESS_LP_LEVEL));

    // to ensure that loud clipping events do not skew things too much
    if (brightness_feature_max > 1.0) {
      brightness_feature_max = 1.0;
    }

    dprint(P_BRIGHTNESS, " updated brightness_max and target_brightness to: ");
    dprintln(P_BRIGHTNESS, brightness_feature_max, 5);

    target_brightness = brightness_feature_max;
  }
  dprint(P_BRIGHTNESS, " target: ");
  dprint(P_BRIGHTNESS, target_brightness);
  dprint(P_BRIGHTNESS, "\tmin: ");
  dprint(P_BRIGHTNESS, brightness_feature_min);
  dprint(P_BRIGHTNESS, "\tmax: ");
  dprint(P_BRIGHTNESS, brightness_feature_max);
  // to ensure the unit is not always on
  // instead of comparing the brightness to the cuttoff_threshold
  // we subtract the cuttoff_threshold from both the target and the max
  target_brightness = (target_brightness - brightness_feature_min) / (brightness_feature_max - brightness_feature_min);
  target_brightness = target_brightness - BRIGHTNESS_CUTTOFF_THRESHOLD;
  if (target_brightness < 0.0) {
    target_brightness = 0.0;
  } else if (target_brightness > 1.0) {
    target_brightness = 1.0;
  }
  dprint(P_BRIGHTNESS, " adjusted: ");
  dprintln(P_BRIGHTNESS, target_brightness);
#if P_CALCULATE_BRIGHTNESS_LENGTH > 0
  Serial.print("calculateSongBrightness() function call length: ");
  Serial.println(millis() - _start);
#endif
  return target_brightness;
}


void setupAudio() {
  ////////////// Audio ////////////
  printDivide();
  Serial.println("Setting up Audio Parameters");
  printMinorDivide();
  AudioMemory(AUDIO_MEMORY);
  Serial.print("Audio Memory has been set to: ");
  Serial.println(AUDIO_MEMORY);
  /////////////////////////////////////////////////////////////////////
  // feature_collector 0-1 are for the song front/rear
  if (PEAK_FEATURE_ACTIVE) {
    feature_collector.linkPeak(&peak1, P_PEAK_VALS);
    feature_collector.linkPeak(&peak2, P_PEAK_VALS);
  }
  if (RMS_FEATURE_ACTIVE) {
    feature_collector.linkRMS(&rms1, P_RMS_VALS);
    feature_collector.linkRMS(&rms2, P_RMS_VALS);
  }
  if (FFT_FEATURES_ACTIVE) {
    fft_manager.linkFFT(&input_fft);
    Serial.println("Linked FFT to FFTManager");
    if (FIRMWARE_MODE == CICADA_MODE) {
      fft_manager.setupCentroid(true, 4000, 16000);
    } else if (FIRMWARE_MODE == PITCH_MODE) {
      fft_manager.setupCentroid(true, 120, 16000);
    }
    Serial.println("Started calculating Centroid in the FFTManager");
    fft_manager.setCalculateFlux(true);
    Serial.println("Started calculating FLUX in the FFTManager");
  }
  Serial.println("Feature collectors have been linked");
  /////////////////////////////////////////////////////////////////////
  // TODO
  // #define BQ1_THRESH 30
  // #define BQ2_
  biquad1.setHighpass(0, 30, 0.85);
  biquad1.setHighpass(1, 50, 0.85);
  biquad1.setHighpass(2, 60, 0.85);
  biquad1.setLowShelf(3, 80, ONSET_BQ1_DB);
  Serial.println("Biquad filter 1 has been configured");

  biquad2.setHighpass(0, 8000, 0.85);
  biquad2.setHighpass(1, 8000, 0.85);
  biquad2.setHighpass(2, 8000, 0.85);
  biquad2.setLowShelf(3, 8000, ONSET_BQ1_DB);
  Serial.println("Biquad filter 2 has been configured");

  amp1.gain(STARTING_GAIN * ENC_GAIN_ADJUST);
  amp2.gain(STARTING_GAIN * ENC_GAIN_ADJUST);
  Serial.print("Set amp1 and amp2 gains to: ");
  Serial.println(STARTING_GAIN * ENC_GAIN_ADJUST);

  // set gain level? automatically?
  // todo make this adapt to when microphones are broken on one or more side...
  Serial.println("Exiting setupAudio()");
  printDivide();
}

void updateOnset() {
#if ONSET_ACTIVE > 0
  // for a onset in theory the spectral flux will be high, the
  // centroid will decrease since the last frame, and there should be an
  // increase of amplitude in the 1k - 3k freq range

  // for all these values we want a 1.0 to be a confident assesment
  // from that feature

  /////////////////////////// Spectral Flux //////////////////////////
  double flux = fft_manager.getSpectralFlux();
  flux = updateScalers(flux, min_flux, max_flux, 0.5);
  last_flux = onset_flux;
  onset_flux = (flux / (max_flux));
  dprint(P_ONSET_FLUX, "onset flux: ");
  dprintln(P_ONSET_FLUX, onset_flux);

  //////////////////////////// Cent Neg Delta ////////////////////////
  double new_cent_degd = fft_manager.getCentroidNegDelta();
  new_cent_degd = updateScalers(new_cent_degd, min_cent_negd, max_cent_negd, 0.5);
  last_cent_degd = onset_cent;

  // Serial.print("onset_cent/min/max: ");
  // Serial.print((new_cent_degd), 8);
  onset_cent = (new_cent_degd / max_cent_negd);

  //////////////////////////// Energy between 1k - 3k //////////////////
  // using the bins instead of the hard coded frequencies saves some cycles
  // double range_rms = fft_manager.getFFTRangeByFreq(1000, 2000);
  double range_rms = fft_manager.getFFTRangeByIdx(23, 46);
  range_rms = updateScalers(range_rms, min_rrms, max_rrms, 0.5);
  last_range_rms = onset_rrms;
  onset_rrms = ((range_rms - min_rrms) / (max_rrms - min_rrms));

  //////////////////////////// Feature Calculation /////////////////////
  double feature = (onset_flux) * onset_cent * onset_rrms * 50;
  feature = updateScalers(feature, min_onset_feature, max_onset_feature, 0.5);
  if (P_ONSET_FEATURES == true) {
    Serial.print("flux / range / feature   :\t");
    Serial.print((onset_cent), 8);
    Serial.print("\t");
    Serial.print(onset_flux, 8);
    Serial.print("\t");
    Serial.print(onset_rrms, 8);
    Serial.print("\t");
    Serial.print(feature, 8);
    // dprintln(P_ONSET, feature);
    Serial.print("\t");
    Serial.println(feature, 8);
  }
  uint8_t red, green, blue;
  ////////////////// Calculate Actual Values ///////////////////////
  red = (current_color * ONSET_RED) * lux_manager.getBrightnessScaler();
  green = (current_color * ONSET_GREEN) * lux_manager.getBrightnessScaler();
  blue = (current_color * ONSET_BLUE) * lux_manager.getBrightnessScaler();

  // note that normally 1.0 is the threshold for onsets
  if (feature >= ONSET_THRESH) {
    // Serial.println("____________________ ONSET ________________________ 3.0");
    dprint(P_ONSET, "onset feature is above threshold: ");
    dprint(P_ONSET, current_feature[i]);
    dprint(P_ONSET, " - ");
    dprint(P_ONSET, threshold);
    neos.colorWipeAdd(red, green, blue, lux_manager.getBrightnessScaler() * user_brightness_scaler);
  }

  /////////////////// LOCAL SCALER RESET //////////////////////////
  // this needs to be after the rest of the logic in this function
  // when it was before it was adjusting the seed values to do odd things
  if (millis() > ((1000 * 60) + BOOT_DELAY) && onset_feature_reset_tmr > onset_feature_reset_time) {
    min_rrms += ((max_rrms - min_rrms) * 0.05);
    max_rrms -= ((max_rrms - min_rrms) * 0.05);
    min_flux += ((max_flux - min_flux) * 0.05);
    max_flux -= ((max_flux - min_flux) * 0.05);
    min_cent_negd += ((max_cent_negd - min_cent_negd) * 0.05);
    max_cent_negd -= ((max_cent_negd - min_cent_negd) * 0.05);
    dprintln(P_SONG_GENERAL, "reset onset feature min and max for rrms, flux and cent_negd ");
    onset_feature_reset_tmr = 0;
  }
#endif // ONSET_ACTIVE
}

void updateAutogain() {
#if (AUTOGAIN_ACTIVE)
  // these are the external values which are driving the auto-gain
  // in theory I think we want to use a combination of average RMS
  // and on/ratio to determine what the appropiate gain is for
  // the units
  // auto_gain.update();
  // after getting the on ratio and rms we need to then reset the values
  // neos.resetOnRatio();
  // feature_collector.resetRMSAvg();
  return;
#endif // AUTOGAIN_ACTIVE
}

void updateSong() {
#if P_NUM_SONG_UPDATES == 1
  song_updates++;
  uint32_t m = song_update_timer;
  if (song_update_timer > 1000) {
    Serial.print("song updates in last second: ");
    Serial.println(song_updates);
    song_updates = 0;
    song_update_timer = 0;
  }
#endif // P_NUM_SONG_UPDATES
  double target_color = 0.0;        // 0.0 - 1.0
  uint8_t red, green, blue;

  /////////////////// Color ////////////////////////////////////////
  target_color = fft_manager.getScaledCentroid();
  last_color = current_color;
  current_color = (target_color * COLOR_LP_LEVEL) + (last_color * (1.0 - COLOR_LP_LEVEL));

#if P_SONG_COLOR > 0
  dprint(P_SONG_COLOR, "target_color: ");
  dprint(P_SONG_COLOR, target_color);
  dprint(P_SONG_COLOR, "\tlast_color: ");
  dprint(P_SONG_COLOR, last_color);
  dprint(P_SONG_COLOR, "\tcurrent_color: ");
  dprint(P_SONG_COLOR, current_color);
#endif

  ////////////////// Calculate Actual Values ///////////////////////
  red = ((1.0 - current_color) * SONG_RED_LOW) + (current_color * SONG_RED_HIGH);
  green = ((1.0 - current_color) * SONG_GREEN_LOW) + (current_color * SONG_GREEN_HIGH);
  blue = ((1.0 - current_color) * SONG_BLUE_LOW) + (current_color * SONG_BLUE_HIGH);

  double target_brightness = 0.0;
  /////////////////// Brightness ///////////////////////////////////
  double __brightness = 0.0;
  if (target_brightness == 0.0) {
    __brightness = calculateSongBrightness();
  } else if (STEREO_FEEDBACK == true) {
    __brightness = calculateSongBrightness();
  }
  if (__brightness > target_brightness) {
    target_brightness = __brightness;
  }
  last_brightness = current_brightness;
  current_brightness = (target_brightness * BRIGHTNESS_LP_LEVEL) + (last_brightness * (1.0 - BRIGHTNESS_LP_LEVEL));

#if P_BRIGHTNESS > 0
  dprint(P_BRIGHTNESS, "last/current_brightness");
  dprint(P_BRIGHTNESS, last_brightness);
  dprint(P_BRIGHTNESS, " => ");
  dprintln(P_BRIGHTNESS, current_brightness);
#endif

  // red = (uint8_t)((double)red * current_brightness);
  // green = (uint8_t)((double)green * current_brightness);
  // blue = (uint8_t)((double)blue * current_brightness);

#if P_SONG_COLOR > 0
  dprint(P_SONG_COLOR, " r: ");
  dprint(P_SONG_COLOR, red);
  dprint(P_SONG_COLOR, " g: ");
  dprint(P_SONG_COLOR, green);
  dprint(P_SONG_COLOR, " b: ");
  dprintln(P_SONG_COLOR, blue);
#endif

  // adding thee target_brightness multiplier will weight the overall brightness in favor
  // of sounds which have more higher frequency content.
  neos.colorWipe(red, green, blue, current_brightness);// * (1.0 + (target_brightness * 0.25)));
#if P_UPDATE_SONG_LENGTH == 1
  Serial.print("updateSong() function call length: ");
  Serial.println(song_update_timer - m);
#endif // P_NUM_SONG_UPDATES
}

double updateScalers(double val, double & min, double & max, double rate) {
  double nval = val;
  if (val < min) {
    min = (min * rate) + (val * (1.0 - rate));
    nval = min;
  }
  if (val > max) {
    max = (max * rate) + (val * (1.0 - rate));
    nval = max;
  }
  return nval;
}

void printColors() {
  fft_manager.printFFTVals();
  if (P_NEO_COLORS) {
    neos.printColors();
  }
}

void updateMode() {
  updateSong();
  updateOnset();
}
*/

#endif // __MODE_CICADA_H_
