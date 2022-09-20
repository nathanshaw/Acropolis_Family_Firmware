    // for playing back the clip
    // SndBuf2 sampler => PoleZero dcBlock => Gain samplerG; // => dac;
    adc  => PoleZero dcBlock => Gain samplerG; // => dac;
    0.99 => dcBlock.blockZero;

    // TODO get a chain for the clicks
    samplerG => HPF clickHPF1 => HPF clickHPF2 => LPF clickLPF1 => LPF clickLPF2 => Gain clickGain => blackhole;
    1000 => clickHPF1.freq => clickHPF2.freq;
    0.75 => clickHPF1.Q => clickHPF2.Q;
    3000 => clickLPF1.freq => clickLPF2.freq;

    // TODO get a chain for the song...
    samplerG => HPF songHPF1 => HPF songHPF2 => LPF songLPF1 => LPF songLPF2 => Gain songGain => blackhole;
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

    // sampler.read(me.dir() + "Cicadas_20190207-143838-repaired.wav");

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
    oscSend.setHost("localhost", 6450);

    fun float calcNegLogLikelihood() {
        float nLog;
        //
        return nLog;
    }

    fun void calcSongFeatures() {
        songFFT.upchuck();
        songFC.upchuck() @=> UAnaBlob blobSongFC;
        oscSend.startMsg("/rms, f");
        oscSend.addFloat(blobSongFC.fval(0)*1000);
        // <<<"sending song rms message : ", blobSongFC.fval(0)*1000>>>;
    }

    fun float calcClickOnsetProb1(float sens) {
        return rmsPosDeltas[0]*fluxPosDeltas[0]*10*sens;
    }

    fun float calcClickOnsetProb2(float sens) {
        return sens*blobClickFC.fval(1)*blobClickFC.fval(1)*blobClickFC.fval(0)*blobClickFC.fval(0)*1500000000;
    }

    fun void calcClickFeatures() {
        0 => int is_onset;
        // update the FIFO with values
        
        // rms pos delta
        updateFIFO(getPosDelta(rmsVals), rmsPosDeltas);
        // spork ~ sendFeatureMsg(2, Math.pow(rmsPosDeltas[0], 2)*10, 0, "RMS Pos Delta"); 
        
        // index 1 is flux
        updateFIFO(blobClickFC.fval(1), fluxVals);
        updateFIFO(getPosDelta(fluxVals), fluxPosDeltas);
        
        // index 2 is centroid
        updateFIFO(blobClickFC.fval(2), centVals);
        
        // index 3 is roff
        updateFIFO(blobClickFC.fval(3), roffVals);
        
        calcClickOnsetProb1(1.5) => float prob1;
        if (prob1 >= onset_thresh) {
            1 => is_onset;
        }
        
        calcClickOnsetProb2(1.5) => float prob2;
        if (prob2 >= onset_thresh) {
            1 => is_onset;
        }
        ///////////////////////////////////////////////////
        if (is_onset) {      
            oscSend.startMsg("/click, f");
            oscSend.addFloat(rmsVals[0]*1000);
            <<<"click detected, sending osc message">>>;
        }
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

    while (1) {
        WIN_SIZE::samp => now;
        clickFFT.upchuck();
        clickFC.upchuck() @=> blobClickFC;
        calcSongFeatures();
        calcClickFeatures();
        // sendFFTMags();
    }
