// #if ARTEFACT_TYPE == EXPLORATOR
double calculateColorFromCentroid(FFTManager1024 *_fft_manager) {
  // Should return a number between 0.0 and 1.0
  // right now we are only polling the first FC for its centroid to use to color both sides
  double cent = _fft_manager->getCentroid();
  if (cent < color_feature_min) {
    color_feature_min = (color_feature_min * 0.9) + (cent * 0.1);
    cent = color_feature_min;
  }
  if (cent > color_feature_max) {
    color_feature_max = (color_feature_max * 0.9) + (cent * 0.1);
    cent = color_feature_max;
  }
  cent = (cent - color_feature_min) / (color_feature_max - color_feature_min);
  return cent;
}


double calculateFeedbackBrightness(FFTManager1024 *_fft_manager) {
  // how much energy is stored in the range of 4000 - 16000 compared to  the entire spectrum?
  double target_brightness = _fft_manager->getFFTRangeByFreq(100, 16000);
  if (target_brightness < 0.01) {
    target_brightness = 0.0;
  } else if (target_brightness > 1.0) {
    target_brightness = 1.0;
  }
  if (target_brightness < brightness_feature_min) {
    if (P_BRIGHTNESS) {
      Serial.print("target_B is less than feature_min: ");
      Serial.print(target_brightness, 5);
      Serial.print(" < ");
      Serial.print(brightness_feature_min, 5);
    }
    brightness_feature_min = (target_brightness * 0.15) + (brightness_feature_min * 0.85);
    if (P_BRIGHTNESS) {
      Serial.print(" updated brightness_min and target_brightness to: ");
      Serial.println(brightness_feature_min, 5);
    }
    target_brightness = brightness_feature_min;
  }
  if (target_brightness > brightness_feature_max) {

    if (P_BRIGHTNESS) {
      Serial.print("target_B is more than feature_max: ");
      Serial.print(target_brightness, 5);
      Serial.print(" > ");
      Serial.print(brightness_feature_max, 5);
    }
    brightness_feature_max = (target_brightness * 0.15) + (brightness_feature_max * 0.85);
    // to ensure that loud clipping events do not skew things too much
    if (brightness_feature_max > 1.0) {
      brightness_feature_max = 1.0;
    }
    if (P_BRIGHTNESS) {
      Serial.print(" updated brightness_max and target_brightness to: ");
      Serial.println(brightness_feature_max, 5);
    }
    target_brightness = brightness_feature_max;
  }
  dprintln(P_BRIGHTNESS);
  dprint(P_BRIGHTNESS, " target - min/max ");
  dprint(P_BRIGHTNESS, target_brightness);
  dprint(P_BRIGHTNESS, " - ");
  dprint(P_BRIGHTNESS, brightness_feature_min);
  dprint(P_BRIGHTNESS, " / ");
  dprintln(P_BRIGHTNESS, brightness_feature_max);

  target_brightness = (target_brightness - brightness_feature_min) / (brightness_feature_max - brightness_feature_min);
  dprint(P_BRIGHTNESS, "target_brightness(2): ");
  dprint(P_BRIGHTNESS, target_brightness);
  dprint(P_BRIGHTNESS, " ");

  return target_brightness;
}

void updateFeedbackLEDs(FFTManager1024 *_fft_manager) {
  // the brightness of the LEDs should mirror the peak gathered from the environment
  // a local min/max which scales periodically should be implemented just like with the Moth
  // a MAX_RMS brightness should be used to determine what the max brightness of the feedback is
  // the LEDs should be updated 30x a second

  // calculate the target color ///////////////////////////////////////
  if (last_led_update_tmr > led_refresh_rate) {
    double target_color = 0.0;
    double target_brightness = 0.0;
    uint8_t red, green, blue;

    if (COLOR_FEATURE == SPECTRAL_CENTROID) {

      target_color = calculateColorFromCentroid(_fft_manager);
      dprint(P_COLOR, "target color: ");
      dprintln(P_COLOR, target_color);
      last_color = current_color;
      current_color = (target_color * 0.2) + (last_color * 0.8);// * COLOR_LP_LEVEL);

      // calculate the preliminary rgb values /////////////////////////////
      red = ((1.0 - current_color) * RED_LOW) + (current_color * RED_HIGH);
      green = ((1.0 - current_color) * GREEN_LOW) + (current_color * GREEN_HIGH);
      blue = ((1.0 - current_color) * BLUE_LOW) + (current_color * BLUE_HIGH);
    } else if (COLOR_FEATURE == SPLIT_BAND) {
      /* Should return a number between 0.0 and 1.0 */
      // Serial.println("about to calculate color based off split-band approach : ");
      // delay(4000);
      double green_d  = _fft_manager->getFFTRangeByFreq(50, 400); // 3 octaves in each band
      double blue_d = _fft_manager->getFFTRangeByFreq(400, 3200);
      double red_d = _fft_manager->getFFTRangeByFreq(3200, 12800);
      // Serial.println("finished getting the energy amount from the fft manager : ");
      // delay(4000);
      red = (uint8_t)((double)MAX_BRIGHTNESS * (red_d / (red_d + green_d + blue_d)));
      green = (uint8_t)((double)MAX_BRIGHTNESS * (green_d / (red_d + green_d + blue_d)));
      blue = (uint8_t)((double)MAX_BRIGHTNESS * (blue_d / (red_d + green_d + blue_d)));
    }

    // calculate the target brightness ///////////////////////////////////
    target_brightness = calculateFeedbackBrightness(_fft_manager);
    /////////////////////////// Apply user brightness scaler ////////////////////////
    if (USER_BS_ACTIVE > 0) {
      target_brightness = target_brightness * user_brightness_scaler;
    }

    last_brightness = current_brightness;
    current_brightness = (target_brightness * 0.8) + (current_brightness * 0.2);

    // calculate the actual values to be sent to the strips
    // red = (uint8_t)((double)red);
    // green = (uint8_t)((double)green);
    // blue = (uint8_t)((double)blue);
    if (P_COLOR == true) {
      Serial.print("rgb:\t");
      Serial.print(red);
      Serial.print("\t");
      Serial.print(green);
      Serial.print("\t");
      Serial.println(blue);
    }
#if ARTEFACT_TYPE == SPECULATOR && HV_MAJOR == 3
    if (current_brightness < BRIGHTNESS_CUTTOFF_THRESHOLD) {
      for (int i = 0; i < NUM_NEOP_MANAGERS; i++) {
        neos[i].colorWipe(0, 0, 0, current_brightness);
      }
    } else {
      for (int i = 0; i < NUM_NEOP_MANAGERS; i++) {
        neos[i].colorWipe(red, green, blue, current_brightness);
      }
    }
#endif
    last_led_update_tmr = 0;
  }
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
  brightness = brightness_tracker.getScaled();
  dprint(P_BRIGHTNESS, "\t"); dprintln(P_BRIGHTNESS, brightness);
  ////////////////////////// When using target_brightness
  if (USE_TARGET_BRIGHTNESS == true) {
    // here the target_brightness value is used as the actual brightness and the actual as the target
    if (brightness > target_brightness) {
      target_brightness += 0.003;
      if (brightness < target_brightness) {
        target_brightness = brightness;
      }
    } else if (brightness < target_brightness) {
      target_brightness -= 0.003;
      if (brightness > target_brightness) {
        target_brightness = brightness;
      }
    }
    brightness = target_brightness;
  }
  /////////////////////////// Reverse ////////////////////////
  if (REVERSE_BRIGHTNESS == true) {
    brightness = 1.0 - brightness;
  }

  //////////////////////// Scale down the brightness and make it more exponential for better results //////////////////
  if (SQUARE_BRIGHTNESS == true && brightness < 1.0) {
    dprint(P_BS + P_BRIGHTNESS, "changing brightness due to SQUARE_BRIGHTNESS | before: ");
    dprint(P_BS + P_BRIGHTNESS, brightness, 4);
    brightness = (brightness) * brightness ;
    dprint(P_BS + P_BRIGHTNESS, " after: ");
    dprintln(P_BS + P_BRIGHTNESS, brightness, 4);
  }

  /////////////////////////// Apply user brightness scaler ////////////////////////
  if (USER_BS_ACTIVE > 0) {
    dprint(P_BS + P_BRIGHTNESS, "changing brightness due to user_brightness_scaler (scaler is: ");
    dprint(P_BS + P_BRIGHTNESS, user_brightness_scaler, 4);
    dprint(P_BS + P_BRIGHTNESS, ") | before: ");
    dprint(P_BS + P_BRIGHTNESS, brightness, 4);
    brightness = brightness * user_brightness_scaler;
    dprint(P_BS + P_BRIGHTNESS, " after: ");
    dprintln(P_BS + P_BRIGHTNESS, brightness, 4);
  }
  /////////////////////// Make sure that it is within bounds ////////////////////
  if (brightness < BRIGHTNESS_CUTTOFF_THRESHOLD) {
    dprint(P_BS + P_BRIGHTNESS, "brightness lower than BRIGHTNESS_CUTTOFF_THRESHOLD of ");
    dprint(P_BS + P_BRIGHTNESS, BRIGHTNESS_CUTTOFF_THRESHOLD);
    dprintln(P_BS + P_BRIGHTNESS, " changing to 0.0");
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
  dprint(P_SATURATION, sat, 4);
  dprint(P_SATURATION, "\tsat min/max: ");
  dprint(P_SATURATION, saturation_tracker.getMin(), 4);
  dprint(P_SATURATION, " / ");
  dprintln(P_SATURATION, saturation_tracker.getMax(), 4);
  return saturation;
}

double calculateHue(FeatureCollector *f, FFTManager1024 *_fft) {
  // In theory, the Hue should be between 0.0 and 1.0 to be useful for the rest of the program
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
    neos[0].printColors();
  }
}

void printHSB() {
  if (P_HSB) {
    Serial.print("h: "); Serial.print(hue);
    Serial.print("\ts: "); Serial.print(saturation);
    Serial.print("\tb: "); Serial.println(brightness);
  }
}
// #endif // ARTEFACT_TYPE == EXPLORATOR
