
int pot_pins[2] = {A3, A7};
int but_pins[6] = {0, 1, 4, 6, 8, 15};

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 2; i++){
    pinMode(pot_pins[i], INPUT);
  }
   for (int i = 0; i < 6; i++){
    pinMode(but_pins[i], INPUT_PULLUP);
  }  
  Serial.println("Ended setup loop");
}

void loop() {
  for (int i = 0; i < 2; i++){
    Serial.print(analogRead(pot_pins[i]));
    Serial.print("\t");
  }
  for (int i = 0; i < 6; i++){
    Serial.print(digitalRead(but_pins[i]));
    Serial.print("\t");
  }
  Serial.println();
  delay(1000);
}
