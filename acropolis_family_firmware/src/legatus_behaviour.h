
#if ARTEFACT_GENUS == LEGATUS

int determineLegatusBehavior()
{
  if (FM_FEEDBACK_ACTIVE == true)
  {
    return FM_FEEDBACK_MODE;
  }
  else if (FEEDBACK_ACTIVE == true)
  {
    return FEEDBACK_MODE;
  }
  else if (PLAYBACK_ACTIVE == true)
  {
    return PLAYBACK_MODE;
  }
  else if (MATCH_PITCH_ACTIVE == true)
  {
    return MATCH_PITCH_MODE;
  }
  return 0;
}



void legatusLoopSamplePlayback()
{
    int avg_time = 1000 * 60 * 0.5;
    // PLAYBACK_INTERVAL = avg_time - (weather_manager.getTemperature() * (avg_time / 40)) + (weather_manager.getHumidity() * (avg_time / 100)); // uint16_t t = random(45, 150);
    if (loop_tmr > loop_print_delay)
    {
      Serial.print("Awaiting playback (recording is TODO) : ");
      Serial.println(PLAYBACK_INTERVAL - last_playback_tmr);
      loop_tmr = 0;
    }
    if (last_playback_tmr > PLAYBACK_INTERVAL)
    {
      Serial.print("PLAYBACK_INTERVAL is : ");
      Serial.println(PLAYBACK_INTERVAL);
      int file_num = random(0, NUM_AUDIO_FILES);
      // playFile(audio_file_names[file_num], 0.5);
      // digitalWrite(PWR_KILL_PIN, LOW); // kill the power I think...
      last_playback_tmr = 0;
      Serial.println("last_playback_tmr is reset now to 0");
    }
}

void activateFeedback(float amp, float dur)
{
  amp = amp * 0.125 * USER_CONTROL_PLAYBACK_GAIN;
  if (amp < 0.01)
  {
    amp = 0.01;
  }
  else if (amp > 0.015)
  {
    amp = 0.015;
  }
  Serial.print("activateFeedback(amp, dur) called: ");
  Serial.print(amp);
  Serial.print("\t");
  Serial.println(dur);

  // connect amp to audio output
  for (float i = 0.0; i < amp; i = i + 0.001)
  {
    updateLegatusPassiveLEDs();
    Serial.print("gain is : ");
    Serial.println(i);
    amp3.gain(i);
    delay(10);
  }

  elapsedMillis t;
  while (t < dur)
  {
    /*
      amp = amp * 0.125 * USER_CONTROL_PLAYBACK_GAIN;
      if (amp < 0.01){
      amp = 0.01;
      } else if (amp > 0.04) {
      amp = 0.04;
      }
      uimanager.update();
      amp3.gain(amp);
    */
    updateLegatusPassiveLEDs();
    delay(10);
  }

  delay(dur);

  for (float i = amp; i > 0.0; i = i - 0.001)
  {
    updateLegatusPassiveLEDs();
    amp3.gain(i);
    delay(10);
    Serial.print("gain is : ");
    Serial.println(i);
  }
  amp3.gain(0.0);
}

void legatusLoopFeedback()
{
  updateLegatusPassiveLEDs();
  uimanager.update();
  int avg_time = 1000 * 60 * 2.0;
  float temp = weather_manager.getTemperature();
  float humid = weather_manager.getHumidity();
  PLAYBACK_INTERVAL = avg_time - (temp * (avg_time / 40)) + (humid * (avg_time / 100)); // uint16_t t = random(45, 150);
  if (loop_tmr > loop_print_delay)
  {
    Serial.println(PLAYBACK_INTERVAL - last_playback_tmr);
    loop_tmr = 0;
  }
  if (last_playback_tmr > PLAYBACK_INTERVAL)
  {
    Serial.print("PLAYBACK_INTERVAL is : ");
    Serial.println(PLAYBACK_INTERVAL);
    float factor = random(1, 8);
    float base_freq = factor * ((temp * 2) + (humid * 2));
    activateFeedback(1.0, PLAYBACK_INTERVAL);
    // turn on oscillator
    // let it run for a period of time
    // turn off oscillator

    // digitalWrite(PWR_KILL_PIN, LOW); // kill the power I think...
    last_playback_tmr = 0;
    Serial.println("last_playback_tmr is reset now to 0");
  }
}

void activateFM(int t, float freq, float amp)
{
  if (amp > 1.0)
  {
    amp = 1.0;
  }
  Serial.print("activateFM called (len, freq, amp): ");
  Serial.print(t);
  Serial.print("\t");
  Serial.print(freq);
  Serial.print("\t");
  Serial.println(amp);

  amp3.gain(starting_gain * amp * USER_CONTROL_PLAYBACK_GAIN);
  sine_fm.frequency(freq);
  for (float i = 0.0; i < 1.0; i = i + 0.0001)
  {
    // make sure we dont pass an amp over 1.0
    if (i > 1.0)
    {
      i = 1.0;
    }
    updateLegatusPassiveLEDs();
    uimanager.update();
    sine_fm.amplitude(i * USER_CONTROL_PLAYBACK_GAIN);
    delay(5);
  }
  for (float i = 0; i < 20; i++)
  {
    updateLegatusPassiveLEDs();
    freq += random(freq * -0.01, freq * 0.01);
    Serial.print("freq:");
    Serial.println(freq);
    sine_fm.frequency(freq);
    float d_time = t * 0.05;
    elapsedMillis m;
    while (m < d_time)
    {
      uimanager.update();
      sine_fm.amplitude(USER_CONTROL_PLAYBACK_GAIN);
      // amp3.gain(starting_gain * amp * USER_CONTROL_PLAYBACK_GAIN);
      updateLegatusPassiveLEDs();
      delay(20);
    }
  }
  for (float i = 1.0; i > 0.0; i = i - 0.0001)
  {
    updateLegatusPassiveLEDs();
    uimanager.update();
    sine_fm.amplitude(i * USER_CONTROL_PLAYBACK_GAIN);
    delay(10);
  }
  sine_fm.amplitude(0.0);
  amp3.gain(starting_gain);
}

int avg_time = 1000 * 60 * 2.0;
void legatusLoopFM()
{
  updateLegatusPassiveLEDs();
  uimanager.update();
  float temp = weather_manager.getTemperature();
  float humid = weather_manager.getHumidity();
  PLAYBACK_INTERVAL = avg_time - (temp * (avg_time / 40)) + (humid * (avg_time / 100)); // uint16_t t = random(45, 150);
  if (loop_tmr > loop_print_delay)
  {
    Serial.println(PLAYBACK_INTERVAL - last_playback_tmr);
    loop_tmr = 0;
  }
  if (last_playback_tmr > PLAYBACK_INTERVAL)
  {
    avg_time = 1000 * 60 * random(1, 4);
    Serial.print("PLAYBACK_INTERVAL is : ");
    Serial.println(PLAYBACK_INTERVAL);
    float factor = random(1, 16);
    float base_freq = factor * ((temp * 2) + (humid * 2));
    float amp = 0.5 + random(0, 1000) / 1000;
    activateFM(PLAYBACK_INTERVAL, base_freq, amp);
    // turn on oscillator
    // let it run for a period of time
    // turn off oscillator

    // digitalWrite(PWR_KILL_PIN, LOW); // kill the power I think...
    last_playback_tmr = 0;
    Serial.println("last_playback_tmr is reset now to 0");
  }
}

#endif // ARTEFACT_GENUS == LEGATUS (for all types)

#if SD_PRESENT
void initSD()
{
  SPI.setMOSI(SD_MOSI); // Audio shield has MOSI on pin 7
  SPI.setSCK(SD_CLK);   // Audio shield has SCK on pin 14
  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  if (!card.init(SPI_HALF_SPEED, SD_CS))
  {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    return;
  }
  else
  {
    Serial.println("Wiring is correct and a card is present.");
  }

  // print the type of card
  Serial.print("\nCard type: ");
  switch (card.type())
  {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
  }

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card))
  {
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    return;
  }

  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  Serial.print("\nVolume type is FAT");
  Serial.println(volume.fatType(), DEC);
  Serial.println();

  volumesize = volume.blocksPerCluster(); // clusters are collections of blocks
  volumesize *= volume.clusterCount();    // we'll have a lot of clusters
  if (volumesize < 8388608ul)
  {
    Serial.print("Volume size (bytes): ");
    Serial.println(volumesize * 512); // SD card blocks are always 512 bytes
  }
  Serial.print("Volume size (Kbytes): ");
  volumesize /= 2;
  Serial.println(volumesize);
  Serial.print("Volume size (Mbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);

  Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);

  // list all files in the card with date and size
  root.ls(LS_R | LS_DATE | LS_SIZE);

  // list the length of all audio files
}
#endif // SD card related functions

void setupSpeciesAudio()
{
  sgtl5000.enable();
  sgtl5000.volume(0.0);
  sgtl5000.inputSelect(AUDIO_INPUT_MIC);
  sgtl5000.micGain(54);

#if SD_PRESENT
  SPI.setMOSI(SD_MOSI);
  SPI.setSCK(SD_CLK);
  if (!(SD.begin(SD_CS)))
  {
    // stop here, but print a message repetitively
    while (1)
    {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
  #else
  Serial.println("WARNING SD_PRESENT IS SET TO FALSE!!!!");
  delay(400000);
#endif

  /////////////////////////////////////////////////////////////////////
  biquad3.setHighpass(0, LBQ1_THRESH, LBQ1_Q);
  // HPF1.setHighpass(1, LBQ1_THRESH, LBQ1_Q);
  // HPF1.setHighpass(2, LBQ1_THRESH, LBQ1_Q);
  // HPF1.setLowShelf(3, LBQ1_THRESH, LBQ1_DB);
  Serial.print("mic HPF has been configured (thresh/Q/dB): ");
  Serial.print(LBQ1_THRESH);
  Serial.print("\t");
  Serial.print(LBQ1_Q);
  Serial.print("\t");
  Serial.println(LBQ1_DB);

  biquad4.setLowpass(0, LBQ2_THRESH, LBQ2_Q);
  // LPF1.setLowpass(1, LBQ2_THRESH, LBQ2_Q);
  // LPF1.setLowpass(2, LBQ2_THRESH, LBQ2_Q);
  // LPF1.setHighShelf(3, LBQ2_THRESH, LBQ2_DB);
  Serial.print("mic LPF has been configured (thresh/Q/dB): ");
  Serial.print(LBQ2_THRESH);
  Serial.print("\t");
  Serial.print(LBQ2_Q);
  Serial.print("\t");
  Serial.println(LBQ2_DB);

  if (ARTEFACT_BEHAVIOUR == PLAYBACK_MODE)
  {
    audio_player.begin();
    biquad1.setHighpass(0, RBQ1_THRESH, RBQ1_Q);
    biquad1.setLowpass(1, RBQ2_THRESH, RBQ2_Q);
    // HPF1.setHighpass(2, LBQ1_THRESH, LBQ1_Q);
    // HPF1.setLowShelf(3, LBQ1_THRESH, LBQ1_DB);
    Serial.println("playback left filters have been configured");

    biquad2.setHighpass(0, RBQ1_THRESH, RBQ1_Q);
    biquad2.setLowpass(0, RBQ2_THRESH, RBQ2_Q);
    // LPF1.setLowpass(2, LBQ2_THRESH, LBQ2_Q);
    // LPF1.setHighShelf(3, LBQ2_THRESH, LBQ2_DB);
    Serial.println("playback right Filters have been configured ");

    printMinorDivide();

    //////////////////////////////////////////////////////////////////////////////////
    amp1.gain(USER_CONTROL_PLAYBACK_GAIN);
    amp2.gain(USER_CONTROL_PLAYBACK_GAIN);
    Serial.print("playback gains are set to : ");
    Serial.println(USER_CONTROL_PLAYBACK_GAIN);

    //////////////////////////////////////////////////////////////////////////////////
    // TODO - make sure ENC_GAIN_ADJ exists for all bots
    Serial.print("Starting gain is now set to: ");
    Serial.println(starting_gain);
    mixer1.gain(0, starting_gain * 0.5);
    mixer1.gain(1, starting_gain * 0.5);
    amp3.gain(starting_gain);

    mixer2.gain(0, 0.33);
    mixer2.gain(1, 0.33);
    mixer2.gain(2, 0.5);
    // Serial.println("Setting up the FFTManager to track the first channel");
    // fft_manager.addInput(&patchCord_fft_input1);
    // patchCord_fft_input2.disconnect();
  }
  else if (ARTEFACT_BEHAVIOUR == MATCH_PITCH_MODE)
  {
    mixer1.gain(0, starting_gain * 4);
    mixer1.gain(1, starting_gain * 4);
    sine_fm.amplitude(0.0);
    sine_fm.frequency(200);
  }
  else if (ARTEFACT_BEHAVIOUR == FEEDBACK_MODE)
  {
    audio_delay1.delay(0, 0);
    mixer1.gain(0, starting_gain);
    mixer1.gain(1, starting_gain);
  }
}

void updateBehaviour(){
  // TODO
  updateLegatusPassiveLEDs();
  ARTEFACT_BEHAVIOUR = determineLegatusBehavior();
  if (ARTEFACT_BEHAVIOUR == PLAYBACK_MODE)
  {
    legatusLoopSamplePlayback();
  }
  else if (ARTEFACT_BEHAVIOUR == FEEDBACK_MODE)
  {
    legatusLoopFeedback();
  }
  else if (ARTEFACT_BEHAVIOUR == FM_FEEDBACK_MODE)
  {
    legatusLoopFM();
  }
}

void setupSpecies() {

    ///////////////////////////// SD Card //////////////////////////////////
#if SD_PRESENT
    neos[i].colorWipe(12, 0, 0, 1.0);
    initSD();
#endif
  if (ARTEFACT_BEHAVIOUR == PLAYBACK_MODE)
  {
    audio_connections[0] = new AudioConnection(i2s1, 0, mixer1, 0);
    audio_connections[1] = new AudioConnection(mixer2, 0, output_usb, 0);
    audio_connections[2] = new AudioConnection(i2s1, 1, mixer1, 1);
    audio_connections[3] = new AudioConnection(audio_player, 0, biquad1, 0);
    audio_connections[4] = new AudioConnection(audio_player, 1, biquad2, 0);
    audio_connections[5] = new AudioConnection(mixer1, 0, output_usb, 1);
    audio_connections[6] = new AudioConnection(mixer1, biquad3);
    audio_connections[7] = new AudioConnection(biquad1, amp1);
    audio_connections[8] = new AudioConnection(biquad2, amp2);
    audio_connections[9] = new AudioConnection(biquad3, biquad4);
    audio_connections[10] = new AudioConnection(amp1, 0, audioOutput, 0);
    audio_connections[11] = new AudioConnection(amp1, 0, mixer2, 0);
    audio_connections[12] = new AudioConnection(amp2, 0, audioOutput, 1);
    audio_connections[13] = new AudioConnection(amp2, 0, mixer2, 1);
    audio_connections[14] = new AudioConnection(biquad4, amp3);
    audio_connections[15] = new AudioConnection(amp3, 0, mixer2, 2);
    audio_connections[16] = new AudioConnection(mixer2, rms1);
    audio_connections[17] = new AudioConnection(mixer2, fft1);
    audio_connections[18] = new AudioConnection(mixer2, peak1);
  }
  else if (ARTEFACT_BEHAVIOUR == FEEDBACK_MODE)
  {
    audio_connections[0] = new AudioConnection(i2s1, 0, mixer1, 0);
    audio_connections[1] = new AudioConnection(i2s1, 1, mixer1, 1);
    audio_connections[2] = new AudioConnection(mixer1, 0, output_usb, 1);
    audio_connections[3] = new AudioConnection(mixer1, biquad3);
    audio_connections[4] = new AudioConnection(biquad3, biquad4);
    audio_connections[5] = new AudioConnection(biquad4, amp3);
    audio_connections[6] = new AudioConnection(amp3, rms1);
    audio_connections[7] = new AudioConnection(amp3, fft1);
    audio_connections[8] = new AudioConnection(amp3, peak1);
    audio_connections[9] = new AudioConnection(amp3, 0, output_usb, 0);
    audio_connections[10] = new AudioConnection(audio_delay1, 0, audioOutput, 0);
    audio_connections[11] = new AudioConnection(audio_delay1, 0, audioOutput, 1);
    audio_connections[12] = new AudioConnection(amp3, 0, audio_delay1, 0);
  }
  else if (ARTEFACT_BEHAVIOUR == FM_FEEDBACK_MODE)
  {
    audio_connections[0] = new AudioConnection(i2s1, 0, mixer1, 0);
    audio_connections[1] = new AudioConnection(i2s1, 1, mixer1, 1);
    audio_connections[2] = new AudioConnection(mixer1, 0, output_usb, 1);
    audio_connections[3] = new AudioConnection(mixer1, biquad3);
    audio_connections[4] = new AudioConnection(biquad3, biquad4);
    audio_connections[5] = new AudioConnection(biquad4, amp3);
    audio_connections[6] = new AudioConnection(amp3, rms1);
    audio_connections[7] = new AudioConnection(amp3, fft1);
    audio_connections[8] = new AudioConnection(amp3, peak1);
    audio_connections[9] = new AudioConnection(amp3, 0, output_usb, 0);
    audio_connections[10] = new AudioConnection(amp3, sine_fm);
    audio_connections[11] = new AudioConnection(sine_fm, 0, audioOutput, 0);
  }
  // setup up some value tracker stuff
  brightness_tracker.setMinMaxUpdateFactor(BGT_MIN_UPDATE_FACTOR, BGT_MAX_UPDATE_FACTOR);
  hue_tracker.setMinMaxUpdateFactor(HUE_MIN_UPDATE_FACTOR, HUE_MAX_UPDATE_FACTOR);
  saturation_tracker.setMinMaxUpdateFactor(SAT_MIN_UPDATE_FACTOR, SAT_MAX_UPDATE_FACTOR);

  #if USER_CONTROLS_ACTIVE
  /////////////// User Controls ////////////////////////////////////////////
  uimanager.addBut(BUT1_PIN, BUT1_PULLUP, BUT1_LOW_VAL, BUT1_HIGH_VAL, BUT1_LOW_CHANGES, &but_test[0], BUT1_NAME);
  uimanager.addBut(BUT2_PIN, BUT2_PULLUP, BUT2_LOW_VAL, BUT2_HIGH_VAL, BUT2_LOW_CHANGES, &PLAYBACK_ACTIVE, BUT2_NAME);
  uimanager.addBut(BUT3_PIN, BUT3_PULLUP, BUT3_LOW_VAL, BUT3_HIGH_VAL, BUT3_LOW_CHANGES, &FM_FEEDBACK_ACTIVE, BUT3_NAME);
  uimanager.addBut(BUT4_PIN, BUT4_PULLUP, BUT4_LOW_VAL, BUT4_HIGH_VAL, BUT4_LOW_CHANGES, &FEEDBACK_ACTIVE, BUT4_NAME);
  uimanager.addBut(BUT5_PIN, BUT5_PULLUP, BUT5_LOW_VAL, BUT5_HIGH_VAL, BUT5_LOW_CHANGES, &but_test[4], BUT5_NAME);
  uimanager.addBut(BUT6_PIN, BUT6_PULLUP, BUT6_LOW_VAL, BUT6_HIGH_VAL, BUT6_LOW_CHANGES, &but_test[5], BUT6_NAME);

  uimanager.addPot(POT1_PIN, POT1_REVERSE, POT1_PLAY, &user_brightness_scaler, POT1_NAME);
  uimanager.addPot(POT2_PIN, POT2_REVERSE, POT2_PLAY, &USER_CONTROL_PLAYBACK_GAIN, POT2_NAME);
  uimanager.addPotRange(0, min_user_brightness_scaler, mid_user_brightness_scaler, max_user_brightness_scaler);
  uimanager.addPotRange(1, min_playback_gain, mid_playback_gain, max_playback_gain);

  uimanager.setup(false);
  uimanager.printAll();
  #else
    Serial.println("WARNING - user controls are disabled");
  #endif
  //////////////////// power control ///////////////////////
  pinMode(PWR_KILL_PIN, OUTPUT);
  digitalWrite(PWR_KILL_PIN, HIGH);
}
#endif // LEGATUS GENUS ONLY!!!