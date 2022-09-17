#ifndef __FEATURE_COLLECTOR_H__
#define __FEATURE_COLLECTOR_H__

#include "audio_utils.h"
#include <Audio.h>

#ifndef RMS_LOG_RESET_MIN
#define RMS_LOG_RESET_MIN 2000
#endif

#ifndef PEAK_LOG_RESET_MIN
#define PEAK_LOG_RESET_MIN 2000
#endif

#ifndef MICROPHONE_TEST_DURATION
#define MICROPHONE_TEST_DURATION 2000
#endif

#ifndef FC_UPDATE_RATE
#define FC_UPDATE_RATE 33
#endif

#ifndef PRINT_PEAK_DEBUG
#define PRINT_PEAK_DEBUG 0
#endif

#ifndef PRINT_RMS_DEBUG
#define PRINT_RMS_DEBUG 0
#endif

class FeatureCollector {
    public:
        FeatureCollector(String _id);
        /////////////////// Microphone Testing /////////////////
        bool testMicrophone();

        /////////////////// Printing ///////////////////////////
        void printFeatures();

        ////////////////// Getting Functions ///////////////////
        String getName() {return name;};
        bool isActive() {return microphone_active;};

        //////////////// Gain Tracking /////////////////////////
        double gain = 1.0;
        double min_gain = 1.0;
        double max_gain = 1.0;
        void updateGain(double g);
        bool ampActive() {return amp_active;};
        void linkAmplifier(AudioAmplifier * amp, double low, double high);

        //////////////// RMS /////////////////////////
        void linkRMS(AudioAnalyzeRMS *r, double s, bool print);
        double getRMS();
        double getRMSPosDelta(){return rms_pos_delta;};
        double getRMSAvg();
        bool   isRMSActive(){ return rms_active;};
        void   resetRMSAvgLog();
        void   resetRmsAvgLog();
        void   printRMSVals();

        //////////////// Peak /////////////////////////
        void   linkPeak(AudioAnalyzePeak *r, double s, bool print);
        bool   isPeakActive(){ return peak_active;};
        double getPeak();
        double getPeakPosDelta() {calculatePeak();return peak_pos_delta;};
        double getPeakAvg();
        double getPeakMin(){return peak_min;};
        double getPeakMax(){return peak_max;};
        void   resetPeakAvgLog();
        void   printPeakVals();
        double peak_pos_delta;

        //////////////// General ///////////////////////
        void update();

        //////////////// Printing ///////////////////////
        void autoPrintRMS(bool p) {print_rms = p;};
        void autoPrintPeak(bool p) {print_peak= p;};

    private:
        ////////////////// Printing //////////////////////
        bool print_rms      = false;
        bool print_peak     = false;

        /////////////// Testing Functions ////////////////
        bool testMicrophoneRMS();
        bool testMicrophonePeak();

        /////////////// General //////////////////////////

        String name = "";
        bool microphone_active = true;
        elapsedMillis last_update_timer;

        //////////////// Gain Tracking ///////////////
        AudioAmplifier *amp_ana[4];
        bool amp_active = false;
        uint8_t audio_amp_add_idx = 0;
        bool gain_tracking_active = false;
        double gain_lower_limit;
        double gain_upper_limit;
        // TODO, make it so linking of gains and tracking is all dones through fc
        // AudioAmplifier *gain_ana[4];

        //////////////// RMS /////////////////////////
        AudioAnalyzeRMS *rms_ana;
        bool rms_active = false;
        void calculateRMS();
        double rms_val;
        double rms_totals;
        unsigned long rms_readings;
        double rms_scaler = 1.0;
        double rms_pos_delta;
        elapsedMillis last_rms_reset;

        //////////////// Peak /////////////////////////
        AudioAnalyzePeak *peak_ana;
        bool peak_active = false;
        void calculatePeak();
        double peak_val;
        double peak_scaler = 1.0;

        double peak_min = 9999.9;
        double peak_max = 0.0;

        double peak_totals = 0;
        unsigned long peak_readings = 0;
        elapsedMillis last_peak_reset;
};

void FeatureCollector::linkPeak(AudioAnalyzePeak *r, double s, bool print) {
    print_peak = print;
    peak_ana = r;
    peak_active = true;
    peak_scaler = s;
};

void FeatureCollector::linkRMS(AudioAnalyzeRMS *r, double s, bool print) {
    print_rms = print;
    rms_ana = r;
    rms_active = true;
    rms_scaler = s;
};

void FeatureCollector::linkAmplifier(AudioAmplifier * amp, double low, double high) { 
    gain_lower_limit = low;
    gain_upper_limit = high;
    if (audio_amp_add_idx < 4) {
        Serial.print("Linked an audio amplifier ");Serial.print(audio_amp_add_idx);printTab();
        amp_ana[audio_amp_add_idx] = amp;
        gain_tracking_active = true;
        audio_amp_add_idx = audio_amp_add_idx + 1;
        Serial.println(audio_amp_add_idx);
    }
    else {
        Serial.println("ERROR, can't link audio amplifier, there are not enough available slots");
    }
}

FeatureCollector::FeatureCollector(String _id) {
    name = _id;
}


void FeatureCollector::updateGain(double g) {
    gain = g;
    if (gain > max_gain) {
        max_gain = gain;
    }
    if (gain < min_gain) {
        min_gain = gain;
    }
    if (gain > gain_upper_limit) {
        gain = gain_upper_limit;
    }
    else if (gain < gain_lower_limit) {
        gain = gain_lower_limit;
    }
    for (int i =  0; i < audio_amp_add_idx; i++) {
        amp_ana[i]->gain(g);
    }
}

bool FeatureCollector::testMicrophoneRMS() {
    // go through and gather 10 features from each channel and make sure it is picking up audio
    uint8_t readings = 0;
    double values = 0.0;
    unsigned long a_time = millis();
    Serial.print("Testing ");Serial.print(name);Serial.println(" Microphone using RMS");
    while (readings < 10 && millis() < a_time + MICROPHONE_TEST_DURATION) {
        if (rms_ana->available()) {
            values += rms_ana->read();
            readings++;
            Serial.print(". ");
            delay(20);
        }
    }
    if (values > 0) {
        Serial.println();
        Serial.print(name);
        Serial.println(" Microphone is good");
        microphone_active = true;
        return true;
    } 
    Serial.println("\nERROR, ");
    Serial.print(name);Serial.println(" Microphone does not work");
    printDivideLn();
    microphone_active = false;
    return false;
}

bool FeatureCollector::testMicrophonePeak() {
    // go through and gather 10 features from each channel and make sure it is picking up audio
    uint8_t readings = 0;
    double values = 0.0;
    unsigned long a_time = millis();
    printDivide();
    Serial.print("Testing ");Serial.print(name);Serial.println(" Microphone using Peak");
    while (readings < 10 && millis() < a_time + MICROPHONE_TEST_DURATION) {
        if (peak_ana->available()) {
            values += peak_ana->read();
            readings++;
            Serial.print(". ");
            delay(20);
        }
    }
    if (values > 0) {
        Serial.println();
        Serial.print(name);
        Serial.println(" Microphone is good");
        microphone_active = true;
        return true;
    } 
    Serial.println("\nERROR, ");
    Serial.print(name);Serial.println(" Microphone does not work");
    printMinorDivideLn();
    microphone_active = false;
    return false;
}

bool FeatureCollector::testMicrophone () {
    if (rms_active) {
        return testMicrophoneRMS();
    } else if (peak_active) {
        return testMicrophonePeak();
    } else {
        Serial.println("Sorry unable to test microphone as neither the RMS or Peak feature is active");
        return false;
    }
}


//////////////// Update Functions ///////////////////////////////
void FeatureCollector::calculatePeak() {
    bool avail = peak_ana->available();
    if (peak_active && avail) {
        double last = peak_val;
        peak_val =  peak_ana->read();
        dprint(PRINT_PEAK_DEBUG, name);
        dprint(PRINT_PEAK_DEBUG, " Peaks (normal, scaled, pos_delta):\t");
        dprint(PRINT_PEAK_DEBUG, peak_val);
        peak_val *= peak_scaler;
        dprint(PRINT_PEAK_DEBUG, "\t");
        dprint(PRINT_PEAK_DEBUG, peak_val);
        peak_pos_delta = getPosDelta(last, peak_val);
        dprint(PRINT_PEAK_DEBUG, "\t");
        dprintln(PRINT_PEAK_DEBUG, peak_pos_delta);
        peak_totals += peak_val;
        peak_readings++;
        if (peak_val > peak_max) {
            peak_max = peak_val;
        } else if (peak_val < peak_min) {
            peak_min = peak_val;
        }
    }
}

void FeatureCollector::resetPeakAvgLog() {
    if (last_peak_reset > PEAK_LOG_RESET_MIN) {
        peak_totals = 0.0;
        peak_readings = 0;
        last_peak_reset = 0;
    }
}

void FeatureCollector::calculateRMS() {
    if (rms_active  && (rms_ana->available())) {
            double _rms = rms_ana->read() * rms_scaler;
        if (_rms > 0.0) {
            double temp = rms_val;
            rms_val = _rms;
            rms_pos_delta = getPosDelta(temp, rms_val);
            rms_totals += rms_val;
            rms_readings++;
        } else {
            dprintln(PRINT_RMS_DEBUG, "WARNING RMS is equal to 0");
        }
    }
}

void FeatureCollector::resetRMSAvgLog() {
    if (last_rms_reset > RMS_LOG_RESET_MIN) {
        rms_totals = 0.0;
        rms_readings = 0;
        last_rms_reset = 0;
    }
}

///////////////////// Getter functions ///////////////////////////////

double FeatureCollector::getRMS() {
    if (rms_active) {
        return rms_val;
    }
    Serial.println("ERROR  - RMS IS NOT AN ACTIVE AUDIO FEATURE : "); Serial.println(name);
    return -1.0;
}

double FeatureCollector::getPeak() {
    if (peak_active) {
        return peak_val;
    }
    Serial.println("ERROR  - Peak IS NOT AN ACTIVE AUDIO FEATURE : "); Serial.println(name);
    return -1.0;
}

double FeatureCollector::getPeakAvg() {
    if (peak_readings > 0 && peak_totals > 0) {
        return ((double)peak_totals / (double)peak_readings);
    }
    return peak_val;
}

double FeatureCollector::getRMSAvg() {
    if (rms_readings > 0 && rms_totals > 0) {
        return ((double)rms_totals / (double)rms_readings);
    }
    return rms_val;
}

//////////////////////////////// Print Functions /////////////////////////////////////////
void FeatureCollector::printFeatures() {
    if (rms_active && print_rms) {
        printRMSVals();
    };
    if (peak_active && print_peak) {
        printPeakVals();
    };
}

void FeatureCollector::printRMSVals() {
    if (rms_active > 0) {
        Serial.print(name); Serial.print(" RMS vals\t");
        Serial.print(rms_val);printTab();
        Serial.print("delta\t");Serial.print(rms_pos_delta);
        Serial.print(" average\t");Serial.println(getRMSAvg());
    }
}

void FeatureCollector::printPeakVals() {
    if (peak_active > 0) {
        Serial.print(name); Serial.print(" Peak vals\t");
        Serial.print(peak_val);printTab();
        Serial.print("delta\t");Serial.print(peak_pos_delta);
        Serial.print(" average\t");Serial.println(getPeakAvg());
    }
}

/////////////////////////////////// UPDATE / INIT //////////////////////////////////////
void FeatureCollector::update() {
    if (microphone_active == true) {
        if (last_update_timer > FC_UPDATE_RATE) {
            last_update_timer = 0;
            calculateRMS();
            calculatePeak();
            printFeatures();
        }
    }
    else {
        if (last_update_timer > 3000){
            Serial.print(name);
            Serial.println(" sorry the microphone does not work, not updating the feature collector");
            last_update_timer = 0;
        }
    }
}
#endif
