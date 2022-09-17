const int SDI = 11;
const int CLK = 32;
const int DM1 = 10;
const int OE = 12;
byte value = 0;

const int pot_pin = A14;
int pot_val = 800;

void setup() {
  // put your setup code here, to run once:
  pinMode(SDI, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(DM1, OUTPUT);
  pinMode(OE, OUTPUT);
  pinMode(pot_pin, INPUT);
  
  value = 0xff;
  digitalWrite(DM1, LOW); digitalWrite(OE, HIGH);
  shiftOut(SDI, CLK, LSBFIRST, value);
  digitalWrite(DM1, HIGH); digitalWrite(OE, LOW);
  delay(500);
}

void checkPot() {
   pot_val = map(constrain(analogRead(pot_pin), 24, 1000), 24, 1000, 1, 99);
   Serial.println(pot_val);
}

int bright;

void loop() {
  long aTime = millis();
  bright = (bright + 5) % 500;
  Serial.println(bright);
  while (millis() < aTime + 200){
    digitalWrite(OE, HIGH); // high is off for the OE/DM2
    delayMicroseconds(500 - bright);
    digitalWrite(OE, LOW); // low is on for the OE
    delayMicroseconds(bright);
  }
}
