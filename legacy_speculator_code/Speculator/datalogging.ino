//// TODO
//// void updateDatalog() {
////   datalog_manager.update();
////   runtime = (double)millis() / 60000;
//// }
//
//void setupDLManagerCicada() {
//  // log data to EEPROM if datalogging is active
//  if (data_logging_active) {
//    Serial.println("configuring datalog_manager timers");
//    datalog_manager.configureAutoTimer(0, DATALOG_START_DELAY_1, DATALOG_TIME_FRAME_1, DATALOG_1_LENGTH);
//    datalog_manager.configureAutoTimer(1, DATALOG_START_DELAY_2, DATALOG_TIME_FRAME_2, DATALOG_2_LENGTH);
//    datalog_manager.configureStaticTimer(2, DATALOG_START_DELAY_3, STATICLOG_RATE_FAST);
//    datalog_manager.configureStaticTimer(3, DATALOG_START_DELAY_4, STATICLOG_RATE_SLOW);
//    datalog_manager.printTimerConfigs();
//
//    // Hardware / Software / Serial Numbers
//    datalog_manager.logSetupConfigByte("Hardware Version major      : ", HV_MAJOR);
//    datalog_manager.logSetupConfigByte("Hardware Version minor      : ", HV_MINOR);
//    datalog_manager.logSetupConfigByte("Software Version major      : ", SV_MAJOR);
//    datalog_manager.logSetupConfigByte("Software Version major      : ", SV_MINOR);
//    datalog_manager.logSetupConfigByte("Software Version major      : ", SV_SUBVERSION);
//    datalog_manager.logSetupConfigByte("Bot ID Number               : ", SERIAL_ID);
//    datalog_manager.logSetupConfigByte("Datalog Active              : ", data_logging_active);
//    datalog_manager.logSetupConfigByte("Firmware Mode               : ", FIRMWARE_MODE);
//    // Lux Sensors
//    printMinorDivide();
//    datalog_manager.logSetupConfigDouble("Lux Low Threshold           : ", LOW_LUX_THRESHOLD);
//    datalog_manager.logSetupConfigDouble("Lux Mid Threshold           : ", MID_LUX_THRESHOLD);
//    datalog_manager.logSetupConfigDouble("Lux High Threshold          : ", HIGH_LUX_THRESHOLD);
//    datalog_manager.logSetupConfigDouble("Brightness Scaler Min       : ", LUX_BS_MIN);
//    datalog_manager.logSetupConfigDouble("Brightness Scaler Max       : ", LUX_BS_MAX);
//    datalog_manager.logSetupConfigByte("Min Brightness              : ", MIN_BRIGHTNESS);
//    datalog_manager.logSetupConfigByte("Max Brightness              : ", MAX_BRIGHTNESS);
//    // Auto Gain
//    printMinorDivide();
//    datalog_manager.logSetupConfigDouble("Starting Gain               : ", STARTING_GAIN);
//    datalog_manager.logSetupConfigDouble("user_brightness_scaler   : ", user_brightness_scaler);
//    datalog_manager.logSetupConfigByte("Autogain Active             : ", AUTOGAIN_ACTIVE);
//    datalog_manager.logSetupConfigDouble("Max Autogain Adjustment     : ", MAX_GAIN_ADJ);
//    datalog_manager.logSetupConfigLong("Autogain Frequency          : ", AUTOGAIN_FREQUENCY);
//    // Autolog settings
//    printMinorDivide();
//    datalog_manager.logSetupConfigLong("Timer 0 Start Time          : ", datalog_manager.getTimerStart(0));
//    datalog_manager.logSetupConfigLong("Timer 0 End Time            : ", datalog_manager.getTimerEnd(0));
//    datalog_manager.logSetupConfigLong("Timer 0 Logging Rate        : ", datalog_manager.getTimerRate(0));
//    datalog_manager.logSetupConfigLong("Timer 1 Start Time          : ", datalog_manager.getTimerStart(1));
//    datalog_manager.logSetupConfigLong("Timer 1 End Time            : ", datalog_manager.getTimerEnd(1));
//    datalog_manager.logSetupConfigLong("Timer 1 Logging Rate        : ", datalog_manager.getTimerRate(1));
//    datalog_manager.logSetupConfigLong("Timer 2 Start Time          : ", datalog_manager.getTimerStart(2));
//    datalog_manager.logSetupConfigLong("Timer 2 End Time            : ", datalog_manager.getTimerEnd(2));
//    datalog_manager.logSetupConfigLong("Timer 2 Logging Rate        : ", datalog_manager.getTimerRate(2));
//    datalog_manager.logSetupConfigLong("Timer 3 Start Time          : ", datalog_manager.getTimerStart(3));
//    datalog_manager.logSetupConfigLong("Timer 3 End Time            : ", datalog_manager.getTimerEnd(3));
//    datalog_manager.logSetupConfigLong("Timer 3 Logging Rate        : ", datalog_manager.getTimerRate(3));
//    printMinorDivide();
//    // runtime log
//    if (STATICLOG_RUNTIME) {
//      datalog_manager.addStaticLog("Program Runtime (minutes)              : ",
//                                   STATICLOG_RUNTIME_TIMER, &runtime);
//    }
//    // the constantly updating logs
//    if (STATICLOG_LUX_VALUES) {
//      datalog_manager.addStaticLog("Lowest Lux Recorded : ",
//                                   STATICLOG_LUX_MIN_MAX_TIMER, &lux_manager.min_reading);
//      datalog_manager.addStaticLog("Highest Lux Recorded : ",
//                                   STATICLOG_LUX_MIN_MAX_TIMER, &lux_manager.max_reading);
//    }
//    /*
//    if (STATICLOG_MASTER_GAIN) {
//      datalog_manager.addStaticLog("Lowest Master Gain  : ",
//                                   STATICLOG_ONSET_GAIN_TIMER, &feature_collector.min_gain);
//      datalog_manager.addStaticLog("Highest Master Gain ",
//                                   STATICLOG_ONSET_GAIN_TIMER, &feature_collector.max_gain);
//      datalog_manager.logSetupConfigDouble("Master Starting Gain          : ", STARTING_GAIN);
//    }
//    */
//    if (STATICLOG_FLASHES) {
//      /*
//        datalog_manager.addStaticLog("Front Total Flashes Detected  : ",
//                                   STATICLOG_FLASHES_TIMER, &total_flashes[0]);
//        datalog_manager.addStaticLog("Rear Total Flashes Detected   : ",
//                                   STATICLOG_FLASHES_TIMER, &total_flashes[1]);
//      */
//    }
//
//    if (STATICLOG_RGB_AVG_VALS) {
//      datalog_manager.addStaticLog("Average Red Value (Front)     : ",
//                                   STATICLOG_RGB_AVG_VALS_TIMER, &neos.red_avg);
//      datalog_manager.addStaticLog("Average Green Value (Front)   : ",
//                                   STATICLOG_RGB_AVG_VALS_TIMER, &neos.green_avg);
//      datalog_manager.addStaticLog("Average Blue Value (Front)    : ",
//                                   STATICLOG_RGB_AVG_VALS_TIMER, &neos.blue_avg);
//    }
//
//    // todo double check the addr
//    // Datalog lux_log_f = Datalog(EEPROM_LUX_LOG_START, "Lux Front", lux_managers->lux, true);
//    // datalog_manager.startAutolog(0);
//    double * ptr;
//    if (AUTOLOG_LUX_F > 0 && lux_manager.sensor_active[0] > 0) {
//      datalog_manager.addAutolog("Front Lux Log ", AUTOLOG_LUX_TIMER, &lux_manager.lux[0]);
//    }
//    if (AUTOLOG_LUX_R > 0 && lux_manager.sensor_active[1] > 0) {
//      datalog_manager.addAutolog("Rear Lux Log ", AUTOLOG_LUX_TIMER, &lux_manager.lux[1]);
//    }
//    if (AUTOLOG_LED_ON_OFF > 0) {
//      ptr = &neos.on_ratio;
//      datalog_manager.addAutolog("Front Led On/Off Ratio Log ", AUTOLOG_LED_ON_OFF_TIMER, ptr);
//    }
//    if (AUTOLOG_BRIGHTNESS_SCALER > 0) {
//      ptr = &lux_manager.brightness_scaler_avg;
//      lux_manager.resetBrightnessScalerAvg();
//      datalog_manager.addAutolog("Front Brightness Scaler Averages ", AUTOLOG_BRIGHTNESS_SCALER_TIMER, ptr);
//    }
//    if (AUTOLOG_FPM > 0) {
//      ptr = &neos.fpm;
//      lux_manager.resetBrightnessScalerAvg();
//      datalog_manager.addAutolog("Front Flashes Per Minute  ", AUTOLOG_FPM_TIMER, ptr);
//    }
//    // printing needs to be at the end so that everything actually displays
//    if (P_EEPROM_CONTENTS > 0) {
//      delay(100);
//      datalog_manager.printAllLogs();
//    } else {
//      Serial.println("Not printing the EEPROM Datalog Contents");
//    }
//  } else {
//    if (P_EEPROM_CONTENTS > 0) {
//      datalog_manager.printAllLogs();
//    }
//  }
//  if (CLEAR_EEPROM_CONTENTS > 0) {
//    delay(100);
//    datalog_manager.clearLogs();
//  } else {
//    Serial.println("Not clearing the EEPROM Datalog Contents");
//  }
//}
