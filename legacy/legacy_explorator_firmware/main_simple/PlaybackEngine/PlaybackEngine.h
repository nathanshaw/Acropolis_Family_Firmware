#ifndef __PLAYBACK_ENGINE_H__
#define __PLAYBACK_ENGINE_H__

#include "Rhythm.h"
#include "../Mechanisms/Mechanisms.h"

class PlaybackEngine {
    public:
      PlaybackEngine();
      void playRhythm(Rhythm * r);
      void update();
      void linkMechanism(BellMechanism * m);
      // void addSolenoidMechanism(uint8_t act, uint8_t damp);
    private:
      BellMechanism * mechanisms[3];
      uint8_t num_mechanisms = 0;
      bool playback_active = false;
      Rhythm * rhythm;
      elapsedMillis last_onset;
};

PlaybackEngine::PlaybackEngine(){

}

void PlaybackEngine::linkMechanism(BellMechanism * m) {
    mechanisms[num_mechanisms] = m;
    num_mechanisms++;
}

void PlaybackEngine::playRhythm(Rhythm * r) {
    if (playback_active == false) {
        playback_active = true;
        rhythm = r;
    } else {
        Serial.println("ERROR, PLAYBACK ENGINE IS ALREADY IN PLAYBACK MODE");
    }
}

void PlaybackEngine::update() {
    for (int i = 0; i < 3; i++) {
        if (!playback_active) {
            return;
        }
        if (last_onset > rhythm->getCurrentOnset()) {
            // TODO - need to get the information from the rhythm and set it up
            double target_f = rhythm->getFreq();
            double distance = 20000.0;
            uint8_t idx = 0;
            for (int i = 0; i < 3; i++) {
                if (abs(mechanisms[i]->getFreq() - target_f) < distance) {
                    distance = abs(mechanisms[i]->getFreq() - target_f);
                    idx = i;
                }
            }
            Serial.print("Chose mechanism ");Serial.print(idx);
            Serial.print(" as it had the closest freq to ");
            Serial.print(target_f);
            Serial.print(" of ");
            Serial.println(mechanisms[idx]->getFreq());

            // TODO - choose a mechanism which is closest to the desired pitch
            mechanisms[idx]->queueStrike(rhythm->getVelocity(), rhythm->getLength());
            mechanisms[idx]->strike();
            rhythm->nextNote();
            if (rhythm->isActive() == false){
                playback_active = false;
            }
            last_onset = 0;
        }
    }
}
#endif // __RHYTHMS_H__
