/*
 * This program acts as a sanity sketch for testing the NeoPixels using Paul's NeoP Library
 * 
 * This sketch is assuming that you will be using the Teensy with the NeoP signal connected to pin #5
 * Is also assumes you are using the "Moth" hardware configuration wherein the strip contains 
 * 12x pixels, with the first six pixels on the front of the board and the remaining six pixels on the rear of the board.
*/

#include <WS2812Serial.h>

const int numled = 10;
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

uint32_t color = RED;
elapsedMicros function_time;
void setup() {
  leds.begin();
  // leds.setBrightness(20);
  Serial.begin(57600);
  colorWipe(RED, 0);
}

void parseSerial() {
  int d = 0;
  if (Serial.available() > 0) {
    int incByte = Serial.read() - 48;

    if (incByte > -1 && incByte < 10){
      Serial.print("incbyte : ");
      Serial.print(incByte);
      d = incByte;
      incByte = Serial.read() - 48; 

      if (incByte > -1 && incByte < 10){
        d = (d * 10) + incByte;
        Serial.print(" ");
        Serial.println(incByte);
      }
    }
  Serial.print("delay is now : ");
  Serial.println(d);
  function_time = 0;
  colorWipe(0, 0); 
  
  delay(d);
  colorWipe(color, 0);
  Serial.println(function_time);
  delay(1000);
  }
}

void loop() {
  // Display the colours constantly 
  // (when a shdn message is received, the progam shuts down for that long)
  // change all the LEDs in 1.5 seconds
  parseSerial();
}

void colorWipe(int color, int wait) {
  for (int i=0; i < leds.numPixels(); i++) {
    leds.setPixel(i, color);
  } 
  leds.show();
}
