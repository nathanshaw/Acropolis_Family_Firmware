/*
 * 
 * 
 */

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

AudioInputI2S            i2s1;           //xy=474,316
AudioFilterBiquad        HP_Filter;        //xy=629,307
AudioAnalyzePeak         Peak;          //xy=843,265
AudioAnalyzeRMS          RMS;           //xy=843,297
AudioAnalyzeFFT1024      FFT;      //xy=843,329
AudioConnection          patchCord1(i2s1, 0, HP_Filter, 0);
AudioConnection          patchCord2(HP_Filter, Peak);
AudioConnection          patchCord3(HP_Filter, RMS);
AudioConnection          patchCord4(HP_Filter, FFT);

#include <Wire.h>
#include "SparkFun_VEML6030_Ambient_Light_Sensor.h"

// #define AL1_ADDR 0x48
#define AL2_ADDR 0x10

SparkFun_Ambient_Light light = SparkFun_Ambient_Light(AL2_ADDR);

// Possible values: .125, .25, 1, 2
// Both .125 and .25 should be used in most cases except darker rooms.
// A gain of 2 should only be used if the sensor will be covered by a dark
// glass.
float gain = .125;

// Possible integration times in milliseconds: 800, 400, 200, 100, 50, 25
// Higher times give higher resolutions and should be used in darker light.
int time = 25;
long luxVal = 0;

int s_pins[2] = {6, 16};

void setup() {
  pinMode(6, OUTPUT);
  pinMode(16, OUTPUT);
  digitalWrite(6, LOW);
  digitalWrite(16, LOW);
  Wire.begin();
  Serial.begin(115200);

  if (light.begin()) {
    Serial.println("Ready to sense some light (1)!");
    // Again the gain and integration times determine the resolution of the lux
    // value, and give different ranges of possible light readings. Check out
    // hoookup guide for more info.
    light.setGain(gain);
    light.setIntegTime(time);
    Serial.println("Reading settings...");
    Serial.print("Gain: ");
    float gainVal = light.readGain();
    Serial.print(gainVal, 3);
    Serial.print(" Integration Time: ");
    int timeVal = light.readIntegTime();
    Serial.println(timeVal);
  }
  else {
    Serial.println("Could not communicate with the sensor (1)!");
  }
}

void readLux() {
    luxVal = light.readLight();
  Serial.print("Ambient Light Reading "); Serial.print(" : ");
  Serial.print(luxVal);
  Serial.println(" Lux");
}

void loop() {
readLux();
digitalWrite(16, HIGH);
// digitalWrite(16, HIGH);
delay(20);
digitalWrite(1
6, LOW);
delay(100);

}
