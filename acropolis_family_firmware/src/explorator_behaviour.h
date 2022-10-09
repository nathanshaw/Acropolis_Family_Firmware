#ifndef _EXPLORATOR_BEHAVIOUR_H__
#define _EXPLORATOR_BEHAVIOUR_H__

#include <Macros.h>

bool updatePassiveLEDs(bool _p = false) {
  // only update the LEDs if there is new FFT manager or new feature collector information
  if (feature_collector.update(fft_manager) == false && fft_manager[dominate_channel].update() == false) {
    return false;
  }

    double s = calculateSaturation(&feature_collector, &fft_manager[dominate_channel]);
    double b = calculateBrightness(&feature_collector, &fft_manager[dominate_channel]); // user brightness scaler is applied in this function
    double h = calculateHue(&feature_collector, &fft_manager[dominate_channel]);
    dprint(_p, "HSB after calculate() functions: ");
    dprint(_p, h, 6);
    dprint(_p, "\t");
    dprint(_p, s, 6);
    dprint(_p, "\t");
    dprintln(_p, b, 6);
    printHSB();
    printRGB();

    // these functions will update the value of the HSB according to weather conditions
    // variables are passed by reference to allow for their change without the function returning anything
    if (_p == true) {
      #if WEATHER_MANAGER_ACTIVE
      neos[0].applyWeatherOffsets(weather_manager, h, s, b, true); 
      neos[0].applyWeatherScaling(weather_manager, h, s, b, true);
      #endif
      dprint(_p, "HSB after weather scaling and offsets functions: ");
      dprint(_p, h, 6);
      dprint(_p, "\t");
      dprint(_p, s, 6);
      dprint(_p, "\t");
      dprintln(_p, b, 6);
    }
    else {
      #if WEATHER_MANAGER_ACTIVE
      neos[0].applyWeatherOffsets(weather_manager, h, s, b, false); 
      neos[0].applyWeatherScaling(weather_manager, h, s, b, false);
      #endif
    }
    b = b * user_brightness_scaler;
    dprint(_p, "brightness after user_brightness_scaler: ");
    dprintln(_p, b, 6);

    // the specific mapping strategy is handled by the NeoPixelManager
    double _lux_bs = lux_manager.getBrightnessScaler();

    neos[0].colorWipeHSB(h, s, b, _lux_bs); // now colorWipe the LEDs with the HSB value
    // neos[0].colorWipe(0, 0, 0, 0.0, 0.0); // now colorWipe the LEDs with the HSB value
    // neos[0].colorWipe(255, 255, 255, 1.0, 1.0); // now colorWipe the LEDs with the HSB value
    // make sure we dont update the value tracker until we have our second loop
    #if P_FUNCTION_TIMES
    updateFunctionTimeStats();
    #endif // P_FUNCTION_TIMEs
    
    return true;
}

void triggerSolenoid(uint8_t num, uint16_t on_time, bool _p = false) {
  // if the solenoid in question is not currently active
  if (sol_active[num] == false) {
    dprint(P_SOLENOID_DEBUG || _p, "WARNING EXITING FROM TIGGER SOLENOID DUE TO INVALID SOLENOID NUMBER OF ");
    dprintln(P_SOLENOID_DEBUG || _p, num);
    return;
  }
  if (sol_state[num] == false && last_sol_action[num] > SOL_COOLDOWN) {
    digitalWrite(s_pins[num], HIGH);
    last_sol_action[num] = 0;
    sol_timers[num] = on_time;
    sol_state[num] = true;
    dprint(P_SOLENOID_DEBUG || _p, millis() / 1000.0);
    dprint(P_SOLENOID_DEBUG || _p, " solenoid ");
    dprint(P_SOLENOID_DEBUG || _p, num);
    dprintln(P_SOLENOID_DEBUG || _p, " enguaged");
  } else if (sol_state[num] == true) {
    dprint(P_SOLENOID_DEBUG || _p, millis() / 1000.0);
    dprint(P_SOLENOID_DEBUG || _p, " did not trigger solenoid ");
    dprint(P_SOLENOID_DEBUG || _p, num);
    dprintln(P_SOLENOID_DEBUG || _p, " as it is already active");
  } else if (last_sol_action[num] < SOL_COOLDOWN) {
    dprintln(P_SOLENOID_DEBUG || _p, "did not trigger solenoids as last_sol_action is less than SOL_COOLDOWN");
  }
}

void updateSolenoids(bool _p = false) {
  // function which should be in the main loop to constantly turn off active solenoids
  for (int i = 0; i < NUM_SOLENOIDS; i++) {
    // if the solenoid is currently enguaged
    if (sol_active[i] == true && sol_state[i] == true) {
      // and it has been enguaged for the right period of time
      if (last_sol_action[i] > sol_timers[i]) {
        // turn it off
        digitalWrite(s_pins[i], LOW);
        // set the solenoid as inactive in the code
        sol_state[i] = false;
        dprint(P_SOLENOID_DEBUG || _p, millis() / 1000.0);
        dprint(P_SOLENOID_DEBUG || _p, " solenoid ");
        dprint(P_SOLENOID_DEBUG || _p, i);
        dprint(P_SOLENOID_DEBUG || _p, " disenguaged after a total of ");
        dprint(P_SOLENOID_DEBUG || _p, last_sol_action[i]);
        dprint(P_SOLENOID_DEBUG || _p, " ms with a target of ");
        dprint(P_SOLENOID_DEBUG || _p, sol_timers[i]);
        dprintln(P_SOLENOID_DEBUG || _p, " ms");
        // note this as an action
        last_sol_action[i] = 0;
        // turn off the "on" timer
        sol_timers[i] = 0;
      }
    }
  }
}

void setOutputs()
{
  /////////////// Solenoid Outputs /////////////////////////////////////////
  for (int i = 0; i < NUM_SOLENOIDS; i++)
  {
    pinMode(s_pins[i], OUTPUT);
    digitalWrite(s_pins[i], LOW); // turns them off
  }

#if TEST_SOLENOIDS == true
  Serial.println("TEST_SOLENOIDS is set to true, system will just test solenoids over and over again forever");
  testSolenoids(1000); // let the system settle
  Serial.println("Finished setting solenoid pins to outputs");
#endif

#if (NUM_MOTORS > 0)
  Serial.println("Starting Motor Set-up");
  motors.flipM1(M1_POLARITY);
  motors.flipM2(M2_POLARITY);
  motors.flipM3(M3_POLARITY);
  motors.enableDrivers();
  motors.setM1Speed(0);
  motors.setM2Speed(0);
  motors.setM3Speed(0);
  delay(500);
  Serial.println("Finished setting up motors");
  Serial.println("-------------------------------------------------------------");
  delay(2500); // let the system settle
#endif
}

#if TEST_SOLENOIDS == true
void testSolenoids(unsigned int len)
{
  elapsedMillis t = 0;
  Serial.print("Testing Solenoids - ");
  while (t < len)
  {
    for (int i = 0; i < NUM_SOLENOIDS; i++)
    {
      if (sol_active[i] == true)
      {
        Serial.print(i);
        digitalWrite(s_pins[i], HIGH); // LOW is on for these transistors as a low opens the gate
        delay(30);
        Serial.print(" ");
        digitalWrite(s_pins[i], LOW);
      }
    }
    Serial.println();
  }
  Serial.println("Finished testing solenoids");
  Serial.println("--------------------------");
};
#endif // test_solenoids, ARTEFACT_GENUS explorator
#if ARTEFACT_SPECIES == EX_CHIRPER

/////////////////////////////// Playback Engine  /////////////////////////////////
// the playback engine handles the playback of melodies and rhythms through motors
// and solenoids, as of right now, the only bot which makes use of this is the
// Explorator
Rhythm rhythm[10] = {
  Rhythm(),
  Rhythm(),
  Rhythm(),
  Rhythm(),
  Rhythm(),
  Rhythm(),
  Rhythm(),
  Rhythm(),
  Rhythm(),
  Rhythm()
};

// actuator pin, dampener pin, frequency, on_time
BellMechanism bells[3] = {
  BellMechanism(s_pins[0], s_pins[1], 20, 100.0, 40),
  BellMechanism(s_pins[2], s_pins[3], 20, 500.0, 40),
  BellMechanism(s_pins[4], s_pins[5], 20, 1000.0, 40)
}

RhythmBank rhythm_bank = RhythmBank();
PlaybackEngine playback_engine = PlaybackEngine();

// chirper loop
void updateBehaviour()
{
  ///////////////// Actuator Outputs //////////////////
  updateSolenoids(P_BEHAVIOUR_UPDATE); // turns off all solenoids which have
  playback_engine.update();
  ACTUATION_DELAY = (30000) + ((weather_manager.getTemperature() + weather_manager.getHumidity()) * lux_manager.getGlobalLux());
  if (last_playback_tmr > ACTUATION_DELAY)
  {
    dprint(P_BEHAVIOUR_UPDATE, "actuation_delay : ");
    dprintln(P_BEHAVIOUR_UPDATE, ACTUATION_DELAY);
    dprintln(P_BEHAVIOUR_UPDATE, "playing rhythm through playback_engine");
    playback_engine.playRhythm(rhythm_bank.getRandomRhythm());
    last_playback_tmr = 0;
  }
  neos[0].updateFeedbackLEDs(&fft_manager[0], P_BEHAVIOUR_UPDATE);
}


#elif ARTEFACT_SPECIES == EX_CHIPPER
/////////////////////////////// Playback Engine  /////////////////////////////////
// the playback engine handles the playback of melodies and rhythms through motors
// and solenoids, as of right now, the only bot which makes use of this is the
// Explorator
Rhythm rhythm[MAX_RHYTHMS] = {
  Rhythm(),
  Rhythm(),
  Rhythm(),
  Rhythm(),
  Rhythm()
};
RhythmBank rhythm_bank = RhythmBank();
PlaybackEngine playback_engine = PlaybackEngine();
// actuator pin, reference to motor, motor number, minimum_on_time for the solenoid
// maximum on time for the solenoid, min_time between actuations for the solenoid
WoodpeckerMechanism pecking_mechanism[1] = {WoodpeckerMechanism(s_pins[0], &motors, 0, S1_MIN, S1_MAX, S1_BETWEEN)};

void updateBehaviour()
{
  /* Behaviour update for Explorator Pecker's standard operating behaviour */

  ///////////////// Actuator Outputs //////////////////
  playback_engine.update(); // will also update all linked mechanisms
  ///////////////// Passive Visual Feedback ///////////
  neos[0].updateFeedbackLEDs(&fft_manager[dominate_channel]);
  // the warmer the temperature the more it will actuate? (10 second decrease at 40 degrees and no decrease when at 0 degrees
  // the higher the humidity the less it will actuate? (100 second increase when 100% humidity , 0 second when at 0 %)
  // the brighter it is the more it will actuate (take 5000 lux and subtract the current reading)
  // activity level adds a base of up to five minutes
  ACTUATION_DELAY = (ACTIVITY_LEVEL * ACTIVITY_LEVEL * 0.05 * 60000) + (weather_manager.getTemperature() * -250) + (weather_manager.getHumidity() * 1000) + (5000 - lux_manager.getGlobalLux());
  bool onset_detected = feature_collector.getOnset(fft_manager[dominate_channel]);
  // uint16_t t = random(45, 150);
  // ACTUATION_DELAY = 1000;
  if (last_playback_tmr > ACTUATION_DELAY || onset_detected)
  {
    dprint(P_BEHAVIOUR_UPDATE, "ACTUATION_DELAY is : ");
    dprintln(P_BEHAVIOUR_UPDATE, ACTUATION_DELAY);
    if (playback_engine.isActive() == false)
    {
      playback_engine.playRhythm(rhythm_bank.getRandomRhythm());
      last_playback_tmr = 0;
    }
    else
    {
      dprintln(P_BEHAVIOUR_UPDATE, "Skipping rhythm as a rhythm is already playing");
    }
  }
}

#elif ARTEFACT_SPECIES == EX_WINDER

WindingMechanism music_box = WindingMechanism(MIN_FRONT_SPEED, MIN_REAR_SPEED, MAX_FRONT_SPEED, MAX_REAR_SPEED, true);

// for the MB Body
void updateBehaviour()
{
  updatePassiveLEDs(P_UPDATE_PASSIVE_LEDS);
  ///////////////// Passive Visual Feedback ///////////
  /*
  if (color_map_mode == COLOR_MAPPING_HSB)
  {
    double s = calculateSaturation(&feature_collector, &fft_manager[dominate_channel]);
    double b = calculateBrightness(&feature_collector, &fft_manager[dominate_channel]); // user brightness scaler is applied in this function
    double h = calculateHue(&feature_collector, &fft_manager[dominate_channel]);
    printHSB();
    printRGB();

    // if (feature_collector.isActive() == true) {
    s = constrain(s+s+s, 0, 1.0);
    b = constrain(b*5, 0, 1.0);
    neos[0].colorWipeHSB(h, s, b); // now colorWipe the LEDs with the HSB value
    // } else {
    // Serial.println("ERROR - not able to updateNeos() as there is no active audio channels");
    // }
  }
  else if (color_map_mode == COLOR_MAPPING_EXPLORATOR)
  {
    updateFeedbackLEDs(&fft_manager[dominate_channel]);
    // Serial.println("Finished running updateFeedbackLEDs()");
    // delay(2000);
  }
  else
  {
    neos[0].colorWipeHSB(0, 0, 0); // now colorWipe the LEDs with the HSB value
    Serial.println("ERROR = that color mode is not implemented in update neos");
  }
  */
  // the warmer the temperature the more it will actuate? (10 second decrease at 40 degrees and no decrease when at 0 degrees
  // the higher the humidity the less it will actuate? (100 second increase when 100% humidity , 0 second when at 0 %)
  // the brighter it is the more it will actuate (take 5000 lux and subtract the current reading)
  // activity level adds a base of up to five minutes
  ACTUATION_DELAY = ACTIVITY_LEVEL * 1.0 * 250;
  ACTUATION_DELAY += weather_manager.getTemperature() * -250;
  ACTUATION_DELAY += weather_manager.getHumidity() * 500;
  ACTUATION_DELAY += 5000 - lux_manager.getGlobalLux();
  if (millis() % 3000 == 0)
  {
    dprint(P_BEHAVIOUR_UPDATE, "ACTUATION_DELAY is : ");
    dprintln(P_BEHAVIOUR_UPDATE, ACTUATION_DELAY);
  }
  if (last_playback_tmr > ACTUATION_DELAY)
  {
    float wind_amount = 2.0 * (1.0 - (weather_manager.getScaledHumid()));// how many times will be box be wound?
    int between_time = ACTUATION_DELAY * 0.01;
    music_box.windBox(wind_amount, between_time);
    last_playback_tmr = 0;
  }
  manualWinding(uimanager, false);
}

#elif ARTEFACT_SPECIES == EX_SPINNER
SpinningMechanism spinner = SpinningMechanism(MIN_FRONT_SPEED, MIN_REAR_SPEED, MAX_FRONT_SPEED, MAX_REAR_SPEED, true);
// for the shaker
void updateBehaviour()
{
  ///////////////// Passive Visual Feedback ///////////
  neos[0].updateFeedbackLEDs(&fft_manager[dominate_channel]);
  spinner.update(P_MECHANISMS);
  // the warmer the temperature the more it will actuate? (10 second decrease at 40 degrees and no decrease when at 0 degrees
  // the higher the humidity the less it will actuate? (100 second increase when 100% humidity , 0 second when at 0 %)
  // the brighter it is the more it will actuate (take 5000 lux and subtract the current reading)
  // activity level adds a base of up to five minutes
  ACTUATION_DELAY = (ACTIVITY_LEVEL * ACTIVITY_LEVEL * 1 * 60000) + (weather_manager.getTemperature() * -250) + (weather_manager.getHumidity() * 1000) + (5000 - lux_manager.getGlobalLux());
  // uint16_t t = random(45, 150);
  // TODO
  // ACTUATION_DELAY = ACTUATION_DELAY * 0.5;
  // ACTUATION_DELAY = 10000;

  if (last_playback_tmr > ACTUATION_DELAY)
  {
    Serial.print("ACTUATION_DELAY is : ");
    Serial.println(ACTUATION_DELAY);

    // on speed, on time, reverse speed, reverse time
    int on_speed = 350;// + (weather_manager.getTemperature() * 10);
    unsigned int vocalisation_length = 10000;// + (weather_manager.getTemperature() * 10);
    // how much does the spin vary? 
    int spin_var = (int)weather_manager.getHumidity();
    int spin_var_period = (int)weather_manager.getTemperature();

    spinner.spinMotor(on_speed, vocalisation_length, spin_var, spin_var_period, P_MECHANISMS);
    last_playback_tmr = 0;
  } else if (last_playback_tmr % 10000 == 0) {
    dprint(P_MECHANISMS, last_playback_tmr - ACTUATION_DELAY);
    dprint(P_MECHANISMS, " ms left before next vocalisation");
  }
}

#elif ARTEFACT_SPECIES == EX_CLAPPER

void updateBehaviour()
{
  updateFeedbackLEDs(&fft_manager[dominate_channel]);
  updateSolenoids(); // turns off all solenoids which need to be turned off
  //  listen for onsets
  if (millis() > 50000 && updateOnset())
  {
    Serial.println("Clapping in reaction to onset detection");
    triggerSolenoid(2, random(20, 50));
    triggerSolenoid(7, random(20, 50));
    last_playback_tmr = 0;
  }
  else if (millis() > 50000 && last_playback_tmr > 5000) {
    // if onset detected, immedietally actuate
    // pause audio analysis for x period of time
    Serial.println("Clapping due to 30 seconds since previous vocalisation session");
    triggerSolenoid(2, random(20, 50));
    triggerSolenoid(7, random(20, 50));
    last_playback_tmr = 0;
  }
}
#endif // Clapper Behaviour
/*
void createPeckRhythm(uint8_t num, uint16_t on_time, uint16_t str ) {
  // 30% chance the peck starts with one loud peck (which is a quarter note when compared to the triplets)
  Serial.print("triggerPeck() ");
  Serial.print(num);
  Serial.print("\t");
  Serial.print(on_time);
  Serial.print("\t");
  Serial.print(str);
  Serial.print("\t");
  if (random(0, 100) < 30) {
    Serial.print("quarter\t");
    digitalWrite(s_pins[num], HIGH);
    delay(str * 1.5);
    digitalWrite(s_pins[num], LOW);
    delay((on_time * 1.5) + random(0, on_time * 0.2) - (str * 1.5));
    
  }
  
  digitalWrite(s_pins[num], HIGH);
  delay(str);
  digitalWrite(s_pins[num], LOW);
  Serial.print("triplet ");
  delay(on_time + random(0, on_time * 0.2) - str);
  
  digitalWrite(s_pins[num], HIGH);
  delay(str);
  digitalWrite(s_pins[num], LOW);
  Serial.print("triplet ");  
  delay(on_time + random(0, on_time * 0.2) - str);
  
  digitalWrite(s_pins[num], HIGH);
  delay(str);
  digitalWrite(s_pins[num], LOW);
  Serial.print("triplet\t");
  delay(on_time + random(0, on_time * 0.2) - str);
  
  // 30 % chance that the peck ends with one single long peck
  if (random(0, 100) < 30) {
    digitalWrite(s_pins[num], HIGH);
    delay(str * 2.5);
    digitalWrite(s_pins[num], LOW);
    Serial.print("quarter\t");
    delay((on_time*1.5) + random(0, on_time * 0.2) - (str*2.5));
  }
  
  // 30% chance that the function is called again
  if (random(0, 100) < 80) {
    if (random(0, 100) < 50) {
      delay(on_time * 4);
      Serial.print("Pause(4x)\t");
    }
    if (random(0, 100) < 50) {
      delay(on_time * 4);
      Serial.print("Pause\t");
    }
    Serial.println();
    triggerPeck(num, on_time, str);
  }
  Serial.println();
}
*/

void configurePlaybackEngine()
{
  // freq, length, onset delay (since last note), velocity
#if ARTEFACT_SPECIES == EX_CHIRPER
  rhythm[0].addPitchedNote(500.0, 40, 0, 0.6);
  rhythm[0].addPitchedNote(500.0, 40, 100, 0.8);
  rhythm[0].addPitchedNote(500.0, 30, 150, 1.0);
  rhythm[0].addPitchedNote(1000.0, 30, 400, 1.0);
  rhythm[0].addPitchedNote(1000.0, 20, 1000, 1.0);
  rhythm[0].addPitchedNote(1000.0, 50, 1500, 1.0);

  rhythm[0].addPitchedNote(100.0, 30, 100, 0.3);
  rhythm[0].addPitchedNote(500.0, 40, 600, 0.4);
  rhythm[0].addPitchedNote(1000.0, 30, 150, 0.5);
  rhythm[0].addPitchedNote(100.0, 20, 700, 0.4);
  rhythm[0].addPitchedNote(500.0, 40, 800, 0.3);
  rhythm[0].addPitchedNote(1000.0, 30, 1500, 0.2);

  rhythm[0].addPitchedNote(100.0, 40, 200, 1.0);
  rhythm[0].addPitchedNote(500.0, 30, 70, 1.0);
  rhythm[0].addPitchedNote(1000.0, 40, 90, 1.0);
  rhythm[0].addPitchedNote(100.0, 40, 60, 1.0);
  rhythm[0].addPitchedNote(500.0, 30, 150, 1.0);
  rhythm[0].addPitchedNote(1000.0, 30, 100, 1.0);

  rhythm[1].addPitchedNote(50.0, 50, 0, 1.05);
  rhythm[1].addPitchedNote(150.0, 50, 500, 1.01);
  rhythm[1].addPitchedNote(250.0, 60, 1000, 1.15);
  rhythm[1].addPitchedNote(550.0, 60, 1500, 1.20);
  rhythm[1].addPitchedNote(450.0, 50, 2000, 1.20);
  rhythm[1].addPitchedNote(1550.0, 40, 2500, 1.20);
  rhythm[1].addPitchedNote(1650.0, 30, 3000, 1.20);

  rhythm[2].addPitchedNote(50.0, 40, 0, 1.35);
  rhythm[2].addPitchedNote(150.0, 30, 400, 1.31);
  rhythm[2].addPitchedNote(250.0, 30, 800, 1.35);
  rhythm[2].addPitchedNote(1350.0, 40, 1200, 1.30);
  rhythm[2].addPitchedNote(450.0, 30, 2000, 1.30);
  rhythm[2].addPitchedNote(550.0, 50, 2400, 1.30);
  rhythm[2].addPitchedNote(1650.0, 20, 2600, 1.30);

  rhythm[3].addPitchedNote(150.0, 40, 0, 1.35);
  rhythm[3].addPitchedNote(150.0, 50, 400, 1.31);
  rhythm[3].addPitchedNote(250.0, 60, 600, 1.35);
  rhythm[3].addPitchedNote(1350.0, 40, 700, 1.30);
  rhythm[3].addPitchedNote(450.0, 40, 800, 1.30);
  rhythm[3].addPitchedNote(550.0, 50, 500, 1.30);
  rhythm[3].addPitchedNote(1650.0, 40, 1000, 1.30);

  rhythm_bank.addRhythm(&rhythm[0]);
  rhythm_bank.addRhythm(&rhythm[1]);
  rhythm_bank.addRhythm(&rhythm[2]);
  rhythm_bank.addRhythm(&rhythm[3]);

  playback_engine.linkMechanism(&bells[0]);
  playback_engine.linkMechanism(&bells[1]);
  playback_engine.linkMechanism(&bells[2]);

  // playback_engine.linkNeoGroup(&neos[0]);
  // playback_engine.linkNeoGroup(&neos[1]);
  // playback_engine.linkNeoGroup(&neos[2]);

#elif ARTEFACT_SPECIES == EX_CHIPPER
  for (int i = 0; i < MAX_RHYTHMS; i++)
  {
    uint32_t quarter = random(120, 750);
    playback_engine.buildPeckRhythm(rhythm[i], quarter);
    if (random(0, 100) < 30)
    {
      rhythm[i].addUnpitchedNote(quarter * 4, 0.75);
      playback_engine.buildPeckRhythm(rhythm[i], quarter);
    }
    rhythm[i].addMotorMove(1, 70, 100);
    rhythm[i].print();
    rhythm_bank.addRhythm(&rhythm[i]);
  }
  playback_engine.linkMechanism(&pecking_mechanism[0]);
  // playback_engine.linkNeoGroup(&neos[1]);

#endif // ARTEFACT_SPECIES == EX_CHIRPER
}
void setupSpeciesAudio() {
  Serial.println("Entered into setupSpeciesAudio() for Explorator genus");
  // ||||||||||||||||||||| Audio Routing ||||||||||||||||||||||||||||||
  audio_connections[0] = new AudioConnection(i2s1, 0, mixer1, 0);
  audio_connections[1] = new AudioConnection(i2s1, 1, mixer1, 1);
  // audio_connections[2] = new AudioConnection(mixer1, biquad1);
  // audio_connections[3] = new AudioConnection(biquad1, biquad2);
  // audio_connections[4] = new AudioConnection(biquad2, amp1);
  audio_connections[2] = new AudioConnection(mixer1, fft1);
  audio_connections[3] = new AudioConnection(mixer1, peak1);
  ////////////////////// Audio
  printMinorDivide();
  Serial.println("Finished setting up audio_connections");
  delay(500);
  // TODO make this proper
  uint32_t lpf = 14000;
  uint32_t hpf = 200;
  double q = 0.8;
  amp1.gain(MAKEUP_GAIN);
  mixer1.gain(0, starting_gain);
  mixer1.gain(1, starting_gain);
  // for all species, set up filtering
  biquad1.setLowpass(0, lpf, q);
  biquad1.setLowpass(1, lpf, q);
  biquad2.setHighpass(0, hpf, q);
  biquad2.setHighpass(1, hpf, q);
#if ARTEFACT_SPECIES == EX_CLAPPER
  biquad1.setLowpass(2, lpf, q);
  biquad1.setLowpass(3, lpf, q);
#else  // ARTEFACT_SPECIES != CLAPPER
  biquad1.setHighpass(2, hpf, q);
  biquad1.setHighpass(3, hpf, q);
#endif // ARTEFACT_SPECIES == CLAPPER
  Serial.println("Starting to configurePlaybackEngine()");
  delay(500);
  configurePlaybackEngine();
  // link the fft manager and feature collector
  fft_manager[0].linkFFT(&fft1, "All");
  fft_manager[0].setCalculateCent(true);
  fft_manager[0].setCalculateFlux(true);
  feature_collector.linkPeak(&peak1, P_PEAK_VALS);
  Serial.println("Finished setupSpeciesAudio()");
  delay(500);
}

void setupSpecies() {
#if NUM_SOLENOIDS > 0 || NUM_MOTORS > 0
  setOutputs();
#endif
  printMajorDivide("starting exploratorSetup() loop");

#if ARTEFACT_SPECIES == EX_CHIPPER
  pecking_mechanism[0].setMotorSpeeds(70, -30);
#endif // ARTEFACT_SPECIES == EX_CHIPPER

  /////////////// User Controls ////////////////////////////////////////////
#if USER_CONTROLS_ACTIVE
#if ARTEFACT_SPECIES == EX_CHIPPER || ARTEFACT_SPECIES == EX_CHIRPER
  uimanager.addBut(BUT1_PIN, BUT1_PULLUP, BUT1_LOW_VAL, BUT1_HIGH_VAL, BUT1_LOW_CHANGES, &but_test[0], BUT1_NAME);
  uimanager.addBut(BUT2_PIN, BUT2_PULLUP, BUT2_LOW_VAL, BUT2_HIGH_VAL, BUT2_LOW_CHANGES, &but_test[1], BUT2_NAME);
  uimanager.addBut(BUT3_PIN, BUT3_PULLUP, BUT3_LOW_VAL, BUT3_HIGH_VAL, BUT3_LOW_CHANGES, &but_test[2], BUT3_NAME);
  uimanager.addBut(BUT4_PIN, BUT4_PULLUP, BUT4_LOW_VAL, BUT4_HIGH_VAL, BUT4_LOW_CHANGES, &but_test[3], BUT4_NAME);

  uimanager.addPot(POT1_PIN, POT1_REVERSE, POT1_PLAY, &ACTIVITY_LEVEL, POT1_NAME);
  uimanager.addPot(POT2_PIN, POT2_REVERSE, POT2_PLAY, &STRIKE_LENGTH, POT2_NAME);

  uimanager.setup(false);
  uimanager.printAll();

#elif ARTEFACT_SPECIES == EX_WINDER
  uimanager.addBut(BUT1_PIN, BUT1_PULLUP, BUT1_LOW_VAL, BUT1_HIGH_VAL, BUT1_LOW_CHANGES, &WIND_FORWARD, BUT1_NAME);
  uimanager.addBut(BUT2_PIN, BUT2_PULLUP, BUT2_LOW_VAL, BUT2_HIGH_VAL, BUT2_LOW_CHANGES, &WIND_BACKWARD, BUT2_NAME);

  uimanager.addPot(POT1_PIN, POT1_REVERSE, POT1_PLAY, &ACTIVITY_LEVEL, POT1_NAME);
  uimanager.addPot(POT2_PIN, POT2_REVERSE, POT2_PLAY, &WINDING_RATE, POT2_NAME);
  uimanager.addPotRange(0, MIN_ACTIVITY_LEVEL, MID_ACTIVITY_LEVEL, MAX_ACTIVITY_LEVEL);
  uimanager.addPotRange(1, MIN_WINDING_RATE, MID_WINDING_RATE, MAX_WINDING_RATE);

  uimanager.setup(false);
  // uimanager.printAll();
#endif // UI controls for the woodpecker and bellbot
#endif // USER_CONTROLS_ACTIVE

  // setupSpeciesAudio();

  printMinorDivide();
  Serial.println("Finished setup Loop");
  colorWipeAll(0, 120, 30, 0.25);
  printMinorDivide();
}
#endif // Explorator genus only 