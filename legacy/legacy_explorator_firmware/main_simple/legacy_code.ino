//////////////////////////////////////////////////////////////////////////
/////////////////////  LDR's /////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// reading of around 50-60 in the office with normal light
// reading of around 150 when covered
uint8_t ldr_pins[2] = {15, 16};

uint16_t ldr_vals[2];

elapsedMillis last_ldr_reading;

#define LDR_POLL_RATE 30000

void updateLDRs(bool printValues) {
  if (last_ldr_reading > LDR_POLL_RATE) {
    ldr_vals[0] = analogRead(ldr_pins[0]);
    ldr_vals[1] = analogRead(ldr_pins[1]);
    if (printValues) {
      Serial.print("LDR Readings: ");
      Serial.print(ldr_vals[0]);
      Serial.print("\t");
      Serial.println(ldr_vals[1]);
    }
    last_ldr_reading = 0;
  }
}

/*
void testLDRs(uint32_t delay_time) {
  Serial.print("Testing LDR Sensors ");
  calibrateAnalogueLDRs(delay_time);
}
*/

void calibrateAnalogLDRs(uint32_t delay_time) {
  // read temp and humidity 10 times and average the reading over the last 10
  int l1  = 0.0;
  int ll1 = 0.0;
  int l2  = 0.0;
  int ll2 = 0.0;
  delay_time = delay_time / 10;
  Serial.println("\n----- starating ldr calibration -----");
  for (int i = 0; i  < 10; i++) {
    l1 = analogRead(ldr_pins[0]);
    Serial.print("l1: "); Serial.print(l1); Serial.print("\t");
    ll1 += l1;
    l2 = analogRead(ldr_pins[1]);
    ll2 += l2;
    Serial.print("l2: "); Serial.print(l2); Serial.print("\n");
    delay(delay_time);
  }
  ldr_vals[0] = ll1 / 10;
  ldr_vals[1] = ll2 / 10;
  // todo set global brightness values for this
  Serial.print("ldr1_brightness  : ");
  Serial.print(ldr_vals[0]);
  Serial.print("\tldr2_brightness  : ");
  Serial.println(ldr_vals[1]);
  Serial.println("ended LDR calibration");
  Serial.println("-------------------------------");
}
