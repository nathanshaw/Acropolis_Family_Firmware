
#if ARTEFACT_TYPE == SPECULATOR



/////////////////////////// CICADA_MODE /////////////////////////////////////////////////
#if FIRMWARE_MODE == CICADA_MODE
void speculatorLoop()
{

}

void updateAugmentations(*FeatureCollector feature_collector, *FFTManager fft_manager)
{
  uint8_t rgb[3]; // red, green, and blue
  float brightness;

  // get brightness according to relative level of cicada song amplitude (0 - 1.0)
  brightness = getBrightness();
  // scale brightness according to most recent ambient light brightness scaler
  // and the current user brightness scaler (determined by physical controls)
  // note this process intentionally can increase the brightness value above 1.0
  brightness = brightness * lux_manager.getLuxScaler() * user_brightness_scaler;
  // getColor sets the values of red, green, and blue (0 - 255) according to
  // relative pitch of cicada song where high pitch corresponds to high color values
  rgb = getColor(&feature_collector, &fft_manager[0]);
  // scale rgb values according to brightness (will limit values to between 0 - starting value)
  // will return 0's if the brightness is lower than the MINIMUM_BRIGHTNESS_THRESHOLD variable
  rgb = scaleRGB(rgb, brightness);
  // update neopixels with the expected color information
  neos.updateFeedback(rgb[0], rgb[1], rgb[2]);
}

uint8_t *getColor(*FeatureCollector feature_collector, *fft_manager)
{
}

void getSongBrightness(FFT_Manager1024 *_fft_manager)
{
  if (last_led_update_tmr > led_refresh_rate)
  {
    double target_brightness = 0.0;
    // calculate the target brightness ///////////////////////////////////
    double calculateFeedbackBrightness(FFTManager1024 * _fft_manager)
    {
      // how much energy is stored in the range of 4000 - 16000 compared to  the entire spectrum?
      double target_brightness = _fft_manager->getFFTRangeByFreq(100, 16000) * user_brightness_scaler * lux_brightness_scaler;
      if (target_brightness < 0.01)
      {
        target_brightness = 0.0;
      }
      else if (target_brightness > 1.0)
      {
        target_brightness = 1.0;
      }
      if (target_brightness < brightness_feature_min)
      {
        brightness_feature_min = (target_brightness * 0.15) + (brightness_feature_min * 0.85);
        target_brightness = brightness_feature_min;
      }
      if (target_brightness > brightness_feature_max)
      {
        brightness_feature_max = (target_brightness * 0.15) + (brightness_feature_max * 0.85);
        // to ensure that loud clipping events do not skew things too much
        if (brightness_feature_max > 1.0)
        {
          brightness_feature_max = 1.0;
        }
        target_brightness = brightness_feature_max;
      }
      target_brightness = (target_brightness - brightness_feature_min) / (brightness_feature_max - brightness_feature_min);
    }
    /////////////////////////// Apply user brightness scaler ////////////////////////
    if (USER_BS_ACTIVE > 0)
    {
      target_brightness = target_brightness;
    }
    last_brightness = current_brightness;
    // current brightness is a global variable
    current_brightness = (target_brightness * 0.5) + (current_brightness * 0.5);
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PITCH_MODE ////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
#elif FIRMWARE_MODE == PITCH_MODE

void speculatorLoop()
{
  double h, s, b;
  if (CALCULATE_DOMINATE_CHANNEL)
  {
    // TODO - erm, look into this code, what does it do again, should it be a part of the
    dominate_channel = feature_collector.getDominateChannel();
    // Serial.print("dominate channel is : ");
    // Serial.println(dominate_channel);
  }
  if (COLOR_MAP_MODE == COLOR_MAPPING_HSB) {
    s = calculateSaturation(&feature_collector, &fft_manager[dominate_channel]);
    b = calculateBrightness(&feature_collector, &fft_manager[dominate_channel]); // user brightness scaler is applied in this function
    h = calculateHue(&feature_collector, &fft_manager[dominate_channel]);

    printHSB();
    printRGB();

    // if (feature_collector.isActive() == true) {
    // the specific mapping strategy is handled by the NeoPixelManager
    neos[0].colorWipeHSB(h, s, b); // now colorWipe the LEDs with the HSB value
    // } else {
    // Serial.println("ERROR - not able to updateNeos() as there is no active audio channels");
  }
  else if (COLOR_MAP_MODE == COLOR_MAPPING_EXPLORATOR) {
    updateFeedbackLEDs(&fft_manager[dominate_channel]);
    // Serial.println("Finished running updateFeedbackLEDs()");
    // delay(2000);
  }
  else if (COLOR_MAP_MODE == COLOR_MAPPING_RGB) {
    // determine the amount of energy contained in each of the three bands
    float total_energy = fft_manager[0].getFFTTotalEnergy() * 0.25;
    int front_inside_out_order[20] = {16, 17, 18, 19, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int rear_inside_out_order[20] = {36, 37, 38, 39, 30, 31, 32, 33, 34, 35, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29};
    Serial.print("total energy : ");
    Serial.print(total_energy, 6);
    total_energy = constrainf(total_energy, 0.0, 1.0);
    Serial.print("\t");
    Serial.println(total_energy, 6);

    // map each individual LED to a range of four bins
    for (int i = 0; i < 20; i++)
    {
      int idx_start = (i * 6) + 10;

      // amount of energy contained in the bin multiplied by the number of bins (ignoring the second half)
      double r = pow((idx_start * (fft_manager[0].getFFTRangeByIdx(idx_start, idx_start + 18))), 0.5) * (user_brightness_scaler - user_brightness_scaler * 0.5);
      idx_start += 2;
      double g = pow((idx_start * (fft_manager[0].getFFTRangeByIdx(idx_start, idx_start + 18))), 0.5) * (user_brightness_scaler - user_brightness_scaler * 0.5);
      idx_start += 2;
      double b = pow((idx_start * (fft_manager[0].getFFTRangeByIdx(idx_start, idx_start + 18))), 0.5) * (user_brightness_scaler - user_brightness_scaler * 0.5);

      // scale up the values between 0.0 and 255 for NeoPixel color values
      Serial.print("r: ");
      Serial.print(r);
      Serial.print("\t");
      r = constrainf(r / 3, 0.0, 0.75);
      r = r - user_brightness_cuttoff;
      r = constrainf(r, 0.0, 0.75);
      Serial.print(r);
      Serial.print("\t");
      Serial.println(user_brightness_cuttoff);

      g = constrainf(g / 3, 0.0, 0.75);
      g = g - user_brightness_cuttoff;
      g = constrainf(g, 0.0, 0.75);

      b = constrainf(b / 3, 0.0, 0.75);
      b = b - user_brightness_cuttoff;
      b = constrainf(b, 0.0, 0.75);

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
        Serial.print(" h,s,b : ");
        Serial.print(h);
        Serial.print("\t");
        Serial.print(s);
        Serial.print("\t");
        Serial.println(br);
      }
      neos[0].colorWipeHSB(h, s, br, user_brightness_scaler, front_inside_out_order[i], front_inside_out_order[i]);
      neos[0].colorWipeHSB(h, s, br, user_brightness_scaler, rear_inside_out_order[i], rear_inside_out_order[i]);
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
#endif // FIRMWARE_MODE
#endif // ARTEFACT_TYPE
