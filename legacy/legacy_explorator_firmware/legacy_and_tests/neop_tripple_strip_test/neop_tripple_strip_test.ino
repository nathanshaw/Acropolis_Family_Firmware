/* WS2812Serial BasicTest Example

   Test LEDs by turning then 7 different colors.

   This example code is in the public domain. */

#include <WS2812Serial.h>

const int numled = 20;
// Usable pins:
//   Teensy LC:   1, 4, 5, 24
//   Teensy 3.2:  1, 5, 8, 10, 31   (overclock to 120 MHz for pin 8)
//   Teensy 3.5:  1, 5, 8, 10, 26, 32, 33, 48
//   Teensy 3.6:  1, 5, 8, 10, 26, 32, 33

byte drawingMemory[numled*3];         //  3 bytes per LED
DMAMEM byte displayMemory[numled*12]; // 12 bytes per LED

WS2812Serial leds5(numled, displayMemory, drawingMemory, 5, WS2812_GRB);
WS2812Serial leds8(numled, displayMemory, drawingMemory, 8, WS2812_GRB);
WS2812Serial leds10(numled, displayMemory, drawingMemory, 10, WS2812_GRB);

#define RED    0xFF0000
#define GREEN  0x00FF00
#define BLUE   0x0000FF
#define YELLOW 0xFFFF00
#define PINK   0xFF1088
#define ORANGE 0xE05800
#define WHITE  0xFFFFFF

// Less intense...
/*
#define RED    0x160000
#define GREEN  0x001600
#define BLUE   0x000016
#define YELLOW 0x101400
#define PINK   0x120009
#define ORANGE 0x100400
#define WHITE  0x101010
*/

void setup() {
  leds5.begin();
  leds8.begin();
  leds10.begin();
}

void loop() {
  // change all the LEDs in 1.5 seconds
  rainbowStrip(leds5);
  rainbowStrip(leds8);
  rainbowStrip(leds10);
}

void rainbowStrip(WS2812Serial &leds) {
  int microsec = 150000 / leds.numPixels();
  colorWipe(leds, RED, microsec);
  colorWipe(leds, GREEN, microsec);
  colorWipe(leds, BLUE, microsec);
  colorWipe(leds, YELLOW, microsec);
  colorWipe(leds, PINK, microsec);
  colorWipe(leds, ORANGE, microsec);
  colorWipe(leds, WHITE, microsec);
}

void colorWipe(WS2812Serial &leds, int color, int wait) {
  for (int i=0; i < leds.numPixels(); i++) {
    leds.setPixel(i, color);
    leds.show();
    delayMicroseconds(wait);
  }
}
