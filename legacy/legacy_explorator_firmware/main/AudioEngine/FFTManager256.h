#ifndef __FFTManager256_H__
#define __FFTManager256_H__

#include "audio_utils.h"

class FFTManager256 {
    public:
        //////////// init ///////////////
        FFTManager256(String _name);
        void linkFFT(AudioAnalyzeFFT256*r);

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

        double getSpectralFlux();

        // setters
        void setCalculateCentroid(bool v) {calculate_centroid = v;};
        void setCalculateFlux(bool v) {calculate_flux= v;};
        void setCentroidActive(bool s) { calculate_centroid = s;}; 
        void setFluxActive(bool s) { calculate_flux = s;}; 
        void setFFTScaler(double s) {fft_scaler = s;};

        void calculateSpectralFlux();

    private:
        String name = "";
        bool fft_active = false;
        AudioAnalyzeFFT256*fft_ana;

        uint32_t getBinsMidFreq256(int bin);

        uint16_t fft_num_bins = 127;
        double fft_vals[127];
        double last_fft_vals[127];
        double fft_scaler = 1.0;

        void calculateFFT();

        double fft_tot_energy = 0.0;
        double relative_bin_pos = 0.0;
        
        uint16_t max_bin = 127;// what is the highest index bin that we care about?
        uint16_t min_bin = 0;// what is the lowest index bin that we care about?

        bool calculate_centroid = false;
        double calculateCentroid();
        double centroid = 0.0;
        double last_centroid = 0.0;

        bool calculate_flux = false;
        double calculateFlux();
        double flux = 0.0;
        // elapsedMillis last_fft_reading;
};

FFTManager::FFTManager(String _id) {
    name = _id;
}

uint32_t FFTManager::getBinsMidFreq256(int bin) {
    return (uint32_t)(bin * 172 + 86);
}

void printFreqRangeOfBin(int idx, int max_idx) {
    Serial.print(idx * 172);
    Serial.print(" - ");
    Serial.println((idx + 1) * 172);
}

void FFTManager::printFFTVals() {
    if (fft_active && (PRINT_FFT_VALS || PRINT_FLUX_VALS || PRINT_CENTROID_VALS)) {
        Serial.print(name); Serial.print(" FFT vals\t");
        if (PRINT_FFT_VALS) {
            // if (USE_SCALED_FFT) {Serial.print("Scaled ");}
            uint8_t w = 12;
            for (int l  = 0; l < w; l++) {
                Serial.println();
                Serial.print(l+min_bin); Serial.print("\t");
                for (int i = l; i < 127; i = i + w) {
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

void FFTManager::linkFFT(AudioAnalyzeFFT256*r) {
    fft_ana = r;
    fft_ana->averageTogether(10); // average together the readings from 10 FFT's before available() returns true, normally produces over 300 fft per second, this will be closer to 30
    fft_active = true;
};

double FFTManager::getRelativeEnergy(uint16_t idx) {
    calculateFFT();
    if (fft_tot_energy > 0) {
        double val = 0.0;
        val = fft_vals[idx] / fft_tot_energy;
        // Serial.println(val);
        return val;
    }
    return 0.0;
}

double FFTManager::getFFTTotalEnergy() {
    calculateFFT();
    if (fft_active) {
        return fft_tot_energy;
    }
    Serial.println("ERROR  - FFT IS NOT AN ACTIVE AUDIO FEATURE : "); Serial.println(name);
    return -1.0;
}


double FFTManager::getFFTRangeByIdx(uint16_t s, uint16_t e) {
    calculateFFT();
    if (fft_active) {
        return fft_ana->read(s, e);
    }
    return -1.0;
}

double FFTManager::getFFTRangeByFreq(uint32_t s, uint32_t e) {
    calculateFFT();
    if (fft_active) {
        uint16_t start_idx = (uint16_t)(s / 172);
        uint16_t end_idx = (uint16_t)(e / 172);
        return fft_ana->read(start_idx, end_idx);
    }
    return -1.0;
}

double FFTManager::calculateFlux() {
    double f = 0.0;
    if (last_fft_vals[0] != 0) {
        for (int i = 2; i < 127; i++) {
            f += pow((fft_vals[i] - last_fft_vals[i]), 2);
        }
        if (f != 0.0) {
            flux = f;
        }
    }
    else {
        Serial.print("last_fft_vals[0] is equal to zero");
    }
    return flux;
}

double FFTManager::getSpectralFlux() {
    calculateFFT();
    dprint(PRINT_FLUX_VALS, "flux: ");
    dprintln(PRINT_FLUX_VALS, flux);
    return flux;
}

/////////////// Calculate Features //////////////////////////////
double FFTManager::calculateCentroid() {
    double mags = 0.0;
    for (int i = 1; i < 127; i++) {
        // take the magnitude of all the bins
        // and multiply if by the mid frequency of the bin
        // then all it to the total cent value
        // we also have to remove the effect the FTT_SCALER has on the bins stored energy
        mags += fft_vals[i] * getBinsMidFreq256(i) / fft_scaler;
    }
    last_centroid = centroid;
    centroid = mags;
    dprint(PRINT_CENTROID_VALS, "centroid : ");
    dprintln(PRINT_CENTROID_VALS, centroid);
    return centroid;
}

double FFTManager::getCentroid() {
    calculateFFT();
    return centroid;
}

double FFTManager::getCentroid(uint16_t min, uint16_t max) {
    calculateFFT();
    double mags = 0.0;
    for (int i = min; i < max; i++) {
        // take the magnitude of all the bins
        // and multiply if by the mid frequency of the bin
        // then all it to the total cent value
        mags += fft_vals[i] * getBinsMidFreq256(i);
    }
    dprint(PRINT_CENTROID_VALS, "centroid : ");
    dprintln(PRINT_CENTROID_VALS, mags);
    return mags;
}

void FFTManager::calculateFFT() {
    if (fft_active && fft_ana->available() == true) {
        dprint(PRINT_FFT_DEBUG, name);
        dprint(PRINT_FFT_DEBUG, " FFT Available - ");
        // dprint(PRINT_FFT_DEBUG, last_fft_reading);
        // last_fft_reading = 0;
        fft_tot_energy = 0.0;
        // int highest = -1;
        // double highest_val = -1.0;
        if (calculate_flux == true) { // only do this if we need to in order to save time
            for (int i = 0; i < 127; i++) {
                last_fft_vals[i] = fft_vals[i];
            }
        }
        for (int i = 0; i < 127; i++) {
            fft_vals[i] = fft_ana->read(i);
        }
        for (int i = 0; i < 127; i++) {
            fft_vals[i] *= fft_scaler;
            fft_tot_energy += fft_vals[i];
        }
        
        // the 10 is to try and get more dynamic range out of the colour mapping
        //relative_bin_pos = (double)(highest_energy_idx - min_bin + ((max_bin - min_bin)*0.25) ) / (double)(max_bin - min_bin - ((max_bin - min_bin)*0.25));
        // if (relative_bin_pos > 1.0) { relative_bin_pos = 1.0;};
        if (calculate_centroid == true) {calculateCentroid();};
        if (calculate_centroid == true && SMOOTH_CENTROID == true) {centroid = (centroid += last_centroid) * 0.5;};
        if (calculate_flux == true) {calculateFlux();};

        if (PRINT_FFT_DEBUG) {
          printFFTVals();
        }
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
