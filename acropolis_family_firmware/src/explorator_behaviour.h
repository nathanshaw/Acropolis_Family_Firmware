
#if ARTEFACT_GENUS == EXPLORATOR 

#if ARTEFACT_SPECIES == EX_WINDER

void windBack(float rotations, int backward_rate)
{
  Serial.println("starting windBack()");
  motors.enableDrivers();
  delay(10);
  enc.write(0);
  enc_pos = 0;
  int target = 20;
  if (M1_POLARITY == false)
  {
    Serial.println("M1_POLARITY is set to false, reversing backward_rate and reversing target");
    backward_rate *= -1;
    // start winding slowly
    for (int i = backward_rate * 0.5; i < backward_rate; i++)
    {
      motors.setM1Speed(i);
      enc_pos = enc.read();
      delay(15);
      updateFeedbackLEDs(&fft_manager[0]);
    }
    motors.setM1Speed(backward_rate);
  }
  else
  {
    for (int i = backward_rate * 0.5; i > backward_rate; i--)
    {
      motors.setM1Speed(i);
      enc_pos = enc.read();
      delay(25);
      updateFeedbackLEDs(&fft_manager[0]);
    }
    motors.setM1Speed(backward_rate);
  }

  // TODO determine how far to wind the box
  last_enc_change = 0;
  while (enc_pos <= (rotations * M1_GEAR_RATIO * COUNTS_PER_REV) && enc_pos >= (rotations * -1 * M1_GEAR_RATIO * COUNTS_PER_REV))
  {
    enc_pos = enc.read();
    if (last_pos != enc_pos)
    {
      Serial.print(last_enc_change);
      Serial.print("\tenc pos: ");
      Serial.print((float)enc_pos / 1500.00);
      Serial.print(" / ");
      Serial.println(rotations);
      last_pos = enc_pos;
      last_enc_change = 0;
    }
    // if 15 seconds pass without being able to get to the target position, then move on to unwinding
    if (last_enc_change > 600)
    {
      Serial.println("Winding taking too, long, breaking from loop");
      motors.setM1Speed(0);
      break;
    }
    else if (last_enc_change > 150)
    {
      motors.setM1Speed(backward_rate * 1.5);
      Serial.print("1.50 - ");
    }
    else if (last_enc_change > 50)
    {
      motors.setM1Speed(backward_rate * 1.25);
      Serial.print("1.25 - ");
    }

    if ((enc_pos >= (rotations * M1_GEAR_RATIO * COUNTS_PER_REV) - 100) && M1_POLARITY == false)
    {
      if (enc_pos >= (rotations * M1_GEAR_RATIO * COUNTS_PER_REV) - 50)
      {
        motors.setM1Speed(10);
        Serial.print((float)enc_pos / 1500.00);
        Serial.println("\t Bringing speed down to 10");
      }
      else
      {
        Serial.print((float)enc_pos / 1500.00);
        Serial.println("Bringing speed down to 20");
        motors.setM1Speed(20);
      }
    }
    else if ((enc_pos <= (rotations * M1_GEAR_RATIO * COUNTS_PER_REV * -1) + 100) && M1_POLARITY == true)
    {
      if (enc_pos <= (rotations * M1_GEAR_RATIO * COUNTS_PER_REV * -1) + 50)
      {
        motors.setM1Speed(-10);
        Serial.print((float)enc_pos / 1500.00);
        Serial.println("Bringing speed down to -10");
      }
      else
      {
        motors.setM1Speed(-20);
        Serial.print((float)enc_pos / 1500.00);
        Serial.println("Bringing speed down to -20");
      }
    }
    updateFeedbackLEDs(&fft_manager[0]);
  }
  last_enc_change = 0;
  Serial.print("enc pos: ");
  Serial.println(enc_pos);
  motors.setM1Speed(0);
  enc.write(0);
  enc_pos = 0;
  Serial.println("----- exiting windBack -------");
}

void windForward(float rotations, int forward_rate)
{
  if (M1_POLARITY == false)
  {
    motors.setM1Speed(forward_rate);
  }
  else
  {
    motors.setM1Speed(forward_rate);
  }

  float EXTRA_REWIND = 0.80;

  while (enc_pos <= (rotations * M1_GEAR_RATIO * COUNTS_PER_REV * EXTRA_REWIND) && enc_pos >= (COUNTS_PER_REV * rotations * -1 * M1_GEAR_RATIO * EXTRA_REWIND))
  {
    enc_pos = enc.read();
    if (last_pos != enc_pos)
    {
      Serial.print("enc pos: ");
      Serial.println((float)enc_pos / 1500.00);
      last_pos = enc_pos;
      last_enc_change = 0;
    }
    updateFeedbackLEDs(&fft_manager[0]);
    if ((enc_pos <= (rotations * M1_GEAR_RATIO * COUNTS_PER_REV * EXTRA_REWIND) - 100) && M1_POLARITY == true)
    {
      motors.setM1Speed(20);
      if (enc_pos <= (rotations * M1_GEAR_RATIO * COUNTS_PER_REV * EXTRA_REWIND) - 50)
      {
        motors.setM1Speed(10);
      }
    }
    else if ((enc_pos >= (rotations * M1_GEAR_RATIO * COUNTS_PER_REV * EXTRA_REWIND * -1) + 100) && M1_POLARITY == false)
    {
      motors.setM1Speed(-20);
      if (enc_pos <= (rotations * M1_GEAR_RATIO * COUNTS_PER_REV * EXTRA_REWIND * -1) + 50)
      {
        motors.setM1Speed(-10);
      }
    }
    updateFeedbackLEDs(&fft_manager[0]);
    if (last_enc_change > 250)
    {
      Serial.println("Unwininding taking too, long, breaking from loop");
      motors.setM1Speed(0);
      break;
    }
  }
  Serial.print("enc pos: ");
  Serial.println((float)enc_pos / 1500.00);
  motors.disableDrivers();
  // (in terms of the encoder)
  last_winding = 0;
}

void windBox(float rotations, int between_delay)
{
  if (last_winding > winding_interval)
  {
    // slowly wind the motor up
    rotations *= 1.0;
    int forward_rate = constrain((float)BASE_FORWARD_RATE * WINDING_RATE, MIN_FORWARD_RATE, MAX_FORWARD_RATE);
    int backward_rate = constrain((float)BASE_BACKWARD_RATE * WINDING_RATE, MIN_BACKWARD_RATE, MAX_BACKWARD_RATE);

    if (M1_POLARITY == true)
    {
      forward_rate *= -1;
      backward_rate *= -1;
    }
    Serial.println("----------------------");
    Serial.print("windBox(rotations): ");
    Serial.println(rotations);
    Serial.print("between_delay: ");
    Serial.println(between_delay);
    Serial.print("forward_rate: ");
    Serial.println(forward_rate);
    Serial.print("backward_rate: ");
    Serial.println(backward_rate);
    Serial.println("----------------------");

    windBack(rotations, backward_rate);

    delay(between_delay);
    //motors.setM1Speed(2);
    windForward(rotations, forward_rate);
  }
  else
  {
    Serial.println("Not winding music box, last winding was too soon");
  }
}

void manualWinding()
{
  // if the wind button is pressed wind the box up
  if (WIND_FORWARD == true)
  {
    Serial.println("WIND_FORWARD is active, winding motor forward...");
    motors.enableDrivers();
    motors.setM1Speed(20);
    while (WIND_FORWARD == true)
    {
      updateFeedbackLEDs(&fft_manager[0]);
      uimanager.update();
      delay(10);
    }
    motors.disableDrivers();
    Serial.println("WIND_FORWARD is no longer active, turning off the motors...");
  }
  // if the unwind button is pressed unwind the box
  else if (WIND_BACKWARD == true)
  {
    Serial.println("WIND_BACKWARD is active, winding motor backward...");
    motors.enableDrivers();
    motors.setM1Speed(-20);
    while (WIND_FORWARD == true)
    {
      updateFeedbackLEDs(&fft_manager[0]);
      uimanager.update();
      delay(10);
    }
    motors.disableDrivers();
    Serial.println("WIND_BACKWARD is no longer active, turning off the motors...");
  }
}

// for the MB Body
void updateBehaviour()
{
  /*
  ///////////////// Passive Visual Feedback ///////////
  if (COLOR_MAP_MODE == COLOR_MAPPING_HSB)
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
  else if (COLOR_MAP_MODE == COLOR_MAPPING_EXPLORATOR)
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
  uimanager.update();

  // the warmer the temperature the more it will actuate? (10 second decrease at 40 degrees and no decrease when at 0 degrees
  // the higher the humidity the less it will actuate? (100 second increase when 100% humidity , 0 second when at 0 %)
  // the brighter it is the more it will actuate (take 5000 lux and subtract the current reading)
  // activity level adds a base of up to five minutes
  ACTUATION_DELAY = ACTIVITY_LEVEL * 1.0 * 250;
  ACTUATION_DELAY += weather_manager.getTemperature() * -250;
  ACTUATION_DELAY += weather_manager.getHumidity() * 1000;
  ACTUATION_DELAY += 5000 - lux_manager.getGlobalLux();
  if (millis() % 30000 == 0)
  {
    Serial.print("ACTUATION_DELAY is : ");
    Serial.println(ACTUATION_DELAY);
  }
  if (last_playback_tmr > ACTUATION_DELAY)
  {
    float wind_amount = 2.0;// how many times will be box be wound?
    int between_time = 500;
    windBox(wind_amount, between_time);
    last_playback_tmr = 0;
  }
  manualWinding();
  */
}

#elif ARTEFACT_SPECIES == EX_SPINNER

int motor_speed = 0;
int target_motor_speed = 0;
int next_motor_speed = 0;
int motor_time = 0;
int next_motor_time = 0;
const int max_motor_speed = 450;
const int min_motor_speed = -450;

void shake(int on_speed, int on_time, int rev_speed, int rev_time)
{
  if (rev_speed > 0)
  {
    rev_speed *= -1;
  }
  target_motor_speed = on_speed;
  next_motor_speed = rev_speed;
  motor_time = on_time;
  next_motor_time = rev_time;

  Serial.print("shaking (onspeed, ontime, revspeed, revtime): ");
  Serial.print(on_speed);
  Serial.print("\t");
  Serial.print(on_time);
  Serial.print("\t");
  Serial.print(rev_speed);
  Serial.print("\t");
  Serial.println(rev_time);

  neos[0].colorWipe(0, 40, 125, 1.0);

  // rev the motor up
  rampSpinnerMotor(0, on_speed, on_time * 0.05);

  // Serial.print("starting / ending pos: ");
  // enc_pos = enc.read();
  // Serial.print(enc_pos);

  // let motor spin, with new color
  neos[0].colorWipe(200, 200, 255, 1.0);
  delay(on_time * 0.95);

  // ramp up motor to it's reverse speed
  neos[0].colorWipe(125, 40, 0, 1.0);
  rampSpinnerMotor(on_speed, rev_speed, rev_time * 0.1);

  neos[0].colorWipe(50, 40, 0, 1.0);
  // let things rotate for a bit
  delay(rev_time * 0.75);
  // rev down to off
  neos[0].colorWipe(25, 20, 0, 1.0);
  rampSpinnerMotor(rev_speed, 0, rev_time * 0.15);

  // enc_pos = enc.read();
  // Serial.print(enc_pos);
  neos[0].colorWipe(0, 0, 0, 1.0);
}

void rampSpinnerMotor(int16_t start, int16_t target, int ramp_total_time)
{
  Serial.print("Ramping Motor (start, target, time) - ");
  Serial.print(start);
  Serial.print("\t");
  Serial.print(target);
  Serial.print("\t");
  Serial.println(ramp_total_time);

  int difference = target - start;
  float step_delay = abs(difference / ramp_total_time) * 1.0;

  Serial.print(" dif: ");
  Serial.print(difference);
  Serial.print(" stepd: ");
  Serial.print(step_delay);
  motors.enableDrivers(0);

  if (difference > 0)
  {
    for (int16_t i = start; i <= target; i++)
    {
      motors.setM1Speed(i);
      // Serial.println(i);
      delayMicroseconds(step_delay);
    }
  }
  else
  {
    for (int16_t i = start; i > target; i--)
    {
      motors.setM1Speed(i);
      delayMicroseconds(step_delay);
      // Serial.println(i);
    }
  }

  if (target == 0)
  {
    motors.setM1Speed(0);
    motors.disableDrivers(0);
  }
  Serial.println("Disabled Drivers");
}

void sustainedShake(int on_speed, int ramp_time, int on_time, int deviation) {
  target_motor_speed = on_speed;
  motor_time = on_time;

  Serial.print("sustained shake (on_speed, ramp_time, on_time): ");
  Serial.print(on_speed);
  Serial.print("\t");
  Serial.print(ramp_time);
  Serial.print("\t");
  Serial.println(on_time);

  // TODO - replace with an update to the visual feedback system
  neos[0].colorWipe(0, 40, 125, 1.0);

  // rev the motor up
  rampSpinnerMotor(0, on_speed, ramp_time * 0.05);

  // Serial.print("starting / ending pos: ");
  // enc_pos = enc.read();
  // Serial.print(enc_pos);

  // let motor spin, with new color
  neos[0].colorWipe(200, 200, 255, 1.0);
  elapsedMillis t;
  while (t < on_time){
    target_motor_speed += map(random(0, deviation), 0, deviation, (-0.5 * deviation), (0.5*deviation));
    motors.setM1Speed(target_motor_speed);
    Serial.print("set motor speed to: ");
    Serial.println(target_motor_speed);
    delay(20 + random(100));
  }

  // ramp up motor to it's reverse speed
  neos[0].colorWipe(125, 40, 0, 1.0);
  rampSpinnerMotor(target_motor_speed, 0, ramp_time * 0.1);
  neos[0].colorWipe(0, 0, 0, 1.0);
  Serial.println("------------ finished with sustained spin ------------------");
}

// for the shaker
void updateBehaviour()
{

  ///////////////// Passive Visual Feedback ///////////
  updateFeedbackLEDs(&fft_manager[dominate_channel]);

  // the warmer the temperature the more it will actuate? (10 second decrease at 40 degrees and no decrease when at 0 degrees
  // the higher the humidity the less it will actuate? (100 second increase when 100% humidity , 0 second when at 0 %)
  // the brighter it is the more it will actuate (take 5000 lux and subtract the current reading)
  // activity level adds a base of up to five minutes
  ACTUATION_DELAY = (ACTIVITY_LEVEL * ACTIVITY_LEVEL * 5 * 60000) + (weather_manager.getTemperature() * -250) + (weather_manager.getHumidity() * 1000) + (5000 - lux_manager.getGlobalLux());
  // uint16_t t = random(45, 150);

  // TODO
  // ACTUATION_DELAY = ACTUATION_DELAY * 0.5;
  ACTUATION_DELAY = 10000;

  if (last_playback_tmr > ACTUATION_DELAY)
  {
    Serial.print("ACTUATION_DELAY is : ");
    Serial.println(ACTUATION_DELAY);

    // on speed, on time, reverse speed, reverse time
    int on_speed = 200;// + (weather_manager.getTemperature() * 10);
    int ramp_time = 200;// + (weather_manager.getTemperature() * 10);
    int on_time = 5000;// + (weather_manager.getTemperature() * 20);
    int deviation = 20;// + (weather_manager.getTemperature() * 10);
    sustainedShake(on_speed, ramp_time, on_time, deviation);
    last_playback_tmr = 0;
  }
}

#elif ARTEFACT_SPECIES == EX_CHIPPER

void updateBehaviour()
{
  ///////////////// Actuator Outputs //////////////////
  playback_engine.update(); // will also update all linked mechanisms

  ///////////////// Passive Visual Feedback ///////////
  updateFeedbackLEDs(&fft_manager[dominate_channel]);

  // the warmer the temperature the more it will actuate? (10 second decrease at 40 degrees and no decrease when at 0 degrees
  // the higher the humidity the less it will actuate? (100 second increase when 100% humidity , 0 second when at 0 %)
  // the brighter it is the more it will actuate (take 5000 lux and subtract the current reading)
  // activity level adds a base of up to five minutes
  // ACTUATION_DELAY = (ACTIVITY_LEVEL * ACTIVITY_LEVEL * 5 * 60000) + (weather_manager.getTemperature() * -250) + (weather_manager.getHumidity() * 1000) + (5000 - lux_manager.getGlobalLux());
  // uint16_t t = random(45, 150);
  // TODO
  ACTUATION_DELAY = 1000;
  if (last_playback_tmr > ACTUATION_DELAY)
  {
    Serial.print("ACTUATION_DELAY is : ");
    Serial.println(ACTUATION_DELAY);
    if (playback_engine.isActive() == false)
    {
      playback_engine.playRhythm(rhythm_bank.getRandomRhythm());
      last_playback_tmr = 0;
    }
    else
    {
      Serial.println("Skipping rhythm as a rhythm is already playing");
    }
  }
}

#elif ARTEFACT_SPECIES == EX_CHIRPER
void updateBehaviour()
{
  ///////////////// Actuator Outputs //////////////////
  updateSolenoids(); // turns off all solenoids which have
  playback_engine.update();
  ACTUATION_DELAY = (30000) + ((weather_manager.getTemperature() + weather_manager.getHumidity()) * lux_manager.getGlobalLux());
  if (last_playback_tmr > ACTUATION_DELAY)
  {
    Serial.print("actuation_delay : ");
    Serial.println(ACTUATION_DELAY);
    Serial.println("playing rhythm through playback_engine");
    playback_engine.playRhythm(rhythm_bank.getRandomRhythm());
    last_playback_tmr = 0;
  }
  updateFeedbackLEDs(&fft_manager[0]);
}

#elif ARTEFACT_SPECIES == EX_CLAPPER

void updateBehaviour()
{
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

  if (COLOR_MAP_MODE == COLOR_MAPPING_HSB)
  {
    double s = calculateSaturation(&feature_collector, &fft_manager[dominate_channel]);
    double b = calculateBrightness(&feature_collector, &fft_manager[dominate_channel]); // user brightness scaler is applied in this function
    double h = calculateHue(&feature_collector, &fft_manager[dominate_channel]);
    printHSB();
    printRGB();

    // if (feature_collector.isActive() == true) {
    b = constrain(b+b, 0, 1.0);
    neos[0].colorWipeHSB(h, s, b); // now colorWipe the LEDs with the HSB value
    // } else {
    // Serial.println("ERROR - not able to updateNeos() as there is no active audio channels");
    // }
  }
  else if (COLOR_MAP_MODE == COLOR_MAPPING_EXPLORATOR)
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

void triggerSolenoid(uint8_t num, uint16_t on_time) {
  // if the solenoid in question is not currently active
  if (sol_active[num] == false) {
    dprint(P_SOLENOID_DEBUG, "WARNING EXITING FROM TIGGER SOLENOID DUE TO INVALID SOLENOID NUMBER OF ");
    dprintln(P_SOLENOID_DEBUG, num);
    return;
  }
  if (sol_state[num] == false && last_sol_action[num] > SOL_COOLDOWN) {
    digitalWrite(s_pins[num], HIGH);
    last_sol_action[num] = 0;
    sol_timers[num] = on_time;
    sol_state[num] = true;
    dprint(P_SOLENOID_DEBUG, millis() / 1000.0);
    dprint(P_SOLENOID_DEBUG, " solenoid ");
    dprint(P_SOLENOID_DEBUG, num);
    dprintln(P_SOLENOID_DEBUG, " enguaged");
  } else if (sol_state[num] == true) {
    dprint(P_SOLENOID_DEBUG, millis() / 1000.0);
    dprint(P_SOLENOID_DEBUG, " did not trigger solenoid ");
    dprint(P_SOLENOID_DEBUG, num);
    dprintln(P_SOLENOID_DEBUG, " as it is already active");
  } else if (last_sol_action[num] < SOL_COOLDOWN) {
    dprintln(P_SOLENOID_DEBUG, "did not trigger solenoids as last_sol_action is less than SOL_COOLDOWN");
  }
}

void updateSolenoids() {
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
        dprint(P_SOLENOID_DEBUG, millis() / 1000.0);
        dprint(P_SOLENOID_DEBUG, " solenoid ");
        dprint(P_SOLENOID_DEBUG, i);
        dprint(P_SOLENOID_DEBUG, " disenguaged after a total of ");
        dprint(P_SOLENOID_DEBUG, last_sol_action[i]);
        dprint(P_SOLENOID_DEBUG, " ms with a target of ");
        dprint(P_SOLENOID_DEBUG, sol_timers[i]);
        dprintln(P_SOLENOID_DEBUG, " ms");
        // note this as an action
        last_sol_action[i] = 0;
        // turn off the "on" timer
        sol_timers[i] = 0;
      }
    }
  }
}

#endif // Explorator genus only 