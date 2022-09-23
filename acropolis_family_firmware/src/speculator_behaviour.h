#ifndef __SPECULATOR_BEHAVIOUR_H__
#define __SPECULATOR_BEHAVIOUR_H__

#include <Macros.h>
/////////////////////////// B_TARGETED_FEEDBACK /////////////////////////////////////////////////
#if BEHAVIOUR_ROUTINE == B_TARGETED_FEEDBACK

uint32_t getSongColor(FeatureCollector *_feature_collector, FFTManager1024 *_fft_manager)
{
  // TODO - WARNING NOT IMPLEMENTED
  Serial.print("WARNING getSongColor() in speculator_behaviour.h is not implemented");
  uint32_t _value = 0;
  return _value;
}

double getSongBrightness(FFTManager1024 *_fft_manager)
{
  if (last_led_update_tmr > led_refresh_rate)
  {
    double target_brightness = 0.0;
    // calculate the target brightness ///////////////////////////////////
    /////////////////////////// Apply user brightness scaler ////////////////////////
    if (USER_BS_ACTIVE > 0)
    {
      target_brightness = target_brightness;
    }
    last_brightness = current_brightness;
    // current brightness is a global variable
    current_brightness = (target_brightness * 0.5) + (current_brightness * 0.5);
  }
  return current_brightness;
}

void updateAugmentations(FeatureCollector *feature_collector, FFTManager1024 *_fft_manager)
{
  uint32_t rgb; // red, green, and blue
  float brightness;

  // get brightness according to relative level of cicada song amplitude (0 - 1.0)
  brightness = getSongBrightness(_fft_manager);
  // scale brightness according to most recent ambient light brightness scaler
  // and the current user brightness scaler (determined by physical controls)
  // note this process intentionally can increase the brightness value above 1.0
  brightness = brightness * lux_manager.getBrightnessScaler() * user_brightness_scaler;
  // getColor sets the values of red, green, and blue (0 - 255) according to
  // relative pitch of cicada song where high pitch corresponds to high color values
  rgb = getSongColor(feature_collector, _fft_manager);
  // scale rgb values according to brightness (will limit values to between 0 - starting value)
  // will return 0's if the brightness is lower than the MINIMUM_BRIGHTNESS_THRESHOLD variable
  // rgb = scaleRGB(rgb, brightness);

  // update neopixels with the expected color information
  Serial.println("WARNING UPDATEAUGMENTIONS FUNCTINO IS SPECULATOR BEHAVIOUR IS BROKEN");
  // neos.updateFeedback(rgb[0], rgb[1], rgb[2]);
}

void updateBehaviour()
{
  updateAugmentations(&feature_collector, &fft_manager[0]);
  Serial.println("WARNING LOOP NOT IMPLEMENTED");
  delay(200);
};

/////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// B_ADAPTIVE_FEEDBACK ////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
#elif BEHAVIOUR_ROUTINE == B_ADAPTIVE_FEEDBACK

void updateBehaviour()
{
  double h, s, b;
  if (CALCULATE_DOMINATE_CHANNEL)
  {
    // TODO - erm, look into this code, what does it do again, should it be a part of the
    dominate_channel = feature_collector.getDominateChannel();
    // Serial.print("dominate channel is : ");
    // dprintln(P_BEHAVIOUR_UPDATE, dominate_channel);
  }
  // Serial.print("color map mode: ");
  if (color_map_mode == COLOR_MAPPING_HSB) {
    dprintln(P_BEHAVIOUR_UPDATE, "COLOR_MAPPING_HSB");

    s = calculateSaturation(&feature_collector, &fft_manager[dominate_channel]);
    b = calculateBrightness(&feature_collector, &fft_manager[dominate_channel]); // user brightness scaler is applied in this function
    h = calculateHue(&feature_collector, &fft_manager[dominate_channel]);

    printHSB();
    printRGB();

    if (feature_collector.isActive() == true) {
      // the specific mapping strategy is handled by the NeoPixelManager
      double _lux_bs = lux_manager.getBrightnessScaler();
      neos[0].colorWipeHSB(h, s, b, _lux_bs); // now colorWipe the LEDs with the HSB value
      // neos[0].colorWipe(0, 0, 0, 0.0, 0.0); // now colorWipe the LEDs with the HSB value
      // neos[0].colorWipe(255, 255, 255, 1.0, 1.0); // now colorWipe the LEDs with the HSB value
      
      #if (P_FUNCTION_TIMES && P_SPECULATOR_LED_UPDATE_RATE)
      // make sure we dont update the value tracker until we have our second loop
      if (neos[0].leds_on){
        updateFunctionTimeStats();
      } else {Serial.println("LEDS not on");}
      #endif
    } else {
      Serial.println("ERROR - not able to updateNeos() as there is no active audio channels");
    }
  }
  else if (color_map_mode == COLOR_MAPPING_EXPLORATOR) {
    dprintln(P_BEHAVIOUR_UPDATE, "COLOR_MAPPING_EXPLORATOR");
    updateFeedbackLEDs(&fft_manager[dominate_channel]);
    // Serial.println("Finished running updateFeedbackLEDs()");
    // delay(2000);
  }
  else if (color_map_mode == COLOR_MAPPING_FFT) {
    dprintln(P_BEHAVIOUR_UPDATE, "COLOR_MAPPING_FFT");
    // determine the amount of energy contained in each of the three bands
    float total_energy = fft_manager[0].getFFTTotalEnergy() * 0.16;
    #if ARTEFACT_SPECIES == SPEC_MINOR
    const int array_size = 20;
    int front_inside_out_order[20] = {16, 17, 18, 19, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int rear_inside_out_order[20] = {36, 37, 38, 39, 30, 31, 32, 33, 34, 35, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29};
    #elif ARTEFACT_SPECIES == SPEC_MAJOR
    const int array_size = 5;
    int front_inside_out_order[5] = {0, 1, 2, 3, 4};
    int rear_inside_out_order[5] = {5, 6, 7, 8, 9};
    #endif
    dprint(P_BEHAVIOUR_UPDATE, "total energy : ");
    dprint(P_BEHAVIOUR_UPDATE, total_energy, 6);
    total_energy = constrainf(total_energy, 0.0, 1.0);
    dprint(P_BEHAVIOUR_UPDATE, "\t");
    dprintln(P_BEHAVIOUR_UPDATE, total_energy, 6);

    // map each individual LED to a range of four bins
    for (int i = 0; i < array_size; i++)
    {
      int idx_start = (i * 6) + 10;

      // amount of energy contained in the bin multiplied by the number of bins (ignoring the second half)
      double r = pow((idx_start * (fft_manager[0].getFFTRangeByIdx(idx_start, idx_start + 18))), 0.5) * (user_brightness_scaler - user_brightness_scaler * 0.5);
      idx_start += 2;
      double g = pow((idx_start * (fft_manager[0].getFFTRangeByIdx(idx_start, idx_start + 18))), 0.5) * (user_brightness_scaler - user_brightness_scaler * 0.5);
      idx_start += 2;
      double b = pow((idx_start * (fft_manager[0].getFFTRangeByIdx(idx_start, idx_start + 18))), 0.5) * (user_brightness_scaler - user_brightness_scaler * 0.5);

      // scale up the values between 0.0 and 255 for NeoPixel color values
      #if USER_CONTROLS_ACTIVE
      Serial.print("r: ");
      Serial.print(r);
      Serial.print("\t");
      r = constrainf(r / 3, 0.0, 0.75);
      r = r - user_brightness_cuttoff;
      r = constrainf(r, 0.0, 0.75);
      Serial.print(r);
      Serial.print("\t");
      dprintln(P_BEHAVIOUR_UPDATE, user_brightness_cuttoff);

      g = constrainf(g / 3, 0.0, 0.75);
      g = g - user_brightness_cuttoff;
      g = constrainf(g, 0.0, 0.75);

      b = constrainf(b / 3, 0.0, 0.75);
      b = b - user_brightness_cuttoff;
      b = constrainf(b, 0.0, 0.75);
      #endif // USER_CONTROLS_ACTIVE

      g = (r + g + b) / 2.25;
      g = constrainf(g, 0.0, 1.0);

      double h = pow(g, 0.5);
      double s = 1.0 - pow((g), 0.5);
      s = s + ADDED_SATURATION;
      if (s < 1.0)
      {
        s = 1.0 - s;
      }
      else if (s > 1.0)
      {
        s = s - 1.0;
      }
      double br = pow(g, 2);

      if (i == 10)
      {
        dprint(P_BEHAVIOUR_UPDATE, " h,s,b : ");
        dprint(P_BEHAVIOUR_UPDATE, h);
        dprint(P_BEHAVIOUR_UPDATE, "\t");
        dprint(P_BEHAVIOUR_UPDATE, s);
        dprint(P_BEHAVIOUR_UPDATE, "\t");
        dprintln(P_BEHAVIOUR_UPDATE, br);
      }
      #if USER_CONTROLS_ACTIVE
      neos[0].colorWipeHSB(h, s, br, user_brightness_scaler, front_inside_out_order[i], front_inside_out_order[i]);
      neos[0].colorWipeHSB(h, s, br, user_brightness_scaler, rear_inside_out_order[i], rear_inside_out_order[i]);
      #else
      neos[0].colorWipeHSB(h, s, br, 1.0, front_inside_out_order[i], front_inside_out_order[i]);
      neos[0].colorWipeHSB(h, s, br, 1.0, rear_inside_out_order[i], rear_inside_out_order[i]);
      #endif
    }
    // this section determines what we do with the HSB values generated from above
    // use the total energy to influence the brightness
    /*
      total_energy = constrainf((total_energy - 0.02) * 20, 0.0, 1.0);

      Serial.print("\t");
      Serial.println(total_energy, 6);
      Serial.print("total energy : ");
      Serial.print(total_energy, 6);
      // use the total energy to influence the brightness
      total_energy = constrainf((total_energy - 0.02) * 20, 0.0, 1.0);

      Serial.print("\t");
      Serial.println(total_energy, 6);

      Serial.print("r,g,b : ");
      Serial.print(r);
      Serial.print("\t");
      Serial.print(g);
      Serial.print("\t");
      Serial.print(b);

      // inner rings
      neos[0].colorWipe(r, 255 - r, 255 - r, total_energy, 16, 19);
      neos[0].colorWipe(r, 255 - r, 255 - r, total_energy, 36, 39);
      // middle rings
      neos[0].colorWipe(255 - g, g, 255 - g, total_energy, 10, 15);
      neos[0].colorWipe(255 - g, g, 255 - g, total_energy, 30, 35);
      // outer rings
      neos[0].colorWipe(255 - b, 255 - b, b, total_energy, 0, 9);
      neos[0].colorWipe(255 - b, 255 - b, b, total_energy, 20, 29);
    */
  }
  else
  {
    Serial.println("ERROR = that color mode is not implemented in update neos");
  }
}
#endif // BEHAVIOUR_ROUTINE for B_ADAPTIVE_FEEDBACK

void setupSpeciesAudio()
{
  amp1.gain(MAKEUP_GAIN);
  amp2.gain(MAKEUP_GAIN);
  mixer1.gain(0, starting_gain);
  mixer1.gain(1, starting_gain);
  //////////////////////// configure filters ////////////////////////
  HPF1.setHighpass(0, LBQ1_THRESH, LBQ1_Q);
  HPF1.setHighpass(1, LBQ1_THRESH, LBQ1_Q);
  HPF1.setHighpass(2, LBQ1_THRESH, LBQ1_Q);
  HPF1.setLowShelf(3, LBQ1_THRESH, LBQ1_DB);
  Serial.print("Song HPF has been configured (thresh/Q/dB): ");
  Serial.print(LBQ1_THRESH);
  Serial.print("\t");
  Serial.print(LBQ1_Q);
  Serial.print("\t");
  Serial.println(LBQ1_DB);

  LPF1.setLowpass(0, LBQ2_THRESH, LBQ2_Q);
  LPF1.setLowpass(1, LBQ2_THRESH, LBQ2_Q);
  LPF1.setLowpass(2, LBQ2_THRESH, LBQ2_Q);
  LPF1.setHighShelf(3, LBQ2_THRESH, LBQ2_DB);
  Serial.print("Song LPF has been configured (thresh/Q/dB): ");
  Serial.print(LBQ2_THRESH);
  Serial.print("\t");
  Serial.print(LBQ2_Q);
  Serial.print("\t");
  Serial.println(LBQ2_DB);

  HPF2.setHighpass(0, RBQ1_THRESH, RBQ1_Q);
  HPF2.setHighpass(1, RBQ1_THRESH, RBQ1_Q);
  HPF2.setHighpass(2, RBQ1_THRESH, RBQ1_Q);
  HPF2.setLowShelf(3, RBQ1_THRESH, RBQ1_DB);
  Serial.print("Click HPF has been configured (thresh/Q/dB): ");
  Serial.print(RBQ1_THRESH);
  Serial.print("\t");
  Serial.print(RBQ1_Q);
  Serial.print("\t");
  Serial.println(RBQ1_DB);

  LPF2.setLowpass(0, RBQ2_THRESH, RBQ2_Q);
  LPF2.setLowpass(1, RBQ2_THRESH, RBQ2_Q);
  LPF2.setLowpass(2, RBQ2_THRESH, RBQ2_Q);
  LPF2.setHighShelf(3, RBQ2_THRESH, RBQ2_DB);
  Serial.print("Click LPF has been configured (thresh/Q/dB): ");
  Serial.print(RBQ2_THRESH);
  Serial.print("\t");
  Serial.print(RBQ2_Q);
  Serial.print("\t");
  Serial.println(RBQ2_DB);
  
  #if ARTEFACT_BEHAVIOUR == B_ADAPTIVE_FEEDBACK
  audio_connections[0] = new AudioConnection(i2s1, 0, mixer1, 0);
  audio_connections[1] = new AudioConnection(i2s1, 1, mixer1, 1);
  audio_connections[2] = new AudioConnection(mixer1, HPF1);
  audio_connections[3] = new AudioConnection(mixer1, HPF2);
  audio_connections[4] = new AudioConnection(HPF1, LPF1);
  audio_connections[5] = new AudioConnection(HPF2, LPF2);
  audio_connections[6] = new AudioConnection(LPF1, amp1);
  audio_connections[7] = new AudioConnection(LPF2, amp2);
  #elif ARTEFACT_BEHAVIOUR == B_TARGETED_FEEDBACK
  audio_connections[0] = new AudioConnection(i2s1, 0, mixer1, 0);
  audio_connections[1] = new AudioConnection(i2s1, 1, mixer1, 1);
  audio_connections[2] = new AudioConnection(mixer1, HPF1);
  audio_connections[3] = new AudioConnection(mixer1, HPF2);
  audio_connections[4] = new AudioConnection(HPF1, LPF1);
  audio_connections[5] = new AudioConnection(HPF2, LPF2);
  audio_connections[6] = new AudioConnection(LPF1, amp1);
  audio_connections[7] = new AudioConnection(LPF2, amp2);
  #endif // species

  #if NUM_PEAK_ANAS > 0
  audio_connections[15] = new AudioConnection(amp1, peak1);
  #endif

  #if NUM_PEAK_ANAS == 2
  audio_connections[16] = new AudioConnection(amp2, peak2);
  #endif

  #if NUM_RMS_ANAS > 0
  audio_connections[17] = new AudioConnection(amp1, rms1);
  #endif
  #if NUM_RMS_ANAS == 2
  audio_connections[18] = new AudioConnection(amp2, rms2);
  #endif
  #if NUM_FFT > 0
  #if ARTEFACT_SPECIES == SPEC_MAJOR
  audio_connections[19] = new AudioConnection(i2s1, 0, fft1, 0);
  #else
  audio_connections[19] = new AudioConnection(mixer1, 0, fft1, 0);
  #endif // fft connection depends on species
  #endif
  #if NUM_FFT == 2
  audio_connections[20] = new AudioConnection(i2s2, 2, fft2, 0);
  #endif
}

void setupSpecies() {
  // TODO - something to make sure this only happens when cicada mode is active?
  for (int i = 0; i < num_active_led_channels; i++) {
    neos[i].setSongColors(SONG_RED_HIGH, SONG_GREEN_HIGH, SONG_BLUE_HIGH);
    Serial.print("REVERSE_HUE is set to                             : \t");
    Serial.println(REVERSE_HUE);
    Serial.print("REVERSE_SATURATION is set to                      : \t");
    Serial.println(REVERSE_SATURATION);
    Serial.print("REVERSE_BRIGHTNESS is set to                      : \t");
    Serial.println(REVERSE_BRIGHTNESS);
    delay(2000);
  }
  // setup up some value tracker stuff
#if color_map_mode == COLOR_MAPPING_HSB
  brightness_tracker.setMinMaxUpdateFactor(BGT_MIN_UPDATE_FACTOR, BGT_MAX_UPDATE_FACTOR);
  hue_tracker.setMinMaxUpdateFactor(HUE_MIN_UPDATE_FACTOR, HUE_MAX_UPDATE_FACTOR);
  saturation_tracker.setMinMaxUpdateFactor(SAT_MIN_UPDATE_FACTOR, SAT_MAX_UPDATE_FACTOR);
#endif // color_map_mode == COLOR_MAPPING_HSB


  /////////////// User Controls ////////////////////////////////////////////
  // TODO make buttons do something for the speculators again
#if USER_CONTROLS_ACTIVE
#if ARTEFACT_SPECIES == SPEC_MINOR

  uimanager.addBut(BUT1_PIN, BUT1_PULLUP, BUT1_LOW_VAL, BUT1_HIGH_VAL, BUT1_LOW_CHANGES, &but_test[0], BUT1_NAME);
  uimanager.addBut(BUT2_PIN, BUT2_PULLUP, BUT2_LOW_VAL, BUT2_HIGH_VAL, BUT2_LOW_CHANGES, &but_test[1], BUT2_NAME);
  uimanager.addBut(BUT3_PIN, BUT3_PULLUP, BUT3_LOW_VAL, BUT3_HIGH_VAL, BUT3_LOW_CHANGES, &but_test[2], BUT3_NAME);
  uimanager.addBut(BUT4_PIN, BUT4_PULLUP, BUT4_LOW_VAL, BUT4_HIGH_VAL, BUT4_LOW_CHANGES, &but_test[3], BUT4_NAME);
  uimanager.addBut(BUT5_PIN, BUT5_PULLUP, BUT5_LOW_VAL, BUT5_HIGH_VAL, BUT5_LOW_CHANGES, &but_test[4], BUT5_NAME);
  uimanager.addBut(BUT6_PIN, BUT6_PULLUP, BUT6_LOW_VAL, BUT6_HIGH_VAL, BUT6_LOW_CHANGES, &but_test[5], BUT6_NAME);
  uimanager.addBut(BUT7_PIN, BUT7_PULLUP, BUT7_LOW_VAL, BUT7_HIGH_VAL, BUT7_LOW_CHANGES, &but_test[6], BUT7_NAME);
  uimanager.addBut(BUT8_PIN, BUT8_PULLUP, BUT8_LOW_VAL, BUT8_HIGH_VAL, BUT8_LOW_CHANGES, &but_test[7], BUT8_NAME);
  uimanager.addBut(BUT9_PIN, BUT9_PULLUP, BUT9_LOW_VAL, BUT9_HIGH_VAL, BUT9_LOW_CHANGES, &but_test[8], BUT9_NAME);
  uimanager.addBut(BUT10_PIN, BUT10_PULLUP, BUT10_LOW_VAL, BUT10_HIGH_VAL, BUT10_LOW_CHANGES, &but_test[9], BUT10_NAME);
  /*
    uimanager.addBut(BUT1_PIN, BUT1_PULLUP, BUT1_LOW_VAL, BUT1_HIGH_VAL, BUT1_LOW_CHANGES, &USE_TARGET_HUE, BUT1_NAME);
    uimanager.addBut(BUT2_PIN, BUT2_PULLUP, BUT2_LOW_VAL, BUT2_HIGH_VAL, BUT2_LOW_CHANGES, &SQUARE_BRIGHTNESS, BUT2_NAME);
    uimanager.addBut(BUT3_PIN, BUT3_PULLUP, BUT3_LOW_VAL, BUT3_HIGH_VAL, BUT3_LOW_CHANGES, &USE_TARGET_BRIGHTNESS, BUT3_NAME);
    uimanager.addBut(BUT4_PIN, BUT4_PULLUP, BUT4_LOW_VAL, BUT4_HIGH_VAL, BUT4_LOW_CHANGES, &REVERSE_HUE, BUT4_NAME);
    uimanager.addBut(BUT5_PIN, BUT5_PULLUP, BUT5_LOW_VAL, BUT5_HIGH_VAL, BUT5_LOW_CHANGES, &REVERSE_SATURATION, BUT5_NAME);
    uimanager.addBut(BUT6_PIN, BUT6_PULLUP, BUT6_LOW_VAL, BUT6_HIGH_VAL, BUT6_LOW_CHANGES, &REVERSE_BRIGHTNESS, BUT6_NAME);
    uimanager.addBut(BUT7_PIN, BUT7_PULLUP, BUT7_LOW_VAL, BUT7_HIGH_VAL, BUT7_LOW_CHANGES, &LED_MAPPING_MODE, BUT7_NAME);
    uimanager.addBut(BUT8_PIN, BUT8_PULLUP, BUT8_LOW_VAL, BUT8_HIGH_VAL, BUT8_LOW_CHANGES, &LED_MAPPING_MODE, BUT8_NAME);
    uimanager.addBut(BUT9_PIN, BUT9_PULLUP, BUT9_LOW_VAL, BUT9_HIGH_VAL, BUT9_LOW_CHANGES, &LED_MAPPING_MODE, BUT9_NAME);
    uimanager.addBut(BUT10_PIN, BUT10_PULLUP, BUT10_LOW_VAL, BUT10_HIGH_VAL, BUT10_LOW_CHANGES, &BOOT_DELAY_ACTIVE, BUT10_NAME);
  */

  uimanager.addPot(POT1_PIN, POT1_REVERSE, POT1_PLAY, &user_brightness_scaler, POT1_NAME);
  uimanager.addPot(POT2_PIN, POT2_REVERSE, POT2_PLAY, &ADDED_SATURATION, POT2_NAME);

  uimanager.addPot(POT4_PIN, POT4_REVERSE, POT4_PLAY, &user_brightness_cuttoff, POT4_NAME);
  uimanager.addPotRange(0, min_user_brightness_scaler, mid_user_brightness_scaler, max_user_brightness_scaler);
  uimanager.addPotRange(1, 0.0, 0.5, 1.0);
  uimanager.addPotRange(2, 0.0, 0.5, 1.0);
  uimanager.addPotRange(3, min_user_brightness_cuttoff, mid_user_brightness_cuttoff, max_user_brightness_cuttoff);

#elif ARTEFACT_SPCIES == SPEC_MAJOR
  uimanager.addBut(BUT1_PIN, BUT1_PULLUP, BUT1_LOW_VAL, BUT1_HIGH_VAL, BUT1_LOW_CHANGES, &color_map_mode, BUT1_NAME);
  uimanager.addBut(BUT2_PIN, BUT2_PULLUP, BUT2_LOW_VAL, BUT2_HIGH_VAL, BUT2_LOW_CHANGES, &SQUARE_BRIGHTNESS, BUT2_NAME);
  uimanager.addBut(BUT3_PIN, BUT3_PULLUP, BUT3_LOW_VAL, BUT3_HIGH_VAL, BUT3_LOW_CHANGES, &USE_TARGET_BRIGHTNESS, BUT3_NAME);
  uimanager.addBut(BUT4_PIN, BUT4_PULLUP, BUT4_LOW_VAL, BUT4_HIGH_VAL, BUT4_LOW_CHANGES, &REVERSE_SATURATION, BUT4_NAME);
  uimanager.addBut(BUT5_PIN, BUT5_PULLUP, BUT5_LOW_VAL, BUT5_HIGH_VAL, BUT5_LOW_CHANGES, &REVERSE_HUE, BUT5_NAME);
  uimanager.addBut(BUT6_PIN, BUT6_PULLUP, BUT6_LOW_VAL, BUT6_HIGH_VAL, BUT6_LOW_CHANGES, &BOOT_DELAY_ACTIVE, BUT6_NAME);
#endif // HV_MAJOR == 2 or 3
  delay(50);
  uimanager.setup(false);
  uimanager.printAll();
#endif // USER_CONTROLS_ACTIVE
}

#endif // ARTEFACT_GENUS - only Speculator