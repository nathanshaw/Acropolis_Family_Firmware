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

elapsedMillis fps;
uint8_t cnt = 0;

void printPeaks() {
  if (peak_L.available() && peak_R.available()) {
    fps = 0;
    uint8_t leftPeak = peak_L.read() * 30.0;
    uint8_t rightPeak = peak_R.read() * 30.0;

    for (cnt = 0; cnt < 30 - leftPeak; cnt++) {
      Serial.print(" ");
    }
    while (cnt++ < 30) {
      Serial.print("<");
    }
    Serial.print("||");
    for (cnt = 0; cnt < rightPeak; cnt++) {
      Serial.print(">");
    }
    while (cnt++ < 30) {
      Serial.print(" ");
    }
    Serial.println();
  }
}

//////////////////////////////////////// VEML LUX Sensor //////////////////////////////////////////
#include "Adafruit_I2CDevice.h"
#include "Adafruit_VEML7700.h"

Adafruit_VEML7700 veml = Adafruit_VEML7700();

void setupVEML() {
  Serial.println("Setting up VEML7700");

  if (!veml.begin()) {
    Serial.println("VEML sensor not found");
    return;
  }
  Serial.println("Sensor found");

  veml.setGain(VEML7700_GAIN_1);
  veml.setIntegrationTime(VEML7700_IT_800MS);

  Serial.print(F("Gain: "));
  switch (veml.getGain()) {
    case VEML7700_GAIN_1: Serial.println("1"); break;
    case VEML7700_GAIN_2: Serial.println("2"); break;
    case VEML7700_GAIN_1_4: Serial.println("1/4"); break;
    case VEML7700_GAIN_1_8: Serial.println("1/8"); break;
  }

  Serial.print(F("Integration Time (ms): "));
  switch (veml.getIntegrationTime()) {
    case VEML7700_IT_25MS: Serial.println("25"); break;
    case VEML7700_IT_50MS: Serial.println("50"); break;
    case VEML7700_IT_100MS: Serial.println("100"); break;
    case VEML7700_IT_200MS: Serial.println("200"); break;
    case VEML7700_IT_400MS: Serial.println("400"); break;
    case VEML7700_IT_800MS: Serial.println("800"); break;
  }

  //veml.powerSaveEnable(true);
  //veml.setPowerSaveMode(VEML7700_POWERSAVE_MODE4);

  veml.setLowThreshold(10000);
  veml.setHighThreshold(20000);
  Serial.println(F("Finished Setting Up VEML7700"));
  Serial.println(F("------------------------------"));
}


void readVEML() {
  Serial.print("Lux: "); Serial.println(veml.readLux());
  Serial.print("White: "); Serial.println(veml.readWhite());
  Serial.print("Raw ALS: "); Serial.println(veml.readALS());

  uint16_t irq = veml.interruptStatus();
  if (irq & VEML7700_INTERRUPT_LOW) {
    Serial.println("** Low threshold");
  }
  if (irq & VEML7700_INTERRUPT_HIGH) {
    Serial.println("** High threshold");
  }
}

/////////////////////////////////////////////// TCA I2C Multiplexer ////////////////////////////////////////////

#include "ClosedCube_TCA9538.h"
// tca9543A
#define TCA_INPUT_PIN  7
#define TCA_OUTPUT_PIN  13
ClosedCube_TCA9538 tca9538;
TCA9538_Registers regs;
#define TCA9538_ADDRESS 0x70

void setupTCA() {
  pinMode(TCA_OUTPUT_PIN, OUTPUT);
  pinMode(TCA_INPUT_PIN, INPUT);

  tca9538.begin(TCA9538_ADDRESS);

  regs.output.rawData = 0xFF;
  regs.polarity.rawData = 0x00;
  regs.input.rawData = 0x00;
  regs.config.rawData = 0xFF;

  regs.config.ports.P7 = 0;

  tca9538.init(regs);
}

void readTCA() {
  val = ~val;

  digitalWrite(OUTPUT_PIN, val); // Arduino Output pin

  printHeader();

  printHighOrLow(val);
  Serial.print("\t\t");

  TCA9538_Input input = tca9538.readInput(); // TCA9538 Input P0 pin
  printHighOrLow(input.ports.P0);
  Serial.print("\t\t\t");

  regs.output.ports.P7 = ~regs.output.ports.P7; // TCA9538 Output P7 pin
  tca9538.writeOutput(regs.output);
  printHighOrLow(tca9538.readOutput().ports.P7);
  Serial.print("\t\t");

  printHighOrLow(digitalRead(INPUT_PIN)); // Arduino Input pin
  Serial.println();

  Serial.println();
}

//////////////////////////////////////////////// Setup Loop /////////////////////////////////////////
void setup() {
  delay(2000);
  Serial.begin(57600);
  setupVEML();
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
      colorWipeLeft(packColor(0, leftPeak, 0));
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
