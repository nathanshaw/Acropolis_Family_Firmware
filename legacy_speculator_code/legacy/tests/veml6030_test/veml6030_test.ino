/*
  This example code will walk you through how to read ambient light values.
  Chances are good that you'll use this sensor in various environments so it'll
  also walk you through setting the gain and integration time that allow for
  different ranges of lux values. For example using the default gain of 100ms
  gives you a maximum reading of 30,199 Lux. This is great for daylight
  readings but not DIRECT sun. Higher integration times mean higher
  resoultions but lower lux values and vice versa: the lowest integration time
  and lowest gain should be used for mid day direct light. Check our hookup
  guide for more information.

  SparkFun Electronics
  Author: Elias Santistevan
  Date: July 2019

	License: This code is public domain but if you use this and we meet someday, get me a beer!

	Feel like supporting our work? Buy a board from Sparkfun!
	https://www.sparkfun.com/products/15436

*/

#include <Wire.h>
#include "SparkFun_VEML6030_Ambient_Light_Sensor.h"

#define AL1_ADDR 0x48
#define AL2_ADDR 0x10

SparkFun_Ambient_Light lights[2] = {
  SparkFun_Ambient_Light(AL1_ADDR),
  SparkFun_Ambient_Light(AL2_ADDR)
};

// Possible values: .125, .25, 1, 2
// Both .125 and .25 should be used in most cases except darker rooms.
// A gain of 2 should only be used if the sensor will be covered by a dark
// glass.
float gain = .125;

// Possible integration times in milliseconds: 800, 400, 200, 100, 50, 25
// Higher times give higher resolutions and should be used in darker light.
int time = 25;
long luxVal = 0;

void setup() {

  Wire.begin();
  Serial.begin(115200);
  delay(5000);
  if (lights[0].begin()) {
    Serial.println("Ready to sense some light (1)!");
    // Again the gain and integration times determine the resolution of the lux
    // value, and give different ranges of possible light readings. Check out
    // hoookup guide for more info.
    lights[0].setGain(gain);
    lights[0].setIntegTime(time);
    Serial.println("Reading settings...");
    Serial.print("Gain: ");
    float gainVal = lights[0].readGain();
    Serial.print(gainVal, 3);
    Serial.print(" Integration Time: ");
    int timeVal = lights[0].readIntegTime();
    Serial.println(timeVal);
  }
  else {
 
    Serial.println("Could not communicate with the sensor (1)!");
  }
  if (lights[1].begin()) {
    Serial.println("Ready to sense some light (2)!");
    lights[1].setGain(gain);
    lights[1].setIntegTime(time);
    Serial.println("Reading settings...");
    Serial.print("Gain: ");
    float gainVal = lights[1].readGain();
    Serial.print(gainVal, 3);
    Serial.print(" Integration Time: ");
    int timeVal = lights[1].readIntegTime();
    Serial.println(timeVal);
  }
  else
  {
    Serial.println("Could not communicate with the sensor (2)!");
  }
}

void loop() {
  for (int i = 0; i < 2; i++) {
    luxVal = lights[i].readLight();
    Serial.print("Ambient Light Reading "); Serial.print(i); Serial.print(" : ");
    Serial.print(luxVal);
    Serial.println(" Lux");
  }
  delay(1000);
}
