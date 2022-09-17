#ifndef __AUDIOENGINE_H__
#define __AUDIOENGINE_H__

#include "../Configuration.h"
#include "FeatureCollector.h"

#ifndef MAX_GAIN_ADJUSTMENT
#define MAX_GAIN_ADJUSTMENT 0.25
#endif

#ifndef PRINT_AUTO_GAIN
#define PRINT_AUTO_GAIN 0
#endif

class AutoGain {
  public:
    // feature collector with gain objects already added to it
    AutoGain(String, FeatureCollector *, double, double, double);
    AutoGain(String, FeatureCollector *, FeatureCollector *, double, double, double);
    void setUpdateRate(uint32_t r) {update_rate = r;};
    void setStartDelay(uint32_t d) {start_delay = d;};
    bool update();
    bool updateExternal(double val);
    void setExternalThresholds(String, double, double, double, double);
    void linkSecondFC(FeatureCollector *_fc );

  private:
    bool read_once = false;
    bool second_collector = false;
    double * ext_val;
    String ext_id;
    double min_gain, max_gain;
    double max_gain_adj;
    // the aboslute min/max values for calculating autogain (for the incomming ext values)
    double ext_min, ext_max;
    // where the auto-gain will kick in
    double ext_low, ext_high;

    double calculateCost(double);
    String id;
    elapsedMillis last_autogain;
    uint32_t update_rate = 0;
    uint32_t start_delay;

    FeatureCollector *fc;
    FeatureCollector *fc2;
};

AutoGain::AutoGain(String _id, FeatureCollector *_fc, double min, double max, double max_gain_a) {
    if (_fc->ampActive() == false) {
        Serial.println("WARNING - \
                the passed feature collector does not have amplifiers active");
    }
    fc = _fc;
    id = _id;
    min_gain = min;
    max_gain = max;
    max_gain_adj = max_gain_a;
}

AutoGain::AutoGain(String _id, FeatureCollector *_fc, FeatureCollector *_fc2, double min, double max, double max_gain_a) {
    if (_fc->ampActive() == false) {
        Serial.println("WARNING - \
                the passed feature collector does not have amplifiers active");
    }
    fc = _fc;
    fc2 = _fc2;
    id = _id;
    min_gain = min;
    max_gain = max;
    max_gain_adj = max_gain_a;
    second_collector = true;
}

void AutoGain::linkSecondFC(FeatureCollector *_fc ) {
    if (second_collector == false) {
        second_collector = false;
        fc2 = _fc;
    }
}

void AutoGain::setExternalThresholds(String _id, double min, double low, double high, double max) {
    ext_id = _id;
    ext_min = min;
    ext_low = low;
    ext_high = high;
    ext_max = max;
}

bool AutoGain::updateExternal(double val) {
    // constrain the values
    if ((last_autogain > update_rate && read_once) || (read_once == false && last_autogain > start_delay)) {
        last_autogain = 0;
        read_once = true;
        double cost = calculateCost(val);
        if (cost != 0.0) {
            double new_gain;
            new_gain = fc->gain + (fc->gain * cost * MAX_GAIN_ADJUSTMENT);
            new_gain = min(max_gain, new_gain);
            Serial.print("updating ");Serial.print(id);Serial.print(" ");Serial.print(ext_id);
            Serial.print(" gain from ");Serial.print(fc->gain);Serial.print(" to ");
            Serial.print(new_gain);
            Serial.print(" with a cost of ");Serial.println(cost);// Serial.print(" auto_gain timer");Serial.print(last_autogain);
            // Serial.println(" / ");Serial.println(update_rate);
            // if there is a cost, then the gain needs to be adjusted
            fc->updateGain(new_gain);
            if (second_collector) {
                fc2->updateGain(new_gain);
            }
            return true;
        }
    }
    return false;
}

double AutoGain::calculateCost(double val) {
    // the higher the cost, the more gain adjustment will happen
    // ranges from -1 to 1, with a neg value corresponding to a decrease in volume
    // if it is withen nominal range then simply return 0.0 for cost
    if (val < ext_min) {
        val = ext_min;
        dprint(PRINT_AUTO_GAIN, "Increasing ");
        dprint(PRINT_AUTO_GAIN, id);
        dprintln(PRINT_AUTO_GAIN, " autogain by the max amount due to external update");
        return 1.0;
    } else if (val > ext_max) {
        val = ext_max;
        dprint(PRINT_AUTO_GAIN, "Decreasing");
        dprint(PRINT_AUTO_GAIN, id);
        dprintln(PRINT_AUTO_GAIN, " autogain by the max amount due to external update");
        return -1.0;
    }
    if (val > ext_low && val < ext_high) {
        return 0.0;
    }
    else if (val < ext_low) {
        // if thee gain needs to be raised
        return (val - ext_min) / (ext_low - ext_min);
    } else if (val > ext_high) {
        return (1.0 - ((val - ext_high) / (ext_max- ext_high))) * -1;
    }
    return 0.0;
}

bool AutoGain::update() {
    bool updated = false;
    // if the feature collector is active
    if (fc->isActive() && read_once) {
        read_once = true;
       Serial.println("WARNING UPDATE IS NOT IMPLEMENTED IN THE AUTOGANI CLASS"); 
    }
    return updated;
}
/*
bool checkSongAutoGain() {
  adjustSongGainLedOnRatio();
  bool success = false;
  for (int i = 0; i < num_channels; i++) {
    ///////////////////////////////////////////////////////////////
    // second check is to see if the song gain needs to be adjusted
    ///////////////////////////////////////////////////////////////
    // calculate the average peak values since the last auto-gain adjust
    double avg_song_peak = total_song_peaks[i] / num_song_peaks[i];
    double cost; // our cost variable
    dprint(PRINT_AUTO_GAIN, "\n--------- song "); dprint(PRINT_AUTO_GAIN, i); dprintln(PRINT_AUTO_GAIN, " -------------");
    dprint(PRINT_AUTO_GAIN, "total_song_peaks ");
    dprintln(PRINT_AUTO_GAIN, total_song_peaks[i]);
    dprint(PRINT_AUTO_GAIN, "num_song_peaks ");
    dprintln(PRINT_AUTO_GAIN, (long) num_song_peaks[i]);
    // if the avg value is more than the max...
    if (avg_song_peak > MAX_SONG_PEAK_AVG) {
      // calculate cost between 0 and 1 with higher cost resulting in higher gain amplification
      cost = 1.0 - (MAX_SONG_PEAK_AVG / avg_song_peak);
      // calculate what the new song_gain will be
      double change = fc[i].gain * MAX_GAIN_ADJUSTMENT * cost;
      fc[i].gain -= change;
      // ensure that what we have is not less than the min
      fc[i].gain = max(fc[i].gain, MIN_SONG_GAIN);
      dprint(PRINT_AUTO_GAIN, "song gain decreased by ");
      dprint(PRINT_AUTO_GAIN, change);
      dprint(PRINT_AUTO_GAIN, " ");
      success = true;
    }
    // if the average value is less than the min....
    else if (avg_song_peak < MIN_SONG_PEAK_AVG) {
      dprintln(PRINT_AUTO_GAIN);
      dprint(PRINT_AUTO_GAIN, "avg_song_peak lower than MIN_SONG_PEAK_AVG ");
      dprintln(PRINT_AUTO_GAIN, avg_song_peak);
      // calculate cost between 0 and 1 with higher cost resulting in higher gain attenuation
      cost = 1.0 - (MIN_SONG_PEAK_AVG / avg_song_peak);
      dprint(PRINT_AUTO_GAIN, "cost : ");
      dprintln(PRINT_AUTO_GAIN, cost);
      // calculate the new song gain
      double change = fc[i].gain * MAX_GAIN_ADJUSTMENT * cost;
      fc[i].gain += change;
      // ensure what we have is not less than the max...
      fc[i].gain = min(fc[i].gain, MAX_SONG_GAIN);
      dprint(PRINT_AUTO_GAIN, "song gain increased by ");
      dprint(PRINT_AUTO_GAIN, change);
      dprint(PRINT_AUTO_GAIN, " ");
      dprintln(PRINT_AUTO_GAIN);
      success = true;
    }
    // now check the avg on time? todo

    ///////////////////////////////////////////////////////////////
    // last thing to do is reset the last_auto_gain_adjustment timer
    ///////////////////////////////////////////////////////////////
    total_song_peaks[i] = 0;
    num_song_peaks[i] = 0;
  }
  if (success) {
    return 1;
  } else {
    return 0;
  }
}
*/

#endif // __AUDIO_ENGINE_H__
