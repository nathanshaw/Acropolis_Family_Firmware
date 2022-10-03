#ifndef _LEGATUS_BEHAVIOUR_H__
#define _LEGATUS_BEHAVIOUR_H__

#include <Macros.h>

//////////////////////////////////// GLOBAL VARIABLES ///////////////////////////////////////
uint8_t colors[3] = {155, 70, 200};
float fcolors[3];
float mix_gains[3] = {0.0, 0.0, 0.0};
double prior_vocalisation_length = 0;
int current_waveform = WAVEFORM_TRIANGLE;


// state machine to keep track of what Legatus is currently doing
int behaviour_state = S_START_RECORDING;

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
     dprint(_p,F("\t"));
     dprint(_p,colors[1]);
     dprint(_p,F("\t"));
     dprintln(_p,colors[2]);
  }
  if (peak1.available())
  {
    int c = (int)(peak1.read() * 4096) * user_brightness_scaler;
    float f = (float)constrain(c, 0, 255) / 255.0;
    brightnessWipe(f);
    dprintln(_p, f);
    updateFunctionTimeStats();
  }
}

void fadeMixer(AudioMixer4 & _mixer, float c1_current, float c2_current, float c1_target, float c2_target)
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

    _mixer.gain(0, c1_current);
    _mixer.gain(1, c1_current);
    _mixer.gain(2, c2_current);
    feature_collector.update(fft_manager);
    fft_manager[0].update();
    updateLegatusPassiveLEDs2();
    delay(5);
  }
}

long playFile(const char *filename, float rate)
{
  if (last_playback_tmr < PLAYBACK_INTERVAL)
  {
    if (last_playback_tmr % 1000 == 0) {
      dprintln(P_AUDIO_PLAYBACK, F("Too soon for sample playback, "));
      dprint(P_AUDIO_PLAYBACK, (last_playback_tmr - PLAYBACK_INTERVAL) / 1000);
      dprintln(P_AUDIO_PLAYBACK, F(" seconds left until vocalisation unlocked"));
      return 0;
    }
  }
  /* function returns the number of MS it took to play the file, or 0, if the file did not play */
  last_playback_tmr = 0;
  dprintln(P_AUDIO_PLAYBACK, F("---------------------------------"));
  dprint(P_AUDIO_PLAYBACK, F("Playing file: "));
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
  fadeMixer(mixer2, 0.0, 0.5, 0.5, 0.1);
  dprint(P_AUDIO_PLAYBACK, F("file length : "));
  dprintln(P_AUDIO_PLAYBACK, audio_player.lengthMillis());
  // dprint(P_AUDIO_PLAYBACK, F("Playback Rate : ");
  //dprintln(P_AUDIO_PLAYBACK, rate);
  dprint(P_AUDIO_PLAYBACK, F("Amplitude : "));
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
        dprint(P_AUDIO_PLAYBACK, F("Updated the user control playback gain: "));
        dprintln(P_AUDIO_PLAYBACK, USER_CONTROL_PLAYBACK_GAIN);
      }
    }
    #endif // USER_CONTROLS_ACTIVE

    feature_collector.update(fft_manager);
    fft_manager[0].update();
    updateLegatusPassiveLEDs2();
    if (last_playback_tmr % 1000 == 0){
      dprint(P_AUDIO_PLAYBACK, F("playtime : "));
      dprint(P_AUDIO_PLAYBACK, last_playback_tmr/1000);
      dprint(P_AUDIO_PLAYBACK, F(" of "));
      dprintln(P_AUDIO_PLAYBACK, PLAYBACK_INTERVAL/1000);
    }
  }
  // change the mixer levels back so the microphone is dominate
  fadeMixer(mixer2, 0.5, 0.1, 0.0, 0.5);
  dprint(P_AUDIO_PLAYBACK, F("Finished playing b124gack file for "));
  dprint(P_AUDIO_PLAYBACK, last_playback_tmr);
  dprintln(P_AUDIO_PLAYBACK, F(" ms"));
  dprint(P_AUDIO_PLAYBACK, F("Next playback interval is: "));
  dprintln(P_AUDIO_PLAYBACK, PLAYBACK_INTERVAL);
  long _return = last_playback_tmr;
  last_playback_tmr = 0;
  return _return;
}

byte buffer[512];
long calculatePlaybackInterval(bool _p = false) {
  long value;
  // this is the "default period" for sample vocalisation delays
  //             ms     sec  minutes
  int avg_time = 1000 * 60 * 2.0;
  // this is the weathermanager dependent scaling for the average time
  double temp_scaler = weather_manager.getScaledTemp();
  double humid_scaler = weather_manager.getScaledTemp();
  double light_scaler = lux_manager.getScaledLux();
  dprintln(_p, F("-------------------------------------"));
  dprint(_p, F("avg_time: "));
  dprintln(_p, avg_time);
  dprint(_p, F("prior_vocalisation_length: "));
  dprintln(_p, prior_vocalisation_length);
  dprint(_p, F("temp_scaler: "));
  dprintln(_p, temp_scaler);
  dprint(_p, F("humid_scaler: "));
  dprintln(_p, humid_scaler);
  dprint(_p, F("light_scaler: "));
  dprintln(_p, light_scaler);
  dprintln(_p, F(""));
  value = avg_time + prior_vocalisation_length;
  dprint(_p, F("PLAYBACK_INTERVAL = avg_time + prior_vocalisation_length: = "));
  dprintln(_p, PLAYBACK_INTERVAL);
  dprintln(_p, F("PLAYBACK_INTERVAL = PLAYBACK_INTERVAL * (temp_scaler + humid_scaler + light_scaler) / 3 "));
  value = value * (temp_scaler + humid_scaler + light_scaler) / 3;
  dprint(_p, F("PLAYBACK_INTERVAL = "));
  dprint(_p, value/1000);
  dprintln(_p, F(" seconds"));
  dprintln(_p, F("-------------------------------------"));
  return value;
}


#if BEHAVIOUR_ROUTINE == B_LEG_SAMP_PLAYBACK

void updateBehaviour()
{
  feature_collector.update(fft_manager);
  fft_manager[0].update();
  updateLegatusPassiveLEDs2();
  PLAYBACK_INTERVAL = (calculatePlaybackInterval(P_CALCULATE_PLAYBACK_INTERVAL) * 0.2) + prior_vocalisation_length;
  // PLAYBACK_INTERVAL = avg_time - (weather_manager.getTemperature() * (avg_time / 40)) + (weather_manager.getHumidity() * (avg_time / 100)); // uint16_t t = random(45, 150);
  // erm, WTF is happening right now in code, need to make it conform with what is written in thesis...
  // if it has been too short a period since last vocalisation, the function will exit
  if (last_playback_tmr < PLAYBACK_INTERVAL) {
    if (loop_tmr > loop_print_delay)
    {
      Serial.print(F("Awaiting playback : "));
      Serial.println(PLAYBACK_INTERVAL - last_playback_tmr);
      Serial.print(F("PLAYBACK_INTERVAL is : "));
      Serial.println(PLAYBACK_INTERVAL);
      loop_tmr = 0;
    }
    return;
  }
  // TODO - this needs to be calculated from current temperature and humidity values
  // along with the current scaled Peak value
  double _temp = weather_manager.getTemperature(); 
  double _humid = weather_manager.getHumidity();
  double _peak = feature_collector.getDominatePeak();
  double vocalisation_chance = 0.001;
  Serial.println("current temp/humid/peak levels are");
  Serial.print(_temp);
  Serial.print("\t");
  Serial.print(_humid);
  Serial.print("\t");
  Serial.println(_peak);
  Serial.print("resulting vocalisation chance is: ");
  if (_peak > 0.0){
    vocalisation_chance = (_temp + _humid) * _peak;
  } else {
    vocalisation_chance = (_temp + _humid) * 0.000001;
  }
  Serial.print(vocalisation_chance, 6);
  long _rand = random();
  long _v = (long)(vocalisation_chance * 2147483647 * 0.05);
  Serial.print("\t");
  Serial.println(_v);
  if (_rand < _v){
    int file_num = random(0, NUM_AUDIO_FILES);
    // file will only play if it has been long enough since last sample playback
    prior_vocalisation_length = playFile(audio_file_names[file_num], 0.5);
    // digitalWrite(PWR_KILL_PIN, LOW); // kill the power I think...
  } else{
    Serial.print(_rand);
    Serial.print(" is greater than ");
    Serial.print(_v);
    Serial.println(" so not vocalising.");
  }
}
#elif BEHAVIOUR_ROUTINE == B_LEG_ECHO_CHAMBER

int startRecording() {
}

int legatusLoopEchoChamber(int state){
  /* this function executes the logic for the temporally reloacted sonic environments. 
  The function follows the following logic:
  1) record sonic environment constantly
  2) when a triggering sonic event occurs pause and store the recording
  3) wait for a period of time determined by weather conditions
  4) playback the stored audio recording
  5) stop recording, delete recording, begin recording again
    S_START_RECORDING       
    S_CONTINUE_RECORDING    
    S_STOP_RECORDING        
    S_REFLECTING            
    S_START_PLAYBACK        
    S_CONTINUE_PLAYBACK     
    S_END_PLAYBACK          
  */
  // Serial.print("state: ");
  //Serial.println(state);
  switch (state)
  {
  case S_START_RECORDING:
    Serial.println(F("startRecording"));
          feature_collector.update(fft_manager);
          fft_manager[0].update();
          updateLegatusPassiveLEDs2();
    if (SD.exists("RECORD.RAW")) {
      // The SD library writes new data to the end of the
      // file, so to start a new recording, the old file
      // must be deleted before new data is written.
      Serial.println(F("Removing existing file before recording"));
      SD.remove("RECORD.RAW");
    }
          feature_collector.update(fft_manager);
          fft_manager[0].update();
          updateLegatusPassiveLEDs2();
    Serial.println(F("opening new file for writing"));
    audio_file = SD.open("RECORD.RAW", FILE_WRITE);
          feature_collector.update(fft_manager);
          fft_manager[0].update();
          updateLegatusPassiveLEDs2();
    if (audio_file) {
      Serial.println(F("the recording file has been created and the record_queue has started"));
      record_queue.begin();
      elapsedMillis loop_t;
      while(last_state_change <  AUDIO_REC_MAX_LENGTH) {
        if (record_queue.available() >= 2) {
          // Fetch 2 blocks from the audio library and copy
          // into a 512 byte buffer.  The Arduino SD library
          // is most efficient when full 512 byte sector size
          // writes are used.
          AudioNoInterrupts();
          memcpy(buffer, record_queue.readBuffer(), 256);
          record_queue.freeBuffer();
          memcpy(buffer+256, record_queue.readBuffer(), 256);
          record_queue.freeBuffer();
          audio_file.write(buffer, 512);
          AudioInterrupts();
        }
        if (loop_t > 25){
          feature_collector.update(fft_manager);
          fft_manager[0].update();
          updateLegatusPassiveLEDs2();
          loop_t = 0;
        }

    // AudioInterrupts();
      // logic to determine when the recording should stop
      // TODO - add high-level novelty feature to drive this behaviour
    }
    Serial.println("recording length has reached AUDIO_REC_MAX_LENGTH");
    record_queue.end();
      while (record_queue.available() > 0) {
        AudioNoInterrupts();
        audio_file.write((byte*)record_queue.readBuffer(), 256);
        record_queue.freeBuffer();
        AudioInterrupts();
        Serial.print(".");
        feature_collector.update(fft_manager);
        fft_manager[0].update();
        updateLegatusPassiveLEDs2();
      }
      Serial.println(" ");
      Serial.println(F("finished writing from queue to buffer"));
      AudioNoInterrupts();
      audio_file.close();
      AudioInterrupts();
      Serial.println(F("closed the audio file, and changing state to REFLECTING"));
      state = S_REFLECTING;
      last_state_change = 0;
      break;
    }else {
      Serial.println(F("WARNING, there was a problem initalising the recording file"));
      delay(5000);
    }
    // there is no break; here so the logic can continue on to the next case
    case S_STOP_RECORDING:
      // Serial.println("S_STOP_RECORDING");
    case S_REFLECTING:
      if (last_state_change < reflection_time) {
        feature_collector.update(fft_manager);
        fft_manager[0].update();
        updateLegatusPassiveLEDs2();
        Serial.print(".");
        break;
      }
      Serial.println(F("finished reflecting...."));
          feature_collector.update(fft_manager);
          fft_manager[0].update();
          updateLegatusPassiveLEDs2();
      if(audio_player.play("RECORD.RAW")) {
        Serial.println(F("startPlaying"));
        last_state_change = 0;
        state = S_CONTINUE_PLAYBACK;
        while(true){
          uimanager.update();
          amp2.gain(USER_CONTROL_PLAYBACK_GAIN * starting_gain);
          feature_collector.update(fft_manager);
          fft_manager[0].update();
          updateLegatusPassiveLEDs2();
          if (!audio_player.isPlaying()) {
            audio_player.stop();
            last_state_change = 0;
            state = S_START_RECORDING;
            return state;
            break;
          }
        }
      }
      break;
  case S_CONTINUE_PLAYBACK:
    Serial.println("continue playback");
          if (!audio_player.isPlaying()) {
            feature_collector.update(fft_manager);
            fft_manager[0].update();
            updateLegatusPassiveLEDs2();
            audio_player.stop();
            last_state_change = 0;
            state = S_START_RECORDING;
            break;
          }
  break;
  case S_START_LISTENING:
    // nothing currently planned here
    feature_collector.update(fft_manager);
    fft_manager[0].update();
    updateLegatusPassiveLEDs2();
    state = S_START_RECORDING;
    last_state_change = 0;
    break;
  default:
    Serial.println(F("WARNING THIS IS NOT A VALID STATE FOR LEGATUS!"));
    break;
  }
  return state;
}

void updateBehaviour() {
    feature_collector.update(fft_manager);
    fft_manager[0].update();
    updateLegatusPassiveLEDs2(P_UPDATE_LEGATUS_PASSIVE_LEDS);
    behaviour_state = legatusLoopEchoChamber(behaviour_state);
}


#elif BEHAVIOUR_ROUTINE == B_LEG_MATCH_PITCH

void matchPitchVocalisation(float _pitch, float _amp, int _dur, bool _p) {
  /* this function realises vocalisations for Legatus during the
  match pitch behaviour 
  _pitch = target pitch
  amp = maximum amp
  dur = length of vocalisation in ms
  */
  dprintln(_p, "Starting matchPitchVocalisation() with pitch:\t");
  dprint(_p, _pitch);
  dprint(_p, "\t amp:\t");
  dprint(_p, _amp);
  dprint(_p, "\t dur:\t");
  dprintln(_p, _dur);
  elapsedMillis time;
  AudioNoInterrupts();
  waveform.amplitude(_amp);
  waveform.begin(current_waveform);
  AudioInterrupts();
  double a_gain = 0.0;
  while (a_gain < _amp){
    a_gain += 0.001;
    amp3.gain(a_gain);
    delay(5);
    feature_collector.update(fft_manager);
    fft_manager[0].update();
    updateLegatusPassiveLEDs2();
    if (time % 100 == 0){
      dprint(_p, "^");
    }
  }
  while(time < _dur){
    feature_collector.update(fft_manager);
    fft_manager[0].update();
    updateLegatusPassiveLEDs2();
    if (time % 500 == 0){
      dprint(_p, ".");
    }
  }
  while (a_gain > 0.0){
    a_gain -= 0.001;
    amp3.gain(a_gain);
    delay(5);
    feature_collector.update(fft_manager);
    fft_manager[0].update();
    updateLegatusPassiveLEDs2();
    if (time % 100 == 0){
      dprint(_p, ",");
    }
  }
  dprintln(_p,"\n Now stopping the waveform ");
  AudioNoInterrupts();
  waveform.amplitude(0.0);
  AudioInterrupts();
}

void updateBehaviour() {
  // TODO
    feature_collector.update(fft_manager);
    fft_manager[0].update();
  updateLegatusPassiveLEDs2(P_UPDATE_LEGATUS_PASSIVE_LEDS);
  float temp = weather_manager.getTemperature();
  float humid = weather_manager.getHumidity();
  PLAYBACK_INTERVAL = calculatePlaybackInterval();
  // TODO - for testing we are going to set the playback interval to a short number
  // PLAYBACK_INTERVAL = 1000;
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
    // turn on oscillator
    // let it run for a period of time
    // turn off oscillator

    // digitalWrite(PWR_KILL_PIN, LOW); // kill the power I think...
    double _amp = (weather_manager.getScaledHumid() + weather_manager.getScaledTemp()) * 0.5;
    matchPitchVocalisation(humid + temp, _amp, PLAYBACK_INTERVAL * 0.05, P_MATCH_PITCH_VOCALISATION);
    last_playback_tmr = 0;
    Serial.println("last_playback_tmr is reset now to 0");
  }
}
#elif BEHAVIOUR_ROUTINE == B_LEG_FEEDBACK
void activateFeedback(float amp, float dur)
{
  mixer1.gain(0, 0.5);
  mixer1.gain(1, 0.5);
  // amp3 is microphone audio after mixer1
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
  Serial.print(F("activateFeedback(amp, dur) called: "));
  Serial.print(amp);
  Serial.print("\t");
  Serial.println(dur);

  // connect amp to audio output
  for (float i = 0.0; i < amp; i = i + 0.001)
  {
    feature_collector.update(fft_manager);
    fft_manager[0].update();
    updateLegatusPassiveLEDs2();
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
    feature_collector.update(fft_manager);
    fft_manager[0].update();
    updateLegatusPassiveLEDs2(P_UPDATE_LEGATUS_PASSIVE_LEDS);
    delay(10);
  }

  Serial.print("waiting for : ");
  Serial.println(dur);
  delay(dur);

  for (float i = amp; i > 0.0; i = i - 0.001)
  {
    feature_collector.update(fft_manager);
    fft_manager[0].update();
    updateLegatusPassiveLEDs2();
    amp3.gain(i);
    delay(10);
    Serial.print("gain is : ");
    Serial.println(i);
  }
  amp3.gain(0.0);
  mixer1.gain(0, 0.0);
  mixer1.gain(1, 0.0); 
}

void updateBehaviour()
{
    feature_collector.update(fft_manager);
    fft_manager[0].update();
  updateLegatusPassiveLEDs2(P_UPDATE_LEGATUS_PASSIVE_LEDS);
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
#elif BEHAVIOUR_ROUTINE == B_LEG_FM_FEEDBACK

void activateFM(int t, float freq, float amp)
{
  if (amp > 1.0)
  {
    amp = 1.0;
  }
  Serial.print(F("activateFM called (len, freq, amp): "));
  Serial.print(t);
  Serial.print("\t");
  Serial.print(freq);
  Serial.print("\t");
  Serial.println(amp);

  amp2.gain(amp);
  sine_fm.frequency(freq);
  for (float i = 0.0; i < 1.0; i = i + 0.001)
  {
    // make sure we dont pass an amp over 1.0
    if (i > 1.0)
    {
      i = 1.0;
    }
    feature_collector.update(fft_manager);
    fft_manager[0].update();
    updateLegatusPassiveLEDs();
    #if USER_CONTROLS_ACTIVE
    uimanager.update();
    #endif // USER_CONTROLS_ACTIVE
    sine_fm.amplitude(i * USER_CONTROL_PLAYBACK_GAIN);
    delay(5);
  }
  for (float i = 0; i < 20; i++)
  {
    feature_collector.update(fft_manager);
    fft_manager[0].update();
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
      feature_collector.update(fft_manager);
      fft_manager[0].update();
      updateLegatusPassiveLEDs();
      delay(10);
    }
  }
  for (float i = 1.0; i > 0.0; i = i - 0.001)
  {
    feature_collector.update(fft_manager);
    fft_manager[0].update();
    updateLegatusPassiveLEDs();
    #if USER_CONTROLS_ACTIVE
    uimanager.update();
    #endif // USER_CONTROLS_ACTIVE
    sine_fm.amplitude(i * USER_CONTROL_PLAYBACK_GAIN);
    delay(5);
  }
  sine_fm.amplitude(0.0);
  amp2.gain(starting_gain);
}

int avg_time = 1000 * 60 * 2.0;

void updateBehaviour()
{
  #if USER_CONTROLS_ACTIVE
  uimanager.update();
  #endif // USER_CONTROLS_ACTIVE
  feature_collector.update(fft_manager);
  fft_manager[0].update();
  updateLegatusPassiveLEDs();
  float temp = weather_manager.getTemperature();
  float humid = weather_manager.getHumidity();
  // PLAYBACK_INTERVAL = avg_time - (temp * (avg_time / 40)) + (humid * (avg_time / 100)); // uint16_t t = random(45, 150);
  PLAYBACK_INTERVAL = 10000;
  if (loop_tmr > loop_print_delay)
  {
    Serial.println(PLAYBACK_INTERVAL - last_playback_tmr);
    loop_tmr = 0;
  }
  if (last_playback_tmr > PLAYBACK_INTERVAL)
  {
    avg_time = 1000 * 60 * random(1, 4);
    Serial.print(F("PLAYBACK_INTERVAL is : "));
    Serial.println(PLAYBACK_INTERVAL);
    float factor = random(1, 16);
    float base_freq = factor * ((temp * 2) + (humid * 2));
    // float amp = 0.5 + random(0, 1000) / 1000;
    float amp = 0.01;
    activateFM(PLAYBACK_INTERVAL*0.1, base_freq, amp);
    // turn on oscillator
    // let it run for a period of time
    // turn off oscillator

    // digitalWrite(PWR_KILL_PIN, LOW); // kill the power I think...
    last_playback_tmr = 0;
    Serial.println("last_playback_tmr is reset now to 0");
  }
}
#endif

#if SD_PRESENT
void initSD()
{
  SPI.setMOSI(SD_MOSI); // Audio shield has MOSI on pin 7
  SPI.setSCK(SD_CLK);   // Audio shield has SCK on pin 14
  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  if (!card.init(SPI_HALF_SPEED, SD_CS))
  {
    Serial.println(F("initialization failed. Things to check:"));
    Serial.println(F("* is a card inserted?"));
    Serial.println(F("* is your wiring correct?"));
    Serial.println(F("* did you change the chipSelect pin to match your shield or module?"));
    return;
  }
  else
  {
    Serial.println(F("Wiring is correct and a card is present."));
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
    Serial.println(F("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card"));
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

  Serial.println(F("\nFiles found on the card (name, date and size in bytes): "));
  root.openRoot(volume);
  delay(500);

  // list all files in the card with date and size
  root.ls(LS_R | LS_DATE | LS_SIZE);

  // list the length of all audio files
}
#endif // SD card related functions

void setupSpeciesAudio()
{
  // audio_player.setMaxBuffers(16);
  // sgtl5000.setAddress(LOW);
  sgtl5000.enable();
  delay(10);
  sgtl5000.inputSelect(AUDIO_INPUT_MIC);
  #if BEHAVIOUR_ROUTINE == B_LEG_FM_FEEDBACK
  sgtl5000.volume(0.01);// this is playback amplitude
  sgtl5000.micGain(36);
  #elif BEHAVIOUR_ROUTINE == B_LEG_ECHO_CHAMBER
  sgtl5000.volume(0.5);// this is playback amplitude
  sgtl5000.micGain(56);
  #else
  sgtl5000.volume(0.5);// this is playback amplitude
  sgtl5000.micGain(72);
  #endif
  // TODO - not sure what this should be....
  // sgtl5000.micGain(70);

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
#else // SD_PRESENT
  // Serial.println("WARNING SD_PRESENT IS SET TO FALSE!!!!");
  // delay(100);
#endif // SD_PRESENT

  /////////////////////////////////////////////////////////////////////
  biquad1.setHighpass(0, LBQ1_THRESH, LBQ1_Q);
  // HPF1.setHighpass(1, LBQ1_THRESH, LBQ1_Q);
  // HPF1.setHighpass(2, LBQ1_THRESH, LBQ1_Q);
  // HPF1.setLowShelf(3, LBQ1_THRESH, LBQ1_DB);
  Serial.print(F("mic HPF has been configured (thresh/Q/dB): "));
  Serial.print(LBQ1_THRESH);
  Serial.print("\t");
  Serial.print(LBQ1_Q);
  Serial.print("\t");
  Serial.println(LBQ1_DB);

  biquad2.setLowpass(0, LBQ2_THRESH, LBQ2_Q);
  // LPF1.setLowpass(1, LBQ2_THRESH, LBQ2_Q);
  // LPF1.setLowpass(2, LBQ2_THRESH, LBQ2_Q);
  // LPF1.setHighShelf(3, LBQ2_THRESH, LBQ2_DB);
  Serial.print("mic LPF has been configured (thresh/Q/dB): ");
  Serial.print(LBQ2_THRESH);
  Serial.print("\t");
  Serial.print(LBQ2_Q);
  Serial.print("\t");
  Serial.println(LBQ2_DB);

#if BEHAVIOUR_ROUTINE == B_LEG_SAMP_PLAYBACK
  audio_player.begin();
  biquad3.setHighpass(0, RBQ1_THRESH, RBQ1_Q);
  biquad3.setLowpass(1, RBQ2_THRESH, RBQ2_Q);
  // HPF1.setHighpass(2, LBQ1_THRESH, LBQ1_Q);
  // HPF1.setLowShelf(3, LBQ1_THRESH, LBQ1_DB);
  Serial.println("playback left filters have been configured");

  biquad4.setHighpass(0, RBQ1_THRESH, RBQ1_Q);
  biquad4.setLowpass(0, RBQ2_THRESH, RBQ2_Q);
  // LPF1.setLowpass(2, LBQ2_THRESH, LBQ2_Q);
  // LPF1.setHighShelf(3, LBQ2_THRESH, LBQ2_DB);
  Serial.println("playback right Filters have been configured ");

  printMinorDivide();

  //////////////////////////////////////////////////////////////////////////////////
  // amp1.gain(0.0);
  // amp2.gain(0.0);
  Serial.print("playback gains are set to : ");
  Serial.println(USER_CONTROL_PLAYBACK_GAIN);

  //////////////////////////////////////////////////////////////////////////////////
  // TODO - make sure ENC_GAIN_ADJ exists for all bots
  Serial.print("Starting gain is now set to: ");
  Serial.println(starting_gain);
  // mixer1 is taking the gain from the two microphones
  mixer1.gain(0, starting_gain);
  mixer1.gain(1, starting_gain);
  // amp3 links the i2s microphones through the biquad filters
  amp3.gain(1.0);

  // mixer2 allows us to switch between the microphones and 
  // the sample playback from driving the visual feedback system
  mixer2.gain(0, 0.33);
  mixer2.gain(1, 0.33);
  mixer2.gain(2, 0.5);
  // Serial.println("Setting up the FFTManager to track the first channel");
  // fft_manager.addInput(&patchCord_fft_input1);
  // patchCord_fft_input2.disconnect();
#elif BEHAVIOUR_ROUTINE == B_LEG_MATCH_PITCH
    // mixer1 provides the first level gain for the microphones
    mixer1.gain(0, starting_gain);
    mixer1.gain(1, starting_gain);
    AudioNoInterrupts();
    waveform.frequency(40);
    waveform.amplitude(0.0);
    AudioInterrupts();
    // sine_fm.amplitude(0.0);
    // sine_fm.frequency(200);
#elif BEHAVIOUR_ROUTINE == B_LEG_FEEDBACK
    audio_delay1.delay(0, 0);
    mixer1.gain(0, 0.0);
    mixer1.gain(1, 0.0);
#elif BEHAVIOUR_ROUTINE == B_LEG_ECHO_CHAMBER
    // audio_player.begin();
    // amp1 provides gain for the i2s input
    amp1.gain(starting_gain);
    // amp2 is the gain for the audio_playback object
    amp2.gain(starting_gain);
#elif BEHAVIOUR_ROUTINE == B_LEG_FM_FEEDBACK
    mixer1.gain(0, STARTING_GAIN);
    mixer1.gain(1, STARTING_GAIN);
    amp1.gain(MAKEUP_GAIN);
#else
    Serial.println(F("SORRY THIS BEHAVOUR ROUTINE IS NOT SUPPORTED"));
    delay(10000);
#endif// BEHAVIOUR ROUTINE
  }

void setupSpecies() {
    ///////////////////////////// SD Card //////////////////////////////////
#if SD_PRESENT
    neos[0].colorWipe(12, 0, 0, 1.0);
    initSD();
    neos[0].colorWipe(0, 12, 0, 1.0);
#endif
  #if BEHAVIOUR_ROUTINE == B_LEG_SAMP_PLAYBACK
    audio_connections[0] = new AudioConnection(i2s1, 0, mixer1, 0);
    audio_connections[2] = new AudioConnection(i2s1, 1, mixer1, 1);
    audio_connections[3] = new AudioConnection(audio_player, 0, biquad1, 0);
    audio_connections[4] = new AudioConnection(audio_player, 1, biquad2, 0);
    //audio_connections[6] = new AudioConnection(mixer1, biquad3);
    audio_connections[6] = new AudioConnection(mixer1, amp3);

    audio_connections[7] = new AudioConnection(biquad1, amp1);
    audio_connections[8] = new AudioConnection(biquad2, amp2);
    //audio_connections[9] = new AudioConnection(biquad3, biquad4);
    audio_connections[10] = new AudioConnection(amp1, 0, i2s_output, 0);
    audio_connections[11] = new AudioConnection(amp1, 0, mixer2, 0);
    audio_connections[12] = new AudioConnection(amp2, 0, i2s_output, 1);
    audio_connections[13] = new AudioConnection(amp2, 0, mixer2, 1);
    //audio_connections[14] = new AudioConnection(biquad4, amp3);
    audio_connections[15] = new AudioConnection(amp3, 0, mixer2, 2);
    // audio_connections[16] = new AudioConnection(mixer2, rms1);
    audio_connections[17] = new AudioConnection(mixer2, fft1);
    audio_connections[18] = new AudioConnection(mixer2, peak1);


    #if AUDIO_USB
    audio_connections[1] = new AudioConnection(mixer2, 0, output_usb, 0);
    audio_connections[5] = new AudioConnection(mixer1, 0, output_usb, 1);
    #endif // AUDIO_USB
    Serial.println("Finished creating audio connections for B_LEG_SAMP_PLAYBACK");
  #elif BEHAVIOUR_ROUTINE == B_LEG_FEEDBACK
    audio_connections[0] = new AudioConnection(i2s1, 0, mixer1, 0);
    audio_connections[1] = new AudioConnection(i2s1, 1, mixer1, 1);
    audio_connections[3] = new AudioConnection(mixer1, biquad3);
    audio_connections[4] = new AudioConnection(biquad3, biquad4);
    audio_connections[5] = new AudioConnection(biquad4, amp3);
    audio_connections[6] = new AudioConnection(amp3, rms1);
    audio_connections[7] = new AudioConnection(amp3, fft1);
    audio_connections[8] = new AudioConnection(amp3, peak1);
    audio_connections[10] = new AudioConnection(audio_delay1, 0, i2s_output, 0);
    audio_connections[11] = new AudioConnection(audio_delay1, 0, i2s_output, 1);
    audio_connections[12] = new AudioConnection(amp3, 0, audio_delay1, 0);

    #if AUDIO_USB
    audio_connections[2] = new AudioConnection(mixer1, 0, output_usb, 1);
    audio_connections[9] = new AudioConnection(amp3, 0, output_usb, 0);
    #endif // AUDIO_USB
    last_playback_tmr = 99999999;
  #elif BEHAVIOUR_ROUTINE == B_LEG_ECHO_CHAMBER
    // we are only using one microphone, so only the first channel of i2s1 goes to the mixer1
    audio_connections[0] = new AudioConnection(i2s1, 0, amp1, 0);
    audio_connections[1] = new AudioConnection(i2s1, 0, record_queue, 0);
    audio_connections[2] = new AudioConnection(i2s1, 1, record_queue, 1);
    // audio_connections[2] = new AudioConnection(i2s1, 0, record_queue, 1);

    // amp2 is the audio player playback gain
    audio_connections[3] = new AudioConnection(audio_player, 0, amp2, 0);
    // audio_connections[4] = new AudioConnection(audio_player, 1, amp2, 0);
    audio_connections[5] = new AudioConnection(audio_player, 0, amp1, 0);
    //audio_connections[6] = new AudioConnection(audio_player, 1, amp1, 0);

    audio_connections[7] = new AudioConnection(amp2, 0, i2s_output, 0);
    audio_connections[8] = new AudioConnection(amp2, 0, i2s_output, 1);

    //audio_connections[6] = new AudioConnection(mixer1, biquad1);
    //audio_connections[7] = new AudioConnection(biquad1, biquad2);
    //audio_connections[8] = new AudioConnection(biquad2, amp1);
    //audio_connections[9] = new AudioConnection(amp1, rms1);
    audio_connections[9] = new AudioConnection(amp1, fft1);
    audio_connections[10] = new AudioConnection(amp1, peak1);
    #if AUDIO_USB
    audio_connections[14] = new AudioConnection(amp1, 0, output_usb, 0);
    audio_connections[15] = new AudioConnection(amp1, 1, output_usb, 1);
    #endif // AUDIO_USB
  #elif BEHAVIOUR_ROUTINE == B_LEG_FM_FEEDBACK
  
    audio_connections[0] = new AudioConnection(i2s1, 0, mixer1, 0);
    audio_connections[1] = new AudioConnection(i2s1, 1, mixer1, 1);
    audio_connections[3] = new AudioConnection(mixer1, biquad1);
    audio_connections[4] = new AudioConnection(biquad1, biquad2);
    audio_connections[5] = new AudioConnection(biquad2, amp1);
    audio_connections[6] = new AudioConnection(amp1, rms1);
    audio_connections[7] = new AudioConnection(amp1, fft1);
    audio_connections[8] = new AudioConnection(amp1, peak1);
    audio_connections[10] = new AudioConnection(amp1, sine_fm);
    audio_connections[11] = new AudioConnection(sine_fm, 0, amp2, 0);
    audio_connections[12] = new AudioConnection(amp2, 0, i2s_output, 0);
    audio_connections[13] = new AudioConnection(amp2, 0, i2s_output, 1);

    #if AUDIO_USB
    audio_connections[9] = new AudioConnection(amp3, 0, output_usb, 0);
    audio_connections[2] = new AudioConnection(mixer1, 0, output_usb, 1);
    #endif  // AUDIO_USB
  #elif BEHAVIOUR_ROUTINE == B_LEG_MATCH_PITCH
    audio_connections[0] = new AudioConnection(i2s1, 0, mixer1, 0);
    audio_connections[1] = new AudioConnection(i2s1, 1, mixer1, 1);
    //audio_connections[3] = new AudioConnection(mixer1, biquad1);
    //audio_connections[4] = new AudioConnection(biquad1, biquad2);
    //audio_connections[5] = new AudioConnection(biquad2, amp1);
    audio_connections[6] = new AudioConnection(mixer1, rms1);
    audio_connections[7] = new AudioConnection(mixer1, fft1);
    audio_connections[8] = new AudioConnection(mixer1, peak1);
    // amp three is how the vocalisation envelope is controlled
    audio_connections[9] = new AudioConnection(waveform ,amp3);
    audio_connections[10] = new AudioConnection(amp3, 0, i2s_output, 0);
    audio_connections[10] = new AudioConnection(amp3, 0, i2s_output, 1);

    #if AUDIO_USB
    audio_connections[9] = new AudioConnection(amp3, 0, output_usb, 0);
    audio_connections[2] = new AudioConnection(mixer1, 0, output_usb, 1);
    #endif  // AUDIO_USB
  #endif// audio routing 
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
  delay(1000);
  #else
    Serial.println("WARNING - user controls are disabled");
  #endif
  //////////////////// power control ///////////////////////
  pinMode(PWR_KILL_PIN, OUTPUT);
  digitalWrite(PWR_KILL_PIN, HIGH);
}
#endif // _LEAGTUS_BEHAVIOUR_H__