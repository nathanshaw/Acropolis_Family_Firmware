/*
   Test code to determine if the NeoPixels and Teensy Audio Library can work together

  Each side of the Moth Bot should have its corresponding LED's changing brightness and color depending on the overall amplitude levels

  Color of the LEDs should correspond to frequencies (green is low freq while yellow is medium and red is high)
  Brightness should correspond to amplitude of the signal

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

//////////////////////////////////////// NeoPixels //////////////////////////////////////////////////

#include <WS2812Serial.h>
#define USE_WS2812SERIAL
#include <FastLED.h>

// How many leds in your strip?
#define NUM_LEDS 12

// Usable pins:
//   Teensy LC:   1, 4, 5, 24
//   Teensy 3.2:  1, 5, 8, 10, 31   (overclock to 120 MHz for pin 8)
//   Teensy 3.5:  1, 5, 8, 10, 26, 32, 33, 48
//   Teensy 3.6:  1, 5, 8, 10, 26, 32, 33
#define DATA_PIN 5

byte drawingMemory[NUM_LEDS * 3];       //  3 bytes per LED
DMAMEM byte displayMemory[NUM_LEDS * 12]; // 12 bytes per LED

// Define the array of leds
// CRGB leds[NUM_LEDS];
WS2812Serial leds(NUM_LEDS, displayMemory, drawingMemory, DATA_PIN, WS2812_GRB);

// void fadeall() { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(250); } }

void colorWipeAll(int color) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds.setPixel(i, color);
    leds.show();
  }
}

void colorWipeLeft(int color) {
  for (int i = 0; i < 6; i++) {
    leds.setPixel(i, color);
    leds.show();
  }
}

void colorWipeRight(int color) {
  for (int i = 6; i < NUM_LEDS; i++) {
    leds.setPixel(i, color);
    leds.show();
  }
}

//////////////////////////////////////// Audio Library ////////////////////////////////////////////

// GUItool: begin automatically generated code
AudioInputI2S            i2s1;           //xy=169,65
AudioOutputUSB           usb1;           //xy=358.00000762939453,102.00000190734863
AudioAnalyzePeak         peak_L;          //xy=362,38
AudioAnalyzePeak         peak_R;          //xy=366.00000381469727,70.00000095367432
AudioConnection          patchCord1(i2s1, 1, peak_L, 0);
AudioConnection          patchCord2(i2s1, 0, usb1, 0);
AudioConnection          patchCord3(i2s1, 1, usb1, 1);
AudioConnection          patchCord4(i2s1, 0, peak_R, 0);
// GUItool: end automatically generated code

//////////////////////////////////////////////// Setup Loop /////////////////////////////////////////
void setup() {
  delay(2000);
  Serial.begin(57600);
  Serial.println("resetting");
  leds.begin();
  colorWipeAll(0x000000);
  Serial.println("----- Colors Wiped -----");
  delay(1000);
  AudioMemory(32);
}

uint32_t packColor(int r, int g, int b) {
  uint32_t color;
  color = r;
  color = (color << 8) + g;
  color = (color << 8) + b;
  return color;
}


void loop() {
  if (peak_L.available() && peak_R.available()) {
    uint8_t leftPeak = peak_L.read() * 255.0;
    uint8_t rightPeak = peak_R.read() * 255.0;
    if (leftPeak > 30) {
      colorWipeLeft(packColor(0, leftPeak,0));
      Serial.print(leftPeak);
      Serial.print("\t");
      Serial.println(rightPeak);
    } else {
      colorWipeLeft(0x000000);
    }
    if (rightPeak > 30) {
      colorWipeRight(rightPeak);
      Serial.print(leftPeak);
      Serial.print("\t");
      Serial.println(rightPeak);
    } else {
      colorWipeRight(0x000000);
    }
  }
}
