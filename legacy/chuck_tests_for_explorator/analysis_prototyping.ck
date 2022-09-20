// for playing back the clip
SndBuf2 sampler => PoleZero dcBlock => Gain samplerG; // => dac;
0.99 => dcBlock.blockZero;

// TODO get a chain for the clicks
samplerG => HPF clickHPF1 => HPF clickHPF2 => LPF clickLPF1 => LPF clickLPF2 => Gain clickGain => dac;
1000 => clickHPF1.freq => clickHPF2.freq;
0.75 => clickHPF1.Q => clickHPF2.Q;
3000 => clickLPF1.freq => clickLPF2.freq;

// TODO get a chain for the song...
samplerG => HPF songHPF1 => HPF songHPF2 => LPF songLPF1 => LPF songLPF2 => Gain songGain => dac;
4000 => songHPF1.freq => songHPF2.freq;
16000 => songLPF1.freq => songLPF2.freq;
1.0 => songHPF1.Q => songHPF2.Q;

songGain => FFT songFFT =^ RMS songRMS =^ FeatureCollector songFC => blackhole;
clickGain => FFT clickFFT =^ RMS clickRMS =^ FeatureCollector clickFC => blackhole;
clickFFT =^ Flux clickFlux =^ clickFC => blackhole;
clickFFT =^ Centroid clickCent =^ clickFC => blackhole;
clickFFT =^ RollOff clickRoff =^ clickFC => blackhole;

clickRoff.percent(0.8);

1.0 => float onset_thresh; // normally 1.0, turns into 0.8 for two seconds after an onset is detected.

///////////////// FFT Params /////////////////////////////////
1024 => songFFT.size => clickFFT.size;
512 => int WIN_SIZE;

Windowing.hann(WIN_SIZE) => songFFT.window => clickFFT.window; 
UAnaBlob blobFFT;

// for storing MIR features for the click
UAnaBlob blobClickFC;

sampler.read(me.dir() + "Cicadas_20190207-143838-repaired.wav");

10 => int history;

float centVals[history];
float centDeltas[history];
float centPosDeltas[history];
float centPosDeltaAvg;
float centAvg;

float roffVals[history];
float roffNegDeltas[history];
float roffNegDeltaAvg;
float roffAvg;

float fluxVals[history];
float fluxPosDeltas[history];
float fluxPosDeltaAvg;

float rmsVals[history]; 
float rmsPosDeltas[history];
float rmsPosDeltaAvg;
float rmsAvg;

float onsetProbs[history];

////////////////////////// OSC Sending to visualiser //////////////////////////////////
OscSend oscSend;
oscSend.setHost("localhost", 6449);

fun float calcNegLogLikelihood() {
    float nLog;
    //
    return nLog;
}

fun void calcSongFeatures() {
    songFFT.upchuck();
    songFC.upchuck() @=> UAnaBlob blobSongFC;
    oscSend.startMsg("/rms, ifi");
    oscSend.addInt(0);
    oscSend.addFloat(blobSongFC.fval(0)*1000);
    oscSend.addInt(0);
}

fun float calcClickOnsetProb1(float sens) {
    return rmsPosDeltas[0]*fluxPosDeltas[0]*10*sens;
}

fun float calcClickOnsetProb2(float sens) {
    return sens*blobClickFC.fval(1)*blobClickFC.fval(1)*blobClickFC.fval(0)*blobClickFC.fval(0)*1500000000;
}

////////////////////// rhythm detection stuff ////////////////////////
10 => int max_rhythms;
Rhythms rhythms[max_rhythms];
0 => int current_rhythm;

24 => int max_notes; // maximum number of notes in a rhythm
0 => int current_note;

time last_onset;
float onset_velocities[max_notes];
dur onset_durations[max_notes];

100::ms => dur min_inter_note_rhythm;  // "debounce" time for the onsets and rhythm detection
2000::ms => dur max_inter_note_rhythm; // max time between onsets to still be considered part of same rhythm
//////////////////////////////////////////////////////////////////////

fun void calcClickFeatures() {
    0 => int is_onset;
    // rms
    updateFIFO(blobClickFC.fval(0)*20000, rmsVals);
    oscSend.startMsg("/rms, f");
    oscSend.addFloat(rmsVals[0]);  
    
    /*
    oscSend.startMsg("/features, ifis");
    oscSend.addInt(1);
    oscSend.addFloat((rmsVals[0]));
    oscSend.addInt(0);
    oscSend.addString("RMS");
    */
    
    // update the FIFO with values
    
    // rms pos delta
    updateFIFO(getPosDelta(rmsVals), rmsPosDeltas);
    spork ~ sendFeatureMsg(2, Math.pow(rmsPosDeltas[0], 2)*10, 0, "RMS Pos Delta"); 
    
    // index 1 is flux
    updateFIFO(blobClickFC.fval(1), fluxVals);
    updateFIFO(getPosDelta(fluxVals), fluxPosDeltas);
    sendFeatureMsg(3, fluxPosDeltas[0]*4, 0, "Flux pos delta * 4"); 
    
    // index 2 is centroid
    updateFIFO(blobClickFC.fval(2), centVals);
    // oscSend.startMsg("/features, ifis");
    // oscSend.addInt(3);
    // oscSend.addFloat(getPosDelta(centVals)*10);
    // oscSend.addInt(0);
    // oscSend.addString("Centroid Pos Delta");
    
    
    // index 3 is roff
    updateFIFO(blobClickFC.fval(3), roffVals);
    /*
    oscSend.startMsg("/features, ifis");
    oscSend.addInt(1);
    oscSend.addFloat(Math.pow(getDelta(roffVals), 2)*400);
    oscSend.addInt(0);
    oscSend.addString("Roff Delta Squared");
    */
    
    calcClickOnsetProb1(1.5) => float prob1;
    if (prob1 >= onset_thresh) {
        sendFeatureMsg(0, prob1, 1, "onset prob1");
        1 => is_onset;
    } else {
        sendFeatureMsg(0, prob1, 0, "onset prob1");
    }
    
    calcClickOnsetProb2(1.5) => float prob2;
    if (prob2 >= onset_thresh) {
        sendFeatureMsg(1, prob2, 1, "onset prob2");
        1 => is_onset;
        0.8 => onset_thresh;
    } else {
        sendFeatureMsg(1, prob2, 0, "onset prob2");
    }
    ///////////////////////////////////////////////////
    if (is_onset) {
        if (now < last_onset + min_inter_note_rhythm) {
            <<<"WARNING --- REJECTING ONSET DUE TO ONSET DEBOUNCING">>>;
        } else {      
            rmsVals[0] => onset_velocities[current_note]; // store the current RMS as the velocity for the current note
            if (current_note == 0) {
                // if it is the first onset of a potential rhythm, then the time is 0
                0::ms => onset_durations[current_note];
                <<<"ONSET DETECTED">>>;
            } else{
                // otherwise store the amount of time which has passed since the last onset (in ms)
                now - last_onset => onset_durations[current_note];
                <<<"RHYTHM DETECTED">>>;
            }
            
            // store the current time as the time of the last onset
            now => last_onset;
            <<<"last_onset set to : ", last_onset>>>;
            
            // needs to be done last (to ensure velocities and times are written to the correct indexes
            current_note++; // keeps track of how many notes we are within the rhythm
            0.8 => onset_thresh; // make it easier to detect onsets for 2 seconds (this is reset to 1.0 in below if statement)
            <<<"onset thresh set to : ", onset_thresh>>>;
            if (current_note >= max_notes) {
                max_notes -1 => current_note;
                <<<"WARNING ONSET DETECTION IS TOO SENSATIVE!!!">>>;
                <<<"EXTREMELY LONG RHYTHMS BEING DETECTED">>>;
            }
        }
    }
    //////////////////////////////////////////////////
    // if it has been more than 2 seconds since the last detected onset, turn off rhythm detection mode...
    if (now > last_onset + max_inter_note_rhythm && current_note != 0) {
        // if we have detected two or more notes which are related to another...
        if (current_note > 1) {
            // store the currently accumulated data on the rhythm into a rhythm object
            rhythms[current_rhythm].init(onset_velocities, onset_durations, current_note);
            current_rhythm + 1 % max_rhythms => current_rhythm; // increment the "current rhythm" which keeps track of what instance of the rhythm class we are updating        
        } 
        0 => current_note; // reset current note, we dont have to "erase" contents of rhythm_velocities or intervals as their values will be overwriten once a new rhythm is detected.
        1.0 => onset_thresh;
        <<<"onset thresh set to : ", onset_thresh>>>;
    }
}


fun void sendFeatureMsg(int f_num, float val, int flag, string name) {
    oscSend.startMsg("/features, ifis");
    oscSend.addInt(f_num);
    oscSend.addFloat(val);
    oscSend.addInt(flag);
    oscSend.addString(name);
    // <<<"feature message send">>>;
}

fun float[] updateFIFO(float val, float b[]){
    for (b.size()-2 => int i; i > -1; i--){
        b[i] => b[i+1];
    }
    val => b[0];
    return b;
}

fun float getDelta(float b[]) {
    return Math.fabs(b[0]-b[1]);   
}

fun float getPosDelta(float b[]) {
    if (b[0] > b[1]) {
        return b[0] - b[1];
    }
    return 0.0;
}

fun float getNegDelta(float b[]) {
    if (b[1] > b[0]) {
        return b[1] - b[0];
    }
    return 0.0;
}

fun void sendFFTMags() {
    "/fft, " => string fft_msg;
    for (0 => int i; i < clickFFT.size()*0.5; i++){
        "f" +=> fft_msg;
        <<<clickFFT.fval(i)>>>;
    }
    /*
    oscSend.startMsg(fft_msg);
    for (0 => int i; i < fft.size(); i++){
        oscSend.addFloat(clickFFT.fval(i));
    }
    */
}

while (1) {
    WIN_SIZE::samp => now;
    clickFFT.upchuck();
    clickFC.upchuck() @=> blobClickFC;
    // calcSongFeatures();
    calcClickFeatures();
    // sendFFTMags();
}