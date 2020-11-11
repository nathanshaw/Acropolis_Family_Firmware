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
    last_brightness = current_brightness;
    current_brightness = (target_brightness * 0.8) + (current_brightness * 0.2);

    // calculate the actual values to be sent to the strips
    red = (uint8_t)((double)red * current_brightness);
    green = (uint8_t)((double)green * current_brightness);
    blue = (uint8_t)((double)blue * current_brightness);
    if (P_COLOR == true) {
      Serial.print("rgb:\t");
      Serial.print(red);
      Serial.print("\t");
      Serial.print(green);
      Serial.print("\t");
      Serial.println(blue);
    }
    for (int i = 0; i < NUM_NEOP_MANAGERS; i++) {
      neos[i].colorWipe(red, green, blue, 1.00);
    }
    last_led_update_tmr = 0;
  }
}
// #endif // ARTEFACT_TYPE == EXPLORATOR
