#ifndef __FFTManager1024_H__
#define __FFTManager1024_H__

#include "audio_utils.h"

#define NUM_FFT_BINS 512

#ifndef PRINT_FLUX_VALS 
#define PRINT_FLUX_VALS 0
#endif

#ifndef PRINT_FFT_DEBUG
#define PRINT_FFT_DEBUG 0
#endif

#ifndef PRINT_CENTROID_VALS
#define PRINT_CENTROID_VALS 0
#endif

#ifndef SMOOTH_CENTROID 
#define SMOOTH_CENTROID 0
#endif

#ifndef PRINT_FFT_VALS
#define PRINT_FFT_VALS 0 
#endif

class FFTManager1024 {
    public:
        //////////// init ///////////////
        FFTManager1024(String _name);
        void linkFFT(AudioAnalyzeFFT1024*r);

        // printers
        void   printFFTVals();

        // getters
        double getFFTRangeByIdx(uint16_t s, uint16_t e);
        double getFFTRangeByFreq(uint32_t s, uint32_t e);
        int    getHighestEnergyBin();
        int    getHighestEnergyBin(int start, int end);
        double getRelativeEnergy(uint16_t);
        double getFFTTotalEnergy();
        double getRelativeBinPos() {return relative_bin_pos;};

        double getCentroid();
        double getLastCentroid(){return last_centroid;};;
        double getCentroid(uint16_t min, uint16_t max);
        double getCentroidDelta();
        double getCentroidPosDelta();
        double getCentroidNegDelta();

        double getSpectralFlux();

        // setters
        void setCalculateCentroid(bool v) {calculate_centroid = v;};
        void setCentroidActive(bool s) { calculate_centroid = s;}; 

        void setCalculateFlux(bool v) {calculate_flux= v;};
        void setFluxActive(bool s) { calculate_flux = s;}; 

        void setFFTScaler(double s) {fft_scaler = s;};

        void calculateSpectralFlux();

    private:
        String name = "";
        bool fft_active = true;
        AudioAnalyzeFFT1024*fft_ana;

        double fft_vals[NUM_FFT_BINS];
        double last_fft_vals[NUM_FFT_BINS];
        double fft_scaler = 1.0;

        void calculateFFT();

        double fft_tot_energy = 0.0;
        double relative_bin_pos = 0.0;

        uint16_t max_bin = NUM_FFT_BINS;// what is the highest index bin that we care about?
        uint16_t min_bin = 0;// what is the lowest index bin that we care about?

        bool calculate_centroid = false;
        double calculateCentroid();
        double centroid = 0.0;
        double last_centroid = 0.0;

        bool calculate_flux = false;
        double calculateFlux();
        double flux = 0.0;
        elapsedMillis last_fft_reading;
};

FFTManager1024::FFTManager1024(String _id) {
    name = _id;
}

uint32_t getBinsMidFreq256(int bin) {
    return (uint32_t)(bin * 172 + 86);
}

uint32_t getBinsMidFreq1024(int bin) {
    return (uint32_t)(bin * 43 + 22);
}

void printFreqRangeOfBin256(int idx) {
    Serial.print(idx * 172);
    Serial.print(" - ");
    Serial.println((idx + 1) * 172);
}

void printFreqRangeOfBin1024(int idx) {
    Serial.print(idx * 43);
    Serial.print(" - ");
    Serial.println((idx + 1) * 43);
}

void FFTManager1024::printFFTVals() {
    if (fft_active && (PRINT_FFT_VALS || PRINT_FLUX_VALS || PRINT_CENTROID_VALS)) {
        Serial.print(name); Serial.print(" FFT vals\t");
        if (PRINT_FFT_VALS) {
            // if (USE_SCALED_FFT) {Serial.print("Scaled ");}
            uint8_t w = 12;
            for (int l  = 0; l < w; l++) {
                Serial.println();
                Serial.print(l+min_bin); Serial.print("\t");
                for (int i = l; i < NUM_FFT_BINS; i = i + w) {
                    if (i != l) {
                        Serial.print(", ");
                        Serial.print(i);
                        Serial.print(":");
                    };
                    Serial.print(fft_vals[i], 5);
                }
            }
        }
    }
    /*
    if (PRINT_HIGHEST_ENERGY_BIN) {
        Serial.print("Bin with highest energy: "); Serial.print(highest_energy_idx);Serial.print(" = ");
        Serial.print(fft_vals[highest_energy_idx]);
        printFreqRangeOfBin(highest_energy_idx, max_bin);
    }
    */
    if (calculate_flux == true && PRINT_FLUX_VALS) {
        Serial.print("flux: ");Serial.print(flux);Serial.println();
    }
    if (calculate_centroid == true && PRINT_CENTROID_VALS) {
        Serial.print("centroid: ");Serial.println(centroid);
    }
}

void FFTManager1024::linkFFT(AudioAnalyzeFFT1024*r) {
    fft_ana = r;
    fft_ana->averageTogether(4); // average together the readings from 10 FFT's before available() returns true, normally produces over 300 fft per second, this will be closer to 30
    fft_active = true;
};

double FFTManager1024::getRelativeEnergy(uint16_t idx) {
    calculateFFT();
    if (fft_tot_energy > 0) {
        double val = 0.0;
        val = fft_vals[idx] / fft_tot_energy;
        // Serial.println(val);
        return val;
    }
    return 0.0;
}

double FFTManager1024::getFFTTotalEnergy() {
    calculateFFT();
    if (fft_active) {
        return fft_tot_energy;
    }
    Serial.println("ERROR  - FFT IS NOT AN ACTIVE AUDIO FEATURE : "); Serial.println(name);
    return -1.0;
}


double FFTManager1024::getFFTRangeByIdx(uint16_t s, uint16_t e) {
    calculateFFT();
    if (fft_active) {
        return fft_ana->read(s, e);
    }
    return -1.0;
}

double FFTManager1024::getFFTRangeByFreq(uint32_t s, uint32_t e) {
    calculateFFT();
    if (fft_active) {
        uint16_t start_idx = (uint16_t)(s / 43);
        uint16_t end_idx = (uint16_t)(e / 43);
        return fft_ana->read(start_idx, end_idx);
    }
    return -1.0;
}

double FFTManager1024::calculateFlux() {
    double f = 0.0;
    if (last_fft_vals[0] != 0) {
        for (int i = 2; i < NUM_FFT_BINS; i++) {
            f += pow((fft_vals[i] - last_fft_vals[i]), 2);
        }
        if (f != 0.0) {
            flux = f;
        }
    }
    else {
        dprintln(PRINT_FFT_DEBUG, "last_fft_vals[0] is equal to zero");
    }
    return flux;
}

double FFTManager1024::getSpectralFlux() {
    calculateFFT();
    dprint(PRINT_FLUX_VALS, "flux: ");
    dprintln(PRINT_FLUX_VALS, flux);
    return flux;
}

/////////////// Calculate Features //////////////////////////////
double FFTManager1024::calculateCentroid() {
    double mags = 0.0;
    for (int i = 1; i < NUM_FFT_BINS; i++) {
        // take the magnitude of all the bins
        // and multiply if by the mid frequency of the bin
        // then all it to the total cent value
        // we also have to remove the effect the FTT_SCALER has on the bins stored energy
        mags += fft_vals[i] * getBinsMidFreq1024(i) / fft_scaler;
    }
    last_centroid = centroid;
    centroid = mags;
    dprint(PRINT_CENTROID_VALS, "centroid : ");
    dprintln(PRINT_CENTROID_VALS, centroid);
    return centroid;
}

double FFTManager1024::getCentroidDelta() {
    return centroid - last_centroid;
}

double FFTManager1024::getCentroidPosDelta() {
    if (last_centroid < centroid) {
        return centroid - last_centroid;
    }
    return 0.0;
}

double FFTManager1024::getCentroidNegDelta() {
    if (last_centroid > centroid) {
        return last_centroid - centroid;
    }
    return 0.0;
}

double FFTManager1024::getCentroid() {
    calculateFFT();
    return centroid;
}

double FFTManager1024::getCentroid(uint16_t min, uint16_t max) {
    calculateFFT();
    double mags = 0.0;
    for (int i = min; i < max; i++) {
        // take the magnitude of all the bins
        // and multiply if by the mid frequency of the bin
        // then all it to the total cent value
        mags += fft_vals[i] * getBinsMidFreq1024(i);
    }
    dprint(PRINT_CENTROID_VALS, "centroid : ");
    dprintln(PRINT_CENTROID_VALS, mags);
    return mags;
}

void FFTManager1024::calculateFFT() {
    if (fft_active && fft_ana->available() == true) {
        dprint(PRINT_FFT_DEBUG, name);
        dprint(PRINT_FFT_DEBUG, " FFT Available, ");
        dprint(PRINT_FFT_DEBUG, " last FFT reading was ");
        dprint(PRINT_FFT_DEBUG, (uint32_t)last_fft_reading);
        dprintln(PRINT_FFT_DEBUG, " ms ago");
        fft_tot_energy = 0.0;
        if (calculate_flux == true) { // only do this if we need to in order to save time
            for (int i = 0; i < NUM_FFT_BINS; i++) {
                last_fft_vals[i] = fft_vals[i];
            }
        }
        for (int i = 0; i < NUM_FFT_BINS; i++) {
            fft_vals[i] = fft_ana->read(i);
        }
        for (int i = 0; i < NUM_FFT_BINS; i++) {
            fft_vals[i] *= fft_scaler;
            fft_tot_energy += fft_vals[i];
        }
        if (calculate_centroid == true) {calculateCentroid();};
        if (calculate_centroid == true && SMOOTH_CENTROID == true) {
            centroid = (centroid + last_centroid) * 0.5;
        };
        if (calculate_flux == true) {calculateFlux();};
        if (PRINT_FFT_DEBUG) {
          printFFTVals();
        }
        last_fft_reading = 0;
    }
}

double getHighestEnergyIdx(double array[], int start, int end) {
    int highest = -1;
    double h_val = 0.0;
    for (int i = start; i <= end ; i++) {
        if (array[i] > h_val){
            highest = i;
            h_val = array[i];
        }
    }
    return highest;
}

#endif // feature_Collector_h
