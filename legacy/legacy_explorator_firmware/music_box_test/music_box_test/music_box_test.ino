/* Test firmware for the Explorator musica

    TODO
    --------
    pins for new PCB

*/
#include "MAX14870Motors/MAX14870Motors.cpp"

#define TEST_MOTOR            true
#define TEST_INTERNAL_LEDS    false
#define TEST_EXTERNAL_LEDS    false
#define TEST_TEMP_HUMID       false
#define TEST_LUX              false
#define TEST_MICROPHONE       false
#define TEST_USER_CONTROLS    true

const int but_pins[2] = {4, 6};
const int pot_pins[2] = {A3, A1};
bool but_vals[2];
int pot_vals[2];
const int enc_pins[2] = {12, 11};

#define GEAR_RATIO            488

#define M1_DIR                20
#define M1_SPEED              21
#define M1_COAST              14
#define M1_FAULT              16

#define M2_DIR                24
#define M2_SPEED              25
#define M2_COAST              26
#define M2_FAULT              27

#define M3_DIR                28
#define M3_SPEED              29
#define M3_COAST              30
#define M3_FAULT              31


MAX14870Motors motor = MAX14870Motors(M1_DIR, M1_SPEED,
                                       M2_DIR, M2_SPEED,
                                       M3_DIR, M3_SPEED,
                                       M1_COAST, M2_COAST, M3_COAST,
                                       M1_FAULT, M2_FAULT, M3_FAULT);

void readUserControls() {
    for (int i  = 0; i < 2; i++) {
      but_vals[i] = digitalRead(but_pins[i]);
      pot_vals[i] = analogRead(pot_pins[i]);
    }
}

void setup() {
  Serial.begin(57600);
  delay(500);

  if (TEST_MOTOR) {
    motor.enableDrivers();
    // TODO rotate once in each direction
  }

  if (TEST_INTERNAL_LEDS) {

  }

  if (TEST_EXTERNAL_LEDS) {

  }

  if (TEST_TEMP_HUMID) {

  }

  if (TEST_LUX) {

  }

  if (TEST_MICROPHONE) {

  }

  /////////////////////////// User Controls /////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////
  pinMode(but_pins[0], INPUT_PULLUP);
  pinMode(but_pins[1], INPUT_PULLUP);
  pinMode(pot_pins[0], INPUT);
  pinMode(pot_pins[1], INPUT);

  if (TEST_USER_CONTROLS) {
    // collect initial reading
    readUserControls();
    // test buttons
    for (int i  = 0; i < 2; i++) {
      Serial.print("Please change state of button ");
      Serial.print(i);
      Serial.println(" to continue");
      while(digitalRead(but_pins[i]) == but_vals[i])){
        delay(10);
      }
      Serial.println("great, moving on\n");
    }
    // TODO - have test for pots too

    // TODO - update current values
    readUserControls();
  }

Serial.println("Setup Loop Finished");
  delay(500);
}

void loop() {
  Serial.println("TODO");
  delay(1000);
}
