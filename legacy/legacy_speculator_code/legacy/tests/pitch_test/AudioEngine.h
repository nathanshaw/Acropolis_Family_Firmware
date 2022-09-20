#ifndef __AUDIOENGINE_H__
#define __AUDIOENGINE_H__

#define USE_SCALED_FFT 1
#include "Configuration.h"

class FeatureCollector {
  public:
    FeatureCollector(String _id);
    void printFeatures();
    String getName(){return id;};
    //////////////// RMS /////////////////////////
    void linkRMS(AudioAnalyzeRMS *r) {
      rms_ana = r;
      rms_active = true;
    };
    double getRMS();
    double getRMSPosDelta();
    void printRMSVals();

    //////////////// Peak /////////////////////////
    void linkPeak(AudioAnalyzePeak *r) {
      peak_ana = r;
      peak_active = true;
    };
    double getPeakVal();
    double getPeakPosDelta();
    void printPeakVals();

    //////////////// Tone /////////////////////////
    void linkTone(AudioAnalyzeToneDetect *r) {
      tone_ana = r;
      tone_active = true;
    };
    double getToneLevel();
    void printToneVals();

    //////////////// Freq /////////////////////////
    void linkFreq(AudioAnalyzeNoteFrequency *r) {
      freq_ana = r;
      freq_active = true;
    };
    double getFreq();
    void printFreqVals();

    //////////////// FFT /////////////////////////
    void linkFFT(AudioAnalyzeFFT256 *r) {
      fft_ana = r;
      fft_active = true;
    };
    void printFFTVals();
    double getFFTRange(uint8_t s, uint8_t e);

    //////////////// General ///////////////////////
    void update();

  private:
    String id = "";

    //////////////// RMS /////////////////////////
    AudioAnalyzeRMS *rms_ana;
    bool rms_active = false;
    void calculateRMS();
    double rms_val;
    double rms_pos_delta;

    //////////////// Peak /////////////////////////
    AudioAnalyzePeak *peak_ana;
    bool peak_active = false;
    void calculatePeak();
    double peak_val;
    double peak_pos_delta;

    //////////////// Tone /////////////////////////
    AudioAnalyzeToneDetect *tone_ana;
    bool tone_active = false;
    void calculateTone();
    double tone_level = 0;

    //////////////// Freq /////////////////////////
    AudioAnalyzeNoteFrequency *freq_ana;
    bool freq_active = false;
    double freq_val;
    double freq_prob;
    void calculateFreq();

    //////////////// FFT /////////////////////////
    AudioAnalyzeFFT256 *fft_ana;
    bool fft_active = false;
    double fft_vals[128];
    void calculateFFT();
    void calculateScaledFFT();
};

class AutoGain {
  public:
    AutoGain(AudioAnalyzeRMS *r, AudioAnalyzePeak *p);
    void update();
  private:
    FeatureCollector fc;

};

//////////////// Update Functions ///////////////////////////////
void FeatureCollector::calculateFFT() {
  if (fft_active && fft_ana->available()) {
    Serial.println("FFT Available");
    for (int i = 0; i < 128; i++) {
      fft_vals[i] = fft_ana->read(i);
    }
  }
}

void FeatureCollector::calculateScaledFFT() {
  if (fft_active && fft_ana->available()) {
    Serial.println("FFT Available");
    double tot;
    for (int i = 0; i < 128; i++) {
      fft_vals[i] = fft_ana->read(i);
      tot += fft_vals[i];
    }
    for (int i = 0; i < 128; i++) {
      fft_vals[i] = fft_vals[i] / tot;
    }
  }
}

void FeatureCollector::calculateTone() {
  if (tone_active && tone_ana->available()) {
    tone_level = tone_ana->read();
  }
}

void FeatureCollector::calculateFreq() {
  if (freq_active  && freq_ana->available()) {
    freq_val = freq_ana->read();
    freq_prob = freq_ana->probability();
  }
}

void FeatureCollector::calculatePeak() {
  if (peak_active && peak_ana->available()) {
    peak_val =  peak_ana->read();
  }
}

void FeatureCollector::calculateRMS() {
  if (rms_active  && rms_ana->available()) {
    rms_val = rms_ana->read();
  }
}



/*
  int getIndexWithHighestVal(double &vals) {
  double m = 0.0;
  int index = 0;  for (int i = 0; i < sizeof(vals)/sizeof(vals[0]); i++) {
    if (vals[i] > m) {
      m = vals[i];
      index = i;
    }
  }
  return index;
  }
*/

///////////////////// Getter functions ///////////////////////////////
double FeatureCollector::getFFTRange(uint8_t s, uint8_t e) {
  if (fft_active) {
    return fft_ana->read(s, e);
  }
  return -1.0;
}

double FeatureCollector::getRMS() {
  if (rms_active) {
    return rms_val;
  }
  Serial.println("ERROR  - RMS IS NOT AN ACTIVE AUDIO FEATURE : "); Serial.println(id);
  return -1.0;

}

double FeatureCollector::getPeakVal() {
  if (peak_active) {
    return peak_val;
  }
  Serial.println("ERROR  - Peak IS NOT AN ACTIVE AUDIO FEATURE : "); Serial.println(id);
  return -1.0;

}

double FeatureCollector::getFreq() {
  if (freq_active) {
    return freq_val;
  }
  Serial.println("ERROR  - Freq IS NOT AN ACTIVE AUDIO FEATURE : "); Serial.println(id);
  return -1.0;

}

double FeatureCollector::getToneLevel() {
  if (tone_active) {
    return tone_level;
  }
  Serial.println("ERROR  - Peak IS NOT AN ACTIVE AUDIO FEATURE : "); Serial.println(id);
  return -1;

}

//////////////////////////////// Print Functions /////////////////////////////////////////

void FeatureCollector::printFeatures() {
  // printMajorDivide((String)(id + " Features "));
  if (rms_active && PRINT_RMS_VALS) {
    printRMSVals();
  };
  if (peak_active && PRINT_PEAK_VALS) {
    printPeakVals();
  };
  if (freq_active && PRINT_FREQ_VALS) {
    printFreqVals();
  };
  if (tone_active && PRINT_TONE_VALS) {
    printToneVals();
  };
  if (fft_active && PRINT_FFT_VALS) {
    printFFTVals();
  };
  // printDivide();
}

void FeatureCollector::printToneVals() {
  if (tone_active > 0) {
    Serial.print(id);
    Serial.print(" tone_level :\t");
    Serial.println(tone_level);
  }
}

void FeatureCollector::printFFTVals() {
  if (fft_active) {
    Serial.print(id); Serial.println(" FFT vals");
    uint8_t w = 8;
    for (int l  = 0; l < w; l++) {
      Serial.println();
      Serial.print(l); Serial.print("\t");
      for (int i = l; i < 128; i = i + w) {
        if (i != l) {Serial.print(", ");};
        Serial.print(fft_vals[i]);
      }
    }
    Serial.println();
  }
}

void FeatureCollector::printFreqVals() {
  if (freq_active > 0) {
    Serial.print(id); Serial.print("\tnote_freq prob/freq:\t");
    Serial.print(freq_prob); printTab();
    Serial.println(freq_val);
  }
}

void FeatureCollector::printRMSVals() {
  if (rms_active > 0) {
    Serial.print(id); Serial.println(" RMS vals\t");
    Serial.println(rms_val);
  }
}


void FeatureCollector::printPeakVals() {
  if (peak_active > 0) {
    Serial.print(id); Serial.println(" Peak vals\t");
    Serial.println(peak_val);
  }
}

/////////////////////////////////// UPDATE / INIT //////////////////////////////////////

void FeatureCollector::update() {
  if (USE_SCALED_FFT) {
    calculateScaledFFT();
  }
  else {
    calculateFFT();
  }

  calculateTone();
  calculatePeak();
  calculateRMS();
  calculateFreq();
  printFeatures();
}

FeatureCollector::FeatureCollector(String _id) {
  id = _id;
}

#endif // __AUDIO_ENGINE_H__
