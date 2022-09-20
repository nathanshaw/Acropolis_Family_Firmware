adc => PoleZero dcBlock => Gain inGain;
0.99 => dcBlock.blockZero;

inGain => FFT fft =^ RMS rms => blackhole;

256 => fft.size;
128 => int WIN_SIZE;
Windowing.hann(WIN_SIZE) => fft.window;
UAnaBlob blobRMS;

OscSend oscSend;
oscSend.setHost("localhost", 6450);

0 => float rmsWeightedAverage;
now => time lastRMSMsg;

fun void sendRMS() {
    oscSend.startMsg("/rms, f");
    oscSend.addFloat(rmsWeightedAverage);
    // <<<"sending song rms message : ", rmsWeightedAverage>>>;
}

fun void calcRMS() {
    rms.upchuck() @=> blobRMS;
    (blobRMS.fval(0) * 1000 + rmsWeightedAverage) * 0.5 => rmsWeightedAverage;
}

while (true) {
    calcRMS();
    fft.size()::samp => now;
    if (now > lastRMSMsg + 0.1::second) {
        now => lastRMSMsg;
        sendRMS();
    }
}
