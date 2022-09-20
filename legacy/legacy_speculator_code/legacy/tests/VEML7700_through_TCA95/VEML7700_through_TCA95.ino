/*
  TCA9548 I2CScanner.pde -- I2C bus scanner for Arduino

  Based on code c. 2009, Tod E. Kurt, http://todbot.com/blog/

*/
#include "Wire.h"
extern "C" {
#include "utility/twi.h" // from Wire library, so we can do bus sc anning
}
#include "Adafruit_VEML7700.h"
Adafruit_VEML7700 veml1 = Adafruit_VEML7700();
Adafruit_VEML7700 veml2 = Adafruit_VEML7700();

#define TCAADDR 0x70
void tcaselect(uint8_t i) {
  if (i > 7) return;
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

// standard Arduino setup()
void setup()
{
  while (!Serial);
  delay(1000);
  Wire.begin();
  Serial.begin(115200);
  Serial.println("\nTCAScanner ready!");
  for (uint8_t t = 0; t < 8; t++) {
    tcaselect(t);
    Serial.print("TCA Port #"); Serial.println(t);
    for (uint8_t addr = 0; addr <= 127; addr++) {
      if (addr == TCAADDR) continue;
      uint8_t data;
      if (! twi_writeTo(addr, &data, 0, 1, 1)) {
        Serial.print("Found I2C 0x"); Serial.println(addr, HEX);
      }
    }
  }

  tcaselect(0);
  if (!veml1.begin()) {
    Serial.println("Sensor not found");
    while (1);
  }
  Serial.println("VEML #1 Sensor found");
  veml1.setGain(VEML7700_GAIN_1);
  veml1.setIntegrationTime(VEML7700_IT_25MS);

  Serial.print(F("Gain: "));
  switch (veml1.getGain()) {
    case VEML7700_GAIN_1: Serial.println("1"); break;
    case VEML7700_GAIN_2: Serial.println("2"); break;
    case VEML7700_GAIN_1_4: Serial.println("1/4"); break;
    case VEML7700_GAIN_1_8: Serial.println("1/8"); break;
  }

  Serial.print(F("Integration Time (ms): "));
  switch (veml1.getIntegrationTime()) {
    case VEML7700_IT_25MS: Serial.println("25"); break;
    case VEML7700_IT_50MS: Serial.println("50"); break;
    case VEML7700_IT_100MS: Serial.println("100"); break;
    case VEML7700_IT_200MS: Serial.println("200"); break;
    case VEML7700_IT_400MS: Serial.println("400"); break;
    case VEML7700_IT_800MS: Serial.println("800"); break;
  }

  veml1.setPowerSaveMode(VEML7700_POWERSAVE_MODE4);
  veml1.powerSaveEnable(true);

  tcaselect(1);
  if (!veml2.begin()) {
    Serial.println("Sensor not found");
    while (1);
  }
  Serial.println("VEML #2 Sensor found");
  veml2.setGain(VEML7700_GAIN_1);
  veml2.setIntegrationTime(VEML7700_IT_25MS);

  Serial.print(F("Gain: "));
  switch (veml2.getGain()) {
    case VEML7700_GAIN_1: Serial.println("1"); break;
    case VEML7700_GAIN_2: Serial.println("2"); break;
    case VEML7700_GAIN_1_4: Serial.println("1/4"); break;
    case VEML7700_GAIN_1_8: Serial.println("1/8"); break;
  }

  Serial.print(F("Integration Time (ms): "));
  switch (veml2.getIntegrationTime()) {
    case VEML7700_IT_25MS: Serial.println("25"); break;
    case VEML7700_IT_50MS: Serial.println("50"); break;
    case VEML7700_IT_100MS: Serial.println("100"); break;
    case VEML7700_IT_200MS: Serial.println("200"); break;
    case VEML7700_IT_400MS: Serial.println("400"); break;
    case VEML7700_IT_800MS: Serial.println("800"); break;
  }

  veml2.setPowerSaveMode(VEML7700_POWERSAVE_MODE4);
  Serial.println();
  Serial.println("setup done");
  Serial.println();
  veml2.powerSaveEnable(true);
}

double v1_lux, v2_lux;
double v1_white, v2_white;
double v1_als, v2_als;

void loop()
{
  tcaselect(0);
  v1_lux = veml1.readLux();
  v1_white = veml1.readWhite();
  v1_als = veml1.readALS();

  
  tcaselect(1);
  v2_lux = veml2.readLux();
  v2_white = veml2.readWhite();
  v2_als = veml2.readALS();
  
  Serial.print("VEML Lux:\t"); Serial.print(v1_lux); Serial.print(" \t\t "); Serial.println(v2_lux);
  Serial.print("VEML White:\t"); Serial.print(v1_white); Serial.print(" \t\t "); Serial.println(v2_white);
  Serial.print("VEML Raw ALS:\t"); Serial.print(v1_als); Serial.print(" \t "); Serial.println(v2_als);

  Serial.println("---------------------");
  delay(1000);
}
