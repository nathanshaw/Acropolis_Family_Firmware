/* Stereo peak meter example, assumes Audio adapter but just uses terminal so no more parts required.

This example code is in the public domain

NOTE the pinout for the Teensy 4.0 and Teensy 3.2 is different for using the I2S microphones

Teensy 3.2
----------
DOUT    13
LRCLK   23
BCLK    9

Teensy 4.0
----------
DOUT    8
LRCLK   20
BCLK    21

Also Note:
----------
Teensyduino 1.47 does not support USB audio for the Teensy 4.0
*/

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

AudioInputI2S        audioInput;         // audio shield: mic or line-in
AudioAnalyzePeak     peak_L;
AudioAnalyzePeak     peak_R;
AudioOutputUSB       usb;

AudioConnection c1(audioInput,0,peak_L,0);
AudioConnection c2(audioInput,1,peak_R,0);
AudioConnection c3(audioInput,0,usb,0);
AudioConnection c4(audioInput,1,usb,1);

void setup() {
  AudioMemory(16);
  Serial.begin(9600);
}

// for best effect make your terminal/monitor a minimum of 62 chars wide and as high as you can.

elapsedMillis fps;
uint8_t cnt=0;

void loop() {
  if(fps > 24) {
    if (peak_L.available() && peak_R.available()) {
      fps=0;
      uint8_t leftPeak=peak_L.read() * 30.0;
      uint8_t rightPeak=peak_R.read() * 30.0;

      for(cnt=0;cnt<30-leftPeak;cnt++) {
        Serial.print(" ");
      }
      while(cnt++<30) {
        Serial.print("<");
      }
      Serial.print("||");
      for(cnt=0;cnt<rightPeak;cnt++) {
        Serial.print(">");
      }
      while(cnt++<30) {
        Serial.print(" ");
      }
      Serial.println();
    }
  }
}
