public class Rhythms {
    int number_of_notes; // number of notes contained in the rhythm
    float note_velocities[]; // scaled from 0 -> 1 
    dur note_times[];
    0 => int initalized;
    
    fun void init (float vels[], dur times[]) {
        vels @=> note_velocities;
        times @=> note_times;
        vels.size() => number_of_notes;
        1 => initalized; 
    }
    
    fun void init (float vels[], dur times[], int notes) {
        // there will be one more note than number of "times"
        float _v[notes];
        dur _t[notes];
        
        for (int i; i < notes; i++) {
            vels[i] => _v[i];
            times[i] => _t[i];
        }
        _v @=> note_velocities;
        _t @=> note_times;
        notes => number_of_notes;
        1 => initalized; 
        printContents();
    }
    
    /*
    fun void playbackRhythm (int idx) {
        1.0 => float scale;
        0 => int i;
        while (i < number_of_notes) {
            // send note event at idx of i
            i++;
            note_times[i]*scale::ms=> now;
        }
    }
    
    fun void playbackRhythm (int idx, float scale) {
        0 => int i;
        while (i < number_of_notes) {
            // send note event at idx of i
            i++;
            note_times[i]*scale::ms=> now;
            
        }
    }
    */
    
    fun void printContents() {
        <<<"--------- Rhythm -----------">>>;
        if (initalized == 0) {
        <<<" \tNot initalized, sorry">>>;   
    } else {
        for (int i; i < number_of_notes; i++) {
            <<<i, ": ", note_velocities[i], note_times[i]>>>;
        }
        <<<"----------------------------">>>;
        // TODO implement this function
    }
    }   
};

