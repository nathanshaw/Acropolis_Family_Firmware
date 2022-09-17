
// to keep track of if the different solenoid outputs are active or not
bool sol_active[] = {false, false, false, false, false, false};
unsigned long sol_timers[6];
elapsedMillis last_sol_action[6];
// time in ms in which the solenoids need to cool down between individual actuations
#define SOL_COOLDOWN 10
// the number of active solenoid channels, 4 for boards 1.1 & 1.0 6 for rev 2.0
#define NUM_SOL_CHANNELS 4

void triggerSolenoid(uint8_t num, uint16_t on_time) {
  // if the solenoid in question is not currently active
  if (sol_active[num] == false && last_sol_action[num] > SOL_COOLDOWN) {
    digitalWrite(s_pins[num], HIGH);
    last_sol_action[num] = 0;
    sol_timers[num] = on_time;
    sol_active[num] = true;
    Serial.print("solenoid ");
    Serial.print(num);
    Serial.println(" enguaged");
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
