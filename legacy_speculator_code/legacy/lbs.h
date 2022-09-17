////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Brightness Scalers////////////////////////
////////////////////////////////////////////////////////////////////////////
// the local brightness scaler will adjust the brightness that would normally be displayed
// to utalise the entire dynamic range available
bool LBS_ACTIVE          =              true;// WARNING - does not currently work!!!

#define P_LBS                           false
// how often should the LBS be recalculated?
#define LBS_TIME_FRAME                 (1000 * 60 * 1)
// once the local min and max have been overwritten how long to collect readings for
// a new min and max before using the new values?
#define LBS_OVERLAP_TIME               (1000 * 30)

elapsedMillis lbs_timer;
// what percent from the low and high will be truncated to the lowest and highest value
#define LBS_LOW_TRUNCATE_THRESH       0.1
#define LBS_HIGH_TRUNCATE_THRESH      0.75
// currently the cicada mode will use integers to determine the LBS
#if FIRMWARE_MODE == CICADA_MODE
uint8_t lbs_min =                     255;
uint8_t lbs_max =                     0;
// to keep track of 
double lbs_min_temp =                 999999999.9; 
double lbs_max_temp =                 0.0;
// this is what the LBS will map the lowest feature results and highest feature results 
// TODO will perhaps need to make a 16bit version of this?, or change all my brightnesses to be stored using 16 bits instead of 8?
// uint8_t lbs_brightness_low =          0;
// uint8_t lbs_brightness_high =         255;
uint8_t lbs_scaler_min_thresh =       0;
uint8_t lbs_scaler_max_thresh =       255;
/////////////////////////////////
#elif FIRMWARE_MODE == PITCH_MODE
double lbs_min =                     1.0;
double lbs_max =                     0.0;
// to keep track of 
double lbs_min_temp =                 1.0; 
double lbs_max_temp =                 0.0;
// this is what the LBS will map the lowest feature results and highest feature results 
// TODO will perhaps need to make a 16bit version of this?, or change all my brightnesses to be stored using 16 bits instead of 8?
// double lbs_brightness_low =          0.0;
// double lbs_brightness_high =         1.0;

double lbs_scaler_min_thresh =       0.0;
double lbs_scaler_max_thresh =       1.0;

#endif // FIRMWARE_MODE
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Local Brightness Scaler /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/*
  void updateLBS(uint8_t feature) {
  if (lbs_timer > LBS_TIME_FRAME) {
    lbs_timer = 0;
    lbs_min = (uint8_t)((double)lbs_min * 1.1);
    lbs_max = (uint8_t)((double)lbs_max * 0.9);
    Serial.print("Reset the lbs timers");
  }
  if (feature > lbs_max) {
    lbs_max = feature;
  } else if (feature < lbs_min) {
    lbs_min = feature;
  } else {
    return;
  }
  // if we do not return then it means we updated the min or max and now
  // need to update the lbs_scaler_min_thresh and max thresb
  // double range  = lbs_max - lbs_min;
  // double lbs_scaler_min_thresh = lbs_min
  // double lbs_scaler_max_thresh =
  dprint(P_LBS, "old lbs min/max : ");
  dprint(P_LBS, lbs_scaler_min_thresh);
  dprint(P_LBS, " / ");
  dprintln(P_LBS, lbs_scaler_max_thresh);
  lbs_scaler_min_thresh = lbs_min + ((lbs_max - lbs_min) * LBS_LOW_TRUNCATE_THRESH);
  lbs_scaler_max_thresh = lbs_max + ((lbs_max - lbs_min) * LBS_HIGH_TRUNCATE_THRESH);
  dprint(P_LBS, "\tnew min/max : ");
  dprint(P_LBS, lbs_scaler_min_thresh);
  dprint(P_LBS, " / ");
  dprintln(P_LBS, lbs_scaler_max_thresh);
  }
*/
void updateLBS(double feature) {
  if (lbs_timer > LBS_TIME_FRAME) {
    lbs_timer = 0;
    lbs_min = lbs_min * 1.1;
    lbs_max = lbs_max * 0.9;
    dprint(P_LBS, "Reset the lbs timers and min/max values: ");
    dprint(P_LBS, lbs_min);
    dprint(P_LBS, "\t");
    dprintln(P_LBS, lbs_max);
  }
  if (feature > lbs_max) {
    lbs_max = feature;
    dprint(P_LBS, "\nold lbs_scaler_min/max_thresh : ");
    dprint(P_LBS, lbs_scaler_min_thresh);
    dprint(P_LBS, " / ");
    dprintln(P_LBS, lbs_scaler_max_thresh);
    lbs_scaler_max_thresh = lbs_max - ((lbs_max - lbs_min) * LBS_HIGH_TRUNCATE_THRESH);
  } else if (feature < lbs_min) {
    lbs_min = feature;
    dprint(P_LBS, "\nold lbs_scaler_min/max_thresh : ");
    dprint(P_LBS, lbs_scaler_min_thresh);
    dprint(P_LBS, " / ");
    dprintln(P_LBS, lbs_scaler_max_thresh);
    lbs_scaler_min_thresh = lbs_min + ((lbs_max - lbs_min) * LBS_LOW_TRUNCATE_THRESH);
  } else {
    return;
  }
  // if we do not return then it means we updated the min or max and now
  // need to update the lbs_scaler_min_thresh and max thresb
  dprint(P_LBS, "new lbs_scaler_min/max_thresh : ");
  dprint(P_LBS, lbs_scaler_min_thresh);
  dprint(P_LBS, " / ");
  dprintln(P_LBS, lbs_scaler_max_thresh);
}

uint8_t applyLBS(uint8_t brightness) {
  dprint(P_LBS, "brightness (Before/After) lbs(uint8_t): ");
  dprint(P_LBS, brightness);
  updateLBS(brightness);
  // constrain the brightness to the low and high thresholds
  dprint(P_LBS, " / ");
  brightness = constrain(brightness,  lbs_scaler_min_thresh, lbs_scaler_max_thresh);
  brightness = map(brightness, lbs_scaler_min_thresh, lbs_scaler_max_thresh, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
  // dprint(P_LBS, " = ");
  dprint(P_LBS, brightness);
  dprint(P_LBS, "\tmin/max thresh: ");
  dprint(P_LBS, lbs_scaler_min_thresh);
  dprint(P_LBS, " / ");
  dprintln(P_LBS, lbs_scaler_max_thresh);
  return brightness;
}

double applyLBS(double brightness) {
  dprint(P_LBS, "brightness (Before/After) lbs(double):\t");
  dprint(P_LBS, brightness);
  updateLBS(brightness);
  // constrain the brightness to the low and high thresholds
  dprint(P_LBS, " / ");
  brightness = constrainf(brightness,  lbs_scaler_min_thresh, lbs_scaler_max_thresh);
  brightness = mapf(brightness, lbs_scaler_min_thresh, lbs_scaler_max_thresh, 0.0, 1.0);
  // dprint(P_LBS, " = ");
  dprintln(P_LBS, brightness);
  dprint(P_LBS, "\t\t\tmin/max thresh: ");
  dprint(P_LBS, lbs_scaler_min_thresh);
  dprint(P_LBS, " / ");
  dprintln(P_LBS, lbs_scaler_max_thresh);
  return brightness;
}
