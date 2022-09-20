
void triggerSolenoid(uint8_t num, uint16_t on_time) {
  // if the solenoid in question is not currently active
  if (sol_active[num] == false && last_sol_action[num] > SOL_COOLDOWN) {
    digitalWrite(s_pins[num], HIGH);
    last_sol_action[num] = 0;
    sol_timers[num] = on_time;
    if (on_time < sol_min[num]) {
      on_time = sol_min[num];
    }
    if (on_time > sol_max[num]) {
      on_time = sol_max[num];
    }
    sol_active[num] = true;
    Serial.print("solenoid ");
    Serial.print(num);
    Serial.println(" engaged");
  } else {
    Serial.print("did not trigger solenoid ");
    Serial.print(num);
    Serial.println(" as it is already active");
  }
}


void updateSolenoids() {
  // function which should be in the main loop to constantly turn off active solenoids
  for (int i = 0; i < NUM_SOL_CHANNELS; i++) {
    // if the solenoid is currently enguaged
    if (sol_active[i] == true) {
      // and it has been enguaged for the right period of time
      if (last_sol_action[i] > sol_timers[i]) {
        // turn it off
        digitalWrite(s_pins[i], LOW);
        // note this as an action
        last_sol_action[i] = 0;
        // turn off the "on" timer
        sol_timers[i] = 0;
        // set the solenoid as inactive in the code
        sol_active[i] = false;
        Serial.print("solenoid ");
        Serial.print(i);
        Serial.println(" disenguaged");
      }
    }
  }
}
