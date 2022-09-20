/* Mechatronic Creatures
  "Bowl Bot" Genus
  using the Adafruit Huzzah ESP8266 Microcontroller
*/
#include <NeopixelManager.h>
#include <WeatherManager.h>
#include <AudioEngine.h>
#include <LuxManager.h>
#include <PrintUtils.h>
#include <UIManager.h>

// TODO - update to the way that motors are taken care of in the main_simple program
#include <DualMAX14870MotorShield.h>

#include "Mechanisms/Mechanisms.h"
#include "PlaybackEngine/PlaybackEngine.h"

#include <WS2812Serial.h>
#include "Configuration.h"
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <EEPROM.h>
#include "Macros.h"

#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// lux managers to keep track of the VEML readings
LuxManager lux_manager = LuxManager(lux_min_reading_delay, lux_max_reading_delay, LUX_MAPPING_SCHEMA);

void testSolenoids(unsigned int len) {
  elapsedMillis t = 0;
  Serial.print("Testing Solenoids - ");
  while (t < len) {
    for (int i = 0; i < 6; i++) {
      Serial.print(i);
      digitalWrite(s_pins[i], HIGH);
      delay(30);
      Serial.print(" ");
      digitalWrite(s_pins[i], LOW);
      delay(100);
    }
    Serial.println();
  }
  Serial.println("Finished testing solenoids");
  Serial.println("--------------------------");
};
//////////////////////////////////////////////////////////////////////////
/////////////////////////  NeoPixels /////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// TODO - dynamically create these objects based on info in the configuration file
#if LED_ACTIVE_CHANNELS == 3
WS2812Serial leds[3] = {
  WS2812Serial(LED1_COUNT, displayMemory[0], drawingMemory[0], LED1_PIN, WS2812_GRB),
  WS2812Serial(LED2_COUNT, displayMemory[1], drawingMemory[1], LED2_PIN, WS2812_GRB),
  WS2812Serial(LED3_COUNT, displayMemory[2], drawingMemory[2], LED3_PIN, WS2812_GRB)
};
NeoGroup neos[3] = {
  NeoGroup(&leds[0], 0, LED1_COUNT, LED1_NAME),
  NeoGroup(&leds[1], 0, LED2_COUNT, LED2_NAME),
  NeoGroup(&leds[2], 0, LED3_COUNT, LED3_NAME)
};
#elif LED_ACTIVE_CHANNELS == 2
WS2812Serial leds[2] = {WS2812Serial(LED1_COUNT, displayMemory[0], drawingMemory[0], LED1_PIN, WS2812_GRB),
                        WS2812Serial(LED2_COUNT, displayMemory[1], drawingMemory[1], LED2_PIN, WS2812_GRB)};
NeoGroup neos[2] = {
  NeoGroup(&leds[0], 0, LED1_COUNT, LED1_NAME),
  NeoGroup(&leds[1], 0, LED2_COUNT, LED2_NAME)
};
#elif LED_ACTIVE_CHANNELS == 1
WS2812Serial leds[1] = {WS2812Serial(LED1_COUNT, displayMemory[0], drawingMemory[0], LED1_PIN, WS2812_GRB)};
NeoGroup neos[2] = {NeoGroup(&leds[0], 0, LED1_COUNT, LED1_NAME)};
#endif

//////////////////////////////////////////////////////////////////////////
///////////////////// H-Bridge Motor Driver //////////////////////////////
//////////////////////////////////////////////////////////////////////////
// H-Bridge Motor (MAX14870)

DualMAX14870MotorShield motors[3] = {DualMAX14870MotorShield(MOT1_DIR_PIN, MOT1_PWM_PIN, 24, 24, MOT1_EN_PIN, MOT1_FAULT_PIN),
                                     DualMAX14870MotorShield(MOT2_DIR_PIN, MOT2_PWM_PIN, 24, 24, MOT2_EN_PIN, MOT2_FAULT_PIN),
                                     DualMAX14870MotorShield(MOT3_DIR_PIN, MOT3_PWM_PIN, 24, 24, MOT3_EN_PIN, MOT3_FAULT_PIN)
                                    };

void testMotor(int w, unsigned int len) {
  motors[w].enableDrivers();
  Serial.println();//"------------------------------");
  Serial.print("Starting Motor Test\n");
  motors[w].setM1Speed(50);
  Serial.print(" 50\t");
  delay(len / 14);
  motors[w].setM1Speed(150);
  Serial.print(" 150\t");
  delay(len / 3.5);
  motors[w].setM1Speed(50);
  Serial.print(" 50\t");
  delay(len / 7);
  // motor.setM1Speed(0);
  // Serial.print(" 0");
  // delay(len / 7);
  motors[w].setM1Speed(-50);
  Serial.print(" -50\t");
  delay(len / 7);
  motors[w].setM1Speed(-250);
  Serial.print(" -250\t");
  delay(len / 3.5);
  motors[w].setM1Speed(-50);
  Serial.print(" -50\t");
  delay(len / 14);
  Serial.print(" 0");
  motors[w].setM1Speed(0);
  motors[w].disableDrivers();
  Serial.println("\nFinished Motor Test");
  Serial.println();//"----------------------------");
}

//////////////////////////////////////////////////////////////////////////
////////////////// temp and humidity /////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
WeatherManager weather_manager = WeatherManager(HUMID_EXTREME_THRESH, TEMP_EXTRME_THRESH, TEMP_HISTORESIS, WM_UPDATE_DELAY);
/*SHTSensor sht;
  double temp = 0.0;
  double humidity = 0.0;
  elapsedMillis last_shtc_poll;

  // the lower the value the less a new reading changes things


  void updateTempHumidity()
  {
  // if it is time to update the temp/humidity
  if (last_shtc_poll > SHTC_POLLING_RATE) {
    // poll the reading
    if (sht.readSample()) {
      Serial.print("----------  SHT ---------\n");
      Serial.print("  RH: ");
      double h = sht.getHumidity();
      Serial.print(h, 2);
      Serial.print("\t");
      humidity = (humidity * (1.0 - HUMIDITY_LOWPASS)) + (h * HUMIDITY_LOWPASS);
      Serial.print(humidity, 2);
      Serial.print("\n");
      Serial.print("  T:  ");
      double t = sht.getTemperature();
      Serial.print(t, 2);
      Serial.print("\t");
      temp = (temp * (1.0 - TEMP_LOWPASS)) + (t * TEMP_LOWPASS);
      Serial.print(temp, 2);
      Serial.print("\n");
    } else {
      Serial.print("Error in temp/humidity sensor readSample()\n");
    }
    Serial.println("---------------------------");
    last_shtc_poll = 0;
  }
  }

  void testTempHumidity(uint32_t delay_time) {
  last_shtc_poll = SHTC_POLLING_RATE + 1;
  updateTempHumidity();
  delay(delay_time);
  }

  void calibrateTempHumidity(uint32_t delay_time) {
  // read temp and humidity 10 times and average the reading over the last 10
  double h  = 0.0;
  double hh = 0.0;
  double t  = 0.0;
  double tt = 0.0;
  double itters = 0;
  delay_time = delay_time / 10;
  Serial.println("starating temp/humidity calibration");
  for (int i = 0; i  < 10; i++) {
    if (sht.readSample()) {
      Serial.print(itters);
      Serial.print("\t");
      h = sht.getHumidity();
      hh += h;
      Serial.print("h : ");
      Serial.print(h);
      t = sht.getTemperature();
      Serial.print("\tt: ");
      Serial.println(t);
      tt += t;
      itters++;
    }
    delay(delay_time);
  }
  humidity = hh / itters;
  temp = tt / itters;
  Serial.print("humidity set to  : ");
  Serial.println(humidity);
  Serial.print("temp set to      : ");
  Serial.println(temp);
  Serial.println("\nended temp/humidity calibration");
  Serial.println("-------------------------------");
  }
*/
//////////////////////////////////////////////////////////////////////////
////////////////// Audio /////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
AudioInputI2S            i2s;              //xy=634,246
AudioAmplifier           input_amp;      //xy=777.1429023742676,277.14284896850586
AudioFilterBiquad        biquad;
AudioAnalyzePeak         peak;             //xy=1139.4286575317383,258.42859840393066
AudioAnalyzeFFT1024      fft;           //xy=1145.4286575317383,290.42859840393066
AudioOutputUSB           usb_output;       //xy=1147.4286575317383,194.42859840393066
AudioConnection          patchCord1(i2s, 0, biquad, 0);
AudioConnection          patchCord2(biquad, input_amp);
AudioConnection          patchCord6(input_amp, peak);
AudioConnection          patchCord7(input_amp, fft);
AudioConnection          patchCordUSB1(input_amp, 0, usb_output, 0);
AudioConnection          patchCordUSB2(input_amp, 0, usb_output, 1);

double peak_val = 0.0;
double last_peak = 0.0;

FFTManager1024 fft_manager = FFTManager1024(FFT_LOWEST_BIN, FFT_HIGHEST_BIN, "Input FFT");
FeatureCollector fc = FeatureCollector("ALL");

Rhythm rhythm[10] = {
  Rhythm(),
  Rhythm(),
  Rhythm(),
  Rhythm(),
  Rhythm(),
  Rhythm(),
  Rhythm(),
  Rhythm(),
  Rhythm(),
  Rhythm()
};

RhythmBank rhythm_bank = RhythmBank();
PlaybackEngine playback_engine = PlaybackEngine();

///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// User Controls ////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
UIManager uimanager = UIManager(UI_POLLING_RATE, P_UIMANAGER);

///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Rhythm detection stuff ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
// the maximum number of rhythms to store
#define MAX_RHYTHMS 10
// the maximum number of notes which can be stored in a single rhythm
#define MAX_NOTES 24

uint8_t active_rhythm; // what rhythm number is to be played next
uint8_t active_notes; // what note number from within the current rhythm will be played next

elapsedMillis last_onset; // when was the last onset detected?
elapsedMillis last_vocalisation; // how long it has been since the bot vocalised last

unsigned long min_inter_note_rhythm = 100; // the shortest amount of time between onsets
unsigned long max_inter_note_rhythm = 1000; // the longest amount of time between onsets

void detectOnset() {
  // given the current audio features determine if an onset has occured
}

#define AUDIO_MEMORY 60

elapsedMillis last_audio_usage_print;
#define AUDIO_USAGE_POLL_RATE 5000

void printAudioUsage() {
  // print the audio usage every second or so
  if (last_audio_usage_print > AUDIO_USAGE_POLL_RATE) {
    Serial.print("memory usage: ");
    Serial.print(AudioMemoryUsageMax());
    Serial.print(" out of ");
    Serial.println(AUDIO_MEMORY);
    last_audio_usage_print = 0;
    AudioMemoryUsageMaxReset();
  }
}

// actuator pin, dampener pin, frequency, on_time
BellMechanism bells[3] = {
  BellMechanism(s_pins[0], s_pins[1], 20, 100.0, 40),
  BellMechanism(s_pins[2], s_pins[3], 20, 500.0, 40),
  BellMechanism(s_pins[4], s_pins[5], 20, 1000.0, 40)
};

//////////////////////////////////////////////////////////////////////////
////////////////// setup / main loops ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool but_test[4];
float pot_test[4];

void setup() {
  /////////////// Solenoid Outputs /////////////////////////////////////////
  pinMode(s_pins[0], OUTPUT);
  pinMode(s_pins[1], OUTPUT);
  pinMode(s_pins[2], OUTPUT);
  pinMode(s_pins[3], OUTPUT);
  pinMode(s_pins[4], OUTPUT);
  pinMode(s_pins[5], OUTPUT);
  digitalWrite(s_pins[0], HIGH);
  digitalWrite(s_pins[1], HIGH);
  digitalWrite(s_pins[2], HIGH);
  digitalWrite(s_pins[3], HIGH);
  digitalWrite(s_pins[4], HIGH);
  digitalWrite(s_pins[5], HIGH);
  Serial.println("Finished setting solenoid pins to outputs");
  Serial.begin(57600);
  testSolenoids(3000);// let the system settle
  printMajorDivide("starting setup loop");

  ///////////////////// NeoPixel Strips /////////////////////////////////////
  printMinorDivide();
  Serial.println("Starting the LED strips");
  leds[0].begin();
  leds[1].begin();
  leds[2].begin();
  for (int i = 0; i < 3; i++) {
    neos[i].colorWipe(80, 10, 70, 0.25);
  }
  Serial.println("Finished starting the LED strips");
  printMinorDivide();

  /////////////// User Controls ////////////////////////////////////////////
  uimanager.addBut(BUT1_PIN, BUT1_REVERSE, BUT1_PULLUP, &but_test[0], BUT1_NAME);
  uimanager.addBut(BUT2_PIN, BUT2_REVERSE, BUT2_PULLUP, &but_test[1], BUT2_NAME);
  uimanager.addBut(BUT3_PIN, BUT3_REVERSE, BUT3_PULLUP, &but_test[2], BUT3_NAME);
  uimanager.addBut(BUT4_PIN, BUT4_REVERSE, BUT4_PULLUP, &but_test[3], BUT4_NAME);

  uimanager.addPot(POT1_PIN, POT1_REVERSE, POT1_PLAY, &pot_test[0], POT1_NAME);
  uimanager.addPot(POT2_PIN, POT2_REVERSE, POT2_PLAY, &pot_test[1], POT2_NAME);

  uimanager.setup();
  uimanager.printAll();

  ///////////////////// Lux Manager /////////////////////////////////
  printMinorDivide();
  Serial.println("Starting LuxManager");
  lux_manager.setLuxThresholds(LOW_LUX_THRESHOLD, MID_LUX_THRESHOLD, HIGH_LUX_THRESHOLD, EXTREME_LUX_THRESHOLD);
  lux_manager.add7700Sensor((String)"Eye-Stock");
  lux_manager.linkNeoGroup(&neos[0]);
  lux_manager.linkNeoGroup(&neos[1]);
  lux_manager.linkNeoGroup(&neos[2]);
  lux_manager.start7700Sensor(VEML7700_GAIN_1, VEML7700_IT_25MS); // todo add this to config_adv? todo
  delay(200);
  lux_manager.calibrate(3000, true);
  Serial.println("Finished starting LuxManager");

  ///////////////////// temp and humidity sensor //
  printMinorDivide();
  Serial.println("Starting WeatherManager");
  weather_manager.init();
  Serial.println("Finished starting WeatherManager");

  ////////////////////// Audio
  printMinorDivide();
  Serial.println("Starting the Audio system");
  AudioMemory(AUDIO_MEMORY);
  // TODO make this proper
  uint32_t lpf = 14000;
  uint32_t hpf = 200;
  double q = 0.8;
  input_amp.gain(60.0);

  biquad.setLowpass(0, lpf, q);
  biquad.setLowpass(1, lpf, q);
  biquad.setHighpass(2, hpf, q);
  biquad.setHighpass(3, hpf, q);

  configurePlaybackEngine();

  fft_manager.linkFFT(&fft);
  fft_manager.setCalculateCent(true);
  fft_manager.setCalculateFlux(true);

  fc.linkPeak(&peak, PRINT_PEAK_VALS);
  Serial.println("Finished starting the LED strips");
  printMinorDivide();

  printMinorDivide();
  delay(3000);
  Serial.println("Finished setup Loop");
  for (int i = 0; i < 3; i++) {
    neos[i].colorWipe(0, 120, 30, 0.25);
  }
  printMinorDivide();
}

void configurePlaybackEngine() {
  // freq, length, onset delay (since last note), velocity
  rhythm[0].addNote(500.0,  40, 0,    0.6);
  rhythm[0].addNote(500.0,  40, 1000, 0.8);
  rhythm[0].addNote(500.0,  30, 1500, 1.0);
  rhythm[0].addNote(1000.0, 30, 1000, 1.0);
  rhythm[0].addNote(1000.0, 20, 1000, 1.0);
  rhythm[0].addNote(1000.0, 50, 1500, 1.0);

  rhythm[0].addNote(100.0,  30, 1000, 1.0);
  rhythm[0].addNote(500.0,  40, 1000, 1.0);
  rhythm[0].addNote(1000.0, 30, 1500, 1.0);
  rhythm[0].addNote(100.0,  20, 1000, 1.0);
  rhythm[0].addNote(500.0,  40, 1000, 1.0);
  rhythm[0].addNote(1000.0, 30, 1500, 1.0);

  rhythm[0].addNote(100.0,  40, 200, 1.0);
  rhythm[0].addNote(500.0,  30, 70,  1.0);
  rhythm[0].addNote(1000.0, 40, 90,  1.0);
  rhythm[0].addNote(100.0,  40, 60,  1.0);
  rhythm[0].addNote(500.0,  30, 150, 1.0);
  rhythm[0].addNote(1000.0, 30, 100, 1.0);

  rhythm[1].addNote(50.0,   50, 0,    1.05);
  rhythm[1].addNote(150.0,  50, 500,  1.01);
  rhythm[1].addNote(250.0,  60, 1000, 1.15);
  rhythm[1].addNote(550.0,  60, 1500, 1.20);
  rhythm[1].addNote(450.0,  50, 2000, 1.20);
  rhythm[1].addNote(1550.0, 40, 2500, 1.20);
  rhythm[1].addNote(1650.0, 30, 3000, 1.20);

  rhythm[2].addNote(50.0,   40, 0,    1.35);
  rhythm[2].addNote(150.0,  30, 400,  1.31);
  rhythm[2].addNote(250.0,  30, 800,  1.35);
  rhythm[2].addNote(1350.0, 40, 1200, 1.30);
  rhythm[2].addNote(450.0,  30, 2000, 1.30);
  rhythm[2].addNote(550.0,  50, 2400, 1.30);
  rhythm[2].addNote(1650.0, 20, 2600, 1.30);

  rhythm[3].addNote(150.0,  40, 0,    1.35);
  rhythm[3].addNote(150.0,  50, 400,  1.31);
  rhythm[3].addNote(250.0,  60, 600,  1.35);
  rhythm[3].addNote(1350.0, 40, 700,  1.30);
  rhythm[3].addNote(450.0,  40, 800,  1.30);
  rhythm[3].addNote(550.0,  50, 500,  1.30);
  rhythm[3].addNote(1650.0, 40, 1000, 1.30);

  rhythm_bank.addRhythm(& rhythm[0]);
  rhythm_bank.addRhythm(& rhythm[1]);
  rhythm_bank.addRhythm(& rhythm[2]);
  rhythm_bank.addRhythm(& rhythm[3]);

  playback_engine.linkMechanism(& bells[0]);
  playback_engine.linkMechanism(& bells[1]);
  playback_engine.linkMechanism(& bells[2]);

  playback_engine.linkNeoGroup(& neos[0]);
  playback_engine.linkNeoGroup(& neos[1]);
  playback_engine.linkNeoGroup(& neos[2]);
}

elapsedMillis last_playback_tmr;

//////////////////////////////// Global Variables /////////////////////////
double color_feature_min = 1.00;
double color_feature_max = 0.0;

elapsedMillis feature_reset_tmr;
elapsedMillis last_led_update_tmr;
const unsigned long led_refresh_rate = 33; // for 30 updates a second
const unsigned long feature_reset_time = (1000 * 2.5);// every 2.5 minute?

double brightness_feature_min = 1.0;
double brightness_feature_max = 0.0;

double current_brightness = 1.0;
double last_brightness = 1.0;

double current_color = 0.5;
double last_color = 0.5;

double last_feature;
double current_feature;

// for mapping the target color to real colors
#define RED_LOW                 0
#define RED_HIGH                50

#define GREEN_LOW               150
#define GREEN_HIGH              50

#define BLUE_LOW                55
#define BLUE_HIGH               255

#define NUM_NEOP_MANAGERS       3

#define SPLIT_BAND              10
// options are SPECTRAL_CENTROID or SPLIT_BAND
#define COLOR_FEATURE          SPLIT_BAND

double calculateColorFromCentroid() {
  /* Should return a number between 0.0 and 1.0 */
  double cent = fft_manager.getCentroid();       // right now we are only polling the first FC for its centroid to use to color both sides
  if (cent < color_feature_min) {
    color_feature_min = (color_feature_min * 0.9) + (cent * 0.1);
    cent = color_feature_min;
  }
  if (cent > color_feature_max) {
    color_feature_max = (color_feature_max * 0.9) + (cent * 0.1);
    cent = color_feature_max;
  }
  cent = (cent - color_feature_min) / (color_feature_max - color_feature_min);
  return cent;
}

#define PRINT_BRIGHTNESS 0
#define PRINT_COLOR      1

double calculateFeedbackBrightness() {
  // how much energy is stored in the range of 4000 - 16000 compared to  the entire spectrum?
  double target_brightness = fft_manager.getFFTRangeByFreq(100, 16000);
  if (target_brightness < 0.01) {
    target_brightness = 0.0;
  } else if (target_brightness > 1.0) {
    target_brightness = 1.0;
  }
  if (target_brightness < brightness_feature_min) {
    if (PRINT_BRIGHTNESS) {
      Serial.print("target_B is less than feature_min: ");
      Serial.print(target_brightness, 5);
      Serial.print(" < ");
      Serial.print(brightness_feature_min, 5);
    }
    brightness_feature_min = (target_brightness * 0.15) + (brightness_feature_min * 0.85);
    if (PRINT_BRIGHTNESS) {
      Serial.print(" updated brightness_min and target_brightness to: ");
      Serial.println(brightness_feature_min, 5);
    }
    target_brightness = brightness_feature_min;
  }
  if (target_brightness > brightness_feature_max) {

    if (PRINT_BRIGHTNESS) {
      Serial.print("target_B is more than feature_max: ");
      Serial.print(target_brightness, 5);
      Serial.print(" > ");
      Serial.print(brightness_feature_max, 5);
    }
    brightness_feature_max = (target_brightness * 0.15) + (brightness_feature_max * 0.85);
    // to ensure that loud clipping events do not skew things too much
    if (brightness_feature_max > 1.0) {
      brightness_feature_max = 1.0;
    }
    if (PRINT_BRIGHTNESS) {
      Serial.print(" updated brightness_max and target_brightness to: ");
      Serial.println(brightness_feature_max, 5);
    }
    target_brightness = brightness_feature_max;
  }
  dprintln(PRINT_BRIGHTNESS);
  dprint(PRINT_BRIGHTNESS, " target - min/max ");
  dprint(PRINT_BRIGHTNESS, target_brightness);
  dprint(PRINT_BRIGHTNESS, " - ");
  dprint(PRINT_BRIGHTNESS, brightness_feature_min);
  dprint(PRINT_BRIGHTNESS, " / ");
  dprintln(PRINT_BRIGHTNESS, brightness_feature_max);

  target_brightness = (target_brightness - brightness_feature_min) / (brightness_feature_max - brightness_feature_min);
  dprint(PRINT_BRIGHTNESS, "target_brightness(2): ");
  dprint(PRINT_BRIGHTNESS, target_brightness);
  dprint(PRINT_BRIGHTNESS, " ");

  return target_brightness;
}

void updateFeedbackLEDs() {
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
      target_color = calculateColorFromCentroid();
      dprint(PRINT_COLOR, "target color: ");
      dprintln(PRINT_COLOR, target_color);
      last_color = current_color;
      current_color = (target_color * 0.2) + (last_color * 0.8);// * COLOR_LP_LEVEL);

      // calculate the preliminary rgb values /////////////////////////////
      red = ((1.0 - current_color) * RED_LOW) + (current_color * RED_HIGH);
      green = ((1.0 - current_color) * GREEN_LOW) + (current_color * GREEN_HIGH);
      blue = ((1.0 - current_color) * BLUE_LOW) + (current_color * BLUE_HIGH);
    } else if (COLOR_FEATURE == SPLIT_BAND) {
      /* Should return a number between 0.0 and 1.0 */
      double green_d  = fft_manager.getFFTRangeByFreq(50, 400); // 3 octaves in each band
      double blue_d = fft_manager.getFFTRangeByFreq(400, 3200);
      double red_d = fft_manager.getFFTRangeByFreq(3200, 12800);
      red = (uint8_t)((double)MAX_BRIGHTNESS * (red_d / (red_d + green_d + blue_d)));
      green = (uint8_t)((double)MAX_BRIGHTNESS * (green_d / (red_d + green_d + blue_d)));
      blue = (uint8_t)((double)MAX_BRIGHTNESS * (blue_d / (red_d + green_d + blue_d)));
    }

    // calculate the target brightness ///////////////////////////////////
    target_brightness = calculateFeedbackBrightness();
    last_brightness = current_brightness;
    current_brightness = (target_brightness * 0.8) + (current_brightness * 0.2);

    // calculate the actual values to be sent to the strips
    red = (uint8_t)((double)red * current_brightness);
    green = (uint8_t)((double)green * current_brightness);
    blue = (uint8_t)((double)blue * current_brightness);
    Serial.print("rgb:\t");
    Serial.print(red);
    Serial.print("\t");
    Serial.print(green);
    Serial.print("\t");
    Serial.println(blue);
    for (int i = 0; i < NUM_NEOP_MANAGERS; i++) {
      neos[i].colorWipe(red, green, blue, 1.0);
    }
    last_led_update_tmr = 0;
  }
}

double ACTUATION_DELAY = 0.0;

void loop() {
  ///////////////// Ambient Lighting //////////////////
  if (lux_manager.getActive() == true) {
    if (lux_manager.update()) {
      // print the updated
      lux_manager.print();
    }
  }
  ///////////////// User Controls /////////////////////
  uimanager.update();
  // uimanager.printAll();

  ///////////////// Temp / Humidity ///////////////////
  if (weather_manager.update()) {
    if (weather_manager.getHumidityShutdown()) {
      Serial.println("HUMIDITY SHUTDOWN");
      delay(10000);
    }
    if (weather_manager.getTempShutdown()) {
      Serial.println("TEMPERATURE SHUTDOWN");
      delay(10000);
    }
    weather_manager.print();
  }

  ///////////////// Actuator Outputs //////////////////
#if HV_MAJOR == 0
  updateSolenoids(); // turns off all solenoids which have
  // been activated using triggerSolenoid
  // for (int i = 0; i < NUM_MOTORS; i++) {
  //   if (active_motors[i] == true) {
  //     updateHBridge(i);
  //   }
  // }
  playback_engine.update();
  ACTUATION_DELAY = (30000) + ((weather_manager.getTemperature() + weather_manager.getHumidity()) * lux_manager.getGlobalLux());
  if (last_playback_tmr > ACTUATION_DELAY) {
    Serial.print("actuation_delay: ");
    Serial.println(ACTUATION_DELAY);
    Serial.println("playing rhythm through playback_engine");
    playback_engine.playRhythm(rhythm_bank.getRandomRhythm());
    last_playback_tmr = 0;
  }
  for (int i = 0; i < 3; i++) {
    bells[i].update();
  }
#endif // HV_MAJOR == 0

  ///////////////// Audio Analysis ////////////////////
  fft_manager.update();
  fc.update();
  ///////////////// Passive Visual Feedback ///////////
  updateFeedbackLEDs();
}
