/**
  Moth Sonic Art Framework Firmware
  Written by Nathan Villicana-Shaw in 2020
  The runtime, boot, and all other configurations are found in the Configuration.h file
*/
// configuration needs to be added first to determine what mode will be added
#include "Configuration.h"

// #include <MemoryFree.h>

// mechanisms needs be be added before the Mode file
#if ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE != CLAPPER_BODY
#include <Mechanisms.h>
#include <PlaybackEngine.h>
#endif

////////////////////////////// Libraries needed for Every Artefact /////////////////////////////////////
#include <LuxManager.h>
#include <PrintUtils.h>
#include <WS2812Serial.h>
#include <NeopixelManager.h>
#include <AudioEngine.h>
#include <Wire.h>
#include <SPI.h>

#if ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE == CLAPPER_BODY
#else
#include <UIManager.h>
#endif

///////////////////////////// Libraries only needed for certain Artefacts ///////////////////////////////
#if ARTEFACT_TYPE == SPECULATOR && HV_MAJOR == 3
#define WEATHER_MANAGER_PRESENT        true
#include <WeatherManager.h>
#elif ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE == WOODPECKER_BODY
#define WEATHER_MANAGER_PRESENT        true
#include <WeatherManager.h>
#elif ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE == BELL_BODY
#define WEATHER_MANAGER_PRESENT        true
#include <WeatherManager.h>
#elif ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE == CLAPPER_BODY
#define WEATHER_MANAGER_PRESENT        false
#else
#define WEATHER_MANAGER_PRESENT        false
#endif

#if ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE == WOODPECKER_BODY
// TODO - update to the way that motors are taken care of in the main_simple program

//////////////////////////////////////////////////////////////////////////
///////////////////// H-Bridge Motor Driver //////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include <MAX14870Motors.h>

MAX14870Motors motors = MAX14870Motors(MOT1_DIR_PIN, MOT1_PWM_PIN,
                                       MOT2_DIR_PIN, MOT2_PWM_PIN,
                                       MOT3_DIR_PIN, MOT3_PWM_PIN,
                                       MOT1_EN_PIN, MOT2_EN_PIN, MOT3_EN_PIN,
                                       MOT1_FAULT_PIN, MOT2_FAULT_PIN, MOT3_FAULT_PIN);
/*
  DualMAX14870MotorShield motors[3] = {DualMAX14870MotorShield(MOT1_DIR_PIN, MOT1_PWM_PIN, 24, 24, MOT1_EN_PIN, MOT1_FAULT_PIN),
                                     DualMAX14870MotorShield(MOT2_DIR_PIN, MOT2_PWM_PIN, 24, 24, MOT2_EN_PIN, MOT2_FAULT_PIN),
                                     DualMAX14870MotorShield(MOT3_DIR_PIN, MOT3_PWM_PIN, 24, 24, MOT3_EN_PIN, MOT3_FAULT_PIN)
                                    };
*/
void rampMotor(int which, int16_t start, int16_t target, int ramp_total_time) {
  Serial.print("Ramping Motor - ");
  Serial.print(which);
  if (which > 2) {
    return;
  };

  int difference = target - start;
  int step_delay = abs(difference / ramp_total_time);

  Serial.print(" dif: ");
  Serial.print(difference);
  Serial.print(" stepd: ");
  Serial.print(step_delay);
  motors.enableDrivers(which);

  if (difference > 0) {
    for (int16_t i = start; i <= target; i++) {
      if (which == 0) {
        motors.setM1Speed(i);
      }
      else if (which == 1) {
        motors.setM2Speed(i);
      }
      else if (which == 2) {
        motors.setM3Speed(i);
      }
      delay(step_delay);
      // Serial.println(i);
    }
  } else {
    for (int16_t i = start; i > target; i--) {
      if (which == 0) {
        motors.setM1Speed(i);
      }
      else if (which == 1) {
        motors.setM2Speed(i);
      }
      else if (which == 2) {
        motors.setM3Speed(i);
      }
      delay(step_delay);
      // Serial.println(i);
    }
  }
  if (target == 0) {
    motors.disableDrivers(which);
  }
  Serial.println("Disabled Drivers");
}

#endif

#if DATALOG_ACTIVE == true
#include <SD.h>
#include <SerialFlash.h>
#include <EEPROM.h>
#endif // DATALOG_ACTIVE

#if ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE == BELL_BODY
/* Mechatronic Creatures
  "Bowl Bot" Genus
  using the Adafruit Huzzah ESP8266 Microcontroller
*/
/////////////////////////////// Playback Engine  /////////////////////////////////
// the playback engine handles the playback of melodies and rhythms through motors
// and solenoids, as of right now, the only bot which makes use of this is the
// Explorator
Rhythm rhythm[10] = {
  Rhythm(),
  Rhythm(),
  Rhythm(),
  Rhythm(),
  Rhythm(),
  Rhythm(),
  Rhythm(),
  Rhythm(),
  Rhythm(),
  Rhythm()
};

RhythmBank rhythm_bank = RhythmBank();
PlaybackEngine playback_engine = PlaybackEngine();

// actuator pin, dampener pin, frequency, on_time
BellMechanism bells[3] = {
  BellMechanism(s_pins[0], s_pins[1], 20, 100.0,  40),
  BellMechanism(s_pins[2], s_pins[3], 20, 500.0,  40),
  BellMechanism(s_pins[4], s_pins[5], 20, 1000.0, 40)
};

#elif ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE == WOODPECKER_BODY
/////////////////////////////// Playback Engine  /////////////////////////////////
// the playback engine handles the playback of melodies and rhythms through motors
// and solenoids, as of right now, the only bot which makes use of this is the
// Explorator
Rhythm rhythm[10] = {
  Rhythm(),
  Rhythm(),
  Rhythm(),
  Rhythm(),
  Rhythm(),
  Rhythm(),
  Rhythm(),
  Rhythm(),
  Rhythm(),
  Rhythm()
};
RhythmBank rhythm_bank = RhythmBank();
PlaybackEngine playback_engine = PlaybackEngine();
// actuator pin, reference to motor, motor number, minimum_on_time for the solenoid
// maximum on time for the solenoid, min_time between actuations for the solenoid
WoodpeckerMechanism pecker[1] = {WoodpeckerMechanism(s_pins[0], &motors, 0, S1_MIN, S1_MAX, S1_BETWEEN)};

#endif // FIRMWARE_MODE and 

///////////////////////////////  LuxManager //////////////////////////////////////
// All artefacts will have a lux_manager
LuxManager lux_manager = LuxManager(lux_min_reading_delay, lux_max_reading_delay, LUX_MAPPING_SCHEMA);

/////////////////////////////// NeoPixelManager //////////////////////////////////

// for the explorators, there are three NeoPixels Strips
#if ARTEFACT_TYPE == SPECULATOR || ARTEFACT_TYPE == LEGATUS
WS2812Serial leds[1] = WS2812Serial(LED1_COUNT, displayMemory[0], drawingMemory[0], LED1_PIN, WS2812_GRB);
NeoGroup neos[1]     = NeoGroup(&leds[0], 0, LED1_COUNT - 1, "All Neos", MIN_FLASH_TIME, MAX_FLASH_TIME);

#elif ARTEFACT_TYPE == EXPLORATOR
// TODO - dynamically create these objects based on info in the configuration file
WS2812Serial leds[num_active_led_channels] = {
#if LED1_ACTIVE == true
  WS2812Serial(LED1_COUNT, displayMemory[0], drawingMemory[0], LED1_PIN, WS2812_GRB)
#if LED2_ACTIVE + LED3_ACTIVE > 0
  ,
#endif
#endif

#if  LED2_ACTIVE == true
  WS2812Serial(LED2_COUNT, displayMemory[1], drawingMemory[1], LED2_PIN, WS2812_GRB)
#if LED3_ACTIVE == true
  ,
#endif
#endif
#if LED3_ACTIVE == true
  WS2812Serial(LED3_COUNT, displayMemory[2], drawingMemory[2], LED3_PIN, WS2812_GRB)
#endif
};

// TODO - likewise for these
NeoGroup neos[num_active_led_channels] = {
#if LED1_ACTIVE == true
  NeoGroup(&leds[0], 0, LED1_COUNT, LED1_NAME)
#if LED2_ACTIVE + LED3_ACTIVE > 0
  ,
#endif
#endif
#if  LED2_ACTIVE == true
#if LED1_ACTIVE == true
  NeoGroup(&leds[1], 0, LED2_COUNT, LED2_NAME)
#else
  NeoGroup(&leds[0], 0, LED2_COUNT, LED2_NAME)
#endif
#if LED3_ACTIVE == true
  ,
#endif
#endif
#if LED3_ACTIVE == true
  NeoGroup(&leds[2], 0, LED3_COUNT, LED3_NAME)
#endif
};

#endif // ARTEFACT_TYPE == xxxx

/////////////////////////////// FFTManager ///////////////////////////////////////
// all artefacts will have an input FFTManager
#if ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE == CLAPPER_BODY
FFTManager1024 fft_manager[1] = {FFTManager1024(FFT_LOWEST_BIN, FFT_HIGHEST_BIN, "FFT")};
#else
FFTManager1024 fft_manager[2] = {FFTManager1024(FFT_LOWEST_BIN, FFT_HIGHEST_BIN, "Front FFT"), FFTManager1024(FFT_LOWEST_BIN, FFT_HIGHEST_BIN, "Rear FFT")};
#endif // fft_managers

/////////////////////////////// FeatureCollector /////////////////////////////////
// all artefacts will have a single FeatureCollector . . .
FeatureCollector feature_collector = FeatureCollector(NUM_MICROPHONES, "All");

////////////////////////////////////////// AutoGain /////////////////////////////////////////////////////
#if AUTOGAIN_ACTIVE == true
// AutoGain autogain = AutoGain(&feature_collector, &fft_manager[0], &neos[0], AUTOGAIN_MIN_GAIN, AUTOGAIN_MAX_GAIN, MAX_GAIN_ADJUSTMENT);
#endif // AUTOGAIN_ACTIVE

/////////////////////////////// WeatherManager   /////////////////////////////////
#if WEATHER_MANAGER_PRESENT
WeatherManager weather_manager = WeatherManager(HUMID_EXTREME_THRESH, TEMP_EXTREME_THRESH, TEMP_HISTERESIS, WEATHER_MANAGER_UPDATE_RATE);
#endif // WEATHER_MANAGER_PRESENT

//////////////////////////////////// User Controls ///////////////////////////////
#if ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE == CLAPPER_BODY
#else
UIManager uimanager = UIManager(UI_POLLING_RATE, P_USER_CONTROLS);
#endif
////////////////////////////// Audio System ///////////////////////////////////////
#if (ARTEFACT_TYPE == EXPLORATOR) && (BODY_TYPE == WOODPECKER_BODY || BODY_TYPE == BELL_BODY)
AudioInputI2S            i2s;              //xy=634,246
AudioAmplifier           amp1;      //xy=777.1429023742676,277.14284896850586
AudioFilterBiquad        biquad1;
AudioAnalyzePeak         peak1;             //xy=1139.4286575317383,258.42859840393066
AudioAnalyzeFFT1024      fft;           //xy=1145.4286575317383,290.42859840393066
AudioAnalyzeClipCounter  clip_counter;
AudioOutputUSB           usb_output;       //xy=1147.4286575317383,194.42859840393066
AudioConnection          patchCord1(i2s, 0, biquad1, 0);
AudioConnection          patchCord2(biquad1, amp1);
AudioConnection          patchCord3(amp1, clip_counter);
AudioConnection          patchCord6(amp1, peak1);
AudioConnection          patchCord7(amp1, fft);
AudioConnection          patchCord10(amp1, 0, usb_output, 0);
AudioConnection          patchCord11(amp1, 0, usb_output, 1);

#elif (ARTEFACT_TYPE == EXPLORATOR) && (BODY_TYPE == CLAPPER_BODY)

AudioInputI2S            i2s1;           //xy=257,388

// only the HPF and AMP, rms and fft
AudioFilterBiquad        left_HPF;   //xy=412,359
AudioFilterBiquad        left_LPF;   //xy=578,359
AudioAmplifier           left_amp;       //xy=738,360
AudioAnalyzeFFT1024      left_fft;       //xy=950,295
AudioAnalyzePeak         left_peak;      //xy=960,327

AudioOutputUSB           output_usb;     //xy=1189,392

AudioConnection          pc_bq1_l(i2s1, 0, left_HPF, 0);
AudioConnection          pc_amp_l(left_HPF, left_amp);
AudioConnection          pc_peak_l(left_amp, left_peak);
AudioConnection          pc_usb_l(left_amp, 0, output_usb, 0);
AudioConnection          pc_usb_r(i2s1, 0, output_usb, 1);
AudioConnection          pc_fft_l(left_amp, left_fft);

#elif (ARTEFACT_TYPE == SPECULATOR) || (ARTEFACT_TYPE == LEGATUS)
/*
   #include <Audio.h>
  #include <Wire.h>
  #include <SPI.h>
  #include <SD.h>
  #include <SerialFlash.h>

  // GUItool: begin automatically generated code
  AudioInputI2S            I2S;           //xy=86.25000762939453,606.5000085830688
  AudioAnalyzeFFT1024      left_fft;       //xy=271.75000762939453,440.75000858306885
  AudioAnalyzeFFT1024      right_fft;      //xy=271.50000762939453,661.2500085830688
  AudioMixer4              input_mixer1;         //xy=287.00000762939453,495.25000858306885
  AudioMixer4              input_mixer2;         //xy=288.00000762939453,557.2500085830688
  AudioFilterBiquad        right_HPF2;     //xy=427.00000762939453,629.7500076293945
  AudioFilterBiquad        right_HPF1;  //xy=427.25000762939453,597.2500076293945
  AudioFilterBiquad        left_HPF1;   //xy=432.25000762939453,511.50000762939453
  AudioFilterBiquad        left_HPF2;      //xy=434.00000762939453,545.0000076293945
  AudioFilterBiquad        left_LPF1;   //xy=570.25,511.25
  AudioFilterBiquad        left_LPF2;      //xy=571,544.75
  AudioFilterBiquad        right_LPF2;     //xy=572,629.75
  AudioFilterBiquad        right_LPF1;  //xy=572.25,597.25
  AudioAmplifier           left_amp2;      //xy=705.0000095367432,544.750007390976
  AudioAmplifier           left_amp1;       //xy=705.2500095367432,511.25000739097595
  AudioAmplifier           right_amp2;     //xy=714.5000114440918,630.7500076293945
  AudioAmplifier           right_amp1;      //xy=714.7500114440918,597.2500076293945
  AudioAnalyzeRMS          left_rms1;       //xy=899.5000152587891,395.0000057220459
  AudioAnalyzeRMS          right_rms2;     //xy=899.2500305175781,761.500018119812
  AudioAnalyzePeak         right_peak2;    //xy=899.7500152587891,732.0000104904175
  AudioAnalyzePeak         left_peak1;      //xy=902.5000286102295,428.00000953674316
  AudioMixer4              output_mixer1;         //xy=911.5000152587891,546.7500076293945
  AudioMixer4              output_mixer2;         //xy=912.5000152587891,609.7500076293945
  AudioAnalyzeRMS          left_rms2;      //xy=921.2500019073486,467.75000381469727
  AudioAnalyzeRMS          right_rms1;      //xy=921.5000152587891,688.7500076293945
  AudioAnalyzePeak         left_peak2;     //xy=922.5000019073486,499.00000381469727
  AudioAnalyzePeak         right_peak1;     //xy=928.5000152587891,657.7500076293945
  AudioOutputUSB           output_usb;     //xy=1083.5000019073486,579.2500038146973
  AudioConnection          patchCord1(I2S, 0, left_fft, 0);
  AudioConnection          patchCord2(I2S, 0, input_mixer1, 0);
  AudioConnection          patchCord3(I2S, 0, input_mixer2, 0);
  AudioConnection          patchCord4(I2S, 1, right_HPF1, 0);
  AudioConnection          patchCord5(I2S, 1, right_HPF2, 0);
  AudioConnection          patchCord6(I2S, 1, right_fft, 0);
  AudioConnection          patchCord7(I2S, 1, input_mixer1, 1);
  AudioConnection          patchCord8(I2S, 1, input_mixer2, 1);
  AudioConnection          patchCord9(input_mixer1, left_HPF1);
  AudioConnection          patchCord10(input_mixer2, left_HPF2);
  AudioConnection          patchCord11(right_HPF2, right_LPF2);
  AudioConnection          patchCord12(right_HPF1, right_LPF1);
  AudioConnection          patchCord13(left_HPF1, left_LPF1);
  AudioConnection          patchCord14(left_HPF2, left_LPF2);
  AudioConnection          patchCord15(left_LPF1, left_amp1);
  AudioConnection          patchCord16(left_LPF2, left_amp2);
  AudioConnection          patchCord17(right_LPF2, right_amp2);
  AudioConnection          patchCord18(right_LPF1, right_amp1);
  AudioConnection          patchCord19(left_amp2, left_rms2);
  AudioConnection          patchCord20(left_amp2, left_peak2);
  AudioConnection          patchCord21(left_amp2, 0, output_mixer1, 1);
  AudioConnection          patchCord22(left_amp2, 0, output_mixer2, 3);
  AudioConnection          patchCord23(left_amp1, left_rms1);
  AudioConnection          patchCord24(left_amp1, left_peak1);
  AudioConnection          patchCord25(left_amp1, left_rms1);
  AudioConnection          patchCord26(left_amp1, 0, output_mixer1, 0);
  AudioConnection          patchCord27(left_amp1, 0, output_mixer2, 2);
  AudioConnection          patchCord28(right_amp2, right_peak2);
  AudioConnection          patchCord29(right_amp2, right_rms2);
  AudioConnection          patchCord30(right_amp2, 0, output_mixer2, 1);
  AudioConnection          patchCord31(right_amp2, 0, output_mixer1, 3);
  AudioConnection          patchCord32(right_amp1, right_rms1);
  AudioConnection          patchCord33(right_amp1, right_peak1);
  AudioConnection          patchCord34(right_amp1, right_rms1);
  AudioConnection          patchCord35(right_amp1, 0, output_mixer2, 0);
  AudioConnection          patchCord36(right_amp1, 0, output_mixer1, 2);
  AudioConnection          patchCord37(output_mixer1, 0, output_usb, 0);
  AudioConnection          patchCord38(output_mixer2, 0, output_usb, 1);
  // GUItool: end automatically generated code
*/

////////////////////////// Audio Objects //////////////////////////////////////////
AudioInputI2S            i2s1;           //xy=811.6666030883789,543.3333549499512
AudioFilterBiquad        left_HPF;       //xy=962.9998664855957,523.3333139419556
AudioFilterBiquad        right_HPF;        //xy=966.6666641235352,556.6667098999023
AudioMixer4              mixer1;         //xy=1106.6667137145996,563.3334369659424
AudioFilterBiquad        left_LPF;       //xy=1243.6665573120117,561.6666469573975
AudioAmplifier           left_amp;           //xy=1369.9999771118164,559.9999208450317
AudioAnalyzeFFT1024      left_fft;       //xy=1570.999870300293,563.666654586792
AudioAnalyzePeak         left_peak;      //xy=1574.999870300293,595.666654586792
AudioOutputUSB           output_usb;     //xy=1581.9999313354492,630.6666469573975

AudioConnection          patchCord1(i2s1, 0, left_HPF, 0);
AudioConnection          patchCord2(i2s1, 1, right_HPF, 0);
AudioConnection          patchCord3(left_HPF, 0, mixer1, 0);
AudioConnection          patchCord4(right_HPF, 0, mixer1, 1);
AudioConnection          patchCord5(mixer1, 0, output_usb, 1);
AudioConnection          patchCord6(mixer1, left_LPF);
AudioConnection          patchCord7(left_LPF, left_amp);
AudioConnection          patchCord8(left_amp, left_fft);
AudioConnection          patchCord9(left_amp, left_peak);
AudioConnection          patchCord10(left_amp, 0, output_usb, 0);
#endif // ARTEFACT_TYPE and BODY_TYPE and FIRMWARE_MODE 

//////////////////////////////////////////////////////////////////////////////////////
// || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || ||
// || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || ||
// || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || ||


void explainSerialCommands(bool p_user_controls) {
  dprintln(p_user_controls, F("The Following Serial Commands are Supported: "));
  dprintln(p_user_controls, F("Print Commands, denoted by a p prefix: "));
  dprintln(p_user_controls, F("brightness_scaler (bs)"));
  dprintln(p_user_controls, F("datalogs (dl)"));
  dprintMinorDivide(p_user_controls);
}

void listenForSerialCommands(bool p_user_controls) {
  if (Serial.available() > 0) {
    int input = Serial.read();
    dprint(p_user_controls, "incbyte : ");
    dprintln(p_user_controls, input);
    //////////////// PRINT COMMANDS ////////////////
    if (input == 'p') {
      input = Serial.read();
      // datalogs
      if (input == 'd') {
        input = Serial.read();
        if (input == 'l') {
          // this is the command to print the datalog
          // datalog_manager.printAllLogs();
        }
      }
      // print brightness scaler
      if (input == 'b') {
        input = Serial.read();
        if (input == 's') {
          dprint(p_user_controls, "Brightness Scalers: ");
          dprintln(p_user_controls, lux_manager.getBrightnessScaler());
          // dprint(p_user_controls, "\t");
          // dprintln(p_user_controls, brightness_scalers[1]);
        }
      }
      /////////////// Changing Values ////////////////////////
      if (input == 's') {
        input = Serial.read();
        if (input == 'g') {
          dprintln(p_user_controls, "what would you like to change the gain to? Please enter in a gain in the format of 1.00.");
          input = Serial.read() - 48;
          dprint(p_user_controls, "A gain of ");
          dprint(p_user_controls, input);
          dprintln(p_user_controls, " has been selected");
        }
      }
    }
  }
}

void updateAudioAnalysis() {
  // update the feature collectors
#if P_FUNCTION_TIMES == true
  function_times = 0;
#endif // P_FUNCTION_TIMES
  for (int i = 0; i < num_fft_managers; i++) {
    if (fft_manager[i].update()) {
#if P_FUNCTION_TIMES == true
      Serial.print("fft_manager update took ");
      Serial.print(function_times); Serial.println(" micro seconds to update");
#endif // P_FUNCTION_TIMES
    }
  }


#if P_FUNCTION_TIMES == true
  function_times = 0;
#endif // P_FUNCTION_TIMES

  if (feature_collector.update(fft_manager)) {
#if P_FUNCTION_TIMES == true
    Serial.print("feature_collector update took ");
    Serial.print(function_times); Serial.println(" micro seconds to update");
#endif // P_FUNCTION_TIMES
  }
#if P_AUDIO_MEMORY_MAX > 0
  // so the printing is not crazy
  if (last_jumper_read > 99500) {
    Serial.print("audio memory max: ");
    Serial.print(AudioMemoryUsageMax());
    Serial.print(" of ");
    Serial.println(AUDIO_MEMORY);
  }
#endif
}

void printArtefactInfo() {
  //////////////// ARTEFACT TYPE /////////////////
  Serial.print("Artefact type:\t");
#if ARTEFACT_TYPE == SPECULATOR
  Serial.println("SPECULATOR");
#elif ARTEFACT_TYPE == EXPLORATOR
  Serial.println("EXPLORATOR");
  Serial.print("Body type  :\t");
  Serial.println(BODY_TYPE);
#elif ARTEFACT_TYPE == LEGATUS
  Serial.println("LEGATUS");
#else
  Serial.println("UNKNOWN!!!!!");
  delay(15000);
#endif // ARTEFACT_TYPE 

  //////////////// Hardware/Software Version /////////////////
  Serial.print("Firmware is version : ");
  Serial.print(SV_MAJOR);
  Serial.print(".");
  Serial.print(SV_MINOR);
  Serial.print(".");
  Serial.println(SV_REVISION);
  Serial.print("Firmware was compiled for a PCB with a hardware revsion of : ");
  Serial.print(HV_MAJOR);
  Serial.print(".");
  Serial.println(HV_MINOR);
  printMinorDivide();

  //////////////// Enclosure Type ///////////////////////////
#if (ARTEFACT_TYPE == SPECULATOR)
  Serial.print("Enclosure type is : ");
  if (ENCLOSURE_TYPE == GROUND_ENCLOSURE) {
    Serial.println("Ground");
  } else if (ENCLOSURE_TYPE == ORB_ENCLOSURE_WITHOUT_HOLE) {
    Serial.println("Orb without hole");
  } else if (ENCLOSURE_TYPE == ORB_ENCLOSURE_WITH_HOLE) {
    Serial.println("Orb with hole");
  } else if (ENCLOSURE_TYPE == NO_ENCLOSURE) {
    Serial.println("no enclosure");
  } else {
    Serial.println("ERROR, enclosure type is not recognized");
    colorWipeAll(255, 0, 0);
    delay(60000);
  }
  printMinorDivide();
#endif // SPECULATOR ARTEFACT TYPE
}

void setupAudio() {
#if AUDIO_USB == false
  Serial.println("Disconnecting usb patch cords");
  patchCord_usb1.disconnect();
  patchCord_usb2.disconnect();
#endif // audio_usb

  // TODO connect and disconnect objects as needed

  ////////////// Audio ////////////
  printMajorDivide("Setting up Audio Parameters");
  AudioMemory(AUDIO_MEMORY);
  Serial.print("Audio Memory has been set to: ");
  Serial.println(AUDIO_MEMORY);
  delay(500);

  /////////////////////////////////////////////////////////////////////
  feature_collector.linkAmplifier(&left_amp, AUTOGAIN_MIN_GAIN, AUTOGAIN_MAX_GAIN, AUTOGAIN_MAX_GAIN_ADJ);
#if NUM_AMPLIFIERS > 1
  feature_collector.linkAmplifier(&right_amp, AUTOGAIN_MIN_GAIN, AUTOGAIN_MAX_GAIN, AUTOGAIN_MAX_GAIN_ADJ);
#endif
  // feature_collector 0-1 are for the song front/rear
  if (PEAK_FEATURE_ACTIVE) {
    feature_collector.linkPeak(&left_peak, P_PEAK_VALS);
#if NUM_PEAK_ANAS > 1
    feature_collector.linkPeak(&right_peak, P_PEAK_VALS);
#endif
  }

#if (RMS_FEATURE_ACTIVE)
  feature_collector.linkRMS(&left_rms, P_RMS_VALS);
#if NUM_RMS_ANAS > 1
  feature_collector.linkRMS(&right_rms, P_RMS_VALS);
#endif
#endif

  if (FFT_FEATURES_ACTIVE) {
    fft_manager[0].linkFFT(&left_fft, "Front");
#if NUM_FFT > 1
    fft_manager[1].linkFFT(&right_fft, "Rear");
#endif

    for (int i = 0; i < num_fft_managers; i++) {
      Serial.print("Linked FFT to FFTManager mumber");
      Serial.println(i);
      fft_manager[i].setupCentroid(true, CENTROID_FEATURE_MIN, CENTROID_FEATURE_MAX);
      fft_manager[i].setPrintFFTValues(P_FFT_VALS);
      fft_manager[i].setPrintCentroidValues(P_CENTROID_VALS);
      Serial.print("Started calculating Centroid in the FFTManager with a min/max of : ");
      Serial.print(CENTROID_FEATURE_MIN);
      Serial.print("\t");
      Serial.println(CENTROID_FEATURE_MAX);

      fft_manager[i].setCalculateFlux(true);
      fft_manager[i].setPrintFluxValues(P_FLUX_VALS);
      Serial.println("Started calculating FLUX in the FFTManager");
    }
  }
  Serial.println("Feature collectors have been linked");

  /////////////////////////////////////////////////////////////////////
  left_HPF.setHighpass(0, LBQ1_THRESH, LBQ1_Q);
  // left_HPF.setHighpass(1, LBQ1_THRESH, LBQ1_Q);
  // left_HPF.setHighpass(2, LBQ1_THRESH, LBQ1_Q);
  // left_HPF.setLowShelf(3, LBQ1_THRESH, LBQ1_DB);
  Serial.print("Left HPF has been configured (thresh/Q/dB): ");
  Serial.print(LBQ1_THRESH);
  Serial.print("\t");
  Serial.print(LBQ1_Q);
  Serial.print("\t");
  Serial.println(LBQ1_DB);

  left_LPF.setLowpass(0, LBQ2_THRESH, LBQ2_Q);
  // left_LPF.setLowpass(1, LBQ2_THRESH, LBQ2_Q);
  // left_LPF.setLowpass(2, LBQ2_THRESH, LBQ2_Q);
  // left_LPF.setHighShelf(3, LBQ2_THRESH, LBQ2_DB);
  Serial.print("Left LPF has been configured (thresh/Q/dB): ");
  Serial.print(LBQ2_THRESH);
  Serial.print("\t");
  Serial.print(LBQ2_Q);
  Serial.print("\t");
  Serial.println(LBQ2_DB);
  
  right_HPF.setHighpass(0, RBQ1_THRESH, RBQ1_Q);
  right_HPF.setHighpass(1, RBQ1_THRESH, RBQ1_Q);
  right_HPF.setHighpass(2, RBQ1_THRESH, RBQ1_Q);
  right_HPF.setLowShelf(3, RBQ1_THRESH, RBQ1_DB);
  Serial.print("Right HPF has been configured (thresh/Q/dB): ");
  Serial.print(RBQ1_THRESH);
  Serial.print("\t");
  Serial.print(RBQ1_Q);
  Serial.print("\t");
  Serial.println(RBQ1_DB);
  
#if NUM_CHANNELS > 1
  right_LPF.setLowpass(0, RBQ2_THRESH, RBQ2_Q);
  right_LPF.setLowpass(1, RBQ2_THRESH, RBQ2_Q);
  right_LPF.setLowpass(2, RBQ2_THRESH, RBQ2_Q);
  right_LPF.setHighShelf(3, RBQ2_THRESH, RBQ2_DB);
  Serial.print("Right LPF has been configured (thresh/Q/dB): ");
  Serial.print(RBQ2_THRESH);
  Serial.print("\t");
  Serial.print(RBQ2_Q);
  Serial.print("\t");
  Serial.println(RBQ2_DB);
#else
  Serial.print("Skipping setup of 'right' channel as it does not exist");
#endif
  printMinorDivide();

  //////////////////////////////////////////////////////////////////////////////////
  // TODO - make sure ENC_GAIN_ADJ exists for all bots
  Serial.print("Starting gain is now set to: ");
  Serial.println(STARTING_GAIN * ENC_GAIN_ADJUST * USER_CONTROL_GAIN_ADJUST);
  feature_collector.setGain(STARTING_GAIN * ENC_GAIN_ADJUST * USER_CONTROL_GAIN_ADJUST, 0);
  // feature_collector.setGain(STARTING_GAIN * ENC_GAIN_ADJUST * USER_CONTROL_GAIN_ADJUST, 1);
  Serial.print("Set amp1 and amp2 gains to: ");

  // set gain level? automatically?
  // todo make this adapt to when microphones are broken on one or more side...

#if ARTEFACT_TYPE == SPECULATOR || ARTEFACT_TYPE == LEGATUS
  mixer1.gain(0, STARTING_GAIN);
  mixer1.gain(0, STARTING_GAIN);
  // Serial.println("Setting up the FFTManager to track the first channel");
  // fft_manager.addInput(&patchCord_fft_input1);
  // patchCord_fft_input2.disconnect();
#endif

  if (P_DOMINATE_CHANNEL) {
    Serial.print("setting the feature collector to print the dominate channel debug info");
  };
  feature_collector.setPrintDominateChannel(P_DOMINATE_CHANNEL);
  if (P_AUTOGAIN) {
    Serial.print("setting the feature collector to print autogain debug info");
  };
  feature_collector.setPrintAutogain(P_AUTOGAIN);

#if AUTOGAIN_ACTIVE == true
  /////////////////////////////////////////////////////////////////////////////////
  feature_collector.activateAutogain(AUTOGAIN_UPDATE_RATE, AUTOGAIN_INITIAL_DELAY);
  Serial.print("Setting autogain's update rate to: ");
  Serial.println(AUTOGAIN_UPDATE_RATE);
  Serial.print("Setting autogain's start delay to : ");
  Serial.println(AUTOGAIN_INITIAL_DELAY);
#if AUTOGAIN_TRACK_ON_RATIO == true
  Serial.println("Setting autogain to track ON RATIO");
  feature_collector.autogainTrackOnRatio(AUTOGAIN_ON_RATIO_TARGET, AUTOGAIN_ON_RATIO_TOLERANCE, AUTOGAIN_ON_RATIO_MIN, AUTOGAIN_ON_RATIO_MAX, &neos);
#endif

#if AUTOGAIN_TRACK_RMS == true
  Serial.println("Setting autogain to track RMS AVG");
  // target value, tolerance, min_thresh, max_thresh
  feature_collector.autogainTrackAvgRMS(AUTOGAIN_RMS_TARGET, AUTOGAIN_RMS_TOLERANCE, AUTOGAIN_RMS_MIN, AUTOGAIN_RMS_MAX);
#endif

#if AUTOGAIN_TRACK_PEAK == true
  Serial.println("Setting autogain to track PEAK AVG");
  feature_collector.autogainTrackAvgPeak(AUTOGAIN_PEAK_TARGET, AUTOGAIN_PEAK_TOLERANCE, AUTOGAIN_PEAK_MIN, AUTOGAIN_PEAK_MAX);
#endif

#endif // AUTOGAIN_ACTIVE
  feature_collector.testMicrophone();
#if CALCULATE_DOMINATE_CHANNEL
  feature_collector.calculateDominateChannel(fft_manager);
#endif
  Serial.println("Exiting setupAudio()");
  printDivide();
}

#if ARTEFACT_TYPE == SPECULATOR
void speculatorSetup() {
  // setup up some value tracker stuff
  brightness_tracker.setMinMaxUpdateFactor(BGT_MIN_UPDATE_FACTOR, BGT_MAX_UPDATE_FACTOR);
  hue_tracker.setMinMaxUpdateFactor(HUE_MIN_UPDATE_FACTOR, HUE_MAX_UPDATE_FACTOR);
  saturation_tracker.setMinMaxUpdateFactor(SAT_MIN_UPDATE_FACTOR, SAT_MAX_UPDATE_FACTOR);

  //////////////// User Controls /////////////////////////////
  explainSerialCommands(true);
  // TODO - need to replace with the UIManager

  /////////////// User Controls ////////////////////////////////////////////
  // TODO make buttons do something for the speculators again
#if HV_MAJOR == 3
  uimanager.addBut(BUT1_PIN, BUT1_PULLUP, BUT1_LOW_VAL, BUT1_HIGH_VAL, &COLOR_MAP_MODE, BUT1_NAME);
  uimanager.addBut(BUT2_PIN, BUT2_PULLUP, BUT2_LOW_VAL, BUT2_HIGH_VAL, &SQUARE_BRIGHTNESS, BUT2_NAME);
  uimanager.addBut(BUT3_PIN, BUT3_PULLUP, BUT3_LOW_VAL, BUT3_HIGH_VAL, &USE_TARGET_BRIGHTNESS, BUT3_NAME);
  uimanager.addBut(BUT4_PIN, BUT4_PULLUP, BUT4_LOW_VAL, BUT4_HIGH_VAL, &REVERSE_HUE, BUT4_NAME);
  uimanager.addBut(BUT5_PIN, BUT5_PULLUP, BUT5_LOW_VAL, BUT5_HIGH_VAL, &REVERSE_SATURATION, BUT5_NAME);
  uimanager.addBut(BUT6_PIN, BUT6_PULLUP, BUT6_LOW_VAL, BUT6_HIGH_VAL, &REVERSE_BRIGHTNESS, BUT6_NAME);
  uimanager.addBut(BUT7_PIN, BUT7_PULLUP, BUT7_LOW_VAL, BUT7_HIGH_VAL, &AUTOGAIN_ACTIVE, BUT7_NAME);
  uimanager.addBut(BUT8_PIN, BUT8_PULLUP, BUT8_LOW_VAL, BUT8_HIGH_VAL, &BOOT_DELAY_ACTIVE, BUT8_NAME);
  uimanager.addBut(BUT9_PIN, BUT9_PULLUP, BUT9_LOW_VAL, BUT9_HIGH_VAL, &LED_MAPPING_MODE, BUT9_NAME);
  uimanager.addBut(BUT10_PIN, BUT10_PULLUP, BUT10_LOW_VAL, BUT10_HIGH_VAL, &but_test[0], BUT10_NAME);

  uimanager.addPot(POT1_PIN, POT1_REVERSE, POT1_PLAY, &user_brightness_scaler, POT1_NAME);
  uimanager.addPot(POT2_PIN, POT2_REVERSE, POT2_PLAY, &ADDED_SATURATION, POT2_NAME);
  uimanager.addPot(POT4_PIN, POT4_REVERSE, POT4_PLAY, &BRIGHTNESS_CUTTOFF_THRESHOLD,  POT4_NAME);
  uimanager.addPotRange(0, min_user_brightness_scaler, mid_user_brightness_scaler, max_user_brightness_scaler);
  uimanager.addPotRange(1, min_user_brightness_cuttoff, mid_user_brightness_cuttoff, max_user_brightness_cuttoff);

#elif HV_MAJOR == 2
  uimanager.addBut(BUT1_PIN, BUT1_PULLUP, BUT1_LOW_VAL, BUT1_HIGH_VAL, &COLOR_MAP_MODE, BUT1_NAME);
  uimanager.addBut(BUT2_PIN, BUT2_PULLUP, BUT2_LOW_VAL, BUT2_HIGH_VAL, &SQUARE_BRIGHTNESS, BUT2_NAME);
  uimanager.addBut(BUT3_PIN, BUT3_PULLUP, BUT3_LOW_VAL, BUT3_HIGH_VAL, &USE_TARGET_BRIGHTNESS, BUT3_NAME);
  uimanager.addBut(BUT4_PIN, BUT4_PULLUP, BUT4_LOW_VAL, BUT4_HIGH_VAL, &REVERSE_SATURATION, BUT4_NAME);
  uimanager.addBut(BUT5_PIN, BUT5_PULLUP, BUT5_LOW_VAL, BUT5_HIGH_VAL, &REVERSE_HUE, BUT5_NAME);
  uimanager.addBut(BUT6_PIN, BUT6_PULLUP, BUT6_LOW_VAL, BUT6_HIGH_VAL, &BOOT_DELAY_ACTIVE, BUT6_NAME);
#endif
  uimanager.setup();
  uimanager.printAll();
  ///////////////////////// Audio //////////////////////////
  setupAudio();

  ///////////////////////// DL Manager //////////////////////////
  // TODO
  // setupDLManagerCicada();
  // printMinorDivide();
}
#endif

#if ARTEFACT_TYPE == LEGATUS
void legatusSetup() {
  // setup up some value tracker stuff
  brightness_tracker.setMinMaxUpdateFactor(BGT_MIN_UPDATE_FACTOR, BGT_MAX_UPDATE_FACTOR);
  hue_tracker.setMinMaxUpdateFactor(HUE_MIN_UPDATE_FACTOR, HUE_MAX_UPDATE_FACTOR);
  saturation_tracker.setMinMaxUpdateFactor(SAT_MIN_UPDATE_FACTOR, SAT_MAX_UPDATE_FACTOR);

  //////////////// User Controls /////////////////////////////
  explainSerialCommands(true);
  // TODO - need to replace with the UIManager

  /////////////// User Controls ////////////////////////////////////////////
  uimanager.addBut(BUT1_PIN, BUT1_PULLUP, BUT1_LOW_VAL, BUT1_HIGH_VAL, &but_test[0], BUT1_NAME);
  uimanager.addBut(BUT2_PIN, BUT2_PULLUP, BUT2_LOW_VAL, BUT2_HIGH_VAL, &but_test[1], BUT2_NAME);
  uimanager.addBut(BUT3_PIN, BUT3_PULLUP, BUT3_LOW_VAL, BUT3_HIGH_VAL, &but_test[2], BUT3_NAME);
  uimanager.addBut(BUT4_PIN, BUT4_PULLUP, BUT4_LOW_VAL, BUT4_HIGH_VAL, &but_test[3], BUT4_NAME);
  uimanager.addBut(BUT5_PIN, BUT5_PULLUP, BUT5_LOW_VAL, BUT5_HIGH_VAL, &but_test[4], BUT5_NAME);
  uimanager.addBut(BUT6_PIN, BUT6_PULLUP, BUT6_LOW_VAL, BUT6_HIGH_VAL, &but_test[5], BUT6_NAME);

  uimanager.addPot(POT1_PIN, POT1_REVERSE, POT1_PLAY, &user_brightness_scaler, POT1_NAME);
  uimanager.addPot(POT2_PIN, POT2_REVERSE, POT2_PLAY, &BRIGHTNESS_CUTTOFF_THRESHOLD,  POT2_NAME);
  uimanager.addPotRange(0, min_user_brightness_scaler, mid_user_brightness_scaler, max_user_brightness_scaler);
  uimanager.addPotRange(1, min_user_brightness_cuttoff, mid_user_brightness_cuttoff, max_user_brightness_cuttoff);

  uimanager.setup();
  uimanager.printAll();
  ///////////////////////// Audio //////////////////////////
  setupAudio();
}
#endif // ARTEFACT_TYPE == LEGATUS

#if ARTEFACT_TYPE == SPECULATOR
void speculatorLoop() {
#if CALCULATE_DOMINATE_CHANNEL
  dominate_channel = feature_collector.getDominateChannel();
  // Serial.print("dominate channel is : ");
  // Serial.println(dominate_channel);
#endif
  if (COLOR_MAP_MODE == COLOR_MAPPING_HSB) {
    double s = calculateSaturation(&feature_collector, &fft_manager[dominate_channel]);   
    double b = calculateBrightness(&feature_collector, &fft_manager[dominate_channel]);    // user brightness scaler is applied in this function
    double h = calculateHue(&feature_collector, &fft_manager[dominate_channel]);
    printHSB();
    printRGB();

    if (feature_collector.isActive() == true) {
      neos[0].colorWipeHSB(h, s, b);// now colorWipe the LEDs with the HSB value
    } else {
      Serial.println("ERROR - not able to updateNeos() as there is no active audio channels");
    }
  }
  else if (COLOR_MAP_MODE == COLOR_MAPPING_EXPLORATOR) {
    updateFeedbackLEDs(&fft_manager[dominate_channel]);
    // Serial.println("Finished running updateFeedbackLEDs()");
    // delay(2000);
  }
  else {
    Serial.println("ERROR = that color mode is not implemented in update neos");
  }
}
#endif // ARTEFACT_TYPE == SPECULATOR

#if ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE  == WOODPECKER_BODY

void exploratorLoop() {
  ///////////////// Actuator Outputs //////////////////
  playback_engine.update(); // will also update all linked mechanisms

  ///////////////// Passive Visual Feedback ///////////
  updateFeedbackLEDs(&fft_manager[dominate_channel]);

  // the warmer the temperature the more it will actuate? (10 second decrease at 40 degrees and no decrease when at 0 degrees
  // the higher the humidity the less it will actuate? (100 second increase when 100% humidity , 0 second when at 0 %)
  // the brighter it is the more it will actuate (take 5000 lux and subtract the current reading)
  // activity level adds a base of up to five minutes
  ACTUATION_DELAY = (ACTIVITY_LEVEL * ACTIVITY_LEVEL * 5 * 60000) + (weather_manager.getTemperature() * -250) + (weather_manager.getHumidity() * 1000) + (5000 - lux_manager.getGlobalLux());
  // uint16_t t = random(45, 150);
  // TODO
  ACTUATION_DELAY = ACTUATION_DELAY * 0.5;
  if (last_playback_tmr > ACTUATION_DELAY) {
    Serial.print("ACTUATION_DELAY is : ");
    Serial.println(ACTUATION_DELAY);
    if (playback_engine.isActive() == false) {
      playback_engine.playRhythm(rhythm_bank.getRandomRhythm());
      last_playback_tmr = 0;
    } else {
      Serial.println("Skipping rhythm as a rhythm is already playing");
    }
  }
}

#elif ARTEFAT_TYPE == EXPLORATOR && BODY_TYPE == BELL_BODY
void exploratorLoop() {
  ///////////////// Actuator Outputs //////////////////
  updateSolenoids(); // turns off all solenoids which have
  playback_engine.update();
  ACTUATION_DELAY = (30000) + ((weather_manager.getTemperature() + weather_manager.getHumidity()) * lux_manager.getGlobalLux());
  if (last_playback_tmr > ACTUATION_DELAY) {
    Serial.print("actuation_delay : ");
    Serial.println(ACTUATION_DELAY);
    Serial.println("playing rhythm through playback_engine");
    playback_engine.playRhythm(rhythm_bank.getRandomRhythm());
    last_playback_tmr = 0;
  }
  updateFeedbackLEDs(&fft_manager[0]);
}

#elif ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE == CLAPPER_BODY

#define LISTENING_STATE 0
#define ACTUATING_STATE 1

double onset_cent = 1000.0;
double onset_rms = 0.0;
double onset_feature = 0.0;

ValueTrackerDouble onset_tracker = ValueTrackerDouble((String)"Onset Feature", &onset_feature, 0.1, 3000, 1.0);
ValueTrackerDouble cent_tracker = ValueTrackerDouble((String)"Cent", &onset_cent, 0.1, 3000, 1.0);
ValueTrackerDouble rms_tracker = ValueTrackerDouble((String)"RMS", &onset_rms, 0.1, 3000, 1.0);

bool updateOnset() {
  // for a onset in theory the spectral flux will be high, the
  // centroid will decrease since the last frame, and there should be an
  // increase of amplitude in the 1k - 3k freq range

  // for all these values we want a 1.0 to be a confident assesment
  // from that feature
  /////////////////////////// Spectral Flux //////////////////////////
  double onset_flux = fft_manager[0].getScaledFlux();
  dprint(P_ONSET_FLUX, "onset flux: ");
  dprintln(P_ONSET_FLUX, onset_flux);

  //////////////////////////// Cent Neg Delta ////////////////////////
  onset_cent = fft_manager[0].getCentroid();
  cent_tracker.update();
  onset_cent = cent_tracker.getScaled();

  //////////////////////////// Energy between 1k - 3k //////////////////
  // using the bins instead of the hard coded frequencies saves some cycles
  // double range_rms = fft_manager.getFFTRangeByFreq(1000, 2000);
  onset_rms = feature_collector.getPeak(0);
  rms_tracker.update();
  onset_rms = rms_tracker.getScaled();

  //////////////////////////// Feature Calculation /////////////////////
  onset_feature = onset_flux + onset_cent + onset_rms;
  onset_tracker.update();
  // onset_feature = onset_tracker.getScaled();

  // note that normally 1.0 is the threshold for onsets
  if (onset_feature >= ONSET_THRESH) {
    if (P_ONSET_FEATURES == true && onset_feature > 0.1) {
      Serial.print("cent / flux / rms :\t");
      Serial.print((onset_cent), 8);
      Serial.print("\t");
      Serial.print(onset_flux, 8);
      Serial.print("\t");
      Serial.print(onset_rms, 8);
      Serial.print("  =  ");
      Serial.println(onset_feature, 8);
    }
    uint8_t red, green, blue;
    ////////////////// Calculate Actual Values ///////////////////////
    red = (current_color * ONSET_RED) * lux_manager.getBrightnessScaler();
    green = (current_color * ONSET_GREEN) * lux_manager.getBrightnessScaler();
    blue = (current_color * ONSET_BLUE) * lux_manager.getBrightnessScaler();
    // Serial.println("____________________ ONSET ________________________ 3.0");
    dprint(P_ONSET, "onset feature is above threshold: ");
    dprint(P_ONSET, onset_feature);
    dprint(P_ONSET, " - ");
    dprint(P_ONSET, ONSET_THRESH);
    neos[0].colorWipeAdd(red, green, blue, lux_manager.getBrightnessScaler() * user_brightness_scaler);
    return true;
  }
  return false;
}


void exploratorLoop() {
  updateSolenoids(); // turns off all solenoids which need to be turned off
  //  listen for onsets
  if (updateOnset() || last_playback_tmr > 60000) {
    triggerSolenoid(2, 40);
    triggerSolenoid(7, 40);
    last_playback_tmr = 0;
  }
  // if onset detected, immedietally actuate
  // pause audio analysis for x period of time
  updateFeedbackLEDs(&fft_manager[dominate_channel]);
}

#endif // Explorator Loops

//////////////////////////////////////////////////////////////////////////
////////////////// setup / main loops ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#if ARTEFACT_TYPE == EXPLORATOR
void exploratorSetup() {

  printMajorDivide("starting exploratorSetup() loop");

#if ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE == WOODPECKER_BODY
  pecker[0].setMotorSpeeds(70, -30);
#endif

  /////////////// User Controls ////////////////////////////////////////////
#if BODY_TYPE == WOODPECKER_BODY || BODY_TYPE == BELL_BODY
  uimanager.addBut(BUT1_PIN, BUT1_PULLUP, BUT1_LOW_VAL, BUT1_HIGH_VAL, &but_test[0], BUT1_NAME);
  uimanager.addBut(BUT2_PIN, BUT2_PULLUP, BUT2_LOW_VAL, BUT2_HIGH_VAL, &but_test[1], BUT2_NAME);
  uimanager.addBut(BUT3_PIN, BUT3_PULLUP, BUT3_LOW_VAL, BUT3_HIGH_VAL, &but_test[2], BUT3_NAME);
  uimanager.addBut(BUT4_PIN, BUT4_PULLUP, BUT4_LOW_VAL, BUT4_HIGH_VAL, &but_test[3], BUT4_NAME);

  uimanager.addPot(POT1_PIN, POT1_PLAY, &ACTIVITY_LEVEL, POT1_NAME);
  uimanager.addPot(POT2_PIN, POT2_PLAY, &STRIKE_LENGTH,  POT2_NAME);

  uimanager.setup();
  uimanager.printAll();
#endif // UI controls for the woodpecker and bellbot

  ////////////////////// Audio
  printMinorDivide();
  Serial.println("Starting the Audio system");
  AudioMemory(AUDIO_MEMORY);
  // TODO make this proper
  uint32_t lpf = 14000;
  uint32_t hpf = 200;
  double q = 0.8;

  left_amp.gain(60.0);

  left_HPF.setLowpass(0, lpf, q);
  left_HPF.setLowpass(1, lpf, q);
#if BODY_TYPE == CLAPPER_BODY
  left_HPF.setLowpass(2, lpf, q);
  left_HPF.setLowpass(3, lpf, q);
#else
  left_HPF.setHighpass(2, hpf, q);
  left_HPF.setHighpass(3, hpf, q);
#endif

  configurePlaybackEngine();

  fft_manager[0].linkFFT(&left_fft, "All");
  fft_manager[0].setCalculateCent(true);
  fft_manager[0].setCalculateFlux(true);

  feature_collector.linkPeak(&left_peak, P_PEAK_VALS);

  Serial.println("Finished starting the LED strips");
  printMinorDivide();

  printMinorDivide();
  Serial.println("Finished setup Loop");
  colorWipeAll(0, 120, 30, 0.25);
  delay(3000);
  printMinorDivide();
}

void setOutputs() {
  /////////////// Solenoid Outputs /////////////////////////////////////////
  for (int i = 0; i < NUM_SOLENOIDS; i++) {
    pinMode(s_pins[i], OUTPUT);
    digitalWrite(s_pins[i], LOW);// turns them off
  }

#if TEST_SOLENOIDS == true
  Serial.println("TEST_SOLENOIDS is set to true, system will just test solenoids over and over again forever");
  testSolenoids(1000);// let the system settle
  Serial.println("Finished setting solenoid pins to outputs");
#endif

#if (NUM_MOTORS > 0)
  Serial.println("Starting Motor Set-up");
  motors.flipM1(M1_POLARITY);
  motors.flipM2(M2_POLARITY);
  motors.flipM3(M3_POLARITY);
  motors.enableDrivers();
  motors.setM1Speed(0);
  motors.setM2Speed(0);
  motors.setM3Speed(0);
  delay(500);
  Serial.println("Finished setting up motors");
  Serial.println("-------------------------------------------------------------");
  delay(2500);// let the system settle
#endif
}
#endif //  ARTEFACT_TYPE == EXPLORATOR

#if (ARTEFACT_TYPE == EXPLORATOR) && TEST_SOLENOIDS == true
void testSolenoids(unsigned int len) {
  elapsedMillis t = 0;
  Serial.print("Testing Solenoids - ");
  while (t < len) {
    for (int i = 0; i < NUM_SOLENOIDS; i++) {
      if (sol_active[i] == true) {
        Serial.print(i);
        digitalWrite(s_pins[i], HIGH); // LOW is on for these transistors as a low opens the gate
        delay(30);
        Serial.print(" ");
        digitalWrite(s_pins[i], LOW);
      }
    }
    Serial.println();
  }
  Serial.println("Finished testing solenoids");
  Serial.println("--------------------------");
};
#endif // test_solenoids, artefact_type explorator

void setupLuxManager() {
  printMinorDivide();
  Serial.println("turning off LEDs for Lux Calibration");
  for (int i = 0; i < num_active_led_channels; i++) {
    neos[i].colorWipe(0, 0, 0, 0.0);
  }
  Serial.println("LEDS off");
  delay(100);

  lux_manager.setLuxThresholds(LOW_LUX_THRESHOLD, MID_LUX_THRESHOLD, HIGH_LUX_THRESHOLD, EXTREME_LUX_THRESHOLD);
  lux_manager.setPrintBrightnessScaler(P_BS);
  Serial.print("lux_manager print_brightness_scaler is set to   : \t");
  Serial.println(P_BS);
  lux_manager.setPrintLuxReadings(P_LUX_READINGS);
  Serial.print("lux_manager print_lux_readings is set to        : \t");
  Serial.println(P_LUX_READINGS);
  lux_manager.setPrintGeneralDebug(P_LUX_MANAGER_DEBUG);
  Serial.print("lux_manager print_general_debug is set to       : \t");
  Serial.println(P_LUX_MANAGER_DEBUG);

  delay(500);
#if (ARTEFACT_TYPE == SPECULATOR) && (HV_MAJOR == 3)
  lux_manager.add6030Sensors(2, 25);
#elif (ARTEFACT_TYPE == SPECULATOR) && (HV_MAJOR == 2)
  lux_manager.addSensorTcaIdx("Front", 0);
  lux_manager.addSensorTcaIdx("Rear", 1);
  lux_manager.startTCA7700Sensors(VEML7700_GAIN_1, VEML7700_IT_25MS); // todo add this to config_adv? todo
#elif (ARTEFACT_TYPE == EXPLORATOR)
  Serial.println("Starting LuxManager");
  lux_manager.add7700Sensor((String)"Eye-Stock");
  lux_manager.start7700Sensor(VEML7700_GAIN_1, VEML7700_IT_25MS); // todo add this to config_adv? todo
  Serial.println("Finished starting LuxManager");
  printMinorDivide();
#endif // HV_MAJOR
  delay(2000);

  if (lux_manager.getSensorActive(0) || lux_manager.getSensorActive(1)) {
    Serial.println("Linked the NeoGroups to the LuxManager");
    for (int i = 0; i < num_active_led_channels; i++) {
      lux_manager.linkNeoGroup(&neos[i]);
    }
    delay(200);
  } else {
    Serial.println("ERROR, unable to link NeoGroups to LuxManager");
  }
  lux_manager.setBrightnessScalerMinMax(LUX_BS_MIN, LUX_BS_MAX);
  lux_manager.calibrate(2000, true);
  if (P_LUMIN) {
    lux_manager.print();
  };
}

#if WEATHER_MANAGER_PRESENT == true
void extremeHumidityShutdown() {
  colorWipeAll(0, 0, 0);
  while (true) {
    Serial.println("HUMIDTY SHUTDOWN INITALISED!!!!!!");
    delay(10000);
  }
  // TODO
}

void extremeTemperatureShutdown() {
  unsigned int times = 0;
  Serial.println("TEMPERATURE SHUTDOWN INITALISED!!!!!!");
  colorWipeAll(255, 0, 0);
  delay(1000);
  colorWipeAll(0, 0, 0);
  while (weather_manager.getTempShutdown() == true) {
    weather_manager.update();
    if (times % 20 == 10) {
      colorWipeAll(255, 0, 0);
    } else if (times % 20 == 11) {
      colorWipeAll(0, 0, 0);
    }
    times++;
    delay(100);
  }
  Serial.println("TEMPERATURE SHUTDOWN LIFTED - resuming normal operation");
}
#endif // WEATHER_MANAGER_PRESENT == true

void colorWipeAll(uint8_t red, uint8_t green, uint8_t blue, float bright) {
  for (int i = 0; i < num_active_led_channels; i++) {
    neos[i].colorWipe(red, green, blue, bright);
  }
}

void colorWipeAll(uint8_t red, uint8_t green, uint8_t blue) {
  colorWipeAll(red, green, blue, 1.0);
}

#if P_AUDIO_USAGE_MAX == true
void printAudioUsage() {
  // print the audio usage every second or so
  if (last_audio_usage_print > AUDIO_USAGE_POLL_RATE) {
    Serial.print("memory usage: ");
    Serial.print(AudioMemoryUsageMax());
    Serial.print(" out of ");
    Serial.println(AUDIO_MEMORY);
    last_audio_usage_print = 0;
    AudioMemoryUsageMaxReset();
  }
}
#endif

#if ARTEFACT_TYPE == EXPLORATOR
void configurePlaybackEngine() {
  // freq, length, onset delay (since last note), velocity
#if BODY_TYPE == BELL_BODY
  rhythm[0].addPitchedNote(500.0,  40, 0,    0.6);
  rhythm[0].addPitchedNote(500.0,  40, 100, 0.8);
  rhythm[0].addPitchedNote(500.0,  30, 150, 1.0);
  rhythm[0].addPitchedNote(1000.0, 30, 400, 1.0);
  rhythm[0].addPitchedNote(1000.0, 20, 1000, 1.0);
  rhythm[0].addPitchedNote(1000.0, 50, 1500, 1.0);

  rhythm[0].addPitchedNote(100.0,  30, 100, 0.3);
  rhythm[0].addPitchedNote(500.0,  40, 600, 0.4);
  rhythm[0].addPitchedNote(1000.0, 30, 150, 0.5);
  rhythm[0].addPitchedNote(100.0,  20, 700, 0.4);
  rhythm[0].addPitchedNote(500.0,  40, 800, 0.3);
  rhythm[0].addPitchedNote(1000.0, 30, 1500, 0.2);

  rhythm[0].addPitchedNote(100.0,  40, 200, 1.0);
  rhythm[0].addPitchedNote(500.0,  30, 70,  1.0);
  rhythm[0].addPitchedNote(1000.0, 40, 90,  1.0);
  rhythm[0].addPitchedNote(100.0,  40, 60,  1.0);
  rhythm[0].addPitchedNote(500.0,  30, 150, 1.0);
  rhythm[0].addPitchedNote(1000.0, 30, 100, 1.0);

  rhythm[1].addPitchedNote(50.0,   50, 0,    1.05);
  rhythm[1].addPitchedNote(150.0,  50, 500,  1.01);
  rhythm[1].addPitchedNote(250.0,  60, 1000, 1.15);
  rhythm[1].addPitchedNote(550.0,  60, 1500, 1.20);
  rhythm[1].addPitchedNote(450.0,  50, 2000, 1.20);
  rhythm[1].addPitchedNote(1550.0, 40, 2500, 1.20);
  rhythm[1].addPitchedNote(1650.0, 30, 3000, 1.20);

  rhythm[2].addPitchedNote(50.0,   40, 0,    1.35);
  rhythm[2].addPitchedNote(150.0,  30, 400,  1.31);
  rhythm[2].addPitchedNote(250.0,  30, 800,  1.35);
  rhythm[2].addPitchedNote(1350.0, 40, 1200, 1.30);
  rhythm[2].addPitchedNote(450.0,  30, 2000, 1.30);
  rhythm[2].addPitchedNote(550.0,  50, 2400, 1.30);
  rhythm[2].addPitchedNote(1650.0, 20, 2600, 1.30);

  rhythm[3].addPitchedNote(150.0,  40, 0,    1.35);
  rhythm[3].addPitchedNote(150.0,  50, 400,  1.31);
  rhythm[3].addPitchedNote(250.0,  60, 600,  1.35);
  rhythm[3].addPitchedNote(1350.0, 40, 700,  1.30);
  rhythm[3].addPitchedNote(450.0,  40, 800,  1.30);
  rhythm[3].addPitchedNote(550.0,  50, 500,  1.30);
  rhythm[3].addPitchedNote(1650.0, 40, 1000, 1.30);

  rhythm_bank.addRhythm(& rhythm[0]);
  rhythm_bank.addRhythm(& rhythm[1]);
  rhythm_bank.addRhythm(& rhythm[2]);
  rhythm_bank.addRhythm(& rhythm[3]);
  * /

  playback_engine.linkBellMechanism(& bells[0]);
  playback_engine.linkBellMechanism(& bells[1]);
  playback_engine.linkBellMechanism(& bells[2]);

  playback_engine.linkNeoGroup(& neos[0]);
  playback_engine.linkNeoGroup(& neos[1]);
  playback_engine.linkNeoGroup(& neos[2]);

#elif BODY_TYPE == WOODPECKER_BODY
  for (int i = 0; i < 10; i++) {
    uint32_t quarter = random(120, 750);
    buildPeckRhythm(i, quarter);
    if (random(0, 100)) {
      rhythm[i].addUnpitchedNote(quarter * 4, 0.75);
      buildPeckRhythm(i, quarter);
    }
    rhythm[i].addMotorMove(1, 70, 100);
    rhythm[i].print();
    rhythm_bank.addRhythm(& rhythm[i]);
  }
  playback_engine.linkMechanism(& pecker[0]);
  playback_engine.linkNeoGroup(& neos[1]);
#endif // BODY_TYPE == BELL_BODY
}

#if BODY_TYPE == WOODPECKER_BODY
void buildPeckRhythm(int idx, uint32_t quarter) {
  uint32_t t = 0;
  uint32_t eigth = quarter / 2;
  uint32_t triplet = quarter / 3;

  // 30% chance to start with a hard strike
  if (random(0, 100) < 30) {
    rhythm[idx].addUnpitchedNote(0, 0.5);
    t = quarter;
  }

  // then 85% chance for a triplet of 3-4
  if (random(0, 100) < 85) {
    for (int i = 0; i < (int)random(3, 4); i++) {
      rhythm[idx].addUnpitchedNote(t, 0.25);
      t = triplet;
    }
  }

  // then 55% chance for a triplet of 3-4
  if (random(0, 100) < 55) {
    for (int i = 0; i < (int)random(3, 4); i++) {
      rhythm[idx].addUnpitchedNote(t, 0.25);
      t = triplet;
    }
  }

  // chance for a pause
  if (random(0, 100) < 45) {
    t += triplet;
    if (random(0, 100) < 45) {
      t += triplet;
    }
  }

  // then 55% chance for a triplet of 3-4
  if (random(0, 100) < 55) {
    for (int i = 0; i < (int)random(3, 4); i++) {
      rhythm[idx].addUnpitchedNote(t, 0.25);
      t = triplet;
    }
  }

  // chance for a pause
  if (random(0, 100) < 45) {
    t += triplet;
    if (random(0, 100) < 45) {
      t += eigth;
    }
    if (random(0, 100) < 65) {
      t += eigth;
    }
  }

  // 30% chance to start with a hard strike
  if (random(0, 100) < 30) {
    rhythm[idx].addUnpitchedNote(t, 0.5);
    t = quarter;
  }
}
#endif // BODY_TYPE == WOODPECKER_BODY
#endif // ARTEFACT_TYPE == EXPLORATOR

#if ARTEFACT_TYPE == LEGATUS
void legatusLoop() {
  Serial.println("WARNING LEGATUS LOOP IS NOT YET IMPLEMENTED");
}
#endif // ARTEFACT_TYPE == LEGATUS
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
#if NUM_SOLENOIDS > 0 || NUM_MOTORS > 0
  setOutputs();
#endif
  delay(1000);
  ///////////////// Serial ///////////////////////////////////
  printDivide();
  Serial.println("Entering the Setup Loop");
  printDivide();
  Serial.println("Serial begun");
  printMinorDivide();
  printArtefactInfo();

  /////////////////// NeoPixels //////////////////f //////////////
  printMinorDivide();
  Serial.println("Starting the LED strips");

  for (int i = 0; i < num_active_led_channels; i++) {
    leds[i].begin();
    Serial.print("neogroup ");
    Serial.println(i);

    neos[i].begin();
    neos[i].colorWipe(12, 12, 12, 1.0);
    printMinorDivide();
    Serial.println("LEDS have been initalised");
    Serial.print("There are ");
    Serial.print(LED1_COUNT);
    Serial.println(" LEDs in the first group");
    Serial.print("There are ");
    Serial.print(LED2_COUNT);
    Serial.println(" LEDs in the second group");
    Serial.print("There are ");
    Serial.print(LED3_COUNT);
    Serial.println(" LEDs in the third group");

    Serial.print("NeoGroup p_lux and p_extreme_lux are both set to : \t"); Serial.println(P_LUMIN);
    neos[i].setPrintLux(P_LUMIN);
    Serial.print("NeoGroup p_lux is set to                         : \t"); Serial.println(P_LUMIN);
    neos[i].setPrintExtremeLux(P_LUMIN);
    Serial.print("NeoGroup p_brightness_scaler is set to           : \t"); Serial.println(P_BS);
    neos[i].setPrintBrightnessScaler(P_BS);
    Serial.print("NeoGroup p_leds_on is set to                     : \t"); Serial.println(P_LEDS_ON);
    neos[i].setPrintLedsOn(P_LEDS_ON);
    Serial.print("NeoGroup p_on_ratio is set to                    : \t"); Serial.println(P_LED_ON_RATIO);
    neos[i].setPrintOnRatio(P_LED_ON_RATIO);
    Serial.print("NeoGroup p_color_wipe is set to                  : \t"); Serial.println(P_COLOR_WIPE);
    neos[i].setPrintColorWipe(P_COLOR_WIPE);
    Serial.print("NeoGroup p_onset is set to                       : \t"); Serial.println(P_ONSET);
    neos[i].setPrintOnRatio(P_ONSET);
    Serial.print("NeoGroup p_onset is set to                       : \t"); Serial.println(P_ONSET);
    neos[i].setPrintPackColors(P_PACK_COLORS);
    printMinorDivide();
    Serial.print("NeoGroup SATURATED_COLORS                        : \t"); Serial.println(P_ONSET);
    neos[i].setSaturatedColors(SATURATED_COLORS);

    neos[i].setOnsetColors(ONSET_RED, ONSET_GREEN, ONSET_BLUE);

    neos[i].setFlashBehaviour(FLASH_DOMINATES);
    Serial.print("NeoGroup flash_behaviour is set to                : \t");
    Serial.println(FLASH_DOMINATES);

    neos[i].changeMapping(LED_MAPPING_MODE);
    // neos[i].setSongFeedbackMode(ROUND);
    Serial.print("LED_MAPPING_MODE is set to                        : \t");
    Serial.println(LED_MAPPING_MODE);

#if (ARTEFACT_TYPE == SPECULATOR)
    neos[i].setSongColors(SONG_RED_HIGH, SONG_GREEN_HIGH, SONG_BLUE_HIGH);
    Serial.print("REVERSE_HUE is set to                             : \t");
    Serial.println(REVERSE_HUE);
    Serial.print("REVERSE_SATURATION is set to                      : \t");
    Serial.println(REVERSE_SATURATION);
    Serial.print("REVERSE_BRIGHTNESS is set to                      : \t");
    Serial.println(REVERSE_BRIGHTNESS);
    delay(2000);
#endif
  }
  Serial.println("Finished starting the LED strips");
  printMinorDivide();

  ///////////////////////// Weather Manager /////////////////////
  // nothing is needed =P
#if WEATHER_MANAGER_PRESENT == true
  printMinorDivide();
  Serial.println("initalising the weather manager");
  weather_manager.init();
  weather_manager.setPrintReadings(P_WEATHER_MANAGER_READINGS);
  Serial.println("finished initalising the weather manager");
  Serial.print("WeatherManager HUMID_EdXTREME_THRESH  : \t");
  Serial.println(HUMID_EXTREME_THRESH);
  Serial.print("WeatherManager TEMP_EXTREME_THRESH   : \t");
  Serial.println(TEMP_EXTREME_THRESH);
  Serial.print("WeatherManager TEMP_HISTERESIS      : \t");
  Serial.println(TEMP_HISTERESIS);
  Serial.print("WEATHER_MANAGER_UPDATE_RATE         : \t");
  Serial.println(WEATHER_MANAGER_UPDATE_RATE);
  weather_manager.update();
  weather_manager.print();
  printMinorDivide();
#endif

  //////////////////////////// Lux Sensors //////////////////////////////
  setupLuxManager();

  ///////////////////////// Artefact Specific Logic ////////////////////////////
#if ARTEFACT_TYPE == SPECULATOR
  speculatorSetup();
#elif ARTEFACT_TYPE == EXPLORATOR
  exploratorSetup();
#elif ARTEFACT_TYPE == LEGATUS
  legatusSetup();
#endif

  /////////////////////////////// Main Loop Delay ////////////////////////////////
  for (int it = 0; it < LED1_COUNT; it++) {
    // we keep readng the jumpers so if we change the jumpers and dont
    // want the boot delay that can happen
    // TODO - replace with the user control manager
    // readJumpers();
    // uimanager.update();
    uint32_t segment = (uint32_t)((double)BOOT_DELAY / (double)LED1_COUNT);
    if (segment > 10) {
      Serial.print("boot delay segment length in seconds is : ");
      Serial.println(segment / 1000, 2);
    }
    neos[0].setPixel(it, 10, 32, 20, 1.0);
    if (BOOT_DELAY_ACTIVE) {
      delay(segment);
#if ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE == CLAPPER_BODY
#else
      uimanager.update();
#endif // only update UIManager if it exists
    }
  }
  neos[0].colorWipe(0, 5, 0, 1.0);
  printMajorDivide("Now starting main() loop");
}

void loop() {
  //////////////// Testing ///////////////////////////////////
  /*
    if (playback_engine.isActive() == true){
    clip_counter.deactivate();
    } else {
    clip_counter.activate();
    Serial.println(clip_counter.getNumClips());
    }
  */

  ///////////////// Ambient Lighting /////////////////////////
  if (lux_manager.getActive() == true) {
    if (lux_manager.update()) {
      // print the updated values
      if (P_LUX_MANAGER_READINGS) {
        lux_manager.print();
      };
    }
  }

  ///////////////// User Controls ////////////////////////////
#if ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE == CLAPPER_BODY
#else
  if (uimanager.update() && P_USER_CONTROLS) {
    uimanager.printAll();
    for (int i = 0; i < NUM_NEOP_MANAGERS; i++) {
      neos[i].changeMapping(LED_MAPPING_MODE);
    }
  }
#endif

  ///////////////// WeatherManager ///////////////////////////
#if WEATHER_MANAGER_PRESENT == true
  if (weather_manager.update() && P_WEATHER_MANAGER_READINGS) {
    weather_manager.print();
  }
  if (weather_manager.getHumidityShutdown() == true) {
    extremeHumidityShutdown();
  } else if (weather_manager.getTempShutdown() == true) {
    extremeTemperatureShutdown();
  } else {
#endif // WEATHER_MANAGER_PRESENT
    // if (lux_manager.getExtremeLux() == true) {
    //   Serial.println("WARNING ------------ updateMode() returning due extreme lux conditions, not updating onset or song...");
    // } else {
    //////////////// Audio Analysis ///////////////////////////
    updateAudioAnalysis();
    // Serial.println("updateAudioAnalysis() function finished running");
    // delay(1000);
    // Serial.println(millis()/1000);
#if ARTEFACT_TYPE == SPECULATOR
    speculatorLoop();
#elif ARTEFACT_TYPE == EXPLORATOR
    exploratorLoop();
#elif ARTEFACT_TYPE == LEGATUS
    legatusLoop();
#endif
    // Serial.println("Artefact Specific update function finished running");
    // delay(1000);
    // TODO
    // updateDatalog();
    // readUserControls(P_USER_CONTROLS);
    // }

#if WEATHER_MANAGER_PRESENT
  }
#endif // WEATHER_MANAGER_PRESENT

#if CALCULATE_DOMINATE_CHANNEL == true
  // Serial.println("Calculating dominate channel");
  // delay(10000);
  if (millis() > first_dominate_channel_calculation && dominate_channel_update > dominate_channel_update_rate) {
    dominate_channel_update = 0;
    feature_collector.calculateDominateChannel(fft_manager);
  }
#endif

#if P_AUDIO_USAGE_MAX == true
  printAudioUsage();
#endif
}
