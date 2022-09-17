#ifndef __RHYTHMS_H__
#define __RHYTHMS_H__

#include <Arduino.h>

#ifndef PRINT_RHYTHM_DEBUG
#define PRINT_RHYTHM_DEBUG 1
#endif

#ifndef RHYTHM_MAX_NOTES
#define RHYTHM_MAX_NOTES 32
#endif

#ifndef MAX_RHYTHMS
#define MAX_RHYTHMS 10
#endif

class Rhythm {

    public:
        Rhythm();

        void addNote(double freq, uint32_t length, uint32_t onset, double velocity);
        void addDampenedNote(double freq, uint32_t onset, double velocity);
        void nextNote();
        double getMedianFreq(){return median_freq;};
        double getCurrentOnset() {return onset_times[current_note];};
        bool isActive() {return active;};
        double getFreq() {return freqs[current_note];};
        uint32_t getLength(){return lengths[current_note];};
        uint32_t getOnset(){return onset_times[current_note];}
        double getVelocity(){return velocitys[current_note];};
        bool getDampOn(){return damp_on[current_note];};

    private:
        // number of notes stored in the rhythm
        uint8_t num_notes = 0;
        // approx freq of notes
        double freqs[RHYTHM_MAX_NOTES];
        // how long should the notes ring for (think of the bells)
        uint32_t lengths[RHYTHM_MAX_NOTES];
        // when are the note onsets in relation to the start of the first note
        // index 0 of this array should always be 0
        uint32_t onset_times[RHYTHM_MAX_NOTES];
        // should be scaled from 0 - 1
        double velocitys[RHYTHM_MAX_NOTES];
        // if true the dampener will not get released if that is an option for the mechanism
        bool damp_on[RHYTHM_MAX_NOTES];
        // is the class currently active?
        bool active = false;
        // which note is currently sounding?
        uint8_t current_note = 0;
        double median_freq = 0.0;
};

Rhythm::Rhythm() {

}


void Rhythm::nextNote() {
    current_note++;
    active = true;
    if (current_note > num_notes) {
        current_note = 0;
        active = false;
        dprintln(PRINT_RHYTHM_DEBUG, "there are no more notes in this rhythm, deactivating");
    } else{
        dprint(PRINT_RHYTHM_DEBUG, "current note now : ");
        dprintln(PRINT_RHYTHM_DEBUG, current_note);
    }
}


void Rhythm::addNote(double freq, uint32_t length, uint32_t onset, double velocity) {
    freqs[num_notes] = freq;
    lengths[num_notes] = length;
    onset_times[num_notes] = onset;
    velocitys[num_notes] = velocity;
    num_notes++;
    double freq_t = 0.0;
    for (int i = 0; i < num_notes; i++) {
        freq_t += freqs[i];
    }
    median_freq = freq_t / num_notes;
}

void Rhythm::addDampenedNote(double freq, uint32_t onset, double velocity) {
    freqs[num_notes] = freq;
    lengths[num_notes] = 0;
    onset_times[num_notes] = onset;
    velocitys[num_notes] = velocity;
    num_notes++;
    double freq_t = 0.0;
    for (int i = 0; i < num_notes; i++) {
        freq_t += freqs[i];
    }
    median_freq = freq_t / num_notes;
}

class RhythmBank {
    public:
       RhythmBank();
       void addRhythm(Rhythm * r);
       // get the index of the rhythm which contains a medium freq closest to the given freq
       Rhythm* getRhythmFromPitch(double freq);
       Rhythm* getRandomRhythm();
       Rhythm* getRhythmByIndex(uint8_t idx);

    private:
       Rhythm* rhythms[MAX_RHYTHMS];
       uint8_t num_rhythms = 0;
       uint8_t last_played;
};

RhythmBank::RhythmBank(){

}

Rhythm* RhythmBank::getRhythmByIndex(uint8_t idx) {
    return rhythms[idx];
}

Rhythm* RhythmBank::getRandomRhythm() {
    long i = random(0, num_rhythms);
    dprintln(PRINT_RHYTHM_DEBUG, "-------------------------");
    dprint(PRINT_RHYTHM_DEBUG, "selected random rhythm: ");
    dprintln(PRINT_RHYTHM_DEBUG, i);
    dprintln(PRINT_RHYTHM_DEBUG, "-------------------------");

    return rhythms[i];
}

void RhythmBank::addRhythm(Rhythm * r) {
    rhythms[num_rhythms] = r;
    num_rhythms++;
}

Rhythm* RhythmBank::getRhythmFromPitch(double freq) {
    uint8_t closest_idx = 0;
    double min_distance = 20000.0;
    for (int i = 0; i < num_rhythms; i++) {
        double val = rhythms[i]->getMedianFreq() - freq;
        double distance = abs(val);
        if (distance < min_distance) {
            min_distance = distance;
            closest_idx = i;
        }
    }
    return rhythms[closest_idx];
}

#endif
