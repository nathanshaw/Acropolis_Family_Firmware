#ifndef __MODEPITCH_H__
#define __MODEPITCH_H__

#include <Audio.h>
#include <Arduino.h>
#include <PrintUtils.h>
#include "AudioEngine.h"
#include "Neos.h"
#include <WS2812Serial.h>

#define INPUT_START_GAIN      12.0

#define BQL_Q                 0.8
#define BQL_THRESH            200
// logic to link left and right values? todo?
#define BQR_Q                 0.8
#define BQR_THRESH            200

#define PRINT_RMS_VALS        1
#define PRINT_FREQ_VALS       0
#define PRINT_TONE_VALS       0
#define PRINT_FFT_VALS        1

#define FFT_ACTIVE            1
#define PEAK_ACTIVE           0
#define RMS_ACTIVE            1
#define TONE_ACTIVE           0
#define FREQ_ACTIVE           0

#define FREQ_UNCERTANITY_ALLOWED 0.99

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

/////////////////////////////// NeoP ////////////////////////////////////
#define NUM_LED 10
#define LED_PIN 5

byte drawingMemory[NUM_LED * 3];       //  3 bytes per LED
DMAMEM byte displayMemory[NUM_LED * 12]; // 12 bytes per LED

WS2812Serial leds(NUM_LED, displayMemory, drawingMemory, LED_PIN, WS2812_GRB);

NeoGroup neos[2] = {
  NeoGroup(&leds, 0, 4, "Front", MIN_FLASH_TIME, MAX_FLASH_TIME),
  NeoGroup(&leds, 5, 10, "Rear", MIN_FLASH_TIME, MAX_FLASH_TIME)
};

FeatureCollector fc[2] = {FeatureCollector("front"), FeatureCollector("rear")};

AudioInputI2S            i2s;           //xy=70,94.00000095367432
AudioAmplifier           input_amp_f;           //xy=224,88.00000095367432
AudioFilterBiquad        biquad_f;        //xy=386.00000381469727,88.00000095367432
AudioAnalyzeRMS          rms_f;           //xy=570.0000267028809,169.0000057220459
AudioAnalyzeToneDetect   tone_detect_f;          //xy=574.0000305175781,73.00000286102295
AudioAnalyzePeak         peak_f;          //xy=574.0000305175781,105.00000286102295
AudioAnalyzeFFT256       fft256_f;      //xy=580.0000305175781,137.00000381469727
AudioAnalyzeNoteFrequency freq_f;      //xy=584.0000305175781,201.00000476837158
AudioAmplifier           input_amp_r;           //xy=224,88.00000095367432
AudioFilterBiquad        biquad_r;        //xy=386.00000381469727,88.00000095367432
AudioAnalyzeRMS          rms_r;           //xy=570.0000267028809,169.0000057220459
AudioAnalyzeToneDetect   tone_detect_r;          //xy=574.0000305175781,73.00000286102295
AudioAnalyzePeak         peak_r;          //xy=574.0000305175781,105.00000286102295
AudioAnalyzeFFT256       fft256_r;      //xy=580.0000305175781,137.00000381469727
AudioAnalyzeNoteFrequency freq_r;      //xy=584.0000305175781,201.00000476837158
AudioOutputUSB           usb;           //xy=1307.33353805542,1409.3331747055054

AudioConnection          patchCord1(i2s, 0, input_amp_f, 0);
AudioConnection          patchCord2(input_amp_f, 0, biquad_f, 0);
AudioConnection          patchCord5(biquad_f, 0, tone_detect_f, 0);
AudioConnection          patchCord6(biquad_f, 0, peak_f, 0);
AudioConnection          patchCord7(biquad_f, 0, fft256_f, 0);
AudioConnection          patchCord8(biquad_f, 0, rms_f, 0);
AudioConnection          patchCord9(biquad_f, 0, freq_f, 0);
AudioConnection          patchCord10(biquad_f, 0, usb, 0);

AudioConnection          patchCord11(i2s, 1, input_amp_r, 0);
AudioConnection          patchCord12(input_amp_r, 0, biquad_r, 0);
AudioConnection          patchCord15(biquad_r, 0, tone_detect_r, 0);
AudioConnection          patchCord16(biquad_r, 0, peak_r, 0);
AudioConnection          patchCord17(biquad_r, 0, fft256_r, 0);
AudioConnection          patchCord18(biquad_r, 0, rms_r, 0);
AudioConnection          patchCord19(biquad_r, 0, freq_r, 0);
AudioConnection          patchCord13(biquad_r, 0, usb, 1);

void linkFeatureCollector() {
  //front
  Serial.println("Linking Feature Collector for Front and Rear");
  if (RMS_ACTIVE) {
    fc[0].linkRMS(&rms_f);
    fc[1].linkRMS(&rms_r);
  };
  if (PEAK_ACTIVE) {
    fc[0].linkPeak(&peak_f);
    fc[1].linkPeak(&peak_r);
  };
  if (FFT_ACTIVE) {
    fc[0].linkFFT(&fft256_f);
    fc[1].linkFFT(&fft256_r);
  };
  if (TONE_ACTIVE) {
    fc[0].linkTone(&tone_detect_f);
    fc[1].linkTone(&tone_detect_r);
  };
  if (FREQ_ACTIVE) {
    fc[0].linkFreq(&freq_f);
    fc[1].linkFreq(&freq_r);
  };
}

void mothSetup() {
  Serial.println("starting moth setup loop");
  printMinorDivide();
  Serial.println("setting up LEDs");
  leds.begin();
  delay(250);
  neos[0].colorWipe(0, 0, 0); // turn off the LEDs
  neos[1].colorWipe(0, 0, 0); // turn off the LEDs
  // left
  input_amp_f.gain(INPUT_START_GAIN);
  biquad_f.setHighpass(0, BQL_THRESH, BQL_Q);
  biquad_f.setHighpass(1, BQL_THRESH, BQL_Q);
  biquad_f.setHighpass(2, BQL_THRESH, BQL_Q);
  biquad_f.setLowShelf(3, BQL_THRESH , -24);
  // right
  input_amp_r.gain(INPUT_START_GAIN);
  biquad_r.setHighpass(0, BQR_THRESH, BQR_Q);
  biquad_r.setHighpass(1, BQR_THRESH, BQR_Q);
  biquad_r.setHighpass(2, BQR_THRESH, BQR_Q);
  biquad_r.setLowShelf(3, BQR_THRESH, -24);

  // audio features
  if (FREQ_ACTIVE) {
    freq_f.begin(FREQ_UNCERTANITY_ALLOWED);
    freq_r.begin(FREQ_UNCERTANITY_ALLOWED);
  }

  // setup the feature collector
  Serial.println("Starting to link Feature Collector");
  linkFeatureCollector();
  Serial.println("Finished Audio Setup Loop");
  printDivide();
}

uint8_t red[2];
uint8_t green[2];
uint8_t blue[2];

bool getColorFromFFT(FeatureCollector *f, uint8_t &red, uint8_t &green, uint8_t &blue) {
  double red_d, green_d, blue_d, tot;
  red_d   = f->getFFTRange(3, 7);
  green_d = f->getFFTRange(7, 20);
  blue_d  = f->getFFTRange(20, 128);
  tot   = red_d + green_d + blue_d;
  red_d   = red_d / tot;
  green_d = green_d / tot;
  blue_d  = blue_d / tot;
  red = (uint8_t)(red_d * (255.0));
  green = (uint8_t)(green_d * 255.0);
  blue = (uint8_t)(blue_d * 255.0);
}

void printColors() {
  Serial.println("printing the colors");
  for (int i = 0; i < 2; i++)  {
    //Serial.print(f->getName());Serial.print("\t");
    Serial.print("red\t"); Serial.print(red[i]);
    Serial.print("\tgreen\t"); Serial.print(green[i]);
    Serial.print("\tblue\t");
    Serial.println(blue[i]);
  }
}

void mothLoop() {
  // update the feature collectors
  for (int i = 0; i < 2; i++) {
    fc[i].update();
    getColorFromFFT(&fc[i], red[i], green[i], blue[i]);
    neos[i].colorWipe(red[i], green[i], blue[i]);
  }
  // printColors();
  // delay(1000);
}

#endif // mode_pitch_h
