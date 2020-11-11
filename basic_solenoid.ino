#if ARTEFACT_TYPE == EXPLORATOR

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


void triggerSolenoid(uint8_t num, uint16_t on_time) {
  // if the solenoid in question is not currently active
  if (sol_active[num] == false) {
    dprint(P_SOLENOID_DEBUG, "WARNING EXITING FROM TIGGER SOLENOID DUE TO INVALID SOLENOID NUMBER OF ");
    dprintln(P_SOLENOID_DEBUG, num);
    return;
  }
  if (sol_state[num] == false && last_sol_action[num] > SOL_COOLDOWN) {
    digitalWrite(s_pins[num], LOW);
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
    dprint(P_SOLENOID_DEBUG, "did not trigger solenoids as last_sol_action is less than SOL_COOLDOWN");
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
        digitalWrite(s_pins[i], HIGH);
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
*/
#endif // ARTEFACT_TYPE == EXPLORATOR
