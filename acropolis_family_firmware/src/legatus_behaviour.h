#ifndef _LEGATUS_BEHAVIOUR_H__
#define _LEGATUS_BEHAVIOUR_H__

#include "configuration/Macros.h"

//////////////////////////////////// GLOBAL VARIABLES ///////////////////////////////////////
uint8_t colors[3] = {155, 70, 200};
float fcolors[3];
float mix_gains[3] = {0.0, 0.0, 0.0};
double prior_vocalisation_length = 0;

/////////////////////////////////// Vocalisation Functions ///////////////////////////////////


void brightnessWipe(double b)
{
  // uses the currently stored color, adjusts the brightness
  neos[0].colorWipe(constrain((int)(colors[0] * b), 0, 255), constrain((int)(colors[1] * b), 0, 255), constrain((int)(colors[2] * b), 0, 255), user_brightness_scaler);
}

void updateLegatusPassiveLEDs(bool _p = false)
{
  if (fft1.available())
  {
    fcolors[2] = fft1.read(1, 8) * 4;   // 172  - 1536  Hz
    fcolors[1] = fft1.read(8, 23) * 4;  // 1536 - 4416  Hz
    fcolors[0] = fft1.read(23, 93) * 4; // 1536 - 15996 Hz
    for (int i = 0; i < 3; i++)
    {
      if (colors[i] > 1.0)
      {
        colors[i] = 1.0;
      }
    }
    for (int i = 0; i < 3; i++)
    {
      colors[i] = (uint8_t)(fcolors[i] * 255);
    }
     dprint(_p,colors[0]);
     dprint(_p,"\t");
     dprint(_p,colors[1]);
     dprint(_p,"\t");
     dprintln(_p,colors[2]);
  }
  if (peak1.available())
  {
    int c = (int)(peak1.read() * 4096) * user_brightness_scaler;
    float f = (float)constrain(c, 0, 255) / 255.0;
    brightnessWipe(f);
    dprintln(_p, f);
  }
}

void fadeMixer(float c1_current, float c2_current, float c1_target, float c2_target)
{
  /*
   * Function for automating mixer fade in and fade out
   */
  float slice1 = (c1_target - c1_current) * 0.01;
  float slice2 = (c2_target - c2_current) * 0.01;

  for (int i = 0; i < 100; i++)
  {

    c1_current = c1_current + slice1;
    c2_current = c2_current + slice2;

    mixer2.gain(0, c1_current);
    mixer2.gain(1, c1_current);
    mixer2.gain(2, c2_current);
    delay(10);
  }
}

long playFile(const char *filename, float rate)
{
  if (last_playback_tmr < PLAYBACK_INTERVAL)
  {
    if (last_playback_tmr % 1000 == 0) {
      dprintln(P_AUDIO_PLAYBACK, "Too soon for sample playback, ");
      dprint(P_AUDIO_PLAYBACK, (last_playback_tmr - PLAYBACK_INTERVAL) / 1000);
      dprintln(P_AUDIO_PLAYBACK, " seconds left until vocalisation unlocked");
      return 0;
    }
  }
  /* function returns the number of MS it took to play the file, or 0, if the file did not play */
  last_playback_tmr = 0;
  dprintln(P_AUDIO_PLAYBACK, "---------------------------------");
  dprint(P_AUDIO_PLAYBACK, "Playing file: ");
  dprintln(P_AUDIO_PLAYBACK, filename);

  amp1.gain(USER_CONTROL_PLAYBACK_GAIN);
  amp2.gain(USER_CONTROL_PLAYBACK_GAIN);
  audio_player.play(filename);
#if USE_RAW_AUDIO_PLAYERj
  // TODO 
  //  audio_player.setPlaybackRate(rate);
#endif
  // first two numbers are mixing in the sample playback with the "starting gain" and "ending gain"
  // the second two numbers are handling the microphone gain 
  // but please note, that if the microphone gain is lowered to 0.0 the feedback leds will cease to function...
  fadeMixer(0, 0.5, 0.5, 0.1);
  dprint(P_AUDIO_PLAYBACK, "file length : ");
  dprintln(P_AUDIO_PLAYBACK, audio_player.lengthMillis());
  // dprint(P_AUDIO_PLAYBACK, "Playback Rate : ");
  //dprintln(P_AUDIO_PLAYBACK, rate);
  dprint(P_AUDIO_PLAYBACK, "Amplitude : ");
  dprintln(P_AUDIO_PLAYBACK, USER_CONTROL_PLAYBACK_GAIN);
  // Start playing the file.  This sketch continues to
  // run while the file plays.  audio_player.play(filename);
  // A brief delay for the library read WAV info
  delay(15);
  // Simply wait for the file to finish playing.
  while (audio_player.isPlaying())
  {
    PLAYBACK_INTERVAL = audio_player.lengthMillis();
    // readUserControls();
    #if USER_CONTROLS_ACTIVE
      double prior_gain = USER_CONTROL_PLAYBACK_GAIN;
    if (uimanager.update())
    {
      if (P_USER_CONTROLS)
      {
        uimanager.printAll();
      }
      if (prior_gain != USER_CONTROL_PLAYBACK_GAIN){
        amp1.gain(USER_CONTROL_PLAYBACK_GAIN);
        amp2.gain(USER_CONTROL_PLAYBACK_GAIN);
        dprint(P_AUDIO_PLAYBACK, "Updated the user control playback gain: ");
        dprintln(P_AUDIO_PLAYBACK, USER_CONTROL_PLAYBACK_GAIN);
      }
    }
    #endif // USER_CONTROLS_ACTIVE

    updateLegatusPassiveLEDs();
    if (last_playback_tmr % 1000 == 0){
      dprint(P_AUDIO_PLAYBACK, "playtime : ");
      dprint(P_AUDIO_PLAYBACK, last_playback_tmr/1000);
      dprint(P_AUDIO_PLAYBACK, " of ");
      dprintln(P_AUDIO_PLAYBACK, PLAYBACK_INTERVAL/1000);
    }
  }
  // change the mixer levels back so the microphone is dominate
  fadeMixer(0.5, 0.0, 0.2, 0.5);
  dprint(P_AUDIO_PLAYBACK, "Finished playing back file for ");
  dprint(P_AUDIO_PLAYBACK, last_playback_tmr);
  dprintln(P_AUDIO_PLAYBACK, " ms");
  dprint(P_AUDIO_PLAYBACK, "Next playback interval is: ");
  dprintln(P_AUDIO_PLAYBACK, PLAYBACK_INTERVAL);
  long _return = last_playback_tmr;
  last_playback_tmr = 0;
  return _return;
}

long calculatePlaybackInterval(bool _p = false) {
  long value;
  // this is the "default period" for sample vocalisation delays
  //             ms     sec  minutes
  int avg_time = 1000 * 60 * 2.0;
  // this is the weathermanager dependent scaling for the average time
  double temp_scaler = weather_manager.getScaledTemp();
  double humid_scaler = weather_manager.getScaledTemp();
  double light_scaler = lux_manager.getScaledLux();
  dprintln(_p, "-------------------------------------");
  dprint(_p, "avg_time: ");
  dprintln(_p, avg_time);
  dprint(_p, "prior_vocalisation_length: ");
  dprintln(_p, prior_vocalisation_length);
  dprint(_p, "temp_scaler: ");
  dprintln(_p, temp_scaler);
  dprint(_p, "humid_scaler: ");
  dprintln(_p, humid_scaler);
  dprint(_p, "light_scaler: ");
  dprintln(_p, light_scaler);
  dprintln(_p, "");
  value = avg_time + prior_vocalisation_length;
  dprint(_p, "PLAYBACK_INTERVAL = avg_time + prior_vocalisation_length: = ");
  dprintln(_p, PLAYBACK_INTERVAL);
  dprintln(_p, "PLAYBACK_INTERVAL = PLAYBACK_INTERVAL * (temp_scaler + humid_scaler + light_scaler) / 3 ");
  value = value * (temp_scaler + humid_scaler + light_scaler) / 3;
  dprint(_p, "PLAYBACK_INTERVAL = ");
  dprint(_p, value/1000);
  dprintln(_p, " seconds");
  dprintln(_p, "-------------------------------------");
  return value;
}

void legatusLoopSamplePlayback()
{
  PLAYBACK_INTERVAL = calculatePlaybackInterval(P_CALCULATE_PLAYBACK_INTERVAL);
  // PLAYBACK_INTERVAL = avg_time - (weather_manager.getTemperature() * (avg_time / 40)) + (weather_manager.getHumidity() * (avg_time / 100)); // uint16_t t = random(45, 150);
  // erm, WTF is happening right now in code, need to make it conform with what is written in thesis...
  if (loop_tmr > loop_print_delay)
  {
    Serial.print("Awaiting playback (recording is TODO) : ");
    Serial.println(PLAYBACK_INTERVAL - last_playback_tmr);
    loop_tmr = 0;
  }
  Serial.print("PLAYBACK_INTERVAL is : ");
  Serial.println(PLAYBACK_INTERVAL);
  int file_num = random(0, NUM_AUDIO_FILES);
  // file will only play if it has been long enough since last sample playback
  prior_vocalisation_length = playFile(audio_file_names[file_num], 0.5);
  // digitalWrite(PWR_KILL_PIN, LOW); // kill the power I think...
  Serial.println("last_playback_tmr is reset now to 0");
}


void activateFeedback(float amp, float dur)
{
  mixer1.gain(0, 0.5);
  mixer1.gain(1, 0.5);
  amp3.gain(1.0); /// derrr, not sure what amp3 is actually doing! TODO
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
      amp3.gain(amp);
    */
    uimanager.update();
    amp1.gain(USER_CONTROL_PLAYBACK_GAIN);
    amp2.gain(USER_CONTROL_PLAYBACK_GAIN);
    updateLegatusPassiveLEDs(P_UPDATE_LEGATUS_PASSIVE_LEDS);
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
  mixer1.gain(0, 0.0);
  mixer1.gain(1, 0.0); 
}

void legatusLoopFeedback()
{
  #if USER_CONTROLS_ACTIVE
  uimanager.update();
  #endif // USER_CONTROLS_ACTIVE
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
    #if USER_CONTROLS_ACTIVE
    uimanager.update();
    #endif // USER_CONTROLS_ACTIVE
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
      #if USER_CONTROLS_ACTIVE
      uimanager.update();
      #endif // USER_CONTROLS_ACTIVE
      sine_fm.amplitude(USER_CONTROL_PLAYBACK_GAIN);
      // amp3.gain(starting_gain * amp * USER_CONTROL_PLAYBACK_GAIN);
      updateLegatusPassiveLEDs();
      delay(20);
    }
  }
  for (float i = 1.0; i > 0.0; i = i - 0.0001)
  {
    updateLegatusPassiveLEDs();
    #if USER_CONTROLS_ACTIVE
    uimanager.update();
    #endif // USER_CONTROLS_ACTIVE
    sine_fm.amplitude(i * USER_CONTROL_PLAYBACK_GAIN);
    delay(10);
  }
  sine_fm.amplitude(0.0);
  amp3.gain(starting_gain);
}

int avg_time = 1000 * 60 * 2.0;
void legatusLoopFM()
{
  #if USER_CONTROLS_ACTIVE
  uimanager.update();
  #endif // USER_CONTROLS_ACTIVE
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
  delay(4000);
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
  // Serial.println("WARNING SD_PRESENT IS SET TO FALSE!!!!");
  // delay(100);
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

  if (ARTEFACT_BEHAVIOUR == B_LEG_SAMP_PLAYBACK)
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
    amp1.gain(0.0);
    amp2.gain(0.0);
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
  else if (ARTEFACT_BEHAVIOUR == B_LEG_MATCH_PITCH)
  {
    mixer1.gain(0, starting_gain * 4);
    mixer1.gain(1, starting_gain * 4);
    sine_fm.amplitude(0.0);
    sine_fm.frequency(200);
  }
  else if (ARTEFACT_BEHAVIOUR == B_LEG_FEEDBACK)
  {
    audio_delay1.delay(0, 0);
    mixer1.gain(0, 0.0);
    mixer1.gain(1, 0.0);
  }
}

void legatusLoopMatchPitch() {
  // TODO
  Serial.println("ERROR - this behaviour is not implemented");
  delay(1000);
}

void updateBehaviour(){
  // TODO
  updateLegatusPassiveLEDs(P_UPDATE_LEGATUS_PASSIVE_LEDS);
  // ARTEFACT_BEHAVIOUR = determineLegatusBehavior();
  if (ARTEFACT_BEHAVIOUR == B_LEG_SAMP_PLAYBACK)
  {
    legatusLoopSamplePlayback();
  }
  else if (ARTEFACT_BEHAVIOUR == B_LEG_FEEDBACK)
  {
    legatusLoopFeedback();
  }
  else if (ARTEFACT_BEHAVIOUR == B_LEG_FM_FEEDBACK)
  {
    legatusLoopFM();
  }
  else if (ARTEFACT_BEHAVIOUR == B_LEG_MATCH_PITCH)
  {
    legatusLoopMatchPitch();
  }
  else {
    Serial.println("ERROR, ARTEFACT_BEHAVIOUR IS NOT SET CORRECTLY!!!");
  }
}

void setupSpecies() {

    ///////////////////////////// SD Card //////////////////////////////////
#if SD_PRESENT
    neos[0].colorWipe(12, 0, 0, 1.0);
    initSD();
    neos[0].colorWipe(0, 12, 0, 1.0);
#endif

  if (ARTEFACT_BEHAVIOUR == B_LEG_SAMP_PLAYBACK)
  {
    audio_connections[0] = new AudioConnection(i2s1, 0, mixer1, 0);
    audio_connections[2] = new AudioConnection(i2s1, 1, mixer1, 1);
    audio_connections[3] = new AudioConnection(audio_player, 0, biquad1, 0);
    audio_connections[4] = new AudioConnection(audio_player, 1, biquad2, 0);
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

    #if AUDIO_USB
    audio_connections[1] = new AudioConnection(mixer2, 0, output_usb, 0);
    audio_connections[5] = new AudioConnection(mixer1, 0, output_usb, 1);
    #endif // AUDIO_USB
    Serial.println("Finished creating audio connections for B_LEG_SAMP_PLAYBACK");
  }
  else if (ARTEFACT_BEHAVIOUR == B_LEG_FEEDBACK)
  {
    audio_connections[0] = new AudioConnection(i2s1, 0, mixer1, 0);
    audio_connections[1] = new AudioConnection(i2s1, 1, mixer1, 1);
    audio_connections[3] = new AudioConnection(mixer1, biquad3);
    audio_connections[4] = new AudioConnection(biquad3, biquad4);
    audio_connections[5] = new AudioConnection(biquad4, amp3);
    audio_connections[6] = new AudioConnection(amp3, rms1);
    audio_connections[7] = new AudioConnection(amp3, fft1);
    audio_connections[8] = new AudioConnection(amp3, peak1);
    audio_connections[10] = new AudioConnection(audio_delay1, 0, audioOutput, 0);
    audio_connections[11] = new AudioConnection(audio_delay1, 0, audioOutput, 1);
    audio_connections[12] = new AudioConnection(amp3, 0, audio_delay1, 0);

    #if AUDIO_USB
    audio_connections[2] = new AudioConnection(mixer1, 0, output_usb, 1);
    audio_connections[9] = new AudioConnection(amp3, 0, output_usb, 0);
    #endif // AUDIO_USB
    last_playback_tmr = 99999999;

  }
  else if (ARTEFACT_BEHAVIOUR == B_LEG_FM_FEEDBACK)
  {
    audio_connections[0] = new AudioConnection(i2s1, 0, mixer1, 0);
    audio_connections[1] = new AudioConnection(i2s1, 1, mixer1, 1);
    audio_connections[3] = new AudioConnection(mixer1, biquad3);
    audio_connections[4] = new AudioConnection(biquad3, biquad4);
    audio_connections[5] = new AudioConnection(biquad4, amp3);
    audio_connections[6] = new AudioConnection(amp3, rms1);
    audio_connections[7] = new AudioConnection(amp3, fft1);
    audio_connections[8] = new AudioConnection(amp3, peak1);
    audio_connections[10] = new AudioConnection(amp3, sine_fm);
    audio_connections[11] = new AudioConnection(sine_fm, 0, audioOutput, 0);

    #if AUDIO_USB
    audio_connections[9] = new AudioConnection(amp3, 0, output_usb, 0);
    audio_connections[2] = new AudioConnection(mixer1, 0, output_usb, 1);
    #endif  // AUDIO_USB
  }
  else if (ARTEFACT_BEHAVIOUR == B_LEG_MATCH_PITCH)
  {
    audio_connections[0] = new AudioConnection(i2s1, 0, mixer1, 0);
    audio_connections[1] = new AudioConnection(i2s1, 1, mixer1, 1);
    audio_connections[3] = new AudioConnection(mixer1, biquad3);
    audio_connections[4] = new AudioConnection(biquad3, biquad4);
    audio_connections[5] = new AudioConnection(biquad4, amp3);
    audio_connections[6] = new AudioConnection(amp3, rms1);
    audio_connections[7] = new AudioConnection(amp3, fft1);
    audio_connections[8] = new AudioConnection(amp3, peak1);
    audio_connections[10] = new AudioConnection(amp3, sine_fm);
    audio_connections[11] = new AudioConnection(sine_fm, 0, audioOutput, 0);

    #if AUDIO_USB
    audio_connections[9] = new AudioConnection(amp3, 0, output_usb, 0);
    audio_connections[2] = new AudioConnection(mixer1, 0, output_usb, 1);
    #endif  // AUDIO_USB
    Serial.println("WARNING - AUDIO ROUTING FOR THIS BEHAVIOUR ROUTINE IS NOT FINISHED");
    delay(3000);
  }
  // setup up some value tracker stuff
  brightness_tracker.setMinMaxUpdateFactor(BGT_MIN_UPDATE_FACTOR, BGT_MAX_UPDATE_FACTOR);
  hue_tracker.setMinMaxUpdateFactor(HUE_MIN_UPDATE_FACTOR, HUE_MAX_UPDATE_FACTOR);
  saturation_tracker.setMinMaxUpdateFactor(SAT_MIN_UPDATE_FACTOR, SAT_MAX_UPDATE_FACTOR);

  #if USER_CONTROLS_ACTIVE
  /////////////// User Controls ////////////////////////////////////////////
  uimanager.addBut(BUT1_PIN, BUT1_PULLUP, BUT1_LOW_VAL, BUT1_HIGH_VAL, BUT1_LOW_CHANGES, &but_test[0], BUT1_NAME);
  uimanager.addBut(BUT2_PIN, BUT2_PULLUP, BUT2_LOW_VAL, BUT2_HIGH_VAL, BUT2_LOW_CHANGES, &but_test[1], BUT2_NAME);
  uimanager.addBut(BUT3_PIN, BUT3_PULLUP, BUT3_LOW_VAL, BUT3_HIGH_VAL, BUT3_LOW_CHANGES, &but_test[2], BUT3_NAME);
  uimanager.addBut(BUT4_PIN, BUT4_PULLUP, BUT4_LOW_VAL, BUT4_HIGH_VAL, BUT4_LOW_CHANGES, &but_test[3], BUT4_NAME);
  uimanager.addBut(BUT5_PIN, BUT5_PULLUP, BUT5_LOW_VAL, BUT5_HIGH_VAL, BUT5_LOW_CHANGES, &but_test[4], BUT5_NAME);
  uimanager.addBut(BUT6_PIN, BUT6_PULLUP, BUT6_LOW_VAL, BUT6_HIGH_VAL, BUT6_LOW_CHANGES, &but_test[5], BUT6_NAME);

  uimanager.addPot(POT1_PIN, POT1_REVERSE, POT1_PLAY, &user_brightness_scaler, POT1_NAME);
  uimanager.addPot(POT2_PIN, POT2_REVERSE, POT2_PLAY, &USER_CONTROL_PLAYBACK_GAIN, POT2_NAME);
  uimanager.addPotRange(0, min_user_brightness_scaler, mid_user_brightness_scaler, max_user_brightness_scaler);
  uimanager.addPotRange(1, MIN_PLAYBACK_GAIN, MID_PLAYBACK_GAIN, MAX_PLAYBACK_GAIN);

  uimanager.setup(false);
  uimanager.printAll();
  delay(3000);
  #else
    Serial.println("WARNING - user controls are disabled");
  #endif
  //////////////////// power control ///////////////////////
  pinMode(PWR_KILL_PIN, OUTPUT);
  digitalWrite(PWR_KILL_PIN, HIGH);
}
#endif // _LEAGTUS_BEHAVIOUR_H__