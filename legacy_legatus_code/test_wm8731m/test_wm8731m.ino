// Advanced Microcontroller-based Audio Workshop
//
// http://www.pjrc.com/store/audio_tutorial_kit.html
// https://hackaday.io/project/8292-microcontroller-audio-workshop-had-supercon-2015
//
// Part 1-2: Test Hardware
//
// Simple beeping is pre-loaded on the Teensy, so
// it will create sound and print info to the serial
// monitor when plugged into a PC.
//
// This program is supposed to be pre-loaded before
// the workshop, so Teensy+Audio will beep when
// plugged in.

#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>
#include <Bounce.h>


// GUItool: begin automatically generated code
AudioInputI2S            i2s1;           //xy=337,270
AudioAmplifier           amp2;           //xy=487,288
AudioAmplifier           amp1;           //xy=488,255
AudioOutputI2S           i2s2;           //xy=629,269
AudioConnection          patchCord1(i2s1, 0, amp1, 0);
AudioConnection          patchCord2(i2s1, 1, amp2, 0);
AudioConnection          patchCord3(amp2, 0, i2s2, 1);
AudioConnection          patchCord4(amp1, 0, i2s2, 0);
AudioControlWM8731master wm8731_1;       //xy=347,303
// GUItool: end automatically generated code


int count = 1;
int a1history = 0, a2history = 0, a3history = 0;

void setup() {
  AudioMemory(10);

  Serial.begin(115200);
  delay(5000);
  if (wm8731_1.enable()) {
    Serial.println("GREAT! the wm8731 has been enabled");
  } else {
    while (true) {
      Serial.println("Crap, we were unable to enable the WM8731");
      delay(5000);
    }
  }
  if (wm8731_1.volume(1.0)) {
    Serial.println("GREAT! the wm8731 volume changed to 100%");
  } else {
    while (true) {
      Serial.println("Crap, we were unable to change WM8731 volume");
      delay(5000);
    }
  }
  amp1.gain(10.0);
  amp2.gain(10.0);
delay(1000);
}


void loop() {
  Serial.print("Beep #");
  Serial.println(count);
  count = count + 1;
  delay(1750);
}
