// Simple WAV file player example
//
// Three types of output may be used, by configuring the code below.
//
//   1: Digital I2S - Normally used with the audio shield:
//         http://www.pjrc.com/store/teensy3_audio.html
//
//   2: Digital S/PDIF - Connect pin 22 to a S/PDIF transmitter
//         https://www.oshpark.com/shared_projects/KcDBKHta
//
//   3: Analog DAC - Connect the DAC pin to an amplified speaker
//         http://www.pjrc.com/teensy/gui/?info=AudioOutputAnalog
//
// To configure the output type, first uncomment one of the three
// output objects.  If not using the audio shield, comment out
// the sgtl5000_1 lines in setup(), so it does not wait forever
// trying to configure the SGTL5000 codec chip.
//
// The SD card may connect to different pins, depending on the
// hardware you are using.  Uncomment or configure the SD card
// pins to match your hardware.
//
// Data files to put on your SD card can be downloaded here:
//   http://www.pjrc.com/teensy/td_libs_AudioDataFiles.html
//
// This example code is in the public domain.

//////////////////////////////////////////////////////////////////////////////
//////////////////////////// Audio Playback //////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
// GUItool: begin automatically generated code
AudioPlaySdWav           playWav1;       //xy=787,620
AudioFilterBiquad        biquad2;        //xy=926,632
AudioFilterBiquad        biquad1;        //xy=928,600
AudioAnalyzePeak         peak_l;         //xy=1104,602
AudioAmplifier           amp2;           //xy=1104,669
AudioAmplifier           amp1;           //xy=1105,636
AudioAnalyzeFFT256       fft256;       //xy=1106,568
AudioOutputI2S           audioOutput;    //xy=1416,682
AudioConnection          patchCord1(playWav1, 0, biquad1, 0);
AudioConnection          patchCord2(playWav1, 1, biquad2, 0);
AudioConnection          patchCord3(biquad2, amp2);
AudioConnection          patchCord4(biquad1, peak_l);
AudioConnection          patchCord5(biquad1, amp1);
AudioConnection          patchCord6(biquad1, fft256);
AudioConnection          patchCord7(amp2, 0, audioOutput, 1);
AudioConnection          patchCord8(amp1, 0, audioOutput, 0);
AudioControlSGTL5000     sgtl5000_1;     //xy=964,717
// GUItool: end automatically generated code

// Use these with the Teensy Audio Shield
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14

#include <WS2812Serial.h>

const int numled = 2;
const int pin = 5;

uint8_t colors[3] = {155, 70, 200};
float fcolors[3];

byte drawingMemory[numled * 3];       //  3 bytes per LED
DMAMEM byte displayMemory[numled * 12]; // 12 bytes per LED

WS2812Serial leds(numled, displayMemory, drawingMemory, pin, WS2812_GRB);

// max settings for the 8 ohm 5W 70 mm clear plastic cones when two are used
#define CLEAR_PLASTIC_8_OHM_70_MM                    0
#define CLEAR_PLASTIC_45_OHM_52_MM                   1
#define SPEAKER_TYPE                                 CLEAR_PLASTIC_8_OHM_70_MM

#if SPEAKER_TYPE == CLEAR_PLASTSIC_8_OHM_70_MM
const int bq_low_freq = 140;
const int bq_low_gain = -96;
const double bq_low_slope = 1.0;
#define MAX_GAIN            0.25

#elif SPEAKER_TYPE == CLEAR_PLASTIC_45_OHM_52_MM
const int bq_low_freq = ;
const int bq_low_gain = -96;
const double bq_low_slope = 1.0;
#define MAX_GAIN            0.16
#endif

float input_gain = MAX_GAIN;

int pot_pins[2] = {A3, A7}; // pins 17 and 21
// int but_pins[6] = {0, 1, 4, 6, 8, 15};
int but_pins[6] = {15, 8, 6, 4, 1, 0};
float pot_vals[2] = {1.0, 1.0};
bool but_vals[6];

// elapsedMillis last_user_control_reading;
// #define USER_CONTROL_POLL_RATE            200

//////////////////////////////////////////////////////////////////////////////
//////////////////////////// User Controls ///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void readUserControls() {
  // if (last_user_control_reading > USER_CONTROL_POLL_RATE) {

  for (int i = 0; i < 2; i++) {
    Serial.print("pot");
    Serial.print(i);
    Serial.print(" ");
    Serial.println(pot_vals[i]);
    if (analogRead(pot_pins[i]) / 1024.0 != pot_vals[i]) {
      pot_vals[i] = (float)analogRead(pot_pins[i]) / 1024.0;
      if (i == 0 ) {
        Serial.print("gain updated to : ");
        Serial.println(pot_vals[0]);
        amp1.gain(pot_vals[i] * MAX_GAIN);
        amp2.gain(pot_vals[i] * MAX_GAIN);
      }
    }
  }
  for (int i = 0; i < 6; i++) {
    Serial.print("but");
    Serial.print(i);
    but_vals[i] = digitalRead(but_pins[i]);
    Serial.print(":\t");
    Serial.println(but_vals[i]);
  }
  // last_user_control_reading = 0;
  // }
}

void setupUserControls() {
  pinMode(pot_pins[0], INPUT);
  pinMode(pot_pins[1], INPUT);
  pinMode(but_pins[0], INPUT_PULLUP);
  pinMode(but_pins[1], INPUT_PULLUP);
  pinMode(but_pins[2], INPUT_PULLUP);
  pinMode(but_pins[3], INPUT_PULLUP);
  pinMode(but_pins[4], INPUT_PULLUP);
  pinMode(but_pins[5], INPUT_PULLUP);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////// NeoPixels ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void brightnessWipe(double b) {
  // uses the currently stored color, adjusts the brightness
  uint32_t c = (constrain((int)(colors[0] * b), 0, 255) << 16) + (constrain((int)(colors[1] * b), 0, 255) << 8) + constrain((int)(colors[2] * b), 0, 255);
  for (int i = 0; i < leds.numPixels(); i++) {
    leds.setPixel(i, c);
  }
  leds.show();
}

void colorWipe(int c) {
  for (int i = 0; i < leds.numPixels(); i++) {
    leds.setPixel(i, c);
  }
  leds.show();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////// Main Loops //////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void setup() {
  delay(2000);
  leds.begin();
  colorWipe(0xffffff);
  setupUserControls();
  Serial.begin(57600);
  delay(5000);
  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(16);
  colorWipe(0xffff00);
  // Comment these out if not using the audio adaptor board.
  // This may wait forever if the SDA & SCL pins lack
  // pullup resistors
  if (!sgtl5000_1.enable()) {
    while (!sgtl5000_1.enable()) {
      Serial.println("Problem finding the SG5000 chip");
      delay(10000);
    }
  }
  colorWipe(0xff0000);
  sgtl5000_1.volume(0.1);
  amp1.gain(input_gain);
  amp2.gain(input_gain);
  biquad1.setLowShelf(0, bq_low_freq, bq_low_gain, bq_low_slope);
  biquad2.setLowShelf(0, bq_low_freq, bq_low_gain, bq_low_slope);
  biquad1.setHighShelf(1, 3000, -6, 0.5);
  biquad2.setHighShelf(1, 3000, -6, 0.5);
  biquad1.setHighShelf(2, 5000, -9, 0.5);
  biquad2.setHighShelf(2, 5000, -9, 0.5);
  biquad1.setHighShelf(2, 8000, -12, 0.5);
  biquad2.setHighShelf(2, 8000, -12, 0.5);

  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  delay(1000);
  colorWipe(0x00ff00);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here, but print a message repetitively
    while (!(SD.begin(SDCARD_CS_PIN))) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
  colorWipe(0x0000ff);
}

void playFile(const char *filename)
{
  Serial.print("Playing file: ");
  Serial.println(filename);
  colorWipe(0x080816);
  // Start playing the file.  This sketch continues to
  // run while the file plays.
  playWav1.play(filename);

  // A brief delay for the library read WAV info
  delay(5);

  // Simply wait for the file to finish playing.
  while (playWav1.isPlaying()) {
    // readUserControls();
    if (fft256.available()) {
      fcolors[2] = fft256.read(1, 8)   * 4;   // 172  - 1536  Hz
      fcolors[1] = fft256.read(8, 23)  * 4;  // 1536 - 4416  Hz
      fcolors[0] = fft256.read(23, 93) * 4; // 1536 - 15996 Hz
      for (int i = 0; i < 3; i++) {
        if (colors[i] > 1.0) {
          colors[i] = 1.0;
        }
      }
      for (int i  = 0; i < 3; i++) {
        colors[i] = (uint8_t)(fcolors[i] * 255);
      }
      Serial.print(colors[0]);
      Serial.print("\t");
      Serial.print(colors[1]);
      Serial.print("\t");
      Serial.println(colors[2]);
    }
    if (peak_l.available()) {
      int c = (int)(peak_l.read() * 4096);
      float f = (float)constrain(c, 0, 255) / 255.0;
      brightnessWipe(f);
      // Serial.println(f);
    }
  }
}


void loop() {
  if (but_vals[0] == true) {
    playFile("SDTEST1.WAV");  // filenames are always uppercase 8.3 format
    delay(random(500, 3000));
    }
  if (but_vals[1] == true) {
    playFile("SDTEST2.WAV");
    delay(random(500, 3000));
  }
    readUserControls();
    if (but_vals[2] == true) {
    playFile("SDTEST3.WAV");
    delay(random(500, 3000));
  }
  readUserControls();
  if (but_vals[3] == true) {
    playFile("SDTEST4.WAV");
    delay(random(500, 3000));
  }
}
