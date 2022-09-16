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

////////////////////////////////////////////////////////////////////////////
///////////////////////// Scaling Modes ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#define LINEAR_SCALING                  0
#define EXP_SCALING                     1
#define LOG_SCALING                     2

////////////////////////////////////////////////////////////////////////////
///////////////////////// Operating Modes //////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// artefact family
#define SPECULATOR                      0
#define EXPLORATOR                      1
#define LEGATUS                         2

// the current modes, or software driven functionality in which 
// the firmware should use.
#define TEST_MODE                       9
// speculator modes
#define CICADA_MODE                     0
#define PITCH_MODE                      1
#define CICADA_MODE_NEW                 2

// Explorator modes
#define CLAP_BACK_MODE            101

// Legatus modes
#define ECHO_FEEDBACK_MODE        200
#define MATCH_PITCH_MODE          201
#define FEEDBACK_MODE             202
#define FM_FEEDBACK_MODE          203
#define PLAYBACK_MODE             204
#define REACTIVE_MODE             205
#define MODULAR_LEGATUS_MODE      206

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


/*
  //updateLoopLength();
  #if P_LOOP_LENGTH == true
  elapsedMicros loop_length = 0;
  unsigned long num_loops = 0;
  unsigned long loop_totals = 0;
  unsigned long longest_loop = 0;
  unsigned long shortest_loop = 0;

  void updateLoopLength() {
  if (num_loops > 0) {
    if (loop_length > longest_loop) {
      longest_loop = loop_length;
      Serial.print("new longest loop (in  micros)   : ");
      Serial.println(longest_loop);
    }
    loop_totals += loop_length;
    if (loop_totals % 1000 == 1) {
      Serial.print("average loop length (in micros) : ");
      Serial.println((double)loop_totals / (double) num_loops);
    }
  }
  num_loops++;
  loop_length = 0;
  }
  #endif//print loop length
*/
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
    return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#endif //__MACROS_H
