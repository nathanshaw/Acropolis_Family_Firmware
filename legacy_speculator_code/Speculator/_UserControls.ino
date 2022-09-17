
void readJumpers() {
  readJumpers(P_USER_CONTROLS);
}

void readJumpers(bool p_user_controls) {
  dprintMinorDivide(p_user_controls);
  bool temp_b;
  //////////// Jumper 1 ///////////////////////
  /////////// Boot Delay //////////////////////
  temp_b = !digitalRead(BUT1_PIN);
  BOOT_DELAY *= temp_b;
  dprint(p_user_controls, "(but1)  - ");
  if (BOOT_DELAY == 0) {
    dprint(p_user_controls, "ON  - ");
  } else {
    dprint(p_user_controls, "OFF - ");
  }
  dprint(p_user_controls, "Boot Delay (in seconds) = ");
  dprintln(p_user_controls, BOOT_DELAY / 1000);

  //////////// Jumper 2 ///////////////////////

#if HV_MAJOR < 3
  SQUARE_BRIGHTNESS = digitalRead(BUT2_PIN);
  dprint(p_user_controls, "(but2)  - ");
  if (SQUARE_BRIGHTNESS == 0) {
    dprint(p_user_controls, "OFF  - WILL NOT square brightness");
  } else {
    dprint(p_user_controls, "ON - WILL square brightness");
  }
  dprintln(p_user_controls);
#else
  SQUARE_BRIGHTNESS = digitalRead(BUT2_PIN);
  dprint(p_user_controls, "(but2)  - ");
  if (SQUARE_BRIGHTNESS == 0) {
    dprint(p_user_controls, "OFF - WILL NOT scale down brightness");
  } else {
    dprint(p_user_controls, "ON  - WILL scale down brightness");
  }
  dprintln(p_user_controls);
#endif//HV_MAJOR

  //////////// Jumper 3 ///////////////////////

  temp_b = digitalRead(BUT3_PIN);
#if FIRMWARE_MODE == CICADA_MODE
  FLASH_DOMINATES = temp_b;
  if (temp_b == 1) {
    dprintln(p_user_controls, "(but3)  - ON  - FLASH_DOMINATES is true, flash will erase other brightness messages");
  } else {
    dprintln(p_user_controls, "(but3)  - OFF - FLASH_DOMINATES is false, flash will be added to other brightness messages");
  }
#else
  if (temp_b == 0) {
    USE_TARGET_BRIGHTNESS = true;
    dprintln(p_user_controls, "(but3)  - OFF  - USE_TARGET_BRIGHTNESS is true");
  } else {
    USE_TARGET_BRIGHTNESS = false;
    dprintln(p_user_controls, "(but3)  - ON  - USE_TARGET_BRIGHTNESS is false");
  }
#endif//FIRMWARE_MODE

  //////////// Jumper 4 ///////////////////////
  //////////// Major Sensitivity Attenuation //

#if HV_MAJOR < 3
  double total_scaler = 0.0;
  temp_b = digitalRead(BUT4_PIN);
  if (temp_b == 1) {
    dprint(p_user_controls, "(but4)  - ON  - user_brightness_scaler not decreased by 66% : ");
  } else {
    dprint(p_user_controls, "(but4)  - OFF - user_brightness_scaler decreased by 66% : ");
    total_scaler -= 0.66;
  }
  dprintln(p_user_controls, total_scaler);
#else
  temp_b = digitalRead(BUT4_PIN);
  if (temp_b == 0) {
    SATURATED_COLORS = false;
    dprintln(p_user_controls, "(but4)  - OFF  - SATURATED_COLORS is false");
  } else {
    SATURATED_COLORS = true;
    dprintln(p_user_controls, "(but4)  - ON  - SATURATED_COLORS is true");
  }
#endif//HV_MAJOR
  ///////////// Jumper 5 //////////////////////
  //////////// Minor Sensitivity Boost ///////////////
#if HV_MAJOR < 3
  temp_b = digitalRead(BUT5_PIN);
  if (temp_b == 1) {
    total_scaler += 1.0;
    dprint(p_user_controls, "(but5)  - ON  - user_brightness_scaler increased by 100% : ");
  } else {
    dprint(p_user_controls, "(but5)  - OFF - user_brightness_scaler not increased by 100% : ");
  }
  dprintln(p_user_controls, total_scaler);
#else
  temp_b = digitalRead(BUT5_PIN);
  if (temp_b == 1) {
    dprint(p_user_controls, "(but5)  - ON  - HUE_FEATURE is now FEATURE_CENTROID");
    HUE_FEATURE = FEATURE_CENTROID;
  } else {
    dprint(p_user_controls, "(but5)  - OFF - HUE_FEATURE is now FEATURE_FLUX");
    HUE_FEATURE = FEATURE_FLUX;
  }
  dprintln(p_user_controls);
#endif//HV_MAJOR
  ///////////// Jumper 6 //////////////////////
  //////////// Starting Gain Boost ////////////////
#if HV_MAJOR < 3
  temp_b = digitalRead(BUT6_PIN);
  if (temp_b == 1) {
    USER_CONTROL_GAIN_ADJUST = 1.5;
    dprint(p_user_controls, "(but6)  - ON  - STARTING_GAIN increased by a factor of 50% : ");
  } else {
    dprint(p_user_controls, "(but6)  - OFF - STARTING_GAIN not increased by a factor of 50% : ");
  }
  dprintln(p_user_controls, STARTING_GAIN * ENC_GAIN_ADJUST * USER_CONTROL_GAIN_ADJUST);
  // this has to be done at the end only for hv 2.1
  user_brightness_scaler = 1.0;
  user_brightness_scaler += total_scaler;
  dprint(p_user_controls, "\nuser_brightness_scaler set to : ");
  dprintln(p_user_controls, user_brightness_scaler);

#else
  temp_b = digitalRead(BUT6_PIN);
  if (temp_b == 1) {
    dprintln(p_user_controls, "(but6)  - ON  - BRIGHTNESS_FEATURE is FEATURE_FFT_ENERGY");
    BRIGHTNESS_FEATURE = FEATURE_FFT_ENERGY;
  } else {
    dprintln(p_user_controls, "(but6)  - OFF - BRIGHTNESS_FEATURE is FEATURE_PEAK_AVG");
    BRIGHTNESS_FEATURE = FEATURE_PEAK_AVG;
  }
#endif//HV_MAJOR

#if HV_MAJOR > 2
  ///////////// Jumper 7 //////////////////////
  ///////////// Center Out Mapping ////////////
  temp_b = digitalRead(BUT7_PIN);
  if (temp_b == 1) {
    dprintln(p_user_controls, "(but7)  - ON  - LED_MAPPING_MODE SET TO CENTER_OUT");
    LED_MAPPING_MODE = LED_MAPPING_CENTER_OUT;
  } else {
    dprintln(p_user_controls, "(but7)  - OFF - LED_MAPPING_MODE remains STANDARD");
    LED_MAPPING_MODE = LED_MAPPING_STANDARD;
  }
  neos.changeMapping(LED_MAPPING_MODE);

  ///////////// Jumper 8 //////////////////////
  temp_b = digitalRead(BUT8_PIN);
  if (temp_b == 1) {
    dprint(p_user_controls, "(but8)  - ON  - REVERSE_HUE set to true");
    REVERSE_SATURATION = true;
  } else {
    dprint(p_user_controls, "(but8)  - OFF - REVERSE_HUE set to false");
    REVERSE_SATURATION = false;
  }
  dprintln(p_user_controls);

  ///////////// Jumper 9 //////////////////////
  temp_b = digitalRead(BUT9_PIN);
  if (temp_b == 1) {
    dprint(p_user_controls, "(but9)  - ON  - REVERSE_SATURATION set to false");
    REVERSE_SATURATION = false;
  } else {
    dprint(p_user_controls, "(but9)  - OFF - REVERSE_SATURATION set to true");
    REVERSE_SATURATION = true;
  }
  dprintln(p_user_controls);

  ///////////// Jumper 10 //////////////////////
  temp_b = digitalRead(BUT10_PIN);
  if (temp_b == 1) {
    dprint(p_user_controls, "(but10) - ON  - REVERSE_BRIGHTNESS set to false");
    REVERSE_BRIGHTNESS = false;
  } else {
    dprint(p_user_controls, "(but10) - OFF - REVERSE_BRIGHTNESS set to true");
    REVERSE_BRIGHTNESS = true;
  }
  dprintln(p_user_controls);
#elif HV_MAJOR == 2
  printDivide();
  dprint(p_user_controls, "STARTING_GAIN/ENC_GAIN_ADJ/USER_CONTROL_GAIN :\t");
  dprint(p_user_controls, STARTING_GAIN);
  dprint(p_user_controls, "\t");
  dprint(p_user_controls, ENC_GAIN_ADJUST);
  dprint(p_user_controls, "\t");
  dprint(p_user_controls, USER_CONTROL_GAIN_ADJUST);
  dprint(p_user_controls, "\ttotal: ");
  dprintln(p_user_controls, STARTING_GAIN * ENC_GAIN_ADJUST * USER_CONTROL_GAIN_ADJUST);
#endif // HV_MAJOR > 2

  dprintMinorDivide(p_user_controls);
}

void setupUserControls() {
  for (int i = 0; i < NUM_JUMPERS; i++) {
    pinMode(jmp_pins[i], INPUT);
  }
#if HV_MAJOR > 2
  for (int i = 0; i < NUM_POTS; i++) {
    pinMode(pot_pins[i], INPUT);
  }
#endif // HV_MAJOR
}

void readUserControls(bool p_user_controls) {
#if P_FUNCTION_TIMES
  elapsedMicros m = 0;
#endif
  if (JUMPERS_POPULATED != true) {
    dprintln(p_user_controls, "Sorry jumpers not populated, exiting readUserControl()");
    return;
  }
  if (last_jumper_read < USER_CONTROL_POLL_RATE) {
    // dprintln(p_user_controls, "Sorry the last reading was too resceent exiting readUserControl();");
    return;
  }
#if HV_MAJOR > 2
  readPots(p_user_controls);
#endif // HV_MAJOR
  readJumpers(p_user_controls);
  last_jumper_read = 0;
  listenForSerialCommands(p_user_controls);
#if P_FUNCTION_TIMES
  Serial.print("readUserControls() took ");
  Serial.print(m);
  Serial.println(" micro seconds to execute");
#endif
}

void explainSerialCommands(bool p_user_controls) {
  dprintln(p_user_controls, "The Following Serial Commands are Supported: ");
  dprintln(p_user_controls, "Print Commands, denoted by a p prefix: ");
  dprintln(p_user_controls, "brightness_scaler (bs)");
  dprintln(p_user_controls, "datalogs (dl)");
  dprintMinorDivide(p_user_controls);
}

void listenForSerialCommands(bool p_user_controls) {
  if (Serial.available() > 0) {
    int input = Serial.read();
    dprint(p_user_controls, "incbyte : ");
    dprintln(p_user_controls, input);
    //////////////// PRINT COMMANDS ////////////////
    if (input == 'p') {
      input = Serial.read();
      // datalogs
      if (input == 'd') {
        input = Serial.read();
        if (input == 'l') {
          // this is the command to print the datalog
          // datalog_manager.printAllLogs();
        }
      }
      // print brightness scaler
      if (input == 'b') {
        input = Serial.read();
        if (input == 's') {
          dprint(p_user_controls, "Brightness Scalers: ");
          dprintln(p_user_controls, lux_manager.getBrightnessScaler());
          // dprint(p_user_controls, "\t");
          // dprintln(p_user_controls, brightness_scalers[1]);
        }
      }
      /////////////// Changing Values ////////////////////////
      if (input == 's') {
        input = Serial.read();
        if (input == 'g') {
          dprintln(p_user_controls, "what would you like to change the gain to? Please enter in a gain in the format of 1.00.");
          input = Serial.read() - 48;
          dprint(p_user_controls, "A gain of ");
          dprint(p_user_controls, input);
          dprintln(p_user_controls, " has been selected");
        }
      }
    }
  }
}


#if HV_MAJOR > 2
void readPots(bool p_user_controls) {
  dprintMinorDivide(p_user_controls);
  dprint(p_user_controls, "Reading Pots: ");
  if (NUM_POTS > 0 && last_jumper_read > USER_CONTROL_POLL_RATE) {
    for (int i = 0; i < NUM_POTS; i++) {
      pot_vals[i] = 1023 - analogRead(pot_pins[i]);
      dprint(p_user_controls, pot_vals[i]);
      dprint(p_user_controls, "\t");
    }
  }
  dprintln(p_user_controls);
  /////////////////////////////////// Use Pot Values to update appropiate runtime values //////////////////////////////////
  if (USER_BRIGHTNESS_OVERDRIVE == true) {
    double scaled_brightness = (double) pot_vals[BS_POT_NUM] / 1023;
    if (pot_vals[BS_POT_NUM] < 512) {
      scaled_brightness = scaled_brightness * 2;
      if (scaled_brightness < POT_BS_MIN) {
        scaled_brightness = POT_BS_MIN;
      }
    } else {
      scaled_brightness = (scaled_brightness - 0.5) * POT_BS_MAX;
      if (scaled_brightness < 1.0) {
        scaled_brightness = 1.0;
      }
    }
    dprint(p_user_controls, "user brightness scaler (pre lux scaler) is now: ");
    dprintln(p_user_controls, scaled_brightness);
    user_brightness_scaler = scaled_brightness;
  }

  BRIGHTNESS_CUTTOFF_THRESHOLD = (double) pot_vals[BC_POT_NUM] / 3400;// results in a range between 0.0 and 0.66 about
  dprint(p_user_controls, "BRIGHTNESS_CUTTOFF_THRESHOLD updated: ");
  dprintln(p_user_controls, BRIGHTNESS_CUTTOFF_THRESHOLD);

  if (USER_ONSET_THRESH_OVERRIDE == true) {
    dprintln(p_user_controls, "WARNING USER_ONSET_THRESH_OVERRIDE is not implemented");
    // _ONSET_THRESH = mapf(_ot, 0.0, 1.0, USER_OT_MIN, USER_OT_MAX);
  }
  // if
  if (P_POT_VALS) {
    printPots(true);
  }
}

void printPots(bool p_user_controls) {
  dprintMinorDivide(p_user_controls);
  dprint(p_user_controls, "Pot vals: ");
  for (int i = 0; i < NUM_POTS; i++) {
    dprint(p_user_controls, pot_vals[i]);
    dprint(p_user_controls, "\t");
  }
  dprintln(p_user_controls);
}
#endif // HV_MAJOR > 2


bool testJumpers(bool p_user_controls) {
  dprintln(p_user_controls, "Testing the PCB for jumpers");
  delay(2000);
  bool populated = true;
  bool values[10];
  values[0] = digitalRead(BUT1_PIN);
  values[1] = digitalRead(BUT2_PIN);
  values[2] = digitalRead(BUT3_PIN);
  values[3] = digitalRead(BUT4_PIN);
  values[4] = digitalRead(BUT5_PIN);
  values[5] = digitalRead(BUT6_PIN);
#if NUM_JUMPERS > 6
  values[6] = digitalRead(BUT7_PIN);
  values[7] = digitalRead(BUT8_PIN);
  values[8] = digitalRead(BUT9_PIN);
  values[9] = digitalRead(BUT10_PIN);
#endif // HV_major > 2
  dprintMinorDivide(p_user_controls);

  for (int i = 0; i < 10; i++) {
    if (values[0] != digitalRead(BUT1_PIN)) {
      populated = false;
      dprintln(p_user_controls, "BUT1_PIN returned multiple values");
    }
    if (values[1] != digitalRead(BUT2_PIN)) {
      populated = false;
      dprintln(p_user_controls, "BUT2_PIN returned multiple values");
    }
    if (values[2] != digitalRead(BUT3_PIN)) {
      populated = false;
      dprintln(p_user_controls, "BUT3_PIN returned multiple values");
    }
    if (values[3] != digitalRead(BUT4_PIN)) {
      populated = false;
      dprintln(p_user_controls, "BUT4_PIN returned multiple values");
    }
    if (values[4] != digitalRead(BUT5_PIN)) {
      populated = false;
      dprintln(p_user_controls, "BUT5_PIN returned multiple values");
    }
    if (values[5] != digitalRead(BUT6_PIN)) {
      populated = false;
      dprintln(p_user_controls, "BUT6_PIN returned multiple values");
    }

#if HV_MAJOR > 2
    if (values[6] != digitalRead(BUT7_PIN)) {
      populated = false;
      dprintln(p_user_controls, "BUT7_PIN returned multiple values");
    }
    if (values[7] != digitalRead(BUT8_PIN)) {
      populated = false;
      dprintln(p_user_controls, "BUT8_PIN returned multiple values");
    }
    if (values[8] != digitalRead(BUT9_PIN)) {
      populated = false;
      dprintln(p_user_controls, "BUT9_PIN returned multiple values");
    }
    if (values[9] != digitalRead(BUT10_PIN)) {
      populated = false;
      dprintln(p_user_controls, "BUT10_PIN returned multiple values");
    }

#endif // HV_MAJOR > 2
    if (populated == true) {
      dprint(p_user_controls, ".\t");
    }
    delay(100);
  }
  if (populated == true) {
    dprintln(p_user_controls, "\nGreat news, the jumpers all seem to be in working order");
  } else {
    dprintln(p_user_controls, "Crap, for some reason some of the jumpers are returning multiple values, are the headers present?");
  }
  return populated;
}
