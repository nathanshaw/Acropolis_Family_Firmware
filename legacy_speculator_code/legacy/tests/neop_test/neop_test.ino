/*
 * This program acts as a sanity sketch for testing the NeoPixels using Paul's NeoP Library
 * 
 * This sketch is assuming that you will be using the Teensy with the NeoP signal connected to pin #5
 * Is also assumes you are using the "Moth" hardware configuration wherein the strip contains 
 * 12x pixels, with the first six pixels on the front of the board and the remaining six pixels on the rear of the board.
*/

#include <WS2812Serial.h>

const int numled = 20;
const int pin = 5;

// Usable pins:
//   Teensy LC:   1, 4, 5, 24
//   Teensy 3.2:  1, 5, 8, 10, 31   (overclock to 120 MHz for pin 8)
//   Teensy 3.5:  1, 5, 8, 10, 26, 32, 33, 48
//   Teensy 3.6:  1, 5, 8, 10, 26, 32, 33

byte drawingMemory[numled*3];         //  3 bytes per LED
DMAMEM byte displayMemory[numled*12]; // 12 bytes per LED

WS2812Serial leds(numled, displayMemory, drawingMemory, pin, WS2812_GRB);

#define RED    0x770000
#define GREEN  0x007700
#define BLUE   0x000077
#define YELLOW 0xFFFF00
#define PINK   0xFF1088
#define ORANGE 0xE05800
#define WHITE  0x777777

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
  leds.begin();
  // leds.setBrightness(20);
  Serial.begin(57600);
}

void loop() {
  // change all the LEDs in 1.5 seconds
  colorWipe(RED, 0);
  delay(600);
  colorWipe(GREEN, 0);
  delay(600);
  colorWipe(BLUE, 0);
  delay(600);
  colorWipe(WHITE, 0);
  delay(600);
}

void colorWipe(int color, int wait) {
  for (int i=0; i < leds.numPixels(); i++) {
    leds.setPixel(i, color);
    leds.show();
    delayMicroseconds(wait);
  } 
}
