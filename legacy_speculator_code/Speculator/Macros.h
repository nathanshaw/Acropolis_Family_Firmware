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
////////////////////// Software Configurations /////////////////////////////
////////////////////////////////////////////////////////////////////////////
#define SV_MAJOR           0
#define SV_MINOR           4
#define SV_SUBVERSION      5

// TODO - fix issue with the center out mapping ignoring the very center ring and instead treating the entire outside pcb as a ring

/* 
// version 0.4.5 integrated the weathermanager into the v3 Speculator code (minus the proper shutdown procedures due
   to hardware limitations), fixed issue with the LuxManager not actually forcing lux readings 
   when it needs to. Now the lux manager and the weather manager should both be operating as expected
// version 0.4.4 fixed some issues with the Speculator v3 implementation of the firmware, the user controls were 
   expanded and fixed up a big, fixed a major issue with getting the VEML6030 sensors to work in the same way
   as the VEML7700 sensors (the lux readings were off and some issues were present with the library)
  
// version 0.4.3 added the luxmanager calibrate function back into the code, it is the firmware used for a pop-up installation in late september

// version 0.4.1 is attempting to add the lux manager back into the mix, the crashing issue is fixed, but the brightness
   scaler functionality is broken
// version 0.4.0 fixed the vast majority of the bugs associated with v 0.3.1, there is still an issue with the lux manager,
   but this version is stable
// version 0.3.1 moved many of the classes into the Arduino general libray, for some reason the restart issue is back
   and there are problems with printing, this is not a real working version, just an inbetween
// version 0.3.0 has lots of changes under the hood concerning how the classees are treated. For the v3 units the temp
   and humidity sensor and the WeatherManager was addded. There is a huge problem with the program having a memory leak
   somewhere, I really neeed to find that beforee v 1.0 can be launched
// version 0.2.7-8 xxxx
   updated some code that used long if/else statements to usee switch/case statements instead. Applied the 
   loacal brightness scalers to all values. Bunch of other minor things to get ready for the next field expedition.
   v8 added hue stuff
// version 0.2.6 removed many global variables which were not being used, moved most global variables to the 
   start of Mode.h and removed a good amount of clutter from the program. This included removing the extra Neopixel manager, 
   and support for "stereo" visual feedback. An option to change the Lux Managers feedback was added so the Lux Manager can
   either update the brightness scaler or the Min/Max brightness for linked NeoPixel Managers.Fixed a bug in the
   smooth HSB where they were all smoothedd against brightness. Change thee code to only use one feature collector
   Updated the way that brightness_max works, now it looks at the brightness of all the LEDs together (so a max 
   value now is 765. Changed way that brightness scaler min and max is sent to the lux manager. v0.2.6 was pushed on 
   August 1st 2020
  
// version 0.2.5 added functionality for the thirdd jumper on v2.1. It now determines the amout of smoothing
   for the HSB values. it also reintegrated LBS into the pitch mode. Made it so you could overide the boot delay.
   Also fixed a bug with the center out feedback setting on the Moth v3. Lastly, HSB smoothing now
   applies to all values not just brightness. pushed v on 07/28/2020

// version 0.2.4 is an expansion of 0.2.3 and further improved the debug printing abilities of the
   progam as a whole. Fixed bug with the lux manager not polling correctly. Updated on 07/27/2020

// version 0.2.3 was created on 07/10/20 in reaction to thee field expedition, added threshold control 
   for the click thresholds using the 4th pot, switched around some of the user controls, a splt was made in
   thee functionality of the pins for v3.0 (removing the brightness scaler), the user brightness_scaler was fixed for
   pitch mode, control for the brightness threshold was added for the fourth pot, references to 'click'
   have been replaced with 'onset' which more accurately represents the functions abilities, improved the debug printing,

// version 0.2.2 was created on 07/07/20 and integrated the pitch and cicada modes for a field expedition

// version 0.2.1 was creaeted on 29.06.20 and got a working version of the code working for PCB v2.1 in Cicada Mode as well as
   improving the frame rate from 8 to 29, and adding support for PCB v3 in Cicada mode (minus the onset)

// version 0.2.0 was created on 07/05/20 as the first attempt to get everything workin on PCB v3

// version 0.1.0 was created on 25.02.20 to address issues with the ground enclosure not being as responsive
   as well as the autobrightness calibration routine being very visible and disruptive when resetting

// version 0.1.0 was created on 24.02.20 as it served as the first viable working song implementation

// version 0.0.5 was created on 20.02.20 in the field at Kiatoke Cicada Grove as a first reaction to 
   how version 0.0.4 was performing, the changes included higher starting gains, a brighter flash,
   and the addition of a flag to differentiate between adding the flash brightness of just displaying
   the brightness
*/


////////////////////////////////////////////////////////////////////////////
///////////////////////// Operating Modes //////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// artefact family
#define SPECULATOR                      0
#define EXPLORATOR                      1
#define LEGATUS                         2

// the current modes, or software driven functionality in which 
// the firmware should use.
#define TEST_MODE                       0
#define CICADA_MODE                     1
#define PITCH_MODE                      2
#define CICADA_MODE_NEW                 3

///////////////////// ENCLOSURE_TYPES /////////////////////
#define ORB_ENCLOSURE_WITH_HOLE         0
#define ORB_ENCLOSURE_WITHOUT_HOLE      1
#define GROUND_ENCLOSURE                2
#define NO_ENCLOSURE                    3

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

////////////////////////// LED MAPPING MODES ////////////////
#define LED_MAPPING_STANDARD      0
#define LED_MAPPING_BOTTOM_UP     1
#define LED_MAPPING_ROUND         2
#define LED_MAPPING_CENTER_OUT    3

////////////////////////// Lux Manager stuff ////////////////
// how long will the lux calibratin routine last for?
#define LUX_CALIBRATION_TIME            4000
// how long the lux sensors need the LEDs to be 
// turned off in order to get an accurate reading
#define LUX_SHDN_LEN                    40


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
