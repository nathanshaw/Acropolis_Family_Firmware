double color_feature_min = 1.00;
double color_feature_max = 0.0;

elapsedMillis feature_reset_tmr;
elapsedMillis last_led_update_tmr;
const unsigned long led_refresh_rate = 33; // for 30 updates a second
const unsigned long feature_reset_time = (1000 * 2.5);// every 2.5 minute?

double brightness_feature_min = 1.0;
double brightness_feature_max = 0.0;

double current_brightness = 1.0;
double last_brightness = 1.0;

double current_color = 0.5;
double last_color = 0.5;

double last_feature;
double current_feature;

// for mapping the target color to real colors
#define RED_LOW                 0
#define RED_HIGH                50

#define GREEN_LOW               150
#define GREEN_HIGH              50

#define BLUE_LOW                55
#define BLUE_HIGH               255

#define NUM_NEOP_MANAGERS       3

#define SPLIT_BAND              10
// options are SPECTRAL_CENTROID or SPLIT_BAND
#define COLOR_FEATURE          SPLIT_BAND


#if MICROPHONE_PRESENT > 0
double calculateColorFromCentroid() {
  /* Should return a number between 0.0 and 1.0 */
  double cent = fft_features.getCentroid();       // right now we are only polling the first FC for its centroid to use to color both sides
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


double calculateFeedbackBrightness() {
  // how much energy is stored in the range of 4000 - 16000 compared to  the entire spectrum?
  double target_brightness = fft_features.getFFTRangeByFreq(100, 16000);
  if (target_brightness < 0.01) {
    target_brightness = 0.0;
  } else if (target_brightness > 1.0) {
    target_brightness = 1.0;
  }
  if (target_brightness < brightness_feature_min) {
    if (PRINT_BRIGHTNESS) {
      Serial.print("target_B is less than feature_min: ");
      Serial.print(target_brightness, 5);
      Serial.print(" < ");
      Serial.print(brightness_feature_min, 5);
    }
    brightness_feature_min = (target_brightness * 0.15) + (brightness_feature_min * 0.85);
    if (PRINT_BRIGHTNESS) {
      Serial.print(" updated brightness_min and target_brightness to: ");
      Serial.println(brightness_feature_min, 5);
    }
    target_brightness = brightness_feature_min;
  }
  if (target_brightness > brightness_feature_max) {

    if (PRINT_BRIGHTNESS) {
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
    if (PRINT_BRIGHTNESS) {
      Serial.print(" updated brightness_max and target_brightness to: ");
      Serial.println(brightness_feature_max, 5);
    }
    target_brightness = brightness_feature_max;
  }
  dprintln(PRINT_BRIGHTNESS);
  dprint(PRINT_BRIGHTNESS, " target - min/max ");
  dprint(PRINT_BRIGHTNESS, target_brightness);
  dprint(PRINT_BRIGHTNESS, " - ");
  dprint(PRINT_BRIGHTNESS, brightness_feature_min);
  dprint(PRINT_BRIGHTNESS, " / ");
  dprintln(PRINT_BRIGHTNESS, brightness_feature_max);

  target_brightness = (target_brightness - brightness_feature_min) / (brightness_feature_max - brightness_feature_min);
  dprint(PRINT_BRIGHTNESS, "target_brightness(2): ");
  dprint(PRINT_BRIGHTNESS, target_brightness);
  dprint(PRINT_BRIGHTNESS, " ");

  return target_brightness;
}

void updateFeedbackLEDFromEnvironment() {
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
      target_color = calculateColorFromCentroid();
      dprint(PRINT_COLOR, "target color: ");
      dprintln(PRINT_COLOR, target_color);
      last_color = current_color;
      current_color = (target_color * 0.2) + (last_color * 0.8);// * COLOR_LP_LEVEL);

      // calculate the preliminary rgb values /////////////////////////////
      red = ((1.0 - current_color) * RED_LOW) + (current_color * RED_HIGH);
      green = ((1.0 - current_color) * GREEN_LOW) + (current_color * GREEN_HIGH);
      blue = ((1.0 - current_color) * BLUE_LOW) + (current_color * BLUE_HIGH);
    } else if (COLOR_FEATURE == SPLIT_BAND) {
      /* Should return a number between 0.0 and 1.0 */
      double green_d  = fft_features.getFFTRangeByFreq(50, 400); // 3 octaves in each band
      double blue_d = fft_features.getFFTRangeByFreq(400, 3200);
      double red_d = fft_features.getFFTRangeByFreq(3200, 12800);
      red = (uint8_t)((double)MAX_BRIGHTNESS * (red_d / (red_d + green_d + blue_d)));
      green = (uint8_t)((double)MAX_BRIGHTNESS * (green_d / (red_d + green_d + blue_d)));
      blue = (uint8_t)((double)MAX_BRIGHTNESS * (blue_d / (red_d + green_d + blue_d)));
    }

    // calculate the target brightness ///////////////////////////////////
    target_brightness = calculateFeedbackBrightness();
    last_brightness = current_brightness;
    current_brightness = (target_brightness * 0.8) + (current_brightness * 0.2);

    // calculate the actual values to be sent to the strips
    red = (uint8_t)((double)red * current_brightness);
    green = (uint8_t)((double)green * current_brightness);
    blue = (uint8_t)((double)blue * current_brightness);

    for (int i = 0; i < NUM_NEOP_MANAGERS; i++) {
      neos[i].colorWipe(red, green, blue);
    }
    last_led_update_tmr = 0;
  }
}
#endif // MICROPHONE_ACTIVE
