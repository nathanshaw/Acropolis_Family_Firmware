/**
  Moth Sonic Art Framework Firmware
  Written by Nathan Villicana-Shaw in 2020
  The runtime, boot, and all other configurations are found in the Configuration.h file
*/
// configuration needs to be added first to determine what mode will be added
#include "Configuration.h"

#if FIRMWARE_MODE == CICADA_MODE
#include "ModeCicada.h"
#elif FIRMWARE_MODE == PITCH_MODE
#include "ModePitch.h"
#endif

// this is a high value to force jumper readings in the setup() loop
elapsedMillis last_jumper_read = 100000;

#if P_FUNCTION_TIMES == true
elapsedMillis function_times = 0;
#endif // P_FUNCTION_TIMES

void updateAudioAnalysis() {
  // update the feature collectors
#if P_FUNCTION_TIMES == true
  function_times = 0;
#endif // P_FUNCTION_TIMES
  if (fft_manager.update()) {
#if P_FUNCTION_TIMES == true
    Serial.print("fft_manager update took ");
    Serial.print(function_times); Serial.println(" micro seconds to update");
#endif // P_FUNCTION_TIMES
  }

#if P_FUNCTION_TIMES == true
  function_times = 0;
#endif // P_FUNCTION_TIMES

  if (feature_collector.update()) {
#if P_FUNCTION_TIMES == true
    Serial.print("feature_collector update took ");
    Serial.print(function_times); Serial.println(" micro seconds to update");
#endif // P_FUNCTION_TIMES
  }
#if P_AUDIO_MEMORY_MAX > 0
  // so the printing is not crazy
  if (last_jumper_read > 99500) {
    Serial.print("audio memory max: ");
    Serial.println(AudioMemoryUsageMax());
  }
#endif
}

void setup() {
  // setup up some value tracker stuff
  brightness_tracker.setMinMaxUpdateFactor(BGT_MIN_UPDATE_FACTOR, BGT_MAX_UPDATE_FACTOR);
  hue_tracker.setMinMaxUpdateFactor(HUE_MIN_UPDATE_FACTOR, HUE_MAX_UPDATE_FACTOR);
  saturation_tracker.setMinMaxUpdateFactor(SAT_MIN_UPDATE_FACTOR, SAT_MAX_UPDATE_FACTOR);
  ///////////////// Serial ///////////////////////////////////
  delay(2000); // to avoid booting to the bootloader
  Serial.begin(SERIAL_BAUD_RATE);
  delay(1000);// so that our Serial messages will appear
  printDivide();
  Serial.println("Entering the Setup Loop");
  printDivide();
  Serial.println("Serial begun");
  printMinorDivide();
  neos.begin();
  //////////////// Hardware/Software Version /////////////////
  Serial.print("Firmware is version : ");
  Serial.print(SV_MAJOR);
  Serial.print(".");
  Serial.print(SV_MINOR);
  Serial.print(".");
  Serial.println(SV_SUBVERSION);
  Serial.print("Firmware was compiled for a PCB with a hardware revsion of : ");
  Serial.print(HV_MAJOR);
  Serial.print(".");
  Serial.println(HV_MINOR);
  printMinorDivide();

  //////////////// Enclosure Type ///////////////////////////
  Serial.print("Enclosure type is: ");
  if (ENCLOSURE_TYPE == GROUND_ENCLOSURE) {
    Serial.println("Ground");
  } else if (ENCLOSURE_TYPE == ORB_ENCLOSURE_WITHOUT_HOLE) {
    Serial.println("Orb without hole");
  } else if (ENCLOSURE_TYPE == ORB_ENCLOSURE_WITH_HOLE) {
    Serial.println("Orb with hole");
  } else if (ENCLOSURE_TYPE == NO_ENCLOSURE) {
    Serial.println("no enclosure");
  } else {
    Serial.println("ERROR, enclosure type is not recognized");
    neos.colorWipe(255, 0, 0, 0);
    delay(60000);
  }
  printMinorDivide();
  //////////////// User Controls /////////////////////////////
  explainSerialCommands(true);
  setupUserControls();
  testJumpers(true);
  readUserControls(true); // to print out the actual final results
  //////////////// Leds //////////////////////////////////////
  neos.colorWipe(12, 12, 12, 1.0);
  printMinorDivide();
  Serial.println("LEDS have been initalised");
  Serial.print("There are ");
  Serial.print(NUM_LED);
  Serial.println(" LEDs");
  Serial.print("NeoGroup p_lux and p_extreme_lux are both set to :\t"); Serial.println(P_LUMIN);
  neos.setPrintLux(P_LUMIN);
  Serial.print("NeoGroup p_lux is set to                         :\t"); Serial.println(P_LUMIN);
  neos.setPrintExtremeLux(P_LUMIN);
  Serial.print("NeoGroup p_brightness_scaler is set to           :\t"); Serial.println(P_BS);
  neos.setPrintBrightnessScaler(P_BS);
  Serial.print("NeoGroup p_leds_on is set to                     :\t"); Serial.println(P_LEDS_ON);
  neos.setPrintLedsOn(P_LEDS_ON);
  Serial.print("NeoGroup p_on_ratio is set to                    :\t"); Serial.println(P_LED_ON_RATIO);
  neos.setPrintOnRatio(P_LED_ON_RATIO);
  Serial.print("NeoGroup p_color_wipe is set to                  :\t"); Serial.println(P_COLOR_WIPE);
  neos.setPrintColorWipe(P_COLOR_WIPE);
  Serial.print("NeoGroup p_onset is set to                       :\t"); Serial.println(P_ONSET);
  neos.setPrintOnRatio(P_ONSET);
  Serial.print("NeoGroup p_onset is set to                       :\t"); Serial.println(P_ONSET);
  neos.setPrintPackColors(P_PACK_COLORS);
  printMinorDivide();
  Serial.print("NeoGroup SATURATED_COLORS                        :\t"); Serial.println(P_ONSET);
  neos.setSaturatedColors(SATURATED_COLORS);

  neos.setFlashColors(ONSET_RED, ONSET_GREEN, ONSET_BLUE);
  neos.setSongColors(SONG_RED_HIGH, SONG_GREEN_HIGH, SONG_BLUE_HIGH);

  neos.setFlashBehaviour(FLASH_DOMINATES);
  Serial.print("NeoGroup flash_behaviour is set to                :\t");
  Serial.println(FLASH_DOMINATES);

  neos.changeMapping(LED_MAPPING_MODE);
  // neos[i].setSongFeedbackMode(ROUND);
  Serial.print("REVERSE_HUE is set to                             :\t");
  Serial.println(REVERSE_HUE);
  Serial.print("REVERSE_SATURATION is set to                      :\t");
  Serial.println(REVERSE_SATURATION);
  Serial.print("REVERSE_BRIGHTNESS is set to                      :\t");
  Serial.println(REVERSE_BRIGHTNESS);
  printMinorDivide();

  ///////////////////////// Audio //////////////////////////
  setupAudio();

  ///////////////////////// auto_gain //////////////////////////
  // target value, tolerance, min_thresh, max_thresh
  /*
    auto_gain.trackAvgRMS(0.1, 0.2, 0.05, 0.2);
    Serial.println("Setting auto_gain to track RMS Avg");
    auto_gain.setUpdateRate(AUTOGAIN_FREQUENCY);
    Serial.print("Setting auto_gain's update rate to: ");
    Serial.println(AUTOGAIN_FREQUENCY);
    auto_gain.setInitialUpdateRate(AUTOGAIN_START_DELAY);
    Serial.print("Setting auto_gain's start delay to: ");
    Serial.println(AUTOGAIN_START_DELAY);
  */


  //////////////////////////// Lux Sensors //////////////////////////////
  printMinorDivide();
  Serial.println("turning off LEDs for Lux Calibration");
  neos.colorWipe(0, 0, 0, 0.0);
  Serial.println("LEDS off");
  delay(100);

  lux_manager.setLuxThresholds(LOW_LUX_THRESHOLD, MID_LUX_THRESHOLD, HIGH_LUX_THRESHOLD, EXTREME_LUX_THRESHOLD);
  lux_manager.setPrintBrightnessScaler(P_BS);
  Serial.print("lux_manager print_brightness_scaler is set to   :\t");
  Serial.println(P_BS);
  lux_manager.setPrintLuxReadings(P_LUX_READINGS);
  Serial.print("lux_manager print_lux_readings is set to        :\t");
  Serial.println(P_LUX_READINGS);
  lux_manager.setPrintGeneralDebug(P_LUX_MANAGER_DEBUG);
  Serial.print("lux_manager print_general_debug is set to       :\t");
  Serial.println(P_LUX_MANAGER_DEBUG);

#if HV_MAJOR > 2
  lux_manager.add6030Sensors(2, 25);
  lux_manager.linkNeoGroup(&neos);
  delay(200);
#else
  lux_manager.addSensorTcaIdx("Front", 0);
  lux_manager.addSensorTcaIdx("Rear", 1);
  lux_manager.startTCA7700Sensors(VEML7700_GAIN_1, VEML7700_IT_25MS); // todo add this to config_adv? todo
#endif // HV_MAJOR
  if ((lux_manager.getSensorActive(0) || lux_manager.getSensorActive(1)) > 0) {
    Serial.println("Linked the NeoGroups to the LuxManager");
    lux_manager.linkNeoGroup(&neos);
    delay(200);
  } else {
    Serial.println("ERROR, unable to link NeoGroups to LuxManager");
  }
  lux_manager.setBrightnessScalerMinMax(LUX_BS_MIN, LUX_BS_MAX);
  lux_manager.calibrate(2000, true);
  lux_manager.print();
#if FIRMWARE_MODE == TEST_MODE
  neos.colorWipe(64, 64, 64, 1.0);
  delay(10000000);
#endif
  ///////////////////////// Weather Manager /////////////////////
  // nothing is needed =P
#if HV_MAJOR > 2
  printMinorDivide();
  Serial.println("initalising the weather manager");
  weather_manager.init();
  weather_manager.setPrintReadings(P_WEATHER_MANAGER_READINGS);
  Serial.println("finished initalising the weather manager");
  Serial.print("WeatherManager HUMID_EXTREME_THRESH  :\t");
  Serial.println(HUMID_EXTREME_THRESH);
  Serial.print("WeatherManager TEMP_EXTREME_THRESH   :\t");
  Serial.println(TEMP_EXTREME_THRESH);
  Serial.print("WeatherManager TEMP_HISTORESIS      :\t");
  Serial.println(TEMP_HISTORESIS);
  Serial.print("WEATHER_MANAGER_UPDATE_RATE         :\t");
  Serial.println(WEATHER_MANAGER_UPDATE_RATE);
  weather_manager.update();
  weather_manager.print();
  printMinorDivide();
#endif // HV_MAJOR > 20
  ///////////////////////// DL Manager //////////////////////////
  // TODO
  // setupDLManagerCicada();
  // printMinorDivide();

  /////////////////////////////// Main Loop Delay ////////////////////////////////

  // Serial.print("segment : ");
  // Serial.println(segment);
  for (int it = 0; it < NUM_LED; it++) {
    // we keep readng the jumpers so if we change the jumpers and dont
    // want the boot delay that can happen
    readJumpers();
    uint32_t segment = (uint32_t)((double)BOOT_DELAY / (double)NUM_LED);
    neos.setPixel(it, 10, 32, 20, 1.0);
    if (digitalRead(BUT1_PIN)) {
      delay(segment);
    }
  }
  neos.colorWipe(0, 5, 0, 1.0);
  printMajorDivide("Now starting main() loop");
}

void loop() {
#if HV_MAJOR > 2
  weather_manager.update();
  if (weather_manager.getHumidityShutdown() == true) {
    neos.colorWipe(0, 0, 0, 0);
    while (true) {
      Serial.println("HUMIDTY SHUTDOWN INITALISED!!!!!!");
      delay(10000);
    }
    // TODO
  } else if (weather_manager.getTempShutdown() == true) {
    unsigned int times = 0;
    Serial.println("TEMPERATURE SHUTDOWN INITALISED!!!!!!");
    neos.colorWipe(255, 0, 0, 0.5);
    delay(1000);
    neos.colorWipe(0, 0, 0, 0.0);
    while (weather_manager.getTempShutdown() == true) {
      weather_manager.update();
      if (times % 20 == 10) {
        neos.colorWipe(255, 0, 0, 0.5);
      } else if (times % 20 == 11) {
        neos.colorWipe(0, 0, 0, 0.0);
      }
      times++;
      delay(100);
    }
    Serial.println("TEMPERATURE SHUTDOWN LIFTED - resuming normal operation");
  } else {
#endif // HV_MAJOR
    if (lux_manager.update()) {
      lux_manager.print();
    }
    // if (lux_manager.getExtremeLux() == true) {jk
    //   Serial.println("WARNING ------------ updateMode() returning due extreme lux conditions, not updating onset or song...");
    // } else {
    updateAudioAnalysis();
    updateMode();
    // updateAutogain();
    // TODO
    // updateDatalog();
    readUserControls(P_USER_CONTROLS);
    // }
#if HV_MAJOR > 2
  }
#endif // HV_MAJOR
  // Serial.println(millis()/1000);
}
