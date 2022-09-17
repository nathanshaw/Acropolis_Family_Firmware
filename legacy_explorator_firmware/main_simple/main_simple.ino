/* Mechatronic Creatures*/

#include "NeopixelManager/NeopixelManager.h"
#include "AudioEngine/AudioEngine.h"
#include "AudioEngine/FFTManager1024.h"
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

#if MOTORS_ACTIVE > 0
#include "MAX14870Motors/MAX14870Motors.cpp"
#endif//motors_active

#if LUX_ACTIVE > 0
#include "LuxManager/LuxManager.h"

#define lux_min_reading_delay (1000 * 15)
#define lux_max_reading_delay (1000 * 60 * 3)
// lux managers to keep track of the VEML readings
LuxManager lux_manager = LuxManager(lux_min_reading_delay, lux_max_reading_delay);
// LuxManager lux_manager = LuxManager(lux_min_reading_delay, lux_max_reading_delay, (String)"Front", &neos[0]);
#endif// lux_active

#if SHT_PRESENT
#include "SHTSensor.h"
#endif // SHT_ACTIVE

// a higher level the more debug printing occurs
#define DEBUG 0
#define TEST_SOLENOIDS 0
#define TEST_MOTOR 0
#define TEST_NEOP 0
// todo make this bring up the stereo test...?
#define TEST_MICS 1
// todo implement this
#define TEST_TEMP_HUMIDITY 0
#define TEST_LDRS 0

#define PRINT_PEAK_VALS 0

// should the program datalog?
#define DATALOG 1
#define PRINT_EEPROM_ON_BOOT 1

//////////////////////////////////////////////////////////////////////////
///////////////////  Debug Printing  /////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void dbPrint(String msg, uint8_t level) {
  if (level <= DEBUG) {
    Serial.print(msg);
  }
}

void dbPrintln(String msg, uint8_t level) {
  if (level <= DEBUG) {
    Serial.println(msg);
  }
}

//////////////////////////////////////////////////////////////////////////
/////////////////////////  NeoPixels /////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#if LEDS_ACTIVE == 3
const uint16_t max_led_count = max(max(LED1_COUNT, LED2_COUNT), LED3_COUNT);
byte drawingMemory[LEDS_ACTIVE][max_led_count * 3];       //  3 bytes per LED
DMAMEM byte displayMemory[LEDS_ACTIVE][max_led_count * 12]; // 12 bytes per LED

WS2812Serial leds[LEDS_ACTIVE] = {WS2812Serial(LED1_COUNT, displayMemory[0], drawingMemory[0], LED1_PIN, WS2812_GRB),
                                  WS2812Serial(LED2_COUNT, displayMemory[1], drawingMemory[1], LED2_PIN, WS2812_GRB),
                                  WS2812Serial(LED3_COUNT, displayMemory[2], drawingMemory[2], LED3_PIN, WS2812_GRB)
                                 };

NeoGroup neos[3] = {
  NeoGroup(&leds[0], 0, LED1_COUNT, "small"),
  NeoGroup(&leds[1], 0, LED2_COUNT, "medium"),
  NeoGroup(&leds[2], 0, LED3_COUNT, "large")
};
#elif LEDS_ACTIVE == 2
const uint16_t max_led_count = max(LED1_COUNT, LED2_COUNT);
byte drawingMemory[LEDS_ACTIVE][max_led_count * 3];       //  3 bytes per LED
DMAMEM byte displayMemory[LEDS_ACTIVE][max_led_count * 12]; // 12 bytes per LED

WS2812Serial leds[LEDS_ACTIVE] = {WS2812Serial(LED1_COUNT, displayMemory[0], drawingMemory[0], LED1_PIN, WS2812_GRB),
                                  WS2812Serial(LED2_COUNT, displayMemory[1], drawingMemory[1], LED2_PIN, WS2812_GRB)
                                 };

NeoGroup neos[LEDS_ACTIVE] = {
  NeoGroup(&leds[0], 0, LED1_COUNT, "small"),
  NeoGroup(&leds[1], 0, LED2_COUNT, "medium"),
};

#elif LEDS_ACTIVE == 1
const uint16_t max_led_count = LED1_COUNT;
byte drawingMemory[LEDS_ACTIVE][max_led_count * 3];       //  3 bytes per LED
DMAMEM byte displayMemory[LEDS_ACTIVE][max_led_count * 12]; // 12 bytes per LED

WS2812Serial leds[LEDS_ACTIVE] = {WS2812Serial(LED1_COUNT, displayMemory[0], drawingMemory[0], LED1_PIN, WS2812_GRB)};

NeoGroup neos[LEDS_ACTIVE] = {
  NeoGroup(&leds[0], 0, LED1_COUNT, "eye_stock")
};
#endif // LEDS_ACTIVE

//////////////////////////////////////////////////////////////////////////
///////////////////// H-Bridge Motor Driver //////////////////////////////
//////////////////////////////////////////////////////////////////////////

#if MOTORS_ACTIVE > 0
MAX14870Motors motors = MAX14870Motors(M1_DIR, M1_SPEED,
                                       M2_DIR, M2_SPEED,
                                       M3_DIR, M3_SPEED,
                                       M1_COAST, M2_COAST, M3_COAST,
                                       M1_FAULT, M2_FAULT, M3_FAULT);

/*
  #if MOTORS_ACTIVE == 3
  // TODO need a better way of dealing with this...
  DualMAX14870MotorShield motor[3] = {
  DualMAX14870MotorShield(M1_DIR, M1_SPEED, 100, 100, M1_COAST, M1_FAULT),
  DualMAX14870MotorShield(M2_DIR, M2_SPEED, 100, 100, M2_COAST, M2_FAULT),
  DualMAX14870MotorShield(M3_DIR, M3_SPEED, 100, 100, M3_COAST, M3_FAULT)
  }; // 31, 32, and 33 are unused pins

  #elif MOTORS_ACTIVE == 2
  DualMAX14870MotorShield motor[2] = {
  DualMAX14870MotorShield(M1_DIR, M1_SPEED, 100, 100, M1_COAST, M1_FAULT),
  DualMAX14870MotorShield(M2_DIR, M2_SPEED, 100, 100, M2_COAST, M2_FAULT)
  }; // 31, 32, and 33 are unused pins
  #elif MOTORS_ACTIVE == 1
  DualMAX14870MotorShield motor[1] = {
  DualMAX14870MotorShield(M1_DIR, M1_SPEED, 100, 100, M1_COAST, M1_FAULT)
  }; // 31, 32, and 33 are unused pins
  #endif // Motors Active
*/

/*
  void testMotors(unsigned int len) {
  for (int i = 0; i < MOTORS_ACTIVE; i++) {
    motor[i].enableDrivers();
    Serial.println();//"------------------------------");
    Serial.print("Starting Motor ");
    Serial.print(i);
    Serial.println(" Test\n");
    motor[i].setM1Speed(50);
    Serial.print(" 50\t");
    delay(len / 14);
    motor[i].setM1Speed(150);
    Serial.print(" 150\t");
    delay(len / 3.5);
    motor[i].setM1Speed(50);
    Serial.print(" 50\t");
    delay(len / 7);
    // motor[i].setM1Speed(0);
    // Serial.print(" 0");
    // delay(len / 7);
    motor[i].setM1Speed(-50);
    Serial.print(" -50\t");
    delay(len / 7);
    motor[i].setM1Speed(-250);
    Serial.print(" -250\t");
    delay(len / 3.5);
    motor[i].setM1Speed(-50);
    Serial.print(" -50\t");
    delay(len / 14);
    Serial.print(" 0");
    motor[i].setM1Speed(0);
    motor[i].disableDrivers();
    Serial.print("\nFinished Motor ");
    Serial.print(i);
    Serial.println("Test");
    Serial.println();//"----------------------------");
  }
  }
*/
void rampMotor(int which, int16_t start, int16_t target, int ramp_total_time) {
  Serial.print("Ramping Motor - ");
  Serial.print(which);
  if (which > 2) {
    return;
  };

  int difference = target - start;
  int step_delay = abs(difference / ramp_total_time);

  Serial.print(" dif: ");
  Serial.print(difference);
  Serial.print(" stepd: ");
  Serial.print(step_delay);
  motors.enableDrivers(which);

  if (difference > 0) {
    for (int16_t i = start; i <= target; i++) {
      if (which == 0) {
        motors.setM1Speed(i);
      }
      else if (which == 1) {
        motors.setM2Speed(i);
      }
      else if (which == 2) {
        motors.setM3Speed(i);
      }
      delay(step_delay);
      // Serial.println(i);
    }
  } else {
    for (int16_t i = start; i > target; i--) {
      if (which == 0) {
        motors.setM1Speed(i);
      }
      else if (which == 1) {
        motors.setM2Speed(i);
      }
      else if (which == 2) {
        motors.setM3Speed(i);
      }
      delay(step_delay);
      // Serial.println(i);
    }
  }
  if (target == 0) {
    motors.disableDrivers(which);
  }
  Serial.println("Disabled Drivers");
}

#endif// MOTORS_ACTIVE

//////////////////////////////////////////////////////////////////////////
//////////////////////////// Tests ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void testSolenoids(unsigned int len) {
  elapsedMillis t = 0;
  Serial.print("Testing Solenoids - ");
  while (t < len) {
    for (int i = 0; i < NUM_SOLENOIDS; i++) {
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
////////////////// temp and humidity /////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#if SHT_PRESENT > 0
SHTSensor sht;

elapsedMillis last_shtc_poll;
uint16_t SHTC_POLLING_RATE = 1000 * 60 * 5; // in ms

// current temperature, min temp recorded, max temp recorded, and avereage temp recorded
double temp = 0.0;
double temp_max = 0.0;
double temp_min = 1000.0;
double temp_avg = 0.0;
double temp_total = 0.0;    // sum of all temps recorded thus far

double humidity = 0.0;
double humid_max = 0.0;
double humid_min = 1000.0;
double humid_avg = 0.0;
double humid_total = 0.0;

uint16_t shtc_readings = 0; // total number of temp readings taken

// the lower the value the less a new reading changes things
#define TEMP_LOWPASS 0.5
#define HUMIDITY_LOWPASS 0.5

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
      updateMinMax(humidity, humid_min, humid_max);
      Serial.print(humidity, 2);
      Serial.print("\n");
      Serial.print("  T:  ");
      double t = sht.getTemperature();
      Serial.print(t, 2);
      Serial.print("\t");
      temp = (temp * (1.0 - TEMP_LOWPASS)) + (t * TEMP_LOWPASS);
      updateMinMax(temp, temp_min, temp_max);
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
#endif// SHT_ACTIVE

//////////////////////////////////////////////////////////////////////////
////////////////// Audio /////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#if MICROPHONE_PRESENT > 0
AudioInputI2S            i2s;              //xy=634,246
AudioAmplifier           input_amp;      //xy=777.1429023742676,277.14284896850586
AudioFilterBiquad        biquad;
AudioAnalyzePeak         peak;             //xy=1139.4286575317383,258.42859840393066
AudioAnalyzeFFT1024      fft;           //xy=1145.4286575317383,290.42859840393066

#if USB_OUTPUT > 0
AudioOutputUSB           usb_output;       //xy=1147.4286575317383,194.42859840393066
AudioConnection          patchCord10(input_amp, 0, usb_output, 0);
AudioConnection          patchCord11(input_amp, 0, usb_output, 1);
#endif

AudioConnection          patchCord1(i2s, 0, biquad, 0);
AudioConnection          patchCord2(biquad, input_amp);
AudioConnection          patchCord6(input_amp, peak);
AudioConnection          patchCord7(input_amp, fft);

double peak_val = 0.0;
double last_peak = 0.0;

FFTManager1024 fft_features = FFTManager1024("Input FFT");
FeatureCollector fc = FeatureCollector("ALL");

#define AUDIO_MEMORY 16

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

#endif // MICROPHONE_ACTIVE


///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Rhythm detection stuff ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
// the maximum number of rhythms to store
#define MAX_RHYTHMS 10
// the maximum number of notes which can be stored in a single rhythm
#define MAX_NOTES 24

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

uint8_t active_rhythm; // what rhythm number is to be played next
uint8_t active_notes; // what note number from within the current rhythm will be played next

elapsedMillis last_onset; // when was the last onset detected?
elapsedMillis last_vocalisation; // how long it has been since the bot vocalised last

unsigned long min_inter_note_rhythm = 100; // the shortest amount of time between onsets
unsigned long max_inter_note_rhythm = 1000; // the longest amount of time between onsets

void detectOnset() {
  // given the current audio features determine if an onset has occured
}

#if BELL_BOT > 0
// actuator pin, dampener pin, frequency, on_time
BellMechanism bells[3] = {
  BellMechanism(s_pins[0], s_pins[1], 20, 100.0, 40),
  BellMechanism(s_pins[2], s_pins[3], 20, 500.0, 40),
  BellMechanism(s_pins[4], s_pins[5], 20, 1000.0, 40)
};
#endif// BELL_BOT

void testSolenoids() {
  Serial.println("Testing Solenoids Now");
  for (int t = 0; t < 4; t++) {
    for (int i = 0; i < 9; i++) {
      digitalWrite(s_pins[i], HIGH);
      delay(50);
      digitalWrite(s_pins[i], LOW);
      delay(100);
    }
    Serial.println();
  }
}

void setupAndTestControls(uint16_t wait) {
  // test the values of the buttons/switches etc over a period of time
  // to ensure that things are actually conneceted to the pins

  ///////////////////////// Buttons ////////////////////////////////////
  Serial.println("Setting up the button pins as INPUTS");
  // first set the button pins to INPUT
  for (int i = 0; i < MAX_BUTTONS; i++) {
    pinMode(but_pins[i], INPUT);
  }

  Serial.print("testing the buttons for consistant readings");
  // buttons
  uint16_t _wait = wait / 20;
  bool _vals[MAX_BUTTONS];
  bool good[MAX_BUTTONS];
  for (int i = 0; i < MAX_BUTTONS; i++) {
    good[i] = true;
  }
  // read each button 20 times to ensure that it returns the same reading each time
  for (int itters = 0; itters < 20; itters++) {
    for (int i = 0; i < MAX_BUTTONS; i++ ) {
      uint16_t temp = digitalRead(but_pins[i]);
      if (itters > 0 && temp != _vals[i]) {
        good[i] = false;
        Serial.print("x");
      } else {
        Serial.print(".");
      }
    }
    delay(_wait);
  }
  Serial.println();
  // now flag any buttons which returned inconsistant results
  for (int i = 0; i < MAX_BUTTONS; i++) {
    if (good[i] == false) {
      Serial.print("Button ");
      Serial.print(i);
      Serial.println(" is returning inconsistant results, flagging the button as inactive");
      but_active[i] = false;
    }
  }

  //////////////////////////////// Pots /////////////////////////////////////////////
  Serial.println("Now Testing for Potentiometers");
  // if the reading is oscilating by more than 3% during this time period
  // from the initial reading, something is wrong.
  uint16_t _pvals[MAX_POTS];
  bool pgood[MAX_POTS];
  for (int i = 0; i < MAX_POTS; i++) {
    pgood[i] = true;
  }
  // read each button 20 times to ensure that it returns the same reading each time
  for (int itters = 0; itters < 20; itters++) {
    for (int i = 0; i < MAX_POTS; i++ ) {
      uint16_t temp = analogRead(pot_pins[i]);
      if (itters == 0) {
        _pvals[i] = temp;
      } else if (temp < _pvals[i] - 20 || temp > _pvals[i] + 20) {
        good[i] = false;
        Serial.print("x");
      } else {
        // if the new value has a plus or minus change of > 20
        Serial.print(".");
      }
    }
    delay(_wait);
  }
  Serial.println();
  // now flag any pot which returned inconsistant results
  for (int i = 0; i < MAX_POTS; i++) {
    if (pgood[i] == false) {
      Serial.print("Pot ");
      Serial.print(i);
      Serial.println(" is returning inconsistant results, flagging the pot as inactive");
      pot_active[i] = false;
    }
  }
  Serial.println("Setup and test controls is now finished, exiting");
  Serial.println("-------------------------------------------");
}

void readControls() {
  Serial.println("READ CONTROLS IS TODO");

}

//////////////////////////////////////////////////////////////////////////
////////////////// setup / main loops ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void setup() {
  analogWriteResolution(10);// change the write resolution to 10 bits
  Serial.begin(115200);
  delay(2000);
  Serial.println("Starting Lyre setup() loop\n");
  
  ///////////////////// user controls /////////////////////////////////////////////////
  // test the user controls and then read the ones that need to be read
  setupAndTestControls(2000);
  readControls();
  delay(1000);
  
  ///////////////////// h-bridge motors ///////////////////////////////////////////////
#if MOTORS_ACTIVE > 0
  Serial.println("Starting Motor Set-up");
  motors.flipM1(M1_POLARITY);
  motors.flipM2(M2_POLARITY);
  motors.flipM3(M3_POLARITY);
  motors.enableDrivers();
  motors.setM1Speed(0);
  motors.setM2Speed(0);
  motors.setM3Speed(0);
  delay(500);
  Serial.println("Finished setting up motors");
  Serial.println("-------------------------------------------------------------");
  delay(2500);// let the system settle
#endif // MOTORS_ACTIVE

  /////////////// solenoid outputs //////////////////////////////////////////////////////
  Serial.println("Starting to set solenoid pins to outputs");
  pinMode(s_pins[0], OUTPUT);
  pinMode(s_pins[1], OUTPUT);
  pinMode(s_pins[2], OUTPUT);
  pinMode(s_pins[3], OUTPUT);
  pinMode(s_pins[4], OUTPUT);
  pinMode(s_pins[5], OUTPUT);
  pinMode(s_pins[6], OUTPUT);
  pinMode(s_pins[7], OUTPUT);
  pinMode(s_pins[8], OUTPUT);

  digitalWrite(s_pins[0], LOW);
  digitalWrite(s_pins[1], LOW);
  digitalWrite(s_pins[2], LOW);
  digitalWrite(s_pins[3], LOW);
  digitalWrite(s_pins[4], LOW);
  digitalWrite(s_pins[5], LOW);
  digitalWrite(s_pins[6], LOW);
  digitalWrite(s_pins[7], LOW);
  digitalWrite(s_pins[8], LOW);
  Serial.println("Finished setting solenoid pins to outputs");
  Serial.println("-------------------------------------------------------------");
  
  /////////////// lux Sensor /////////////////////////////////////////////////////////
#if LUX_ACTIVE
  Serial.println("Attempting to add Lux Sensor");
  lux_manager.addLuxSensor("Top");
  Serial.println("Added sensor. Now starting sensor with gain and integration time");
  lux_manager.startSensors(VEML7700_GAIN_1, VEML7700_IT_25MS);
  delay(200); // let things settle for a bit
  Serial.println("Starting calibration of Lux Sensor");
  lux_manager.calibrate(3000);
  Serial.println("Finished calibrating, now linkibng neopixel managers to the lux_manager");
  lux_manager.linkNeoGroup(&neos[0]);
  lux_manager.linkNeoGroup(&neos[1]);
  lux_manager.linkNeoGroup(&neos[2]);
  Serial.println("Finished linking Neopixel managers to lux_manager");
  Serial.println("-------------------------------------------------------------");
#endif

  ///////////////////// NeoPixel Strips ////////////////////////////////////////////////
#if LEDS_ACTIVE > 0
  Serial.println("Starting and Testing LEDs");
  for (int i = 0;  i < 3; i++) {
    leds[i].begin();
    Serial.print("started leds #");
    Serial.print(i);
    Serial.println(" and turning blueish");
    neos[i].colorWipe(40, 24, 120);
    delay(1000);
    neos[i].colorWipe(0, 0, 0);
  }
  Serial.println("Finished testing LEDs");
  Serial.println("-------------------------------------------------------------");
#endif // LEDS_ACTIVE

  ///////////////////// temp and humidity sensor /////////////////////////////////////////
#if SHT_ACTIVE > 0
  Serial.println("Starting Wire for SHT temp/humid sensor");
  Wire.begin();
  delay(500);
  if (sht.init()) {
    Serial.print("SHT temp/humidity init(): success\n");
    SHT_ACTIVE = true;
    sht.setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM); // only supported by SHT3x
    Serial.println("Setting the accuracy of the SHT sensor");
    calibrateTempHumidity(4000);
  } else {
    Serial.print("WARNING: SHT temp / humidity init(): failed\n");
    neos[EYE_STOCK_NEOS]colorWipe(255, 120, 20);
    SHT_ACTIVE = false;
    delay(5000);
  }
  Serial.println("Finished setting up SHT sensor");
  Serial.println("-------------------------------------------------------------");
#endif //SHT_ACTIVE

  ////////////////////// Audio ////////////////////////////////////////////////////////////////
#if MICROPHONE_ACTIVE > 0
  Serial.println("Starting to setup microphone and audio");
  AudioMemory(AUDIO_MEMORY);
  // TODO make this proper
  uint32_t lpf = 14000;
  uint32_t hpf = 200;
  double q = 0.8;
  input_amp.gain(30.0);
  Serial.println("input amp gain set");
  biquad.setLowpass(0, lpf, q);
  biquad.setLowpass(1, lpf, q);
  biquad.setHighpass(2, hpf, q);
  biquad.setHighpass(3, hpf, q);
  Serial.print("biquad filter is now configured");
  Serial.println("TODO - add microphone test like with the MOTH to this code");
  Serial.println("Finished setting up Audio");
  Serial.println("-------------------------------------------------------------");
#endif// MICROPHONE_ACTIVE
  // freq, length, onset time, velocity
  /*
    rhythm[0].addNote(100.0, 200, 0, 1.0);
    rhythm[0].addNote(500.0, 250, 0, 1.0);
    rhythm[0].addNote(1000.0, 1000, 0, 1.0);
    rhythm[0].addNote(100.0, 1000, 1500, 1.0);
    rhythm[0].addNote(100.0, 200, 300, 1.0);
    rhythm[0].addNote(100.0, 200, 300, 1.0);
    rhythm[0].addNote(100.0, 200, 300, 1.0);
  */
  Serial.println("adding rhythms to the rhythm array");
  rhythm[0].addNote(500.0, 0, 100, 1.0);
  rhythm[0].addNote(500.0, 0, 100, 1.0);
  rhythm[0].addNote(500.0, 0, 150, 1.0);
  rhythm[0].addNote(1000.0, 0, 100, 1.0);
  rhythm[0].addNote(1000.0, 0, 100, 1.0);
  rhythm[0].addNote(1000.0, 0, 150, 1.0);

  rhythm[0].addNote(100.0, 0, 100, 1.0);
  rhythm[0].addNote(500.0, 0, 100, 1.0);
  rhythm[0].addNote(1000.0, 0, 150, 1.0);
  rhythm[0].addNote(100.0, 0, 100, 1.0);
  rhythm[0].addNote(500.0, 0, 100, 1.0);
  rhythm[0].addNote(1000.0, 0, 150, 1.0);

  rhythm[0].addNote(100.0, 0, 200, 1.0);
  rhythm[0].addNote(500.0, 0, 70, 1.0);
  rhythm[0].addNote(1000.0, 0, 90, 1.0);
  rhythm[0].addNote(100.0, 0, 60, 1.0);
  rhythm[0].addNote(500.0, 0, 150, 1.0);
  rhythm[0].addNote(1000.0, 0, 100, 1.0);

  rhythm[1].addNote(50.0, 500, 0, 1.05);
  rhythm[1].addNote(150.0, 500, 500, 1.01);
  rhythm[1].addNote(250.0, 600, 1000, 1.15);
  rhythm[1].addNote(550.0, 600, 1500, 1.20);
  rhythm[1].addNote(450.0, 500, 2000, 1.20);
  rhythm[1].addNote(1550.0, 400, 2500, 1.20);
  rhythm[1].addNote(1650.0, 300, 3000, 1.20);

  rhythm[2].addNote(50.0, 400, 0, 1.35);
  rhythm[2].addNote(150.0, 300, 400, 1.31);
  rhythm[2].addNote(250.0, 300, 800, 1.35);
  rhythm[2].addNote(1350.0, 400, 1200, 1.30);
  rhythm[2].addNote(450.0, 300, 2000, 1.30);
  rhythm[2].addNote(550.0, 500, 2400, 1.30);
  rhythm[2].addNote(1650.0, 200, 2600, 1.30);

  rhythm[3].addNote(150.0, 0, 0, 1.35);
  rhythm[3].addNote(150.0, 0, 400, 1.31);
  rhythm[3].addNote(250.0, 0, 600, 1.35);
  rhythm[3].addNote(1350.0, 0, 700, 1.30);
  rhythm[3].addNote(450.0, 0, 800, 1.30);
  rhythm[3].addNote(550.0, 0, 500, 1.30);
  rhythm[3].addNote(1650.0, 0, 1000, 1.30);

  rhythm_bank.addRhythm(& rhythm[0]);
  // rhythm_bank.addRhythm(& rhythm[1]);
  // rhythm_bank.addRhythm(& rhythm[2]);
  // rhythm_bank.addRhythm(& rhythm[3]);
  Serial.println("Finished adding rhythms to the rhythm array");

#if BELL_BOT > 0
  playback_engine.linkMechanism(& bells[0]);
  playback_engine.linkMechanism(& bells[1]);
  playback_engine.linkMechanism(& bells[2]);
#endif // BELL_BOT

#if MICROPHONE_ACTIVE > 0
  fft_features.linkFFT(&fft);
  Serial.print("FFT features linked, ");
  fft_features.setCentroidActive(true);
  Serial.print("Centroid Active Set, ");
  fft_features.setFluxActive(true);
  Serial.print("Flux Active Set, ");
  fft_features.setFFTScaler(100);
  Serial.print("FFT Scaler set to 100, ");

  fc.linkPeak(&peak, 1000.0, PRINT_PEAK_VALS);
  Serial.println("Peak linked to FC");

  // TODO TEST the microphone

#endif // MICROPHONE_ACTIVE

  Serial.println("Finished setup Loop");
  delay(800);
  Serial.println("---------------------------------- -");
}

elapsedMillis last_playback_tmr;

void updateActuators() {
#if ACTUATOR_CONFIG == WOODPECKER_BOT
  updateHBridge();
#elif ACTUATOR_CONFIG == BELL_BOT
  playback_engine.update();
  if (last_playback_tmr > 1000) {
    Serial.println("playing rhythm through playback_engine");
    playback_engine.playRhythm(rhythm_bank.getRandomRhythm());
    last_playback_tmr = 0;
  }
#endif
  updateSolenoids();
}


void updateEnvironmentalSensors() {
#if MICROPHONE_PRESENT
  if (MICROPHONE_ACTIVE) {
    // fft_features.calculateFFT();
    fc.update();
  }
#endif

#if PCB_REVISION == 0
  updateLDRs(true);
#elif PCB_REVISION == 1
#if SHT_PRESENT

  if (SHT_ACTIVE) {
    updateTempHumidity();
  }

#endif // SHT_PRESENT

#if LUX_PRESENT > 0
  if (LUX_ACTIVE) {
    lux_manager.update();
  }
#endif //LUX_PRESENT
#endif // PC_REVISION
}



void determineActivity() {
  // should it be a state machine?
  //  state 0 = waiting
  //  state 1 = preparing
  //  state 2 = actuating
  
  // a default value of 1 minute between actuations will be used as a base line
  // what will the bot do given the current environmental data and its given programming
  
  // the temperature and ambiant light levels are positively correlated with actuation times
  
  // humidity is negatively correlated with actuation times

  // Once a sonic event is flagged to happen the bot will wait until the PEAK/RMS/FFT power is below the "threshold" for x frames in a row
  // before begenning its actuation

  // TODO - which sonic event that is chosen could depend on environmental conditions (some sounds at night vs daytime for instance).
}

//////////////////////////////// Global Variables /////////////////////////

void loop() {
  // this function keeps all the needed update functions in one place
  updateDatalog();
  updateEnvironmentalSensors();
  updateFeedbackLEDFromEnvironment();
  updateActuators();
  determineActivity();
}
