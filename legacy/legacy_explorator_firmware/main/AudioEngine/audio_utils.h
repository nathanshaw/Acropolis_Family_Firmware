#ifndef __AUDIO_UTILS__
#define __AUDIO_UTILS__

double getPosDelta(double last, double current) {
    if (current > last) {
        return current - last;
    }
    return 0.0;
}

double getDelta(double last, double current) {
    return current - last;
}

#endif
