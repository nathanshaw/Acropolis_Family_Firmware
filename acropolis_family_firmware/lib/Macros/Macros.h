/**
 * Marlin 3D Printer Firmware
 * Copyright (C) 2016 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef MACROS_H
#define MACROS_H

// min and max values for the H-bridge motors
#define MIN_FRONT_SPEED 40
#define MAX_FRONT_SPEED 450
#define MIN_REAR_SPEED -40
#define MAX_REAR_SPEED -450

////////////////////////////////////////////////////////////////////////////
///////////////////////// Scaling Modes ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#define LINEAR_SCALING                  0
#define EXP_SCALING                     1
#define LOG_SCALING                     2

////////////////////////////////////////////////////////////////////////////
///////////////////////// Artefact Genus //////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// artefact family
#define SPECULATOR                      0
#define EXPLORATOR                      1
#define LEGATUS                         2

////////////////////////////////////////////////////////////////////////////
///////////////////////// Artefact Species //////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// Speculator Species include: SPEC_MINOR and SPEC_MAJOR
// Explorator Species include: EX_CHIPPER, EX_CHIRPER, EX_CLAPPER, EX_SPINNER, and EX_WINDER
// Legatus Species include: LEG_MAJOR
////////////////////////////
#define SPEC_MAJOR              0
#define SPEC_MINOR              1
#define EX_CHIPPER              10
#define EX_CHIRPER              11
#define EX_CLAPPER              12
#define EX_SPINNER              13
#define EX_WINDER               14
#define LEG_MAJOR               20

/////////////////////////////////
// All artefact behaviour states
//////////////////////////////
#define B_TARGETED_FEEDBACK 100
#define B_ADAPTIVE_FEEDBACK 101
#define B_CHIRPER_BASIC     200
#define B_CHIPPER_BASIC     201
#define B_CLAPPER_BASIC     202
#define B_SPINNER_BASIC     203
#define B_LEG_MATCH_PITCH   300
#define B_LEG_FEEDBACK      301
#define B_LEG_FM_FEEDBACK   302
#define B_LEG_SAMP_PLAYBACK 303
#define B_LEG_ECHO_CHAMBER  304

/////////////////////////////////
// Legatus behaviour states
/////////////////////////////////
#define S_START_LISTENING       0
#define S_CONTINUE_LISTENING    1
#define S_STOP_LISTENING        2
#define S_START_RECORDING       3
#define S_CONTINUE_RECORDING    4
#define S_STOP_RECORDING        5
#define S_REFLECTING            6
#define S_START_PLAYBACK        7
#define S_CONTINUE_PLAYBACK     8
#define S_END_PLAYBACK          9

///////////////////// ENCLOSURE_TYPES /////////////////////
#define ORB_ENCLOSURE_WITH_HOLE         0
#define ORB_ENCLOSURE                   1
#define GROUND_ENCLOSURE                2
#define NO_ENCLOSURE                    3
#define LEGATUS_ENCLOSURE               10
#define EXPLORATOR_MB_ENCLOSURE         20
#define EXPLORATOR_CLAPPER_ENCLOSURE    21
#define EXPLORATOR_PECKER_ENCLOSURE     22
#define EXPLORATOR_CHURPER_ENCLOSURE    23
#define EXPLORATOR_SHAKER_ENCLOSURE     24

///////////////////// Audio Features //////////////////////
// Some macros to make it easier to set some software settings
#define RMS_RAW               0
#define RMS_DELTA             2
#define PEAK_RAW              1
#define PEAK_DELTA            3
#define MAX_ENERGY_BIN        7
#define SPECTRAL_CENTROID     8
#define SPECTRAL_FLUX         9
#define ALL_FEATURES          10

/////////////////////// colors ////////////////////////////
#define RED                   0xff0000
#define GREEN                 0x00ff00
#define BLUE                  0x0000ff
#define YELLOW                0x101400
#define PINK                  0x120009
#define ORANGE                0x100400
#define WHITE                 0xf0f0f0
#define PURPLE                0xf000f0

////////////////////// datatypes /////////////////
#define DATATYPE_DOUBLE        0
#define DATATYPE_SHORT         1
#define DATATYPE_LONG          2
#define DATATYPE_BYTE          3

#define MIN3(a, b, c)       min(min(a, b), c)
#define MIN4(a, b, c, d)    min(MIN3(a, b, c), d)
#define MIN5(a, b, c, d, e) min(MIN4(a, b, c, d), e)
#define MAX3(a, b, c)       max(max(a, b), c)
#define MAX4(a, b, c, d)    max(MAX3(a, b, c), d)
#define MAX5(a, b, c, d, e) max(MAX4(a, b, c, d), e)

#define UNEAR_ZERO(x) ((x) < 0.000001)
#define NEAR_ZERO(x) WITHIN(x, -0.000001, 0.000001)
#define NEAR(x,y) NEAR_ZERO((x)-(y))

///////////////////////// Time Units ////////////////////////
#define TEN_SECONDS 10000
#define ONE_MINUTE  60000
#define ONE_HOUR    3600000
#define ONE_DAY     86400000

// the Baud rate that our bots will be using
#define SERIAL_BAUD_RATE          115200

////////////////////////// Lux Manager stuff ////////////////
// how long will the lux calibratin routine last for?
#define LUX_CALIBRATION_TIME            4000
// how long the lux sensors need the LEDs to be 
// turned off in order to get an accurate reading
#define LUX_SHDN_LEN                    50


#define LUX_ADJUSTS_MIN_MAX 0
#define LUX_ADJUSTS_BS      1
#define LUX_ADJUSTS_BS_AND_MIN_MAX      2

double maxf(double v1, double v2){
  if (v1 < v2) {
    return v2;
  } else if (v1 > v2) {
    return v1;
  } else {
    return v1;
  }
}


double minf(double v1, double v2){
  if (v1 < v2) {
    return v1;
  } else if (v1 > v2) {
    return v2;
  } else {
    return v1;
  }
}

double constrainf(double val, double _min, double _max) {
  if (val > _max) {
    return _max;
  } else if (val < _min) {
    return _min;
  } else {
    return val;
  }
}

double mapf(double val, double in_min, double in_max, double out_min, double out_max) {
  if (out_min < out_max){
    return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  } else {
    return out_min - ((val - in_min) / (in_max - in_min) * (out_min - out_max));
  }
}

bool wrap(double & v){
  // WARNING - this function is dangerous and should only be used
  // by the wrapValues function
    if (v > 1.0){
        v = v - 1.0;
        return true;
    } else if (v < 0.0) {
        v = v + 1.0;
        return true;
    }
    return false;
}

double wrapValues(double v){
  // a variation on the constraion function which returns
  // a value between 0.0 and 1.0, but instead of scaling
  // it wraps around the values so 1.1 returns 0.1
    while(wrap(v)){};
    return v;
}

#endif //__MACROS_H
