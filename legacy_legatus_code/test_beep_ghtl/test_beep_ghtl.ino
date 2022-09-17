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


AudioSynthWaveform    waveform1;
AudioOutputI2S        i2s1;
AudioConnection       patchCord1(waveform1, 0, i2s1, 0);
AudioConnection       patchCord2(waveform1, 0, i2s1, 1);
AudioControlSGTL5000  codec;
// AudioControlWM8731master    codec;       //xy=347,303

int count=1;

void setup() {
  AudioMemory(10);
  Serial.begin(115200);
  if(!codec.enable()){
    while(!codec.enable()){
      Serial.println("codec not enabled");
      delay(1000);
    }
  }
  codec.volume(0.8);
  waveform1.begin(WAVEFORM_SINE);
  delay(1000);
}


void loop() {
  Serial.print("Beep #");
  Serial.println(count);
  count = count + 1;
  waveform1.frequency(440);
  waveform1.amplitude(0.9);
  delay(250);
  waveform1.amplitude(0);
  delay(1750);
}
