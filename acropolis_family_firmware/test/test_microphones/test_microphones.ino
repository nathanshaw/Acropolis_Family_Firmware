// FFT Test
//
// Compute a 1024 point Fast Fourier Transform (spectrum analysis)
// on audio connected to the Left Line-In pin.  By changing code,
// a synthetic sine wave can be input instead.
//
// The first 40 (of 512) frequency analysis bins are printed to
// the Arduino Serial Monitor.  Viewing the raw data can help you
// understand how the FFT works and what results to expect when
// using the data to control LEDs, motors, or other fun things!
//
// This example code is in the public domain.

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#if ARTEFACT_GENUS == LEGATUS
  const int myInput = AUDIO_INPUT_MIC;
#else
  const int myInput = AUDIO_INPUT_LINEIN;
#endif

// Create the Audio components.  These should be created in the
// order data flows, inputs/sources -> processing -> outputs
//
AudioInputI2S          audioInput;         // audio shield: mic or line-in
AudioAmplifier         gain1;
AudioAnalyzeFFT1024    myFFT;
AudioOutputUSB         audioOutput;        // audio shield: headphones & line-out

// Connect either the live input or synthesized sine wave
AudioConnection patchCord1(audioInput, 0, gain1, 0);
AudioConnection patchCord2(gain1, 0, myFFT, 0);

AudioConnection patchCord3(audioInput, 0, audioOutput, 0);
AudioConnection patchCord4(gain1, 0, audioOutput, 1);
//AudioConnection patchCord1(sinewave, 0, myFFT, 0);

//AudioControlSGTL5000 audioShield;

void setup() {
  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(AUDIO_MEMORY);
  gain1.gain(72.0);

  // Enable the audio shield and set the output volume.

  // Configure the window algorithm to use
  myFFT.windowFunction(AudioWindowHanning1024);
  //myFFT.windowFunction(NULL);

  // Create a synthetic sine wave, for testing
  // To use this, edit the connections above
}

void loop() {
  float n;
  int i;

  if (myFFT.available()) {
    // each time new FFT data is available
    // print it all to the Arduino Serial Monitor
    Serial.print("FFT: ");
    for (i=0; i<40; i++) {
      n = myFFT.read(i);
      if (n >= 0.01) {
        Serial.print(n);
        Serial.print(" ");
      } else {
        Serial.print("  -  "); // don't print "0.00"
      }
    }
    Serial.println();
  }
}
