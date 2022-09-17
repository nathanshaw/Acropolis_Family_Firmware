#ifndef __MODE_PITCH_H__
#define __MODE_PITCH_H__

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
WeatherManager weather_manager = WeatherManager(HUMID_EXTREME_THRESH, TEMP_EXTREME_THRESH, TEMP_HISTORESIS, WEATHER_MANAGER_UPDATE_RATE);
#endif // HV_MAJOR

FeatureCollector feature_collector = FeatureCollector("All");

// DLManager datalog_manager = DLManager((String)"Datalog Manager");
FFTManager1024 fft_manager = FFTManager1024(FFT_LOWEST_BIN, FFT_HIGHEST_BIN, "Input FFT");

// AutoGain auto_gain = AutoGain(&feature_collector, &fft_manager, MIN_GAIN, MAX_GAIN, MAX_GAIN_ADJ);

// UIManager ui_manager = UIManager(UI_POLLING_RATE, POT_PLAY, P_UIMANAGER);

ValueTrackerDouble hue_tracker        = ValueTrackerDouble((String)"HUE", &hue, HUE_DECAY_FACTOR, HUE_DECAY_DELAY, HUE_LP_LEVEL);
ValueTrackerDouble saturation_tracker = ValueTrackerDouble((String)"SATURATION", &saturation, SAT_DECAY_FACTOR, SAT_DECAY_DELAY, SATURATION_LP_LEVEL);
ValueTrackerDouble brightness_tracker = ValueTrackerDouble((String)"BRIGHTNESS", &brightness, BGT_DECAY_FACTOR, BGT_DECAY_DELAY, BRIGHTNESS_LP_LEVEL);

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

void setupAudio() {
  ////////////// Audio ////////////
  printMajorDivide("Setting up Audio Parameters");
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
    fft_manager.setPrintFFTValues(P_FFT_VALS);
    fft_manager.setPrintCentroidValues(P_CENTROID_VALS);
    Serial.println("Started calculating Centroid in the FFTManager");
    fft_manager.setCalculateFlux(true);
    fft_manager.setPrintFluxValues(P_FLUX_VALS);
    Serial.println("Started calculating FLUX in the FFTManager");
  }
  Serial.println("Feature collectors have been linked");
  /////////////////////////////////////////////////////////////////////
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
  amp1.gain(STARTING_GAIN * ENC_GAIN_ADJUST * USER_CONTROL_GAIN_ADJUST);
  amp2.gain(STARTING_GAIN * ENC_GAIN_ADJUST * USER_CONTROL_GAIN_ADJUST);
  Serial.print("Set amp1 and amp2 gains to: ");
  Serial.println(STARTING_GAIN * ENC_GAIN_ADJUST * USER_CONTROL_GAIN_ADJUST);
  // set gain level? automatically?
  // todo make this adapt to when microphones are broken on one or more side...
  Serial.println("Exiting setupAudio()");
  printDivide();
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
#endif
}
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// Start of PITCH functions ////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////


bool getColorFromFFTSingleRange(FFTManager1024 *f, uint8_t s, uint8_t e) {
  double tot, frac;
  frac = f->getFFTRangeByIdx(s, e);
  tot = f->getFFTRangeByIdx(FFT_LOWEST_BIN, 128);
  frac = frac / tot;
  // RGBConverter::HsvToRgb(frac, 0.5, 1, 0, red, green, blue);
  return 1;
}

double calculateBrightness(FeatureCollector *f, FFTManager1024 *_fft) {
  double b = 0.0;
  dprintMinorDivide(P_BRIGHTNESS);
  dprint(P_BRIGHTNESS, "calculating HSB Brightness: ");
  switch (BRIGHTNESS_FEATURE) {
    case (FEATURE_PEAK_AVG):
      dprintln(P_BRIGHTNESS, "feature is PEAK_AVG");
      b = f->getDominatePeakAvg();
      f->resetDominatePeakAvg();
      break;
    case (FEATURE_PEAK):
      dprintln(P_BRIGHTNESS, "feature is PEAK");
      b = f->getDominatePeak();
      break;
    case (FEATURE_RMS_AVG):
      dprintln(P_BRIGHTNESS, "feature is RMS_AVG");
      b = f->getDominateRMSAvg();
      f->resetDominateRMSAvg();
      break;
    case (FEATURE_RMS):
      dprintln(P_BRIGHTNESS, "feature is RMS");
      b = f->getDominateRMS();
      break;
    case (FEATURE_FFT_ENERGY):
      dprintln(P_BRIGHTNESS, "feature is FFT_ENERGY");
      b = _fft->getFFTTotalEnergy();
      break;
    case (FEATURE_STRONG_FFT):
      // range index is what the highest energy bin is within the range we care about
      // b = _fft->getFFTTotalEnergy();
      Serial.println("WARNING - FEATURE_STONG_FFT is not currently implemented");
      break;
    default:
      Serial.println("ERROR - calculateHSBBrightness() does not accept that  BRIGHTNESS_FEATURE");
      break;
  }
  ///////////////////////// If user controls are in place to scale the brightness it is done now //////////////////////
  /////////////////////// Make sure that it is within bounds ////////////////////
  dprint(P_BRIGHTNESS, "brightness is: ");
  dprint(P_BRIGHTNESS, b);
  brightness = b;
  brightness_tracker.update();
  brightness = brightness_tracker.getScaledAvg();
  dprint(P_BRIGHTNESS, "\t"); dprintln(P_BRIGHTNESS, brightness);
  ////////////////////////// When using target_brightness
  if (USE_TARGET_BRIGHTNESS == true) {
    // here the target_brightness value is used as the actual brightness and the actual as the target
    if (brightness > target_brightness) {
      target_brightness += 0.003;
      if (brightness < target_brightness){
        target_brightness = brightness;
      }
    } else if (brightness < target_brightness) {
      target_brightness -= 0.003;
      if (brightness > target_brightness){
        target_brightness = brightness;
      }
    }
    brightness = target_brightness;
  }
  /////////////////////////// Reverse ////////////////////////
  if (REVERSE_BRIGHTNESS == true) {
    brightness = 1.0 - brightness;
  }
  /////////////////////////// Apply user brightness scaler ////////////////////////
  if (USER_BS_ACTIVE > 0 && (user_brightness_scaler > 1.01 || user_brightness_scaler < 0.99)) {
    dprint(P_BS + P_BRIGHTNESS, "changing brightness due to user_brightness_scaler (scaler is: ");
    dprint(P_BS + P_BRIGHTNESS, user_brightness_scaler, 4);
    dprint(P_BS + P_BRIGHTNESS, ") | before: ");
    dprint(P_BS + P_BRIGHTNESS, brightness, 4);
    brightness = brightness * user_brightness_scaler;
    dprint(P_BS + P_BRIGHTNESS, " after: ");
    dprintln(P_BS + P_BRIGHTNESS, brightness, 4);
  }

  //////////////////////// Scale down the brightness and make it more exponential for better results //////////////////
  if (SQUARE_BRIGHTNESS == true && brightness < 1.0) {
    dprint(P_BS + P_BRIGHTNESS, "changing brightness due to SQUARE_BRIGHTNESS | before: ");
    dprint(P_BS + P_BRIGHTNESS, brightness, 4);
    brightness = (brightness) * brightness ;
    dprint(P_BS + P_BRIGHTNESS, " after: ");
    dprintln(P_BS + P_BRIGHTNESS, brightness, 4);
  }
    /////////////////////// Make sure that it is within bounds ////////////////////
  if (brightness < BRIGHTNESS_CUTTOFF_THRESHOLD) {
      dprint(P_BS + P_BRIGHTNESS, "brightness lower than BRIGHTNESS_CUTTOFF_THRESHOLD of ");
      dprint(P_BS + P_BRIGHTNESS,BRIGHTNESS_CUTTOFF_THRESHOLD);
      dprintln(P_BS + P_BRIGHTNESS," changing to 0.0");
      brightness = 0;
    } else if (brightness > 1.0 + BRIGHTNESS_CUTTOFF_THRESHOLD) {
      dprint(P_BS + P_BRIGHTNESS, "brightness higher than 1.0 (");
      dprint(P_BS + P_BRIGHTNESS, brightness);
      dprintln(P_BS + P_BRIGHTNESS, "), changing to 1.0");
      brightness = 1.0;
    } else {
      brightness = brightness - BRIGHTNESS_CUTTOFF_THRESHOLD;
    }
  dprintMinorDivide(P_BRIGHTNESS);
  return brightness;
}

double calculateSaturation(FeatureCollector *f, FFTManager1024 *_fft) {
  double sat = 0.0;
  switch (SATURATION_FEATURE) {
    case (FEATURE_PEAK_AVG):
      sat = f->getDominatePeakAvg();
      // Serial.println(sat);
      // Serial.print("sat set to  : ");Serial.println(hsb[i][1]);
      f->resetDominatePeakAvg();
      break;
    case (FEATURE_RMS_AVG):
      sat = f->getDominateRMSAvg();
      // Serial.print("sat set to  : ");Serial.println(hsb[i][1]);
      f->resetDominateRMSAvg();
      break;
    case (FEATURE_FFT_RELATIVE_ENERGY):
      // get how much energy is stored in the max bin, get the amount of energy stored in all bins
      sat = _fft->getRelativeEnergy(_fft->getHighestEnergyIdx());
      // Serial.print("highestEnergyIdx: ");Serial.println(_fft->getHighestEnergyIdx());
      // Serial.print("relative energy in highest energy bin: ");Serial.println(sat);
      break;
    case (FEATURE_FLUX):
      // sat = (_fft->getFlux() - 20) / 60;
      sat = _fft->getFlux();
      break;
    default:
      Serial.print("ERROR - calculateSaturation() does not accept that  SATURATION_FEATURE");
      break;
  }
  saturation = sat;
  saturation_tracker.update();
  dprint(P_SATURATION, "saturation before/after scaling: ");
  dprint(P_SATURATION, sat);
  saturation = saturation_tracker.getScaledAvg();
  saturation = (9.9 * log10((double)saturation + 1.0)) - (2.0 * (double)saturation);
  if (REVERSE_SATURATION == true) {
    saturation = 1.0 - saturation;
  }
  dprint(P_SATURATION, " / ");
  dprint(P_SATURATION, sat,4);
  dprint(P_SATURATION, "\tsat min/max: ");
  dprint(P_SATURATION, saturation_tracker.getMin(),4);
  dprint(P_SATURATION, " / ");
  dprintln(P_SATURATION, saturation_tracker.getMax(),4);
  return saturation;
}

double calculateHue(FeatureCollector *f, FFTManager1024 *_fft) {
  /* In theory, the Hue should be between 0.0 and 1.0 to be useful for the rest of the program */
  double h = 0.0;
  switch (HUE_FEATURE) {
    case FEATURE_FFT_BIN_RANGE:
      h = getColorFromFFTSingleRange(_fft, 3, 20);
      break;
    case FEATURE_FFT:
      h = (double) constrain(_fft->getHighestEnergyIdx(), 7, 255);
      break;
    case FEATURE_FFT_MAX_BIN:
      // calculate the bin with the most energy,
      // Serial.print("Highest energy bin is: ");Serial.println(f->getHighestEnergyBin(FFT_LOWEST_BIN, FFT_HIGHEST_BIN));
      // map the bin  index to a hue value
      h = (double) (_fft->getHighestEnergyIdx(FFT_LOWEST_BIN, FFT_HIGHEST_BIN) - FFT_LOWEST_BIN) / FFT_HIGHEST_BIN;
      // Serial.print("max bin hu e is : ");Serial.println(hue);
      break;
    case FEATURE_PEAK_AVG:
      h = f->getDominatePeakAvg();
      f->resetDominatePeakAvg();
      break;
    case FEATURE_PEAK:
      h = f->getDominatePeak();
      break;
    case FEATURE_RMS_AVG:
      h = f->getDominateRMSAvg();
      break;
    case FEATURE_RMS:
      h = f->getDominateRMS();
      break;
    case FEATURE_CENTROID:
      // the centroid will be a frequency between about 200 and 50000
      // first I need to move it to a more linear scale
      h = (double)_fft->getCentroid();
      // next I need to scale down to a value between 0.0 and 1.0
      break;
    case FEATURE_FLUX:
      h = _fft->getFlux();
      break;
    default:
      Serial.println("ERROR - calculateHue() does not accept that HUE_FEATURE");
      break;
  }
  dprint(P_HUE, "Hue is: ");
  dprint(P_HUE, hue);
  hue = h;
  hue_tracker.update();
  hue = hue_tracker.getScaledAvg();
  /////////////////////////// Reverse ////////////////////////
  if (REVERSE_HUE == true) {
    hue = 1.0 - hue;
  }
  if (hue > 1.0) {
    hue = 1.0;
  } else if (hue < 0.0) {
    hue = 0.0;
  }
  dprint(P_HUE, "\t");
  dprintln(P_HUE, hue);
  return hue;
}

void printRGB() {
  // fft_manager.printFFTVals();
  if (P_NEO_COLORS) {
    neos.printColors();
  }
}

void printHSB() {
    if (P_HSB) {
      Serial.print("h: "); Serial.print(hue);
      Serial.print("\ts: "); Serial.print(saturation);
      Serial.print("\tb: "); Serial.println(brightness);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// END of PITCH ////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
void updateMode() {
  if (COLOR_MAP_MODE == COLOR_MAPPING_HSB) {
    double s = calculateSaturation(&feature_collector, &fft_manager);
    double b = calculateBrightness(&feature_collector, &fft_manager);    // user brightness scaler is applied in this function
    double h = calculateHue(&feature_collector, &fft_manager);
    printHSB();
    printRGB();
    if (feature_collector.isActive() == true) {     
      neos.colorWipeHSB(h, s, b);// now colorWipe the LEDs with the HSB value
    } else {
      Serial.println("ERROR - not able to updateNeos() as there is no active audio channels");
    }
  }
  else {
    Serial.println("ERROR = that color mode is not implemented in update neos");
  }
}


#endif // __MODE_PITCH_H__
