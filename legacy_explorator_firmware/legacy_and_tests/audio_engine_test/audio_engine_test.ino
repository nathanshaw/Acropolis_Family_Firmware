// for testing the lyre audio engines
  #include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

AudioInputI2S            i2s;            //xy=634,246
AudioAmplifier           amp1;           //xy=777.1429023742676,277.14284896850586
AudioAmplifier           input_amp;      //xy=788,240
AudioFilterBiquad        biquad;         //xy=951.4286575317383,241.42859840393066
AudioFilterBiquad        biquad1;        //xy=952.8571243286133,275.71428298950195
AudioAnalyzeRMS          rms;            //xy=1135.4286575317383,322.42859840393066
AudioAnalyzeToneDetect   tone_detect;    //xy=1139.4286575317383,226.42859840393066
AudioAnalyzePeak         peak;           //xy=1139.4286575317383,258.42859840393066
AudioAnalyzeFFT1024      fft1024;        //xy=1145.4286575317383,290.42859840393066
AudioOutputUSB           usb_output;     //xy=1147.4286575317383,194.42859840393066
AudioAnalyzeNoteFrequency note_freq;      //xy=1149.4286575317383,354.42859840393066
AudioConnection          patchCord1(i2s, 0, input_amp, 0);
AudioConnection          patchCord2(i2s, 1, amp1, 0);
AudioConnection          patchCord3(amp1, biquad1);
AudioConnection          patchCord4(input_amp, biquad);
AudioConnection          patchCord5(biquad, tone_detect);
AudioConnection          patchCord6(biquad, peak);
AudioConnection          patchCord7(biquad, fft1024);
AudioConnection          patchCord8(biquad, rms);
AudioConnection          patchCord9(biquad, note_freq);
AudioConnection          patchCord10(biquad, 0, usb_output, 0);
AudioConnection          patchCord11(biquad1, 0, usb_output, 1);

double peak_val = 0.0;
double last_peak = 0.0;
double peak_delta = 0.0

double rms_val;
double last_rms;
double rms_delta;

double tone_val;
double last_tone;

double freq_val;
double last_freq;

double freq_prob;
double last_freq_prob; // not sure I need this...

int fft_bin_max; //  the bin number which contains the most energy...

double onsetNovelty() {
  double novelty;
  // the signal should determine if an onset is detected
  return novelty;
}

void gatherAudioFeatures() {
  // this function will collect and store all the available audio features
  if (peak.available()) {
    last_peak = peak_val;
    peak_val = peak.read() * 1000.0;
    peak_delta = abs(last_peak - peak_val);
    Serial.print("p - ");
    Serial.print(peak_val, 6);
  }
  // rms
  if (rms.available()) {
    last_rms = rms_val;
    rms_val = (double)rms.read() * 1000.0;
    rms_delta = abs(last_rms - rms_val);
    Serial.print("\trms - ");
    Serial.println(rms_val, 6);
  }
  // tone detect
  if (tone_detect.available()) {
    last_tone = tone_val;
    tone_val = tone_detect.read();
    // tone_delta = abs(last_tone - tone_val);
    Serial.print("tone - ");
    Serial.println(tone_val);
  }
  // frequency
  if (note_freq.available()) {
    last_freq = freq_val;
    freq_val = note_freq.read();
    last_freq_prob = freq_prob;
    freq_prob = note_freq.probability();
    Serial.print("freq - ");
    Serial.print(freq_val);
    Serial.print(" freq prob: ");
    Serial.println(freq_prob);
  }
  // FFT
  /* TODO
    if (fft.available()) {

    }
  */
  // Serial.println();
}
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

/*
  public class Song() {
  // class to store the contents of a song which can be recalled and then played black when deemed suitable
  // contains a list of onsets along with some added information?
  }
  public class Onset() {
  // class to store onsets, should instead be a structure which is contained within a class for rhythms/songs?
  unsigned long when; // when did the onset occur?
  double amp; // how loud was this onset?
  unsigned long dur; // how long did this note last?
  }
*/

void printAudioFeatures() {
  Serial.print("peak: "); Serial.print(peak_val);
  Serial.print("\trms: "); Serial.print(rms_val);
  Serial.print("\ttone: "); Serial.print(tone_val);
  Serial.println();
}

#define AUDIO_MEMORY 24

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


void setup() {
  // put your setup code here, to run once:
  AudioMemory(24);
}

#define NOVELTY_THRESH 1.0

void loop() {
  // put your main code here, to run repeatedly:
  gatherAudioFeatures();
  if (onsetNovelty() > NOVELTY_THRESH) {
    
    }
  }
}
