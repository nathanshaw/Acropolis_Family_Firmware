
const int pwr_int = 2;
const int pwr_kill = 3;

void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  pinMode(pwr_int, INPUT);
  pinMode(pwr_kill, OUTPUT);
  Serial.begin(57600);
  delay(5000);
  Serial.println("setup() done");
}

bool readBut() {
  if (digitalRead(pwr_int) == LOW) {
    Serial.println("POWER BUTTON HAS BEEN PRESSED, ENABLE SHUTDOWN ROUTINES");
    digitalWrite(13, LOW);
    return true;
  }
  return false;
}

void loop() {
  delay(10000);
  digitalWrite(pwr_kill, LOW);
}
