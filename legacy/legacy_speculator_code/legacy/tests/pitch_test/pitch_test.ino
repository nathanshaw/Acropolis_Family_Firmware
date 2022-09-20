
// lower pitches will be blue
// followed by purple
// green
// then yellow
// orange
// red
// white will be noise

// the brightness will be mapped by taking the rms or peak of the overall signal

#include "ModePitch.h"

void setup() {
  Serial.begin(57600);
  delay(3000);
  printMajorDivide(" Starting Setup Loop ");
  Serial.println("Started Serial");
  AudioMemory(30);
  mothSetup();
}

void loop() {
  mothLoop();
}
