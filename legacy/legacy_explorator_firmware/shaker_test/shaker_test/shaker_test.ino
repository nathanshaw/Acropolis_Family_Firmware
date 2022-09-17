// user controls
// microphone
// lux sensor
// temperature and humidity sensor
// shake
// LEDs (sensor stock) ()

#include <WS2812Serial.h>
#include <Wire.h>
#include "SparkFun_VEML6030_Ambient_Light_Sensor.h"
#include "SHTSensor.h"
#include "MAX14870Motors.h"
#include "Encoder.h"
#include <Audio.h>
#include <SPI.h>
#include <SerialFlash.h>

#define P_COLOR true

///////////// Audio //////////////
AudioInputI2S            i2s1;           //xy=474,316
AudioFilterBiquad        HP_Filter;        //xy=629,307
AudioAnalyzePeak         Peak;          //xy=843,265
AudioAnalyzeRMS          RMS;           //xy=843,297
AudioAnalyzeFFT1024      FFT;      //xy=843,329
AudioConnection          patchCord1(i2s1, 0, HP_Filter, 0);
AudioConnection          patchCord2(HP_Filter, Peak);
AudioConnection          patchCord3(HP_Filter, RMS);
AudioConnection          patchCord4(HP_Filter, FFT);

// staging strikes

bool shake_staged = false;

// User Controls ////////////////

const int but_pins[2] = {17, 33};
bool but_vals[2] = {false, false};

////////// LEDs //////////////////////////
const int num_led = 10;
const int led_pin = 5;
byte drawing_memory[num_led * 3];       //  3 bytes per LED
DMAMEM byte display_memory[num_led * 12]; // 12 bytes per LED

WS2812Serial leds(num_led, display_memory, drawing_memory, led_pin, WS2812_GRB);

#define RED    0x770000
#define GREEN  0x007700
#define BLUE   0x000077
#define YELLOW 0xFFFF00
#define PINK   0xFF1088
#define ORANGE 0xE05800
#define WHITE  0x777777
#define BLACK  0x000000

///////////// Light Sensor ///////////////
#define AL1_ADDR 0x48
#define AL2_ADDR 0x10

SparkFun_Ambient_Light lux_sensor = SparkFun_Ambient_Light(AL2_ADDR);
float lux_gain = 0.125;
int int_time = 25;
double lux_val = 0.0;

/*
   Behaviour -

   attempt a BPM detection and then synchronize its own actions withh those movements
*/

///////////// Temp / Humid Sensor ////////

SHTSensor sht;
float humid_val = 0.0;
float temp_val = 0.0;

///////////// Motor Stuff //////////
Encoder enc(20, 21);
long enc_position = 0;

#define GEAR_RATIO            488

#define M1_DIR                26
#define M1_SPEED              25
#define M1_COAST              28
#define M1_FAULT              27

#define M2_DIR                29
#define M2_SPEED              32
#define M2_COAST              31
#define M2_FAULT              30

MAX14870Motors motor = MAX14870Motors(M1_DIR, M1_SPEED, M1_COAST, M1_FAULT,
                                      M2_DIR, M2_SPEED, M2_COAST, M2_FAULT,
                                      M2_DIR, M2_SPEED, M2_COAST, M2_FAULT);

void testLEDs() {
  colorWipe(RED, 0);
  delay(600);
  colorWipe(GREEN, 0);
  delay(600);
  colorWipe(BLUE, 0);
  delay(600);
  colorWipe(WHITE, 0);
  delay(600);
  colorWipe(BLACK, 0);
}

void colorWipe(int color, int wait) {
  for (int i = 0; i < leds.numPixels(); i++) {
    leds.setPixel(i, color);
    leds.show();
    delayMicroseconds(wait);
  }
}

void readTempHumid() {
  if (sht.readSample()) {
    Serial.print("SHT:\n");
    Serial.print("  RH: ");
    humid_val = sht.getHumidity();
    Serial.print(humid_val, 2);
    Serial.print("\n");
    Serial.print("  T:  ");
    temp_val = sht.getTemperature();
    Serial.print(temp_val, 2);
    Serial.print("\n");
  } else {
    Serial.print("Error in readSample()\n");
  }
}

void readButtons() {
  for (int i = 0; i < 2; i++) {
    if (digitalRead(but_pins[i]) != but_vals[i]) {
      but_vals[i] = !but_vals[i];
      Serial.print("but val ");
      Serial.print(i);
      Serial.print(" changed to ");
      Serial.println(but_vals[i]);
      // the first switch will trigger a shaking event
      if (i == 0 && but_vals[i] == false) {
        shake_staged = true;
      }
    }
    Serial.println();
  }
}

void setup() {
  motor.disableDrivers();
  motor.setM1Speed(0);
  Serial.begin(57600);
  delay(1000);

  // Motor ////////////////
  Serial.println("Testing the Motor");
  motor.enableDrivers();
  motor.setM1Speed(450);
  delay(500);
  motor.setM1Speed(0);
  motor.disableDrivers();
  Serial.println("Motor testing complete");
  delay(1000);

  // User Controls //////
  pinMode(but_pins[0], INPUT_PULLUP);
  pinMode(but_pins[1], INPUT_PULLUP);
  readButtons();

  // LEDs ///////////////
  Serial.println("configuring LEDs");
  leds.begin();
  leds.setBrightness(100);
  testLEDs();
  Serial.println("finished configuring LEDs");

  // Temp / Humid Sensor ////
  Wire.begin();
  if (sht.init()) {
    Serial.println("Temperature and Humidity Sensor initalized");
    readTempHumid();
    delay(1000);
  } else {
    Serial.println("Temperature and Humidity Sensor failed to initalize");
  }

  // Lux Sensor /////////
  Serial.println("configuring lux sensor");
  delay(500);
  if (lux_sensor.begin()) {
    Serial.println("lux sensor began, now configuring");
    delay(500);
    lux_sensor.setGain(lux_gain);
    lux_sensor.setIntegTime(int_time);
    Serial.print("gain set to             : ");
    Serial.println(lux_sensor.readGain());
    Serial.print("integration time set to : ");
    Serial.println(lux_sensor.readIntegTime());
    Serial.print("current lux reading     : ");
    lux_val = lux_sensor.readLight();
    Serial.println(lux_val);
    Serial.println("finished configuring lux sensor");
  } else {
    Serial.println("ERROR, lux sensor is not active!!!");
    delay(10000);
  }

}

void shake(int on_speed, int on_time, int rev_speed, int rev_time) {
  Serial.print("shaking with the following parameters: ");
  Serial.print(on_speed);
  Serial.print("\t");
  Serial.print(on_time);
  Serial.print("\t");
  Serial.print(rev_speed);
  Serial.print("\t");
  Serial.println(rev_time);
  colorWipe(GREEN, 0);
  motor.enableDrivers();
  motor.setM1Speed(on_speed);
  Serial.print("starting / ending pos: ");
  enc_position = enc.read();
  Serial.print(enc_position);
  delay(on_time);
  colorWipe(RED, 0);
  if (rev_speed > 0) {
    rev_speed *= -1;
  }
  motor.setM1Speed(rev_speed);
  delay(rev_time);
  motor.setM1Speed(0);
  motor.disableDrivers();
  enc_position = enc.read();
  Serial.print(enc_position);
  colorWipe(BLACK, 0);
}

void readLux(bool printme) {
  lux_val = lux_sensor.readLight();
  if (printme) {
    Serial.print("lux value updated to : ");
    Serial.println(lux_val);
  }
}

void shakeEvent(int num) {
  for (int i = 0; i < num;  i++) {
    shake(450 - random(0, 150), 30 + random(0,  40), 200, 20);
    delay(random(10, 100));
  }
}

elapsedmillis last_led_update_tmr;
long led_refresh_rate = 10;

void updateFeedbackLEDs(FFTManager1024 *_fft_manager) {
  // the brightness of the LEDs should mirror the peak gathered from the environment
  // a local min/max which scales periodically should be implemented just like with the Moth
  // a MAX_RMS brightness should be used to determine what the max brightness of the feedback is
  // the LEDs should be updated 30x a second

  // calculate the target color ///////////////////////////////////////
  if (last_led_update_tmr > led_refresh_rate) {
    double target_color = 0.0;
    double target_brightness = 0.0;
    uint8_t red, green, blue;

    if (COLOR_FEATURE == SPECTRAL_CENTROID) {
      target_color = calculateColorFromCentroid(_fft_manager);
      dprint(P_COLOR, "target color: ");
      dprintln(P_COLOR, target_color);
      last_color = current_color;
      current_color = (target_color * 0.2) + (last_color * 0.8);// * COLOR_LP_LEVEL);

      // calculate the preliminary rgb values /////////////////////////////
      red = ((1.0 - current_color) * RED_LOW) + (current_color * RED_HIGH);
      green = ((1.0 - current_color) * GREEN_LOW) + (current_color * GREEN_HIGH);
      blue = ((1.0 - current_color) * BLUE_LOW) + (current_color * BLUE_HIGH);
    } else if (COLOR_FEATURE == SPLIT_BAND) {
      /* Should return a number between 0.0 and 1.0 */
      // Serial.println("about to calculate color based off split-band approach : ");
      // delay(4000);
      double green_d  = _fft_manager->getFFTRangeByFreq(50, 400); // 3 octaves in each band
      double blue_d = _fft_manager->getFFTRangeByFreq(400, 3200);
      double red_d = _fft_manager->getFFTRangeByFreq(3200, 12800);
      // Serial.println("finished getting the energy amount from the fft manager : ");
      // delay(4000);
      red = (uint8_t)((double)MAX_BRIGHTNESS * (red_d / (red_d + green_d + blue_d)));
      green = (uint8_t)((double)MAX_BRIGHTNESS * (green_d / (red_d + green_d + blue_d)));
      blue = (uint8_t)((double)MAX_BRIGHTNESS * (blue_d / (red_d + green_d + blue_d)));
    }

    // calculate the target brightness ///////////////////////////////////
    target_brightness = calculateFeedbackBrightness(_fft_manager);
    /////////////////////////// Apply user brightness scaler ////////////////////////
    if (USER_BS_ACTIVE > 0) {
      target_brightness = target_brightness * user_brightness_scaler;
    }

    last_brightness = current_brightness;
    current_brightness = (target_brightness * 0.8) + (current_brightness * 0.2);

    // calculate the actual values to be sent to the strips
    // red = (uint8_t)((double)red);
    // green = (uint8_t)((double)green);
    // blue = (uint8_t)((double)blue);
    if (P_COLOR == true) {
      Serial.print("rgb:\t");
      Serial.print(red);
      Serial.print("\t");
      Serial.print(green);
      Serial.print("\t");
      Serial.println(blue);
    }
    last_led_update_tmr = 0;
  }
}


void loop() {
  // put your main code here, to run repeatedly:
  readLux(true);
  readTempHumid();
  readButtons();
  if (shake_staged == true) {
    shakeEvent(20);
  }
  delay(1000);
}
