
#if ARTEFACT_GENUS == LEGATUS

    if (DISABLE_USER_CONTROLS == false)
    {
      uimanager.update();
    }
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
#endif // LEGATUS GENUS ONLY!!!