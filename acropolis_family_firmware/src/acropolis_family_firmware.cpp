#include <Arduino.h>

// configuration needs to be added first to determine what mode will be added
#include "configuration/Configuration.h"

// #include <MemoryFree.h>
#if ARTEFACT_GENUS == EXPLORATOR
// #include <explorator_functions.ino>
#if ARTEFACT_SPECIES == EX_CLAPPER || ARTEFACT_SPECIES == EX_WINDER
// no need for the mechanisms or playback engine for the music box guy
#else
#include <Mechanisms.h>
#include <PlaybackEngine.h>
#endif
#endif

////////////////////////////// Libraries needed for Every Artefact /////////////////////////////////////
// #include "lib/LuxManager/LuxManager.h"
#include <LuxManager.h>
// #include "lib/PrintUtils/PrintUtils.h"
#include <PrintUtils.h>
#include <WS2812Serial.h>
// #include "lib/NeopixelManager/NeopixelManager.h"
#include <NeopixelManager.h>
// #include "lib/AudioEngine/AudioEngine.h"
#include <AudioEngine.h>
#include <Wire.h>
#include <SPI.h>
#include <UIManager.h>
////////////////////////////////////////////////////////////////
///////////////////////// SD CARD //////////////////////////////
////////////////////////////////////////////////////////////////
#if ARTEFACT_GENUS == LEGATUS
#include <SD.h>
Sd2Card card;
SdVolume volume;
SdFile root;
#endif // SD card stuff

////////////////////////////////////////////////////////////////
///////////////////////// ENCODER //////////////////////////////
////////////////////////////////////////////////////////////////
#if ARTEFACT_GENUS == EXPLORATOR && (ARTEFACT_SPECIES == EX_SPINNER)
#include "Encoder.h"
Encoder enc(20, 21);
long enc_pos = 0;
#elif ARTEFACT_GENUS == EXPLORATOR && (ARTEFACT_SPECIES == EX_WINDER)
#include "Encoder.h"
Encoder enc(12, 11);
long enc_pos = 0;
#endif // encoder library

#if WEATHER_MANAGER_ACTIVE == true
#include <WeatherManager.h>
#endif

////////////////////////////////////////////////////////////////
///////////////////////// Audio Routing ////////////////////////
////////////////////////////////////////////////////////////////

// Dynamic audio routine connections
AudioConnection *audio_connections[20];

#if ARTEFACT_GENUS == LEGATUS
File audio_file;
#endif

#if ARTEFACT_GENUS == SPECULATOR
////////////////////////// Audio Objects //////////////////////////////////////////
AudioInputI2S            i2s1;           //xy=248,198
AudioMixer4              mixer1;         //xy=368,212
AudioFilterBiquad        HPF1;           //xy=498,197
AudioFilterBiquad        HPF2;           //xy=499,233
AudioFilterBiquad        LPF1;           //xy=619,196
AudioFilterBiquad        LPF2;           //xy=621,233
AudioAmplifier           amp1;           //xy=736,198
AudioAmplifier           amp2;           //xy=738,232

#if NUM_PEAK_ANAS > 0
AudioAnalyzePeak         peak1;          //xy=902,231
#endif
#if NUM_PEAK_ANAS > 1
AudioAnalyzePeak         peak2;          //xy=902,266
#endif

#if NUM_RMS_ANAS > 0
AudioAnalyzeRMS          rms1;          //xy=902,231
#endif
#if NUM_RMS_ANAS > 1
AudioAnalyzeRMS          rms2;          //xy=902,266
#endif

#if NUM_FFT > 0
AudioAnalyzeFFT1024      fft1;           //xy=902,160
#endif
#if NUM_FFT > 1
AudioAnalyzeFFT1024      fft2;           //xy=903,196
#endif

#if AUDIO_USB
AudioOutputUSB           output_usb;     //xy=916,300
AudioConnection          patchCordUSB2(mixer1, 0, output_usb, 1);
AudioConnection          patchCordUSB1(amp1, 0, output_usb, 0);
#endif
//////////////////////////////////////////////////////////////////////
#elif (ARTEFACT_GENUS == EXPLORATOR)
AudioInputI2S i2s1;        //xy=1203.3333854675293,1356.3334693908691
AudioMixer4 mixer1;        //xy=1373.3333549499512,1335.0000076293945
AudioAmplifier amp1;       //xy=1519.3333206176758,1330.0000114440918
AudioFilterBiquad biquad1;    //xy=1651.6667022705078,1330.6666841506958
AudioFilterBiquad biquad2;    //xy=1651.6667022705078,1330.6666841506958
AudioAnalyzeFFT1024 fft1;  //xy=1846.3333740234375,1254.9999675750732
AudioAnalyzePeak peak1;    //xy=1851.3332901000977,1288.6667175292969

#if AUDIO_USB
AudioOutputUSB output_usb; //xy=1857.0000457763672,1368.666639328003
AudioConnection patchCordUSB2(mixer1, 0, output_usb, 1);
AudioConnection patchCordUSB1(HPF1, 0, output_usb, 0);
#endif

//////////////////////////////////////////////////////////////////////
#elif ARTEFACT_GENUS == LEGATUS 
// NOTE, the default configuration is for playback mode
////////////////////////// Audio Objects //////////////////////////////////////////
AudioInputI2S i2s1; //xy=723.2500076293945,775.5000114440918

AudioMixer4 mixer1; //xy=884.5000114440918,788.0000114440918
AudioMixer4 mixer2; //xy=1375.0000190734863,652.5000705718994

AudioFilterBiquad biquad1; //xy=944.2500305175781,635.000018119812
AudioFilterBiquad biquad2; //xy=944.7500152587891,667.0000095367432
AudioFilterBiquad biquad3; //xy=1025.500015258789,760.5000114440918
AudioFilterBiquad biquad4; //xy=1160.250015258789,761.0000133514404

AudioAmplifier amp1; //xy=1130.000015258789,636.0000095367432
AudioAmplifier amp2; //xy=1132.750015258789,669.0000095367432
AudioAmplifier amp3; //xy=1295.0000190734863,761.5000114440918

AudioRecordQueue record_queue;
// AudioPlayQueue play_queue;
AudioOutputI2S i2s_output; //xy=1356.0000190734863,604.5000095367432

AudioAnalyzeRMS rms1;     //xy=1578.5000228881836,682.5000114440918
AudioAnalyzeFFT1024 fft1; //xy=1578.4999885559082,714.2499980926514
AudioAnalyzePeak peak1;   //xy=1582.4999885559082,746.2499980926514

#if OUTPUT_USB
// TOOD - not sure if this routing makes any sense
AudioOutputUSB output_usb; //xy=1589.4999885559082,781.2499980926514
AudioConnection patchCordUSB2(mixer1, 0, output_usb, 1);
AudioConnection patchCordUSB1(amp1, 0, output_usb, 0);
#endif // OUTPUT_USB

#if USE_RAW_AUDIO_PLAYER
AudioPlaySdRaw audio_player; //xy=767.0000267028809,648.750018119812
#else
AudioPlaySdWav audio_player; //xy=767.0000267028809,648.750018119812
#endif

// this should only be created if using the FM mode??
#if BEHAVIOUR_ROUTINE == B_LEG_FM_FEEDBACK
AudioSynthWaveformSineModulated sine_fm; //xy=1054.285743713379,2268.5713901519775
#elif BEHAVIOUR_ROUTINE == B_LEG_FEEDBACK
AudioEffectDelay audio_delay1;
#elif BEHAVIOUR_ROUTINE == B_LEG_MATCH_PITCH
AudioSynthWaveform waveform;
#endif

AudioControlSGTL5000 sgtl5000; //xy=997.7500152587891,513.2500104904175
#endif                         // audio system for legatus modular firmware

//////////////////////////////////////////////////////////////////////////
///////////////////// H-Bridge Motor Driver //////////////////////////////
//////////////////////////////////////////////////////////////////////////
#if NUM_MOTORS > 0
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
void rampMotor(int which, int16_t start, int16_t target, int ramp_total_time)
{
  Serial.print("Ramping Motor - ");
  Serial.print(which);
  if (which > 2)
  {
    return;
  };

  int difference = target - start;
  int step_delay = abs(difference / ramp_total_time);

  Serial.print(" dif: ");
  Serial.print(difference);
  Serial.print(" stepd: ");
  Serial.print(step_delay);
  motors.enableDrivers(which);

  if (difference > 0)
  {
    for (int16_t i = start; i <= target; i++)
    {
      if (which == 0)
      {
        motors.setM1Speed(i);
      }
      else if (which == 1)
      {
        motors.setM2Speed(i);
      }
      else if (which == 2)
      {
        motors.setM3Speed(i);
      }
      delay(step_delay);
      // Serial.println(i);
    }
  }
  else
  {
    for (int16_t i = start; i > target; i--)
    {
      if (which == 0)
      {
        motors.setM1Speed(i);
      }
      else if (which == 1)
      {
        motors.setM2Speed(i);
      }
      else if (which == 2)
      {
        motors.setM3Speed(i);
      }
      delay(step_delay);
      // Serial.println(i);
    }
  }
  if (target == 0)
  {
    motors.disableDrivers(which);
  }
  Serial.println("Disabled Drivers");
}
#endif // motor stuff

#if DATALOG_ACTIVE == true
#include <DLManager.h>
//#include <SerialFlash.h>
//#include <EEPROM.h>
DLManager datalog_manager = DLManager();
#if LOG_RUNTIME
datalog_manager.addAutolog("runtime", runtime);
#endif // log runtime
#endif // DATALOG_ACTIVE

///////////////////////////////  LuxManager //////////////////////////////////////
// All artefacts will have a lux_manager
LuxManager lux_manager = LuxManager(lux_min_reading_delay, lux_max_reading_delay, LUX_MAPPING_SCHEMA);

/////////////////////////////// NeoPixelManager //////////////////////////////////


// for the explorators, there are three NeoPixels Strips
#if ARTEFACT_GENUS == SPECULATOR || ARTEFACT_GENUS == LEGATUS

WS2812Serial leds[1] = {WS2812Serial(LED1_COUNT, displayMemory[0], drawingMemory[0], LED1_PIN, WS2812_GRB)};
NeoGroup neos[1] = {NeoGroup(&leds[0], 0, LED1_COUNT - 1, "All Neos", MIN_FLASH_TIME, MAX_FLASH_TIME)};

#elif ARTEFACT_GENUS == EXPLORATOR
// no idea why I have to make a temporary int here, but I do....
int _max_leds = max(LED1_COUNT, LED2_COUNT);
int max_leds = max(_max_leds, LED3_COUNT);
// byte drawingMemory[3][max_leds*3];         //  3 bytes per LED
// DMAMEM byte displayMemory[3][max_leds*12]; // 12 bytes per LED

// TODO - dynamically create these objects based on info in the configuration file
WS2812Serial leds[num_active_led_channels] = {
#if LED1_ACTIVE
  WS2812Serial(LED1_COUNT, displayMemory[0], drawingMemory[0], LED1_PIN, WS2812_GRB)
#if LED2_ACTIVE + LED3_ACTIVE > 0 || LED1_ACTIVE > 0
  , // very important to add a comma, as there is more than one NeoPixel control object
#endif
#endif

#if LED2_ACTIVE 
  WS2812Serial(LED2_COUNT, displayMemory[1], drawingMemory[1], LED2_PIN, WS2812_GRB)
#if LED3_ACTIVE 
  ,
#endif
#endif
#if LED3_ACTIVE
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
#if LED2_ACTIVE == true
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

#endif // ARTEFACT_GENUS == xxxx

/////////////////////////////// FFTManager ///////////////////////////////////////
// all artefacts will have an input FFTManager
#if NUM_FFT == 1
FFTManager1024 fft_manager[1] = {FFTManager1024(FFT_LOWEST_BIN, FFT_HIGHEST_BIN, "FFT")};
#elif NUM_FFT == 2
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
#if WEATHER_MANAGER_ACTIVE
WeatherManager weather_manager = WeatherManager(HUMID_EXTREME_THRESH, TEMP_EXTREME_THRESH, TEMP_HISTERESIS, WEATHER_MANAGER_UPDATE_RATE);

#endif // WEATHER_MANAGER_ACTIVE

//////////////////////////////////////////////////////////////////////
#if USER_CONTROLS_ACTIVE
UIManager uimanager = UIManager(UI_POLLING_RATE, P_USER_CONTROLS);
#endif





/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// Start of PITCH functions ////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
bool getColorFromFFTSingleRange(FFTManager1024 * f, uint8_t s, uint8_t e)
{
  double tot, frac;
  frac = f->getFFTRangeByIdx(s, e);
  tot = f->getFFTRangeByIdx(FFT_LOWEST_BIN, 128);
  frac = frac / tot;
  // RGBConverter::HsvToRgb(frac, 0.5, 1, 0, red, green, blue);
  return 1;
}

double calculateBrightness(FeatureCollector * f, FFTManager1024 * _fft)
{
  double b = 0.0;
  dprintMinorDivide(P_BRIGHTNESS);
  dprint(P_BRIGHTNESS, "calculating HSB Brightness: ");
  switch (BRIGHTNESS_FEATURE)
  {
    case (FEATURE_PEAK_AVG):
      dprintln(P_BRIGHTNESS, "feature is PEAK_AVG");
      b = f->getDominatePeakAvg();
      f->resetDominatePeakAvg();
      break;
    case (FEATURE_PEAK):
      dprintln(P_BRIGHTNESS, "feature is PEAK");
      // f->printPeakVals();
      b = f->getDominatePeak();
      break;
    case (FEATURE_RMS_AVG):
      dprintln(P_BRIGHTNESS, "feature is RMS_AVG");
      b = f->getDominateRMSAvg();
      f->resetDominateRMSAvg();
      break;
    case (FEATURE_RMS):
      dprintln(P_BRIGHTNESS, "feature is RMS");
      b = f->getDominateRMS() * 100;
      break;
    case (FEATURE_FFT_ENERGY):
      dprintln(P_BRIGHTNESS, "feature is FFT_ENERGY");
      b = _fft->getFFTTotalEnergy()*10;
      break;
    case FEATURE_FFT:
      dprint(P_BRIGHTNESS, "BGT feature FEATURE_FFT");
      b = (double)constrain(_fft->getHighestEnergyIdx(), 7, 255);
      break;
    case (FEATURE_FFT_RELATIVE_ENERGY):
      // get how much energy is stored in the max bin, get the amount of energy stored in all bins
      b = _fft->getRelativeEnergy(_fft->getHighestEnergyIdx());
      // Serial.print("highestEnergyIdx: ");Serial.println(_fft->getHighestEnergyIdx());
      // Serial.print("relative energy in highest energy bin: ");Serial.println(sat);
      break;
    case (FEATURE_STRONG_FFT):
      // range index is what the highest energy bin is within the range we care about
      // b = _fft->getFFTTotalEnergy();
      Serial.println("WARNING - FEATURE_STONG_FFT is not currently implemented");
      break;
    default:
      Serial.println("ERROR - calculateHSBBrightness() does not accept that  BRIGHTNESS_FEATURE");
      break;
  }
  ///////////////////////// If user controls are in place to scale the brightness it is done now //////////////////////
  /////////////////////// Make sure that it is within bounds ////////////////////
  dprint(P_BRIGHTNESS, "brightness is: ");
  dprint(P_BRIGHTNESS, b);
  brightness = b;

  // Scale the brightness value according to a rolling average 
  brightness_tracker.update();
  brightness = brightness_tracker.getScaled();
  dprint(P_BRIGHTNESS, "\t");
  dprintln(P_BRIGHTNESS, brightness);
  ////////////////////////// When using target_brightness
  if (USE_TARGET_BRIGHTNESS == true)
  {
    // here the target_brightness value is used as the actual brightness and the actual as the target
    if (brightness > target_brightness)
    {
      target_brightness += 0.003;
      if (brightness < target_brightness)
      {
        target_brightness = brightness;
      }
    }
    else if (brightness < target_brightness)
    {
      target_brightness -= 0.003;
      if (brightness > target_brightness)
      {
        target_brightness = brightness;
      }
    }
    dprint(P_BRIGHTNESS, "brightness changed due to target: ");
    dprint(P_BRIGHTNESS, brightness);
    dprint(P_BRIGHTNESS, " = ");
    brightness = target_brightness;
    dprintln(P_BRIGHTNESS, brightness);
  }

  /////////////////////////// Reverse ////////////////////////
  if (REVERSE_BRIGHTNESS == true)
  {
    brightness = 1.0 - brightness;
  }

  #if ADD_B_OFFSET
  brightness += 0.1;
  #endif

  //////////////////////// Scale down the brightness and make it more exponential for better results //////////////////
  if (SQUARE_BRIGHTNESS == true && brightness < 1.0)
  {
    dprint(P_BS + P_BRIGHTNESS, "changing brightness due to SQUARE_BRIGHTNESS | before: ");
    dprint(P_BS + P_BRIGHTNESS, brightness, 4);
    brightness = (brightness) * brightness;
    dprint(P_BS + P_BRIGHTNESS, " after: ");
    dprintln(P_BS + P_BRIGHTNESS, brightness, 4);
  }

  /////////////////////////// Apply user brightness scaler ////////////////////////
  if (USER_BS_ACTIVE > 0 && (user_brightness_scaler > 1.001 || user_brightness_scaler < 0.999))
  {
    dprint(P_BS + P_BRIGHTNESS, "brightness adj due to user_bs (scaler is: ");
    dprint(P_BS + P_BRIGHTNESS, user_brightness_scaler, 4);
    dprint(P_BS + P_BRIGHTNESS, ") | before: ");
    dprint(P_BS + P_BRIGHTNESS, brightness, 4);
    brightness = brightness * user_brightness_scaler;
    dprint(P_BS + P_BRIGHTNESS, " after: ");
    dprintln(P_BS + P_BRIGHTNESS, brightness, 4);
  }
  /////////////////////// Make sure that it is within bounds ////////////////////
  #if USER_CONTROLS_ACTIVE
  if (brightness < user_brightness_cuttoff)
  {
    dprint(P_BS + P_BRIGHTNESS, "brightness lower than user_brightness_cuttoff of ");
    dprint(P_BS + P_BRIGHTNESS, user_brightness_cuttoff);
    dprintln(P_BS + P_BRIGHTNESS, " changing to 0.0");
    brightness = 0;
  }
  else if (brightness > 1.0 + user_brightness_cuttoff)
  {
    dprint(P_BS + P_BRIGHTNESS, "brightness higher than 1.0 (");
    dprint(P_BS + P_BRIGHTNESS, brightness);
    dprintln(P_BS + P_BRIGHTNESS, "), changing to 1.0");
    brightness = 1.0;
  }
  else
  {
    brightness = brightness - user_brightness_cuttoff;
  }
  #endif // USER_CONTROLS_ACTIVE
  dprintMinorDivide(P_BRIGHTNESS);
  return brightness;
}

double calculateSaturation(FeatureCollector * f, FFTManager1024 * _fft)
{
  double sat = 0.0;
  switch (SATURATION_FEATURE)
  {
    case (FEATURE_PEAK_AVG):
      dprint(P_SATURATION, "Feature is FEATURE_PEAK_AVG: ");
      sat = f->getDominatePeakAvg();
      // Serial.println(sat);
      // Serial.print("sat set to  : ");Serial.println(hsb[i][1]);
      f->resetDominatePeakAvg();
      break;
    case (FEATURE_RMS_AVG):
      dprint(P_SATURATION, "Feature is FEATURE_RMS_AVG: ");
      sat = f->getDominateRMSAvg();
      // Serial.print("sat set to  : ");Serial.println(hsb[i][1]);
      f->resetDominateRMSAvg();
      break;
    case (FEATURE_FFT_RELATIVE_ENERGY):
      dprint(P_SATURATION, "Feature is FEATURE_FFT_RELATIVE_ENERGY: ");
      // get how much energy is stored in the max bin, get the amount of energy stored in all bins
      sat = _fft->getRelativeEnergy(_fft->getHighestEnergyIdx());
      // Serial.print("highestEnergyIdx: ");Serial.println(_fft->getHighestEnergyIdx());
      // Serial.print("relative energy in highest energy bin: ");Serial.println(sat);
      break;
    case (FEATURE_FLUX):
      // sat = (_fft->getFlux() - 20) / 60;
      dprint(P_SATURATION, "Feature is FEATURE_FLUX: ");
      sat = _fft->getFlux();
      break;
    default:
      Serial.print("ERROR - calculateSaturation() does not accept that  SATURATION_FEATURE");
      break;
  }
  saturation = sat;
  saturation_tracker.update();
  dprint(P_SATURATION, "saturation before/after scaling: ");
  dprint(P_SATURATION, saturation);
  saturation = saturation_tracker.getRAvg();
  // saturation = (9.9 * log10((double)saturation + 1.0)) - (2.0 * (double)saturation);
  if (REVERSE_SATURATION == true)
  {
    saturation = 1.0 - saturation;
  }
  saturation += ADDED_SATURATION; // just add some base saturation to make the feedback more colourful
  saturation = constrainf(saturation, 0.0, 1.0);
  dprint(P_SATURATION, " / ");
  dprint(P_SATURATION, saturation, 4);
  dprint(P_SATURATION, "\tsat min/max: ");
  dprint(P_SATURATION, saturation_tracker.getMin(), 4);
  dprint(P_SATURATION, " / ");
  dprintln(P_SATURATION, saturation_tracker.getMax(), 4);
  return saturation;
}

double calculateHue(FeatureCollector * f, FFTManager1024 * _fft)
{
  // In theory, the Hue should be between 0.0 and 1.0 to be useful for the rest of the program
  double h = 0.0;
  switch (HUE_FEATURE)
  {
    case FEATURE_FFT_BIN_RANGE:
      dprint(P_HUE, "Hue feature FEATURE_FFT_BIN_RANGE");
      h = getColorFromFFTSingleRange(_fft, 3, 20);
      break;
    case FEATURE_FFT:
      dprint(P_HUE, "Hue feature FEATURE_FFT");
      h = (double)constrain(_fft->getHighestEnergyIdx(), 7, 255);
      break;
    case FEATURE_FFT_MAX_BIN:
      dprint(P_HUE, "Hue feature FEATURE_FFT_MAX_BIN");
      // calculate the bin with the most energy,
      // Serial.print("Highest energy bin is: ");Serial.println(f->getHighestEnergyBin(FFT_LOWEST_BIN, FFT_HIGHEST_BIN));
      // map the bin  index to a hue value
      h = (double)(_fft->getHighestEnergyIdx(FFT_LOWEST_BIN, FFT_HIGHEST_BIN) - FFT_LOWEST_BIN) / FFT_HIGHEST_BIN;
      // Serial.print("max bin hu e is : ");Serial.println(hue);
      break;
    case FEATURE_PEAK_AVG:
      dprint(P_HUE, "Hue feature FEATURE_PEAK_AVG");
      h = f->getDominatePeakAvg();
      f->resetDominatePeakAvg();
      break;
    case FEATURE_PEAK:
      dprint(P_HUE, "Hue feature FEATURE_PEAK");
      h = f->getDominatePeak();
      break;
    case FEATURE_RMS_AVG:
      dprint(P_HUE, "Hue feature FEATURE_RMS_AVG");
      h = f->getDominateRMSAvg();
      break;
    case FEATURE_RMS:
      dprint(P_HUE, "Hue feature FEATURE_RMS");
      h = f->getDominateRMS();
      break;
    case FEATURE_CENTROID:
      dprint(P_HUE, "Hue feature FEATURE_CENTROID");
      // the centroid will be a frequency between about 200 and 50000
      // first I need to move it to a more linear scale
      h = (double)_fft->getCentroid();
      // next I need to scale down to a value between 0.0 and 1.0
      break;
    case FEATURE_FLUX:
      dprint(P_HUE, "Hue feature FEATURE_FLUX");
      h = _fft->getFlux();
      break;
    default:
      Serial.println("ERROR - calculateHue() does not accept that HUE_FEATURE");
      break;
  }
  dprint(P_HUE, " Initial value is: ");
  dprintln(P_HUE, hue);
  hue = h;
  hue_tracker.update();
  hue = hue_tracker.getRAvg();
  /////////////////////////// Reverse ////////////////////////
  if (REVERSE_HUE == true)
  {
    hue = 1.0 - hue;
  }
  if (hue > 1.0)
  {
    hue = 1.0;
  }
  else if (hue < 0.0)
  {
    hue = 0.0;
  }
  dprint(P_HUE, " after processing ");
  dprintln(P_HUE, hue);
  return hue;
}

void printHSB()
{
  if (P_HSB)
  {
    Serial.print("h: ");
    Serial.print(hue);
    Serial.print("\ts: ");
    Serial.print(saturation);
    Serial.print("\tb: ");
    Serial.println(brightness);
  }
}

void printRGB()
{
  // fft_manager.printFFTVals();
  if (P_NEO_COLORS)
  {
    neos[0].printColors();
  }
}

// #endif // ARTEFACT_GENUS == EXPLORATOR

//////////////////////////////////////////////////////////////////////////////////////
// || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || ||
// || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || ||
// || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || || ||

void explainSerialCommands(bool p_user_controls)
{
  dprintln(p_user_controls, F("The Following Serial Commands are Supported: "));
  dprintln(p_user_controls, F("Print Commands, denoted by a p prefix: "));
  dprintln(p_user_controls, F("brightness_scaler (bs)"));
  dprintln(p_user_controls, F("datalogs (dl)"));
  dprintMinorDivide(p_user_controls);
}

void listenForSerialCommands(bool p_user_controls)
{
  if (Serial.available() > 0)
  {
    int input = Serial.read();
    dprint(p_user_controls, "incbyte : ");
    dprintln(p_user_controls, input);
    //////////////// PRINT COMMANDS ////////////////
    if (input == 'p')
    {
      input = Serial.read();
      // datalogs
      if (input == 'd')
      {
        input = Serial.read();
        if (input == 'l')
        {
          // this is the command to print the datalog
          // datalog_manager.printAllLogs();
        }
      }
      // print brightness scaler
      if (input == 'b')
      {
        input = Serial.read();
        if (input == 's')
        {
          dprint(p_user_controls, "Brightness Scalers: ");
          dprintln(p_user_controls, lux_manager.getBrightnessScaler());
          // dprint(p_user_controls, "\t");
          // dprintln(p_user_controls, brightness_scalers[1]);
        }
      }
      /////////////// Changing Values ////////////////////////
      if (input == 's')
      {
        input = Serial.read();
        if (input == 'g')
        {
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

void updateAudioAnalysis()
{
  // update the feature collectors
#if P_FUNCTION_TIMES == true
  //Serial.print("time between updateAudioAnalysis() function calls is ");
  //Serial.print(function_times);
  //Serial.println(" micro seconds");
  //function_times = 0;
#endif // P_FUNCTION_TIMES
  for (int i = 0; i < NUM_FFT; i++)
  {
    // fft_manager[i].printFFTVals();
    if (fft_manager[i].update())
    {
#if P_FUNCTION_TIMES == true
      // Serial.print("fft_manager update took ");
      // Serial.print(function_times);
      // Serial.println(" micro seconds to update");
#endif // P_FUNCTION_TIMES
    }
  }

#if P_FUNCTION_TIMES == true
  // function_times = 0;
#endif // P_FUNCTION_TIMES

  if (feature_collector.update(fft_manager))
  {
#if P_FUNCTION_TIMES == true
    // Serial.print("feature_collector update took ");
    // Serial.print(function_times);
    // Serial.println(" micro seconds to update");
#endif // P_FUNCTION_TIMES
  }
#if P_AUDIO_MEMORY_MAX > 0
  // so the printing is not crazy
  if (last_audio_use_print > AUDIO_USE_RESET_RATE)
  {
    Serial.print("audio memory max: ");
    Serial.print(AudioMemoryUsageMax());
    Serial.print(" of ");
    Serial.println(AUDIO_MEMORY);
    last_audio_use_print = 0;
  }
#endif
}

void colorWipeAll(uint8_t red, uint8_t green, uint8_t blue, float bright)
{
  for (int i = 0; i < num_active_led_channels; i++)
  {
    neos[i].colorWipe(red, green, blue, bright);
  }
}

void colorWipeAll(uint8_t red, uint8_t green, uint8_t blue)
{
  colorWipeAll(red, green, blue, 1.0);
}

void printArtefactInfo()
{
  //////////////// ARTEFACT TYPE /////////////////
  Serial.print("Artefact type:\t");
#if ARTEFACT_GENUS == SPECULATOR
  Serial.println("SPECULATOR");
#elif ARTEFACT_GENUS == EXPLORATOR
  Serial.println("EXPLORATOR");
  Serial.print("Body type  :\t");
  if (ARTEFACT_SPECIES == EX_WINDER)
  {
    Serial.println("MB Body");
  }
  else if (ARTEFACT_SPECIES == EX_CHIPPER)
  {
    Serial.println("Pecker Body");
  }
  else if (ARTEFACT_SPECIES == EX_CHIRPER)
  {
    Serial.println("Bell Body");
  }
  else if (ARTEFACT_SPECIES == EX_CLAPPER)
  {
    Serial.println("Clapper Body");
  }
  else if (ARTEFACT_SPECIES == EX_SPINNER)
  {
    Serial.println("Shaker Body");
  }
  else
  {
    Serial.println("WARNING - BODY TYPE IS NOT RECOGNIZED");
  }

#elif ARTEFACT_GENUS == LEGATUS
  Serial.println("LEGATUS");
#else
  Serial.println("UNKNOWN!!!!!");
  delay(15000);
#endif // ARTEFACT_GENUS

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
  Serial.print("Enclosure type is : ");
  if (ENCLOSURE_TYPE == GROUND_ENCLOSURE)
  {
    Serial.println("Ground");
  }
  else if (ENCLOSURE_TYPE == ORB_ENCLOSURE)
  {
    Serial.println("Orb without hole");
  }
  else if (ENCLOSURE_TYPE == ORB_ENCLOSURE_WITH_HOLE)
  {
    Serial.println("Orb with hole");
  }
  else if (ENCLOSURE_TYPE == NO_ENCLOSURE)
  {
    Serial.println("no enclosure");
  }
  else
  {
    Serial.println("ERROR, enclosure type is not recognized");
    colorWipeAll(255, 0, 0);
    delay(60000);
  }
  printMinorDivide();
}


////////////////////////////////////////////////////////////////////////////
#if ONSET_DETECTION_ACTIVE
bool updateOnset()
{
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
  if (onset_feature >= ONSET_THRESH)
  {
    if (P_ONSET_FEATURES == true && onset_feature > 0.1)
    {
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
#endif // ONSET_DETECTION is active
//////////////////////////////////////////

void setupLuxManager()
{
  printMinorDivide();
  Serial.println("turning off LEDs for Lux Calibration");
  for (int i = 0; i < num_active_led_channels; i++)
  {
    neos[i].colorWipe(0, 0, 0, 0.0);
  }
  Serial.println("LEDS off");
  delay(100);

  lux_manager.setLuxThresholds(NIGHT_LUX_THRESHOLD, LOW_LUX_THRESHOLD, MID_LUX_THRESHOLD, HIGH_LUX_THRESHOLD, EXTREME_LUX_THRESHOLD);
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
#if ARTEFACT_SPECIES == SPEC_MINOR
  Serial.println("Starting LuxManager");
  lux_manager.add6030Sensors(2, 25);
  Serial.println("Finished starting LuxManager");
#elif ARTEFACT_SPECIES == SPEC_MAJOR
  Serial.println("Starting LuxManager");
  // lux_manager.addSensorTcaIdx("Front", 0);
  // lux_manager.addSensorTcaIdx("Rear", 1);
  lux_manager.add7700Sensor((String) "Front");
  lux_manager.add7700Sensor((String) "Rear");
  lux_manager.startTCA7700Sensors(VEML7700_GAIN_1, VEML7700_IT_25MS); // todo add this to config_adv? todo
  Serial.println("Finished starting LuxManager");
#elif (ARTEFACT_GENUS == EXPLORATOR)
  Serial.println("Starting LuxManager");
#if ARTEFACT_SPECIES == EX_WINDER
  lux_manager.add6030Sensors(2, 25);
#else
  lux_manager.add7700Sensor((String) "Eye-Stock");
  lux_manager.start7700Sensor(VEML7700_GAIN_1, VEML7700_IT_25MS); // todo add this to config_adv? todo
#endif
  Serial.println("Finished starting LuxManager");
  printMinorDivide();
#elif ARTEFACT_GENUS == LEGATUS
  Serial.println("Starting LuxManager");
  lux_manager.add6030Sensors(2, 25);
  Serial.println("Finished starting LuxManager");
#endif // HV_MAJOR
  delay(1000);

  if (lux_manager.getSensorActive(0) || lux_manager.getSensorActive(1))
  {
    Serial.println("Linked the NeoGroups to the LuxManager");
    for (int i = 0; i < num_active_led_channels; i++)
    {
      lux_manager.linkNeoGroup(&neos[i]);
    }
    delay(200);
  }
  else
  {
    Serial.println("ERROR, unable to link NeoGroups to LuxManager");
  }
  lux_manager.setBrightnessScalerMinMax(LUX_BS_MIN, LUX_BS_MAX);
  lux_manager.calibrate(2000, true);
  if (P_LUMIN)
  {
    lux_manager.print();
  };
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////// WeatherManager //////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if WEATHER_MANAGER_ACTIVE == true
void extremeHumidityShutdown()
{
  colorWipeAll(0, 0, 0);
#if ARTEFACT_GENUS == LEGATUS
  digitalWrite(PWR_KILL_PIN, LOW);
#endif
}

void extremeTemperatureShutdown()
{
  unsigned int times = 0;
  Serial.println("TEMPERATURE SHUTDOWN INITALISED!!!!!!");
  colorWipeAll(255, 0, 0);
  delay(1000);
  colorWipeAll(0, 0, 0);
  while (weather_manager.getTempShutdown() == true)
  {
    weather_manager.update();
    if (times % 20 == 10)
    {
      colorWipeAll(255, 0, 0);
    }
    else if (times % 20 == 11)
    {
      colorWipeAll(0, 0, 0);
    }
    times++;
    delay(100);
  }
  Serial.println("TEMPERATURE SHUTDOWN LIFTED - resuming normal operation");
}
#endif // WEATHER_MANAGER_ACTIVE == true

#if P_AUDIO_USAGE_MAX == true
void printAudioUsage()
{
  // print the audio usage every second or so
  if (last_audio_usage_print > AUDIO_USAGE_POLL_RATE)
  {
    Serial.print("memory usage: ");
    Serial.print(AudioMemoryUsageMax());
    Serial.print(" out of ");
    Serial.println(AUDIO_MEMORY);
    last_audio_usage_print = 0;
    AudioMemoryUsageMaxReset();
  }
}
#endif

elapsedMillis loop_tmr;
const int loop_print_delay = 1000;

/////////////////////////// Species-Specific Behaviour //////////////////////////////////
// include the artefact-specific operating logic depending on what genus we have...
// each behaviour.h files contains a updateBehaviour() function that is called in the main loop
#if ARTEFACT_GENUS == SPECULATOR
#include <speculator_behaviour.h>
#elif ARTEFACT_GENUS == EXPLORATOR
// mechanisms needs be be added before the Mode file
#include <explorator_behaviour.h>
#elif ARTEFACT_GENUS == LEGATUS
#include <legatus_behaviour.h>
#endif

/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// Setup Loops ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
void setupAudio() {
  ////////////// Audio ////////////
  printMajorDivide("Setting up Audio Parameters");
  AudioMemory(AUDIO_MEMORY);
  Serial.print("Audio Memory has been set to: ");
  Serial.println(AUDIO_MEMORY);
  delay(500);
  ////////////// Linking audio Analysis objects //////////////////////////////////////////////
  // amp objects
#if NUM_AMPLIFIERS == 1
  feature_collector.linkAmplifier(&amp1, AUTOGAIN_MIN_GAIN, AUTOGAIN_MAX_GAIN, AUTOGAIN_MAX_GAIN_ADJ);
#elif NUM_AMPLIFIERS == 2
  feature_collector.linkAmplifier(&amp1, AUTOGAIN_MIN_GAIN, AUTOGAIN_MAX_GAIN, AUTOGAIN_MAX_GAIN_ADJ);
  feature_collector.linkAmplifier(&amp2, AUTOGAIN_MIN_GAIN, AUTOGAIN_MAX_GAIN, AUTOGAIN_MAX_GAIN_ADJ);
#endif
  /////////////////////////

  // peak objectcs
#if NUM_PEAK_ANAS == 1
    feature_collector.linkPeak(&peak1, P_PEAK_VALS);
#elif NUM_PEAK_ANAS == 2
    feature_collector.linkPeak(&peak1, P_PEAK_VALS);
    feature_collector.linkPeak(&peak2, P_PEAK_VALS);
#endif
  /////////////////////////

// RMS objectics
#if NUM_RMS_ANAS == 1 
  feature_collector.linkRMS(&rms1, P_RMS_VALS);
#elif NUM_RMS_ANAS == 2
  feature_collector.linkRMS(&rms1, P_RMS_VALS);
  feature_collector.linkRMS(&rms2, P_RMS_VALS);
#endif
  /////////////////////////

#if NUM_FFT == 1 
    fft_manager[0].linkFFT(&fft1, "Front");
#elif NUM_FFT == 2
    fft_manager[0].linkFFT(&fft1, "Front");
    fft_manager[1].linkFFT(&fft2, "Rear");
#endif
  /////////////////////////

  for (int i = 0; i < NUM_FFT; i++)
  {
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
  Serial.println("Feature collectors have been linked");

  // update the species-specific audio routing and configutation here
  setupSpeciesAudio();
  // feature_collector.testMicrophone();
  Serial.println("Exiting setupAudio()");
  printDivide();
}

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  ///////////////// Serial ///////////////////////////////////
  Serial.begin(SERIAL_BAUD_RATE);
  delay(200);
  printDivide();
  Serial.println("Entering the Setup Loop");
  printDivide();
  Serial.println("Serial begun");
  printMinorDivide();
  printArtefactInfo();
  //////////////////////////////////////////////////////////////

  /////////////////// NeoPixels ////////////////////////////////
  printMinorDivide();
  Serial.print("Starting ");
  Serial.print(num_active_led_channels);
  Serial.println(" LED strips");
  delay(100);
  for (int i = 0; i < num_active_led_channels; i++)
  {
    leds[i].begin();
    Serial.print("WS2813Serial Object ");
    Serial.print(i);
    Serial.println(" .begin() called");
    delay(2000);

    neos[i].begin();
    neos[i].colorWipe(12, 12, 12, 1.0);
    printMinorDivide();
    Serial.println("NeoPixel Managers have been initalised");
    Serial.print("There are ");
    Serial.print(LED1_COUNT);
    Serial.println(" LEDs in the first group");
    Serial.print("There are ");
    Serial.print(LED2_COUNT);
    Serial.println(" LEDs in the second group");
    Serial.print("There are ");
    Serial.print(LED3_COUNT);
    Serial.println(" LEDs in the third group");

    neos[i].activateTempScaling(TEMP_SCALES_HUE, TEMP_SCALES_SAT, TEMP_SCALES_BGT);
    neos[i].activateHumidScaling(HUMID_SCALES_HUE, HUMID_SCALES_SAT, HUMID_SCALES_BGT);
    neos[i].activateTempOffsets(TEMP_OFFSETS_HUE, TEMP_OFFSETS_SAT, TEMP_OFFSETS_BGT);
    neos[i].activateHumidOffsets(HUMID_OFFSETS_HUE, HUMID_OFFSETS_SAT, HUMID_OFFSETS_BGT);
    Serial.println("Temperature and Humidity Offsets have been set:");
    Serial.print("TEMP_SCALES_HUE: ");Serial.println(TEMP_SCALES_HUE);
    Serial.print("TEMP_SCALES_SAT: ");Serial.println(TEMP_SCALES_SAT);
    Serial.print("TEMP_SCALES_BGT: ");Serial.println(TEMP_SCALES_BGT);
    Serial.print("HUMID_SCALES_HUE: ");Serial.println(HUMID_SCALES_HUE);
    Serial.print("HUMID_SCALES_SAT: ");Serial.println(HUMID_SCALES_SAT);
    Serial.print("HUMID_SCALES_BGT: ");Serial.println(HUMID_SCALES_BGT);
    Serial.print("TEMP_OFFSETS_HUE: ");Serial.println(TEMP_OFFSETS_HUE);
    Serial.print("TEMP_OFFSETS_SAT: ");Serial.println(TEMP_OFFSETS_SAT);
    Serial.print("TEMP_OFFSETS_BGT: ");Serial.println(TEMP_OFFSETS_BGT);
    Serial.print("HUMID_OFFSETS_HUE: ");Serial.println(HUMID_OFFSETS_HUE);
    Serial.print("HUMID_OFFSETS_SAT: ");Serial.println(HUMID_OFFSETS_SAT);
    Serial.print("HUMID_OFFSETS_BGT: ");Serial.println(HUMID_OFFSETS_BGT);


    Serial.print("NeoGroup p_lux and p_extreme_lux are both set to : \t");
    Serial.println(P_LUMIN);
    neos[i].setPrintLux(P_LUMIN);
    Serial.print("NeoGroup p_lux is set to                         : \t");
    Serial.println(P_LUMIN);
    neos[i].setPrintExtremeLux(P_LUMIN);
    Serial.print("NeoGroup p_brightness_scaler is set to           : \t");
    Serial.println(P_BS);
    neos[i].setPrintBrightnessScaler(P_BS);
    Serial.print("NeoGroup p_leds_on is set to                     : \t");
    Serial.println(P_LEDS_ON);
    neos[i].setPrintLedsOn(P_LEDS_ON);
    Serial.print("NeoGroup p_on_ratio is set to                    : \t");
    Serial.println(P_LED_ON_RATIO);
    neos[i].setPrintOnRatio(P_LED_ON_RATIO);
    Serial.print("NeoGroup p_color_wipe is set to                  : \t");
    Serial.println(P_COLOR_WIPE);
    neos[i].setPrintColorWipe(P_COLOR_WIPE);
    Serial.print("NeoGroup p_onset is set to                       : \t");
    Serial.println(P_ONSET);
    neos[i].setPrintOnRatio(P_ONSET);
    Serial.print("NeoGroup p_onset is set to                       : \t");
    Serial.println(P_ONSET);
    neos[i].setPrintPackColors(P_PACK_COLORS);
    printMinorDivide();
    Serial.print("NeoGroup SATURATED_COLORS                        : \t");
    Serial.println(P_ONSET);
    neos[i].setSaturatedColors(SATURATED_COLORS);

    // onset stuff
    neos[i].setOnsetColors(ONSET_RED, ONSET_GREEN, ONSET_BLUE);

    neos[i].setFlashBehaviour(FLASH_DOMINATES);
    Serial.print("NeoGroup flash_behaviour is set to                : \t");
    Serial.println(FLASH_DOMINATES);

    neos[i].changeMapping(LED_MAPPING_MODE);
    Serial.print("LED_MAPPING_MODE is set to                        : \t");
    Serial.println(LED_MAPPING_MODE);
  }
  Serial.println("Finished starting the LED strips");
  printMinorDivide();

  ///////////////////////// Weather Manager /////////////////////
  // nothing is needed =P
#if WEATHER_MANAGER_ACTIVE == true
  neos[0].colorWipe(2, 12, 0, 1.0);
  printMinorDivide();
  Serial.println("initalising the weather manager");
  weather_manager.init();
  Serial.println("finished initalising the weather manager");
  neos[0].colorWipe(0, 5, 12, 1.0);
  delay(1000);
  weather_manager.setPrintReadings(P_WEATHER_MANAGER_READINGS);
  Serial.print("WeatherManager HUMID_EXTREME_THRESH  : \t");
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
  neos[0].colorWipe(0, 0, 0, 1.0);
  setupLuxManager();
  neos[0].colorWipe(0, 0, 12, 1.0);

  //////////////// User Controls /////////////////////////////
  explainSerialCommands(true);
  // TODO - need to replace with the UIManager

  //////////////// setup audio /////////////////////////////
  setupAudio();

  ///////////////////////// Artefact Specific Logic ////////////////////////////
  setupSpecies();

  /////////////////////////////// Main Loop Delay ////////////////////////////////
  for (int it = 0; it < LED1_COUNT; it++)
  {
    // we keep readng the jumpers so if we change the jumpers and dont
    // want the boot delay that can happen
    // TODO - replace with the user control manager
    // readJumpers();
    // uimanager.update();
    uint32_t segment = (uint32_t)((double)BOOT_DELAY / (double)LED1_COUNT);
    if (segment > 10)
    {
      Serial.print("boot delay segment length in seconds is : ");
      Serial.println(segment / 1000, 2);
    }
    neos[0].setPixel(it, 10, 32, 20, 1.0);
    if (BOOT_DELAY_ACTIVE)
    {
      delay(segment);
#if USER_CONTROLS_ACTIVE
      uimanager.update();
#endif // only update UIManager if it exists
    }
  }
  #if DATALOG_ACTIVE
  // start datalogging if active
  if (data_logging_active == true) {
    setupDLManager();
  }
  #endif
  neos[0].colorWipe(0, 0, 0, 0.0);
  ////////////////////////////////////////////////////////////
  //////////////// Testing ///////////////////////////////////
  ////////////////////////////////////////////////////////////
  // TODO - this testing should occur in the setup, or exist as a species testMode loop
  /*
    if (playback_engine.isActive() == true){
    clip_counter.deactivate();
    } else {
    clip_counter.activate();
    Serial.println(clip_counter.getNumClips());
    }
  */
  printMajorDivide("Now starting main() loop");
  neos[0].colorWipe(10, 15, 10, 0.5);
  delay(1000);
  #if P_FUNCTION_TIMES
    // loop_length_value_tracker.reset();
  #endif
}

  ////////////////////////////////////////////////////////////
  ///////////////// Main Loop ////////////////////////////////
  ////////////////////////////////////////////////////////////

void loop()
{
  ////////////////////////////////////////////////////////////
  ///////////////// Ambient Lighting /////////////////////////
  ////////////////////////////////////////////////////////////
  // Acropolis family artefacts all have ambient light sensors, so we 
  // always must update our lux_manager
  if (lux_manager.getActive() == true)
  {
    if (lux_manager.update())
    {
      // print the updated values
      if (P_LUX_MANAGER_READINGS)
      {
        lux_manager.print();
      };
    }
  } else {
      if (P_LUX_MANAGER_READINGS)
      {
        Serial.println("WARNING - lux_manager is not active");
      }
  }
  ////////////////////////////////////////////////////////////
  ///////////////// User Controls ////////////////////////////
  ////////////////////////////////////////////////////////////
// update the user controls if active
#if USER_CONTROLS_ACTIVE
  if (uimanager.update())
  {
    if (P_USER_CONTROLS)
    {
      uimanager.printAll();
      for (int i = 0; i < NUM_NEOP_MANAGERS; i++)
      {
        neos[i].changeMapping(LED_MAPPING_MODE);
      }
    }
  }
#endif // USER_CONTROLS_ACTIVE

////////////////////////////////////////////////////////////
///////////////// WeatherManager ///////////////////////////
////////////////////////////////////////////////////////////
#if WEATHER_MANAGER_ACTIVE == true
  if (weather_manager.update()) { 
    if (P_WEATHER_MANAGER_READINGS)
    {
      weather_manager.print();
    }
    if (weather_manager.getHumidityShutdown() == true)
    {
      extremeHumidityShutdown();
    }
    else if (weather_manager.getTempShutdown() == true)
    {
      extremeTemperatureShutdown();
    }
  }
#endif // WEATHER_MANAGER_ACTIVE
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  //////////////// Audio Analysis ///////////////////////////
  ////////////////////////////////////////////////////////////
  updateAudioAnalysis();
#if CALCULATE_DOMINATE_CHANNEL == true
  // Serial.println("Calculating dominate channel");
  // delay(10000);
  if (millis() > first_dominate_channel_calculation && dominate_channel_update > dominate_channel_update_rate)
  {
    dominate_channel_update = 0;
    feature_collector.calculateDominateChannel(fft_manager);
  }
#endif

#if P_AUDIO_USAGE_MAX == true
  printAudioUsage();
#endif
  ////////////////////////////////////////////////////////////
  // TODO - autogain is missing here
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  //////////////// Species-Specific Behaviour ////////////////
  ////////////////////////////////////////////////////////////
  updateBehaviour();
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  //////////////// Datalogging ///////////////////////////////
  ////////////////////////////////////////////////////////////
#if DATALOG_ACTIVE
  if (data_logging_active) {
    updateDatalog();
  }
 #endif
}