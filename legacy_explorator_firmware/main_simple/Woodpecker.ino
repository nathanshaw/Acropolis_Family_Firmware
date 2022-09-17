
void peck(int which, bool move_table, int number_of_hits, int base_delay, int random_range, float rest_scaler) {
  Serial.println("---------------------------------");
  Serial.print("Pecking #");
  Serial.println(which);
  if (move_table) {
    Serial.println("Moving the table");
#if LEDS_ACTIVE
    neos[which].colorWipe(65, 40, 70);
#endif // LEDS_ACTIVE
    rampMotor(which, 0, 400, 300);
    rampMotor(which, 400, 0, 300);
  }
  // now that moving the table has been delt with, we can start actually hitting the wood.
  for (int i = 0; i < number_of_hits; i++) {
    Serial.print(i);
    int d = base_delay + random(random_range * -0.5, random_range * 0.5);
    if (d > sol_max[which]) {
      d = sol_max[which];
    } else if (d < sol_min[which]) {
      d = sol_min[which];
    }
    Serial.print(" d: ");
    Serial.print(d);
#if LEDS_ACTIVE
    neos[which].colorWipe(26, 12, 30);
#endif //LEDS_ACTIVE
    digitalWrite(s_pins[which * 3], HIGH);
    delay(d);
#if LEDS_ACTIVE
    neos[which].colorWipe(0, 0, 0);
#endif //LEDS_ACTIVE   
    Serial.print(" rest: ");
    Serial.println(d*rest_scaler);
    digitalWrite(s_pins[which * 3], LOW);
    
    if (i == 0) {
      delay(d * rest_scaler);
    }
      delay(d * rest_scaler);
  }
}

/*
  void testPecker() {
  Serial.println("Testing the Woodpecker Arms");
  // move the block to a new spot
  for (int p = 0; p < 2; p++) {
    Serial.print("Testing arm #");
    Serial.println(p);
    neos[p].colorWipe(50, 50, 70);
    motor[p].enableDrivers();
    motor[p].setM1Speed(200 + (p * -420));
    delay(1000);
    motor[p].setM1Speed(0);
    motor[p].disableDrivers();
    neos[p].colorWipe(0, 0, 0);
    Serial.println("Moved the wooden disk");
    delay(2000);
    Serial.println("Starting to Peck");
    for (int i = 80; i > 60; i--) {
      Serial.print(i);
      digitalWrite(s_pins[p * 3], HIGH);
      neos[p].colorWipe(i, i, i);
      delay(i);//  + (random() * 10));
      digitalWrite(s_pins[p * 3], LOW);
      neos[p].colorWipe(0, 0, 0);
      delay(i * 2);//   + (random() * 50));
      Serial.print(", ");
    }
    Serial.println();
    Serial.print("Done testing the Woodpecker Arm ");
    Serial.println(p);
    Serial.println();
  }
  Serial.println("All arm testing is now done");
  }
*/
