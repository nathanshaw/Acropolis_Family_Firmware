
/**
  Moth Sonic Art Framework Firmware
  Written by Nathan Villicana-Shaw in 2020
  The runtime, boot, and all other configurations are found in the Configuration.h file
*/
// configuration needs to be added first to determine what mode will be added
#include "Configuration.h"

// #include <MemoryFree.h>

// mechanisms needs be be added before the Mode file
#if ARTEFACT_TYPE == EXPLORATOR
// #include <explorator_functions.ino>
#if BODY_TYPE == CLAPPER_BODY || BODY_TYPE == MB_BODY
// no need for the mechanisms or playback engine for the music box guy
#else
#include <Mechanisms.h>
#include <PlaybackEngine.h>
#endif
#endif
////////////////////////////// Libraries needed for Every Artefact /////////////////////////////////////
#include <LuxManager.h>
#include <PrintUtils.h>
#include <WS2812Serial.h>
#include <NeopixelManager.h>
#include <AudioEngine.h>
#include <Wire.h>
#include <SPI.h>

#if ARTEFACT_TYPE == LEGATUS
#include <SD.h>
Sd2Card card;
SdVolume volume;
SdFile root;
#endif // SD card stuff

#if ARTEFACT_TYPE == EXPLORATOR && (BODY_TYPE == SHAKER_BODY)

#include "Encoder.h"
Encoder enc(20, 21);
long enc_pos = 0;

#elif ARTEFACT_TYPE == EXPLORATOR && (BODY_TYPE == MB_BODY)
min_playback_gain100
#include "Encoder.h"
    Encoder enc(12, 11);
long enc_pos = 0;

#endif // encoder library

#if ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE == CLAPPER_BODY
// TODO
#else
#include <UIManager.h>
#endif

///////////////////////////// Libraries only needed for certain Artefacts ///////////////////////////////
#if ARTEFACT_TYPE == SPECULATOR && HV_MAJOR == 3
#define WEATHER_MANAGER_PRESENT true
#elif ARTEFACT_TYPE == SPECULATOR && HV_MAJOR == 1 && HV_MINOR == 1
#define WEATHER_MANAGER_PRESENT true
#elif ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE == WOODPECKER_BODY
#define WEATHER_MANAGER_PRESENT true
#elif ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE == BELL_BODY
#define WEATHER_MANAGER_PRESENT true
#elif ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE == CLAPPER_BODY
#define WEATHER_MANAGER_PRESENT false
#elif ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE == SHAKER_BODY
#define WEATHER_MANAGER_PRESENT true
#elif ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE == MB_BODY
#define WEATHER_MANAGER_PRESENT true
#elif ARTEFACT_TYPE == LEGATUS
#define WEATHER_MANAGER_PRESENT true
#else
#define WEATHER_MANAGER_PRESENT false
#endif // if weather manager present?

#if WEATHER_MANAGER_PRESENT == true
#include <WeatherManager.h>
#endif

#if NUM_MOTORS > 0

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
    Rhythm()};

RhythmBank rhythm_bank = RhythmBank();
PlaybackEngine playback_engine = PlaybackEngine();

// actuator pin, dampener pin, frequency, on_time
BellMechanism bells[3] = {
    BellMechanism(s_pins[0], s_pins[1], 20, 100.0, 40),
    BellMechanism(s_pins[2], s_pins[3], 20, 500.0, 40),
    BellMechanism(s_pins[4], s_pins[5], 20, 1000.0, 40)};

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
    Rhythm()};
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
NeoGroup neos[1] = NeoGroup(&leds[0], 0, LED1_COUNT - 1, "All Neos", MIN_FLASH_TIME, MAX_FLASH_TIME);

#elif ARTEFACT_TYPE == EXPLORATOR
// TODO - dynamically create these objects based on info in the configuration file
WS2812Serial leds[num_active_led_channels] = {
#if LED1_ACTIVE == true
    WS2812Serial(LED1_COUNT, displayMemory[0], drawingMemory[0], LED1_PIN, WS2812_GRB)
#if LED2_ACTIVE + LED3_ACTIVE > 0
        ,
#endif
#endif

#if LED2_ACTIVE == true
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

#endif // ARTEFACT_TYPE == xxxx

/////////////////////////////// FFTManager ///////////////////////////////////////
// all artefacts will have an input FFTManager
#if (ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE == CLAPPER_BODY) || (ARTEFACT_TYPE == LEGATUS) || BODY_TYPE == MB_BODY
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

#if (ARTEFACT_TYPE == SPECULATOR)
////////////////////////// Audio Objects //////////////////////////////////////////
AudioInputI2S i2s1;          //xy=811.6666030883789,543.3333549499512
AudioFilterBiquad left_HPF;  //xy=962.9998664855957,523.3333139419556
AudioFilterBiquad right_HPF; //xy=966.6666641235352,556.6667098999023
AudioMixer4 mixer1;          //xy=1106.6667137145996,563.3334369659424
AudioFilterBiquad left_LPF;  //xy=1243.6665573120117,561.6666469573975
AudioAmplifier left_amp;     //xy=1369.9999771118164,559.9999208450317
AudioAnalyzeFFT1024 fft1;    //xy=1570.999870300293,563.666654586792
AudioAnalyzePeak peak1;      //xy=1574.999870300293,595.666654586792
AudioOutputUSB output_usb;   //xy=1581.9999313354492,630.6666469573975

AudioConnection patchCord1(i2s1, 0, left_HPF, 0);
AudioConnection patchCord2(i2s1, 1, right_HPF, 0);
AudioConnection patchCord3(left_HPF, 0, mixer1, 0);
AudioConnection patchCord4(right_HPF, 0, mixer1, 1);
AudioConnection patchCord5(mixer1, 0, output_usb, 1);
AudioConnection patchCord6(mixer1, left_LPF);
AudioConnection patchCord7(left_LPF, left_amp);
AudioConnection patchCord8(left_amp, fft1);
AudioConnection patchCord9(left_amp, peak1);
AudioConnection patchCord10(left_amp, 0, output_usb, 0);
#endif

#if ARTEFACT_TYPE == LEGATUS && FIRMWARE_MODE == MODULAR_LEGATUS_MODE
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

AudioOutputI2S audioOutput; //xy=1356.0000190734863,604.5000095367432

AudioAnalyzeRMS rms1;     //xy=1578.5000228881836,682.5000114440918
AudioAnalyzeFFT1024 fft1; //xy=1578.4999885559082,714.2499980926514
AudioAnalyzePeak peak1;   //xy=1582.4999885559082,746.2499980926514

AudioEffectDelay audio_delay1;

AudioOutputUSB output_usb; //xy=1589.4999885559082,781.2499980926514

#if USE_RAW_AUDIO_PLAYER == true
AudioPlaySdRaw audio_player; //xy=767.0000267028809,648.750018119812
#else
AudioPlaySdWav audio_player; //xy=767.0000267028809,648.750018119812
#endif

AudioSynthWaveformSineModulated sine_fm; //xy=1054.285743713379,2268.5713901519775

AudioControlSGTL5000 sgtl5000; //xy=997.7500152587891,513.2500104904175
#endif                         // audio system for legatus modular firmware

#if ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE == CLAPPER_BODY
#else
UIManager uimanager = UIManager(UI_POLLING_RATE, P_USER_CONTROLS);
#endif

#if (ARTEFACT_TYPE == EXPLORATOR)
AudioInputI2S i2s1;         //xy=1203.3333854675293,1356.3334693908691
AudioMixer4 mixer1;         //xy=1373.3333549499512,1335.0000076293945
AudioAmplifier left_amp;    //xy=1519.3333206176758,1330.0000114440918
AudioFilterBiquad left_HPF; //xy=1651.6667022705078,1330.6666841506958
AudioAnalyzeFFT1024 fft1;   //xy=1846.3333740234375,1254.9999675750732
AudioAnalyzePeak peak1;     //xy=1851.3332901000977,1288.6667175292969
AudioOutputUSB output_usb;  //xy=1857.0000457763672,1368.666639328003

AudioConnection patchCord1(i2s1, 0, mixer1, 0);
AudioConnection patchCord2(i2s1, 1, mixer1, 1);
AudioConnection patchCord3(mixer1, left_amp);
AudioConnection patchCord4(mixer1, 0, output_usb, 1);
AudioConnection patchCord5(left_amp, left_HPF);
AudioConnection patchCord6(left_HPF, peak1);
AudioConnection patchCord7(left_HPF, 0, output_usb, 0);
AudioConnection patchCord8(left_HPF, fft1);
#endif

// #if ARTEFACT_TYPE == EXPLORATOR
double calculateColorFromCentroid(FFTManager1024 *_fft_manager)
{
  // Should return a number between 0.0 and 1.0
  // right now we are only polling the first FC for its centroid to use to color both sides
  double cent = _fft_manager->getCentroid();
  if (cent < color_feature_min)
  {
    color_feature_min = (color_feature_min * 0.9) + (cent * 0.1);
    cent = color_feature_min;
  }
  if (cent > color_feature_max)
  {
    color_feature_max = (color_feature_max * 0.9) + (cent * 0.1);
    cent = color_feature_max;
  }
  cent = (cent - color_feature_min) / (color_feature_max - color_feature_min);
  return cent;
}

double calculateFeedbackBrightness(FFTManager1024 *_fft_manager)
{
  // how much energy is stored in the range of 4000 - 16000 compared to  the entire spectrum?
  double target_brightness = _fft_manager->getFFTRangeByFreq(100, 16000);
  if (target_brightness < 0.01)
  {
    target_brightness = 0.0;
  }
  else if (target_brightness > 1.0)
  {
    target_brightness = 1.0;
  }
  if (target_brightness < brightness_feature_min)
  {
    if (P_BRIGHTNESS)
    {
      Serial.print("target_B is less than feature_min: ");
      Serial.print(target_brightness, 5);
      Serial.print(" < ");
      Serial.print(brightness_feature_min, 5);
    }
    brightness_feature_min = (target_brightness * 0.15) + (brightness_feature_min * 0.85);
    if (P_BRIGHTNESS)
    {
      Serial.print(" updated brightness_min and target_brightness to: ");
      Serial.println(brightness_feature_min, 5);
    }
    target_brightness = brightness_feature_min;
  }
  if (target_brightness > brightness_feature_max)
  {

    if (P_BRIGHTNESS)
    {
      Serial.print("target_B is more than feature_max: ");
      Serial.print(target_brightness, 5);
      Serial.print(" > ");
      Serial.print(brightness_feature_max, 5);
    }
    brightness_feature_max = (target_brightness * 0.15) + (brightness_feature_max * 0.85);
    // to ensure that loud clipping events do not skew things too much
    if (brightness_feature_max > 1.0)
    {
      brightness_feature_max = 1.0;
    }
    if (P_BRIGHTNESS)
    {
      Serial.print(" updated brightness_max and target_brightness to: ");
      Serial.println(brightness_feature_max, 5);
    }
    target_brightness = brightness_feature_max;
  }
  dprintln(P_BRIGHTNESS);
  dprint(P_BRIGHTNESS, " target - min/max ");
  dprint(P_BRIGHTNESS, target_brightness);
  dprint(P_BRIGHTNESS, " - ");
  dprint(P_BRIGHTNESS, brightness_feature_min);
  dprint(P_BRIGHTNESS, " / ");
  dprintln(P_BRIGHTNESS, brightness_feature_max);

  target_brightness = (target_brightness - brightness_feature_min) / (brightness_feature_max - brightness_feature_min);
  dprint(P_BRIGHTNESS, "target_brightness(2): ");
  dprint(P_BRIGHTNESS, target_brightness);
  dprint(P_BRIGHTNESS, " ");

  return target_brightness;
}

void updateFeedbackLEDs(FFTManager1024 *_fft_manager)
{
  // the brightness of the LEDs should mirror the peak gathered from the environment
  // a local min/max which scales periodically should be implemented just like with the Moth
  // a MAX_RMS brightness should be used to determine what the max brightness of the feedback is
  // the LEDs should be updated 30x a second

  // calculate the target color ///////////////////////////////////////
  if (last_led_update_tmr > led_refresh_rate)
  {
    double target_color = 0.0;
    double target_brightness = 0.0;
    uint8_t red, green, blue;

    if (COLOR_FEATURE == SPECTRAL_CENTROID)
    {
      target_color = calculateColorFromCentroid(_fft_manager);
      dprint(P_COLOR, "target color: ");
      dprintln(P_COLOR, target_color);
      last_color = current_color;
      current_color = (target_color * 0.2) + (last_color * 0.8); // * COLOR_LP_LEVEL);

      // calculate the preliminary rgb values /////////////////////////////
      red = ((1.0 - current_color) * RED_LOW) + (current_color * RED_HIGH);
      green = ((1.0 - current_color) * GREEN_LOW) + (current_color * GREEN_HIGH);
      blue = ((1.0 - current_color) * BLUE_LOW) + (current_color * BLUE_HIGH);
    }
    else if (COLOR_FEATURE == SPLIT_BAND)
    {
      /* Should return a number between 0.0 and 1.0 */
      // Serial.println("about to calculate color based off split-band approach : ");
      // delay(4000);
      double green_d = _fft_manager->getFFTRangeByFreq(50, 400); // 3 octaves in each band
      double blue_d = _fft_manager->getFFTRangeByFreq(400, 3200);
      double red_d = _fft_manager->getFFTRangeByFreq(3200, 12800);
      // Serial.println("finished getting the energy amount from the fft manager : ");
      // delay(4000);
      red = (uint8_t)((double)MAX_BRIGHTNESS * (red_d / (red_d + green_d + blue_d)));
      green = (uint8_t)((double)MAX_BRIGHTNESS * (green_d / (red_d + green_d + blue_d)));
      blue = (uint8_t)((double)MAX_BRIGHTNESS * (blue_d / (red_d + green_d + blue_d)));
    }

    // calculate the target brightness ///////////////////////////////////
    target_brightness = calculateFeedbackBrightness(_fft_manager);
    /////////////////////////// Apply user brightness scaler ////////////////////////
    if (USER_BS_ACTIVE > 0)
    {
      target_brightness = target_brightness * user_brightness_scaler;
    }

    last_brightness = current_brightness;
    current_brightness = (target_brightness * 0.5) + (current_brightness * 0.5);

    // calculate the actual values to be sent to the strips
    // red = (uint8_t)((double)red);
    // green = (uint8_t)((double)green);
    // blue = (uint8_t)((double)blue);
    if (P_COLOR == true)
    {
      Serial.print("rgb:\t");
      Serial.print(red);
      Serial.print("\t");
      Serial.print(green);
      Serial.print("\t");
      Serial.println(blue);
    }
#if ARTEFACT_TYPE == SPECULATOR && HV_MAJOR == 3
    if (current_brightness < user_brightness_cuttoff)
    {
      for (int i = 0; i < NUM_NEOP_MANAGERS; i++)
      {
        neos[i].colorWipe(0, 0, 0, current_brightness);
      }
    }
    else
    {
      for (int i = 0; i < NUM_NEOP_MANAGERS; i++)
      {
        neos[i].colorWipe(red, green, blue, current_brightness);
      }
    }
#elif ARTEFACT_TYPE == LEGATUS
    neos[0].colorWipe(red, green, blue, current_brightness);
#elif ARTEFACT_TYPE == EXPLORATOR
#if BODY_TYPE == MB_BODY
    neos[0].colorWipe(red, green, blue, current_brightness);
    // neos[0].colorWipe(30, 30, 31, 0.05);
#else
    neos[0].colorWipe(red, green, blue, current_brightness);
#endif
#endif
    last_led_update_tmr = 0;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// Start of PITCH functions ////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
bool getColorFromFFTSingleRange(FFTManager1024 *f, uint8_t s, uint8_t e)
{
  double tot, frac;
  frac = f->getFFTRangeByIdx(s, e);
  tot = f->getFFTRangeByIdx(FFT_LOWEST_BIN, 128);
  frac = frac / tot;
  // RGBConverter::HsvToRgb(frac, 0.5, 1, 0, red, green, blue);
  return 1;
}

#if COLOR_MAPPING == COLOR_MAPPING_HSB

double calculateBrightness(FeatureCollector *f, FFTManager1024 *_fft)
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
    b = f->getDominatePeak();
    break;
  case (FEATURE_RMS_AVG):
    dprintln(P_BRIGHTNESS, "feature is RMS_AVG");
    b = f->getDominateRMSAvg();
    f->resetDominateRMSAvg();
    break;
  case (FEATURE_RMS):
    dprintln(P_BRIGHTNESS, "feature is RMS");
    b = f->getDominateRMS();
    break;
  case (FEATURE_FFT_ENERGY):
    dprintln(P_BRIGHTNESS, "feature is FFT_ENERGY");
    b = _fft->getFFTTotalEnergy();
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
  // brightness_tracker.update();
  // brightness = brightness_tracker.getRAvg();
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
    Serial.print("brightness changed due to target: ");
    Serial.print(brightness);
    Serial.print(" = ");
    brightness = target_brightness;
    Serial.println(brightness);
  }

  /////////////////////////// Reverse ////////////////////////
  if (REVERSE_BRIGHTNESS == true)
  {
    brightness = 1.0 - brightness;
  }

  //////////////////////// Scale down the brightness and make it more exponential for better results //////////////////
  if (SQUARE_BRIGHTNESS == true && brightness < 1.0)
  {
    dprint(P_BS + P_BRIGHTNESS, "changing brightness due to SQUARE_BRIGHTNESS | before: ");
    dprint(P_BS + P_BRIGHTNESS, brightness, 4);
    brightness = (brightness)*brightness;
    dprint(P_BS + P_BRIGHTNESS, " after: ");
    dprintln(P_BS + P_BRIGHTNESS, brightness, 4);
  }

  /////////////////////////// Apply user brightness scaler ////////////////////////
  if (USER_BS_ACTIVE > 0)
  {
    dprint(P_BS + P_BRIGHTNESS, "changing brightness due to user_brightness_scaler (scaler is: ");
    dprint(P_BS + P_BRIGHTNESS, user_brightness_scaler, 4);
    dprint(P_BS + P_BRIGHTNESS, ") | before: ");
    dprint(P_BS + P_BRIGHTNESS, brightness, 4);
    brightness = brightness * user_brightness_scaler;
    dprint(P_BS + P_BRIGHTNESS, " after: ");
    dprintln(P_BS + P_BRIGHTNESS, brightness, 4);
  }
  /////////////////////// Make sure that it is within bounds ////////////////////
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
  dprintMinorDivide(P_BRIGHTNESS);
  return brightness;
}

double calculateSaturation(FeatureCollector *f, FFTManager1024 *_fft)
{
  double sat = 0.0;
  switch (SATURATION_FEATURE)
  {
  case (FEATURE_PEAK_AVG):
    sat = f->getDominatePeakAvg();
    // Serial.println(sat);
    // Serial.print("sat set to  : ");Serial.println(hsb[i][1]);
    f->resetDominatePeakAvg();
    break;
  case (FEATURE_RMS_AVG):
    sat = f->getDominateRMSAvg();
    // Serial.print("sat set to  : ");Serial.println(hsb[i][1]);
    f->resetDominateRMSAvg();
    break;
  case (FEATURE_FFT_RELATIVE_ENERGY):
    // get how much energy is stored in the max bin, get the amount of energy stored in all bins
    sat = _fft->getRelativeEnergy(_fft->getHighestEnergyIdx());
    // Serial.print("highestEnergyIdx: ");Serial.println(_fft->getHighestEnergyIdx());
    // Serial.print("relative energy in highest energy bin: ");Serial.println(sat);
    break;
  case (FEATURE_FLUX):
    // sat = (_fft->getFlux() - 20) / 60;
    sat = _fft->getFlux();
    break;
  default:
    Serial.print("ERROR - calculateSaturation() does not accept that  SATURATION_FEATURE");
    break;
  }
  saturation = sat;
  saturation_tracker.update();
  dprint(P_SATURATION, "saturation before/after scaling: ");
  dprint(P_SATURATION, sat);
  saturation = saturation_tracker.getRAvg();
  // saturation = (9.9 * log10((double)saturation + 1.0)) - (2.0 * (double)saturation);
  if (REVERSE_SATURATION == true)
  {
    saturation = 1.0 - saturation;
  }
  saturation += ADDED_SATURATION; // just add some base saturation to make the feedback more colourful
  if (saturation > 1.0)
  {
    saturation = 1.0;
  }
  dprint(P_SATURATION, " / ");
  dprint(P_SATURATION, sat, 4);
  dprint(P_SATURATION, "\tsat min/max: ");
  dprint(P_SATURATION, saturation_tracker.getMin(), 4);
  dprint(P_SATURATION, " / ");
  dprintln(P_SATURATION, saturation_tracker.getMax(), 4);
  return saturation;
}

double calculateHue(FeatureCollector *f, FFTManager1024 *_fft)
{
  // In theory, the Hue should be between 0.0 and 1.0 to be useful for the rest of the program
  double h = 0.0;
  switch (HUE_FEATURE)
  {
  case FEATURE_FFT_BIN_RANGE:
    h = getColorFromFFTSingleRange(_fft, 3, 20);
    break;
  case FEATURE_FFT:
    h = (double)constrain(_fft->getHighestEnergyIdx(), 7, 255);
    break;
  case FEATURE_FFT_MAX_BIN:
    // calculate the bin with the most energy,
    // Serial.print("Highest energy bin is: ");Serial.println(f->getHighestEnergyBin(FFT_LOWEST_BIN, FFT_HIGHEST_BIN));
    // map the bin  index to a hue value
    h = (double)(_fft->getHighestEnergyIdx(FFT_LOWEST_BIN, FFT_HIGHEST_BIN) - FFT_LOWEST_BIN) / FFT_HIGHEST_BIN;
    // Serial.print("max bin hu e is : ");Serial.println(hue);
    break;
  case FEATURE_PEAK_AVG:
    h = f->getDominatePeakAvg();
    f->resetDominatePeakAvg();
    break;
  case FEATURE_PEAK:
    h = f->getDominatePeak();
    break;
  case FEATURE_RMS_AVG:
    h = f->getDominateRMSAvg();
    break;
  case FEATURE_RMS:
    h = f->getDominateRMS();
    break;
  case FEATURE_CENTROID:
    // the centroid will be a frequency between about 200 and 50000
    // first I need to move it to a more linear scale
    h = (double)_fft->getCentroid();
    // next I need to scale down to a value between 0.0 and 1.0
    break;
  case FEATURE_FLUX:
    h = _fft->getFlux();
    break;
  default:
    Serial.println("ERROR - calculateHue() does not accept that HUE_FEATURE");
    break;
  }
  dprint(P_HUE, "Hue is: ");
  dprint(P_HUE, hue);
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
  dprint(P_HUE, "\t");
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

#endif // if color mapping is HSB

void printRGB()
{
  // fft_manager.printFFTVals();
  if (P_NEO_COLORS)
  {
    neos[0].printColors();
  }
}

// #endif // ARTEFACT_TYPE == EXPLORATOR

#if ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE == MB_BODY

void windBack(float rotations, int backward_rate)
{
  Serial.println("starting windBack()");
  motors.enableDrivers();
  delay(10);
  enc.write(0);
  enc_pos = 0;
  int target = 20;
  if (M1_POLARITY == false)
  {
    Serial.println("M1_POLARITY is set to false, reversing backward_rate and reversing target");
    backward_rate *= -1;
    // start winding slowly
    for (int i = backward_rate * 0.5; i < backward_rate; i++)
    {
      motors.setM1Speed(i);
      enc_pos = enc.read();
      delay(15);
      updateFeedbackLEDs(&fft_manager[0]);
    }
    motors.setM1Speed(backward_rate);
  }
  else
  {
    for (int i = backward_rate * 0.5; i > backward_rate; i--)
    {
      motors.setM1Speed(i);
      enc_pos = enc.read();
      delay(25);
      updateFeedbackLEDs(&fft_manager[0]);
    }
    motors.setM1Speed(backward_rate);
  }

  // TODO determine how far to wind the box
  while (enc_pos <= (rotations * M1_GEAR_RATIO * COUNTS_PER_REV) && enc_pos >= (rotations * -1 * M1_GEAR_RATIO * COUNTS_PER_REV))
  {
    enc_pos = enc.read();
    if (last_pos != enc_pos)
    {
      Serial.print(last_enc_change);
      Serial.print("\tenc pos: ");
      Serial.print((float)enc_pos / 1500.00);
      Serial.print(" / ");
      Serial.println(rotations);
      last_pos = enc_pos;
      last_enc_change = 0;
    }
    // if 15 seconds pass without being able to get to the target position, then move on to unwinding
    if (last_enc_change > 300)
    {
      Serial.println("Winding taking too, long, breaking from loop");
      motors.setM1Speed(0);
      break;
    }
    else if (last_enc_change > 150)
    {
      motors.setM1Speed(backward_rate * 1.5);
      Serial.print("1.50 - ");
    }
    else if (last_enc_change > 50)
    {
      motors.setM1Speed(backward_rate * 1.25);
      Serial.print("1.25 - ");
    }

    if ((enc_pos >= (rotations * M1_GEAR_RATIO * COUNTS_PER_REV) - 100) && M1_POLARITY == false)
    {
      if (enc_pos >= (rotations * M1_GEAR_RATIO * COUNTS_PER_REV) - 50)
      {
        motors.setM1Speed(10);
        Serial.print((float)enc_pos / 1500.00);
        Serial.println("\t Bringing speed down to 10");
      }
      else
      {
        Serial.print((float)enc_pos / 1500.00);
        Serial.println("Bringing speed down to 20");
        motors.setM1Speed(20);
      }
    }
    else if ((enc_pos <= (rotations * M1_GEAR_RATIO * COUNTS_PER_REV * -1) + 100) && M1_POLARITY == true)
    {
      if (enc_pos <= (rotations * M1_GEAR_RATIO * COUNTS_PER_REV * -1) + 50)
      {
        motors.setM1Speed(-10);
        Serial.print((float)enc_pos / 1500.00);
        Serial.println("Bringing speed down to -10");
      }
      else
      {
        motors.setM1Speed(-20);
        Serial.print((float)enc_pos / 1500.00);
        Serial.println("Bringing speed down to -20");
      }
    }
    updateFeedbackLEDs(&fft_manager[0]);
  }
  last_enc_change = 0;
  Serial.print("enc pos: ");
  Serial.println(enc_pos);
  motors.setM1Speed(0);
  enc.write(0);
  enc_pos = 0;
  Serial.println("----- exiting windBack -------");
}

void windForward(float rotations, int forward_rate)
{
  if (M1_POLARITY == false)
  {
    motors.setM1Speed(forward_rate);
  }
  else
  {
    motors.setM1Speed(forward_rate);
  }

  float EXTRA_REWIND = 0.80;

  while (enc_pos <= (rotations * M1_GEAR_RATIO * COUNTS_PER_REV * EXTRA_REWIND) && enc_pos >= (COUNTS_PER_REV * rotations * -1 * M1_GEAR_RATIO * EXTRA_REWIND))
  {
    enc_pos = enc.read();
    if (last_pos != enc_pos)
    {
      Serial.print("enc pos: ");
      Serial.println((float)enc_pos / 1500.00);
      last_pos = enc_pos;
      last_enc_change = 0;
    }
    updateFeedbackLEDs(&fft_manager[0]);
    if ((enc_pos <= (rotations * M1_GEAR_RATIO * COUNTS_PER_REV * EXTRA_REWIND) - 100) && M1_POLARITY == true)
    {
      motors.setM1Speed(20);
      if (enc_pos <= (rotations * M1_GEAR_RATIO * COUNTS_PER_REV * EXTRA_REWIND) - 50)
      {
        motors.setM1Speed(10);
      }
    }
    else if ((enc_pos >= (rotations * M1_GEAR_RATIO * COUNTS_PER_REV * EXTRA_REWIND * -1) + 100) && M1_POLARITY == false)
    {
      motors.setM1Speed(-20);
      if (enc_pos <= (rotations * M1_GEAR_RATIO * COUNTS_PER_REV * EXTRA_REWIND * -1) + 50)
      {
        motors.setM1Speed(-10);
      }
    }
    updateFeedbackLEDs(&fft_manager[0]);
    if (last_enc_change > 250)
    {
      Serial.println("Unwininding taking too, long, breaking from loop");
      motors.setM1Speed(0);
      break;
    }
  }
  Serial.print("enc pos: ");
  Serial.println((float)enc_pos / 1500.00);
  motors.disableDrivers();
  // (in terms of the encoder)
  last_winding = 0;
}

void windBox(float rotations, int between_delay)
{
  if (last_winding > winding_interval)
  {
    // slowly wind the motor up
    rotations *= 1.0;
    int forward_rate = constrain((float)BASE_FORWARD_RATE * WINDING_RATE, MIN_FORWARD_RATE, MAX_FORWARD_RATE);
    int backward_rate = constrain((float)BASE_BACKWARD_RATE * WINDING_RATE, MIN_BACKWARD_RATE, MAX_BACKWARD_RATE);

    if (M1_POLARITY == true)
    {
      forward_rate *= -1;
      backward_rate *= -1;
    }
    Serial.println("----------------------");
    Serial.print("windBox(rotations): ");
    Serial.println(rotations);
    Serial.print("between_delay: ");
    Serial.println(between_delay);
    Serial.print("forward_rate: ");
    Serial.println(forward_rate);
    Serial.print("backward_rate: ");
    Serial.println(backward_rate);
    Serial.println("----------------------");

    windBack(rotations, backward_rate);

    delay(between_delay);
    //motors.setM1Speed(2);
    windForward(rotations, forward_rate);
  }
  else
  {
    Serial.println("Not winding music box, last winding was too soon");
  }
}

void manualWinding()
{
  // if the wind button is pressed wind the box up
  if (WIND_FORWARD == true)
  {
    Serial.println("WIND_FORWARD is active, winding motor forward...");
    motors.enableDrivers();
    motors.setM1Speed(20);
    while (WIND_FORWARD == true)
    {
      updateFeedbackLEDs(&fft_manager[0]);
      uimanager.update();
      delay(10);
    }
    motors.disableDrivers();
    Serial.println("WIND_FORWARD is no longer active, turning off the motors...");
  }
  // if the unwind button is pressed unwind the box
  else if (WIND_BACKWARD == true)
  {
    Serial.println("WIND_BACKWARD is active, winding motor backward...");
    motors.enableDrivers();
    motors.setM1Speed(-20);
    while (WIND_FORWARD == true)
    {
      updateFeedbackLEDs(&fft_manager[0]);
      uimanager.update();
      delay(10);
    }
    motors.disableDrivers();
    Serial.println("WIND_BACKWARD is no longer active, turning off the motors...");
  }
}

// for the MB Body
void exploratorMBLoop()
{
  ///////////////// Passive Visual Feedback ///////////
  updateFeedbackLEDs(&fft_manager[0]);
  uimanager.update();

  // the warmer the temperature the more it will actuate? (10 second decrease at 40 degrees and no decrease when at 0 degrees
  // the higher the humidity the less it will actuate? (100 second increase when 100% humidity , 0 second when at 0 %)
  // the brighter it is the more it will actuate (take 5000 lux and subtract the current reading)
  // activity level adds a base of up to five minutes
  ACTUATION_DELAY = ACTIVITY_LEVEL * 1.0 * 60000;
  ACTUATION_DELAY += weather_manager.getTemperature() * -250;
  ACTUATION_DELAY += weather_manager.getHumidity() * 1000;
  ACTUATION_DELAY += 5000 - lux_manager.getGlobalLux();
  if (millis() % 30000 == 0)
  {
    Serial.print("ACTUATION_DELAY is : ");
    Serial.println(ACTUATION_DELAY);
  }
  if (last_playback_tmr > ACTUATION_DELAY)
  {
    float wind_amount = 0.33;
    int between_time = 500;
    windBox(wind_amount, between_time);
    last_playback_tmr = 0;
  }
  manualWinding();
}

#elif ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE == SHAKER_BODY

int motor_speed = 0;
int target_motor_speed = 0;
int next_motor_speed = 0;
int motor_time = 0;
int next_motor_time = 0;
const int max_motor_speed = 450;
const int min_motor_speed = -450;

void shake(int on_speed, int on_time, int rev_speed, int rev_time)
{
  if (rev_speed > 0)
  {
    rev_speed *= -1;
  }
  target_motor_speed = on_speed;
  next_motor_speed = rev_speed;
  motor_time = on_time;
  next_motor_time = rev_time;

  Serial.print("shaking (onspeed, ontime, revspeed, revtime): ");
  Serial.print(on_speed);
  Serial.print("\t");
  Serial.print(on_time);
  Serial.print("\t");
  Serial.print(rev_speed);
  Serial.print("\t");
  Serial.println(rev_time);

  neos[0].colorWipe(0, 40, 125, 1.0);

  // rev the motor up
  rampShakerMotor(0, on_speed, on_time * 0.05);

  // Serial.print("starting / ending pos: ");
  // enc_pos = enc.read();
  // Serial.print(enc_pos);

  // let motor spin, with new color
  neos[0].colorWipe(200, 200, 255, 1.0);
  delay(on_time * 0.95);

  // ramp up motor to it's reverse speed
  neos[0].colorWipe(125, 40, 0, 1.0);
  rampShakerMotor(on_speed, rev_speed, rev_time * 0.1);

  neos[0].colorWipe(50, 40, 0, 1.0);
  // let things rotate for a bit
  delay(rev_time * 0.75);
  // rev down to off
  neos[0].colorWipe(25, 20, 0, 1.0);
  rampShakerMotor(rev_speed, 0, rev_time * 0.15);

  // enc_pos = enc.read();
  // Serial.print(enc_pos);
  neos[0].colorWipe(0, 0, 0, 1.0);
}

void rampShakerMotor(int16_t start, int16_t target, int ramp_total_time)
{
  Serial.print("Ramping Motor (start, target, time) - ");
  Serial.print(start);
  Serial.print("\t");
  Serial.print(target);
  Serial.print("\t");
  Serial.println(ramp_total_time);

  int difference = target - start;
  float step_delay = abs(difference / ramp_total_time) * 1.0;

  Serial.print(" dif: ");
  Serial.print(difference);
  Serial.print(" stepd: ");
  Serial.print(step_delay);
  motors.enableDrivers(0);

  if (difference > 0)
  {
    for (int16_t i = start; i <= target; i++)
    {
      motors.setM1Speed(i);
      // Serial.println(i);
      delayMicroseconds(step_delay);
    }
  }
  else
  {
    for (int16_t i = start; i > target; i--)
    {
      motors.setM1Speed(i);
      delayMicroseconds(step_delay);
      // Serial.println(i);
    }
  }

  if (target == 0)
  {
    motors.setM1Speed(0);
    motors.disableDrivers(0);
  }
  Serial.println("Disabled Drivers");
}

// for the shaker
void exploratorLoop()
{

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

  if (last_playback_tmr > ACTUATION_DELAY)
  {
    Serial.print("ACTUATION_DELAY is : ");
    Serial.println(ACTUATION_DELAY);

    int option = random(0, 2);

    if (option == 0)
    {
      // on speed, on time, reverse speed, reverse time
      int on_speed = 200 + (weather_manager.getTemperature() * 10);
      int on_time = 250 + (weather_manager.getTemperature() * 20);
      int reverse_speed = 200 + (weather_manager.getTemperature() * 10);
      int reverse_time = -250 - (weather_manager.getTemperature() * 10);
      shake(on_speed, on_time, reverse_speed, reverse_time);
    }
    else if (option > 0)
    {
      int times = random(2, 12);
      for (int i = 0; i < times; i++)
      {
        int d_time = random(5, 50);
        int ot = random(20, 100);
        int d_factor = random(0, 3);
        delay(d_time * d_factor);
        shake(400, ot, -250, ot / 2);
      }
    }
    last_playback_tmr = 0;
  }
}

#elif ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE == WOODPECKER_BODY

void exploratorLoop()
{
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
  ACTUATION_DELAY = ACTUATION_DELAY * 25.5;
  if (last_playback_tmr > ACTUATION_DELAY)
  {
    Serial.print("ACTUATION_DELAY is : ");
    Serial.println(ACTUATION_DELAY);
    if (playback_engine.isActive() == false)
    {
      playback_engine.playRhythm(rhythm_bank.getRandomRhythm());
      last_playback_tmr = 0;
    }
    else
    {
      Serial.println("Skipping rhythm as a rhythm is already playing");
    }
  }
}

#elif ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE == BELL_BODY
void exploratorLoop()
{
  ///////////////// Actuator Outputs //////////////////
  updateSolenoids(); // turns off all solenoids which have
  playback_engine.update();
  ACTUATION_DELAY = (30000) + ((weather_manager.getTemperature() + weather_manager.getHumidity()) * lux_manager.getGlobalLux());
  if (last_playback_tmr > ACTUATION_DELAY)
  {
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

ValueTrackerDouble onset_tracker = ValueTrackerDouble((String) "Onset Feature", &onset_feature, 0.1, 3000, 1.0);
ValueTrackerDouble cent_tracker = ValueTrackerDouble((String) "Cent", &onset_cent, 0.1, 3000, 1.0);
ValueTrackerDouble rms_tracker = ValueTrackerDouble((String) "RMS", &onset_rms, 0.1, 3000, 1.0);

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

void exploratorLoop()
{
  updateSolenoids(); // turns off all solenoids which need to be turned off
  //  listen for onsets
  if (millis() > 90000 && (updateOnset() || last_playback_tmr > 60000))
  {
    triggerSolenoid(2, 25);
    triggerSolenoid(7, 25);
    last_playback_tmr = 0;
  }
  // if onset detected, immedietally actuate
  // pause audio analysis for x period of time
  if (COLOR_MAP_MODE == COLOR_MAPPING_HSB)
  {
    double s = calculateSaturation(&feature_collector, &fft_manager[dominate_channel]);
    double b = calculateBrightness(&feature_collector, &fft_manager[dominate_channel]); // user brightness scaler is applied in this function
    double h = calculateHue(&feature_collector, &fft_manager[dominate_channel]);
    printHSB();
    printRGB();

    // if (feature_collector.isActive() == true) {
    neos[0].colorWipeHSB(h, s, b); // now colorWipe the LEDs with the HSB value
    // } else {
    // Serial.println("ERROR - not able to updateNeos() as there is no active audio channels");
    // }
  }
  else if (COLOR_MAP_MODE == COLOR_MAPPING_EXPLORATOR)
  {
    updateFeedbackLEDs(&fft_manager[dominate_channel]);
    // Serial.println("Finished running updateFeedbackLEDs()");
    // delay(2000);
  }
  else
  {
    Serial.println("ERROR = that color mode is not implemented in update neos");
  }
}

#endif // Explorator Loops

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
  function_times = 0;
#endif // P_FUNCTION_TIMES
  for (int i = 0; i < num_fft_managers; i++)
  {
    if (fft_manager[i].update())
    {
#if P_FUNCTION_TIMES == true
      Serial.print("fft_manager update took ");
      Serial.print(function_times);
      Serial.println(" micro seconds to update");
#endif // P_FUNCTION_TIMES
    }
  }

#if P_FUNCTION_TIMES == true
  function_times = 0;
#endif // P_FUNCTION_TIMES

  if (feature_collector.update(fft_manager))
  {
#if P_FUNCTION_TIMES == true
    Serial.print("feature_collector update took ");
    Serial.print(function_times);
    Serial.println(" micro seconds to update");
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

void printArtefactInfo()
{
  //////////////// ARTEFACT TYPE /////////////////
  Serial.print("Artefact type:\t");
#if ARTEFACT_TYPE == SPECULATOR
  Serial.println("SPECULATOR");
#elif ARTEFACT_TYPE == EXPLORATOR
  Serial.println("EXPLORATOR");
  Serial.print("Body type  :\t");
  /*  */ if (BODY_TYPE == MB_BODY)
  {
    Serial.println("MB Body");
  }
  else if (BODY_TYPE == WOODPECKER_BODY)
  {
    Serial.println("Pecker Body");
  }
  else if (BODY_TYPE == BELL_BODY)
  {
    Serial.println("Bell Body");
  }
  else if (BODY_TYPE == CLAPPER_BODY)
  {
    Serial.println("Clapper Body");
  }
  else if (BODY_TYPE == SHAKER_BODY)
  {
    Serial.println("Shaker Body");
  }
  else
  {
    Serial.println("WARNING - BODY TYPE IS NOT RECOGNIZED");
  }

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
  if (ENCLOSURE_TYPE == GROUND_ENCLOSURE)
  {
    Serial.println("Ground");
  }
  else if (ENCLOSURE_TYPE == ORB_ENCLOSURE_WITHOUT_HOLE)
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
#endif // SPECULATOR ARTEFACT TYPE
}

#if ARTEFACT_TYPE == LEGATUS

uint8_t colors[3] = {155, 70, 200};
float fcolors[3];

void brightnessWipe(double b)
{
  // uses the currently stored color, adjusts the brightness
  neos[0].colorWipe(constrain((int)(colors[0] * b), 0, 255), constrain((int)(colors[1] * b), 0, 255), constrain((int)(colors[2] * b), 0, 255), user_brightness_scaler);
}

void updateLegatusPassiveLEDs()
{
  if (fft1.available())
  {
    fcolors[2] = fft1.read(1, 8) * 4;   // 172  - 1536  Hz
    fcolors[1] = fft1.read(8, 23) * 4;  // 1536 - 4416  Hz
    fcolors[0] = fft1.read(23, 93) * 4; // 1536 - 15996 Hz
    for (int i = 0; i < 3; i++)
    {
      if (colors[i] > 1.0)
      {
        colors[i] = 1.0;
      }
    }
    for (int i = 0; i < 3; i++)
    {
      colors[i] = (uint8_t)(fcolors[i] * 255);
    }
    // Serial.print(colors[0]);
    // Serial.print("\t");
    // Serial.print(colors[1]);
    // Serial.print("\t");
    // Serial.println(colors[2]);
  }
  if (peak1.available())
  {
    int c = (int)(peak1.read() * 4096) * user_brightness_scaler;
    float f = (float)constrain(c, 0, 255) / 255.0;
    brightnessWipe(f);
    // Serial.println(f);
  }
}

#if ARTEFACT_TYPE == LEGATUS
float mix_gains[3] = {0.0, 0.0, 0.0};

void fadeMixer(float c1_current, float c2_current, float c1_target, float c2_target)
{
  float slice1 = (c1_target - c1_current) * 0.01;
  float slice2 = (c2_target - c2_current) * 0.01;

  for (int i = 0; i < 100; i++)
  {

    c1_current = c1_current + slice1;
    c2_current = c2_current + slice2;

    mixer2.gain(0, c1_current);
    mixer2.gain(1, c1_current);
    mixer2.gain(2, c2_current);
    delay(10);
  }
}

void playFile(const char *filename, float rate)
{
  Serial.print("Playing file: ");
  Serial.println(filename);
  audio_player.play(filename);
#if USE_RAW_AUDIO_PLAYER == true
  audio_player.setPlaybackRate(rate);
#endif
  fadeMixer(0, 0.5, 0.5, 0.3);
  Serial.print("file length : ");
  Serial.println(audio_player.lengthMillis());
  Serial.print("Playback Rate : ");
  Serial.println(rate);
  // Start playing the file.  This sketch continues to
  // run while the file plays.  audio_player.play(filename);

  // A brief delay for the library read WAV info
  delay(15);

  // Simply wait for the file to finish playing.
  while (audio_player.isPlaying())
  {
    // readUserControls();
    if (uimanager.update())
    {
      if (P_USER_CONTROLS)
      {
        uimanager.printAll();
      }
      amp1.gain(USER_CONTROL_PLAYBACK_GAIN);
      amp2.gain(USER_CONTROL_PLAYBACK_GAIN);
      Serial.print("Updated the user control playback gain: ");
      Serial.println(USER_CONTROL_PLAYBACK_GAIN);
    }
    updateLegatusPassiveLEDs();
  }
  // change the mixer levels back so the microphone is dominate
  fadeMixer(0.5, 0.0, 0.3, 1.0);
  Serial.println("Finished playing back file...");
}
#endif // playback mode

// only for LEGATUS
void setupAudio()
{
  ////////////// Audio ////////////
  printMajorDivide("Setting up Audio Parameters");
  AudioMemory(AUDIO_MEMORY);
  Serial.print("Audio Memory has been set to: ");
  Serial.println(AUDIO_MEMORY);
  delay(500);

  sgtl5000.enable();
  sgtl5000.volume(0.0);
  sgtl5000.inputSelect(AUDIO_INPUT_MIC);
  sgtl5000.micGain(54);

#if SD_PRESENT
  SPI.setMOSI(SD_MOSI);
  SPI.setSCK(SD_CLK);
  if (!(SD.begin(SD_CS)))
  {
    // stop here, but print a message repetitively
    while (1)
    {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
#endif

  /////////////////////////////////////////////////////////////////////
  // feature_collector 0-1 are for the song front/rear
  if (PEAK_FEATURE_ACTIVE)
  {
    feature_collector.linkPeak(&peak1, P_PEAK_VALS);
  }
  else
  {
    Serial.println("ERROR, PEAK_FEATURE_ACTIVE is false, should be set to true!!!");
    delay(10000);
  }

  if (RMS_FEATURE_ACTIVE)
  {
    feature_collector.linkRMS(&rms1, P_RMS_VALS);
  }
  else
  {
    Serial.println("ERROR, RMS_FEATURE_ACTIVE should be set to true, is currently false!!!");
    delay(10000);
  }

  if (FFT_FEATURES_ACTIVE)
  {
    fft_manager[0].linkFFT(&fft1, "Front");

    Serial.print("Linked FFT to FFTManager mumber");
    Serial.println(0);
    fft_manager[0].setupCentroid(true, CENTROID_FEATURE_MIN, CENTROID_FEATURE_MAX);
    fft_manager[0].setPrintFFTValues(P_FFT_VALS);
    fft_manager[0].setPrintCentroidValues(P_CENTROID_VALS);
    Serial.print("Started calculating Centroid in the FFTManager with a min/max of : ");
    Serial.print(CENTROID_FEATURE_MIN);
    Serial.print("\t");
    Serial.println(CENTROID_FEATURE_MAX);

    fft_manager[0].setCalculateFlux(true);
    fft_manager[0].setPrintFluxValues(P_FLUX_VALS);
    Serial.println("Started calculating FLUX in the FFTManager");

    Serial.println("Feature collectors have been linked");
  }
  else
  {
    Serial.println("ERROR, FFT_FEATURES_ACTIVE should be set to true, is currently false!!!");
    delay(10000);
  }

  /////////////////////////////////////////////////////////////////////
  biquad3.setHighpass(0, LBQ1_THRESH, LBQ1_Q);
  // left_HPF.setHighpass(1, LBQ1_THRESH, LBQ1_Q);
  // left_HPF.setHighpass(2, LBQ1_THRESH, LBQ1_Q);
  // left_HPF.setLowShelf(3, LBQ1_THRESH, LBQ1_DB);
  Serial.print("mic HPF has been configured (thresh/Q/dB): ");
  Serial.print(LBQ1_THRESH);
  Serial.print("\t");
  Serial.print(LBQ1_Q);
  Serial.print("\t");
  Serial.println(LBQ1_DB);

  biquad4.setLowpass(0, LBQ2_THRESH, LBQ2_Q);
  // left_LPF.setLowpass(1, LBQ2_THRESH, LBQ2_Q);
  // left_LPF.setLowpass(2, LBQ2_THRESH, LBQ2_Q);
  // left_LPF.setHighShelf(3, LBQ2_THRESH, LBQ2_DB);
  Serial.print("mic LPF has been configured (thresh/Q/dB): ");
  Serial.print(LBQ2_THRESH);
  Serial.print("\t");
  Serial.print(LBQ2_Q);
  Serial.print("\t");
  Serial.println(LBQ2_DB);

  if (ARTEFACT_BEHAVIOUR == PLAYBACK_MODE)
  {
    audio_player.begin();
    biquad1.setHighpass(0, RBQ1_THRESH, RBQ1_Q);
    biquad1.setLowpass(1, RBQ2_THRESH, RBQ2_Q);
    // left_HPF.setHighpass(2, LBQ1_THRESH, LBQ1_Q);
    // left_HPF.setLowShelf(3, LBQ1_THRESH, LBQ1_DB);
    Serial.println("playback left filters have been configured");

    biquad2.setHighpass(0, RBQ1_THRESH, RBQ1_Q);
    biquad2.setLowpass(0, RBQ2_THRESH, RBQ2_Q);
    // left_LPF.setLowpass(2, LBQ2_THRESH, LBQ2_Q);
    // left_LPF.setHighShelf(3, LBQ2_THRESH, LBQ2_DB);
    Serial.println("playback right Filters have been configured ");

    printMinorDivide();

    //////////////////////////////////////////////////////////////////////////////////
    amp1.gain(USER_CONTROL_PLAYBACK_GAIN);
    amp2.gain(USER_CONTROL_PLAYBACK_GAIN);
    Serial.print("playback gains are set to : ");
    Serial.println(USER_CONTROL_PLAYBACK_GAIN);

    //////////////////////////////////////////////////////////////////////////////////
    // TODO - make sure ENC_GAIN_ADJ exists for all bots
    Serial.print("Starting gain is now set to: ");
    Serial.println(STARTING_GAIN);
    mixer1.gain(0, STARTING_GAIN * 0.5);
    mixer1.gain(1, STARTING_GAIN * 0.5);
    amp3.gain(STARTING_GAIN);

    mixer2.gain(0, 0.33);
    mixer2.gain(1, 0.33);
    mixer2.gain(2, 0.5);
    // Serial.println("Setting up the FFTManager to track the first channel");
    // fft_manager.addInput(&patchCord_fft_input1);
    // patchCord_fft_input2.disconnect();
  }
  else if (ARTEFACT_BEHAVIOUR == MATCH_PITCH_MODE)
  {
    mixer1.gain(0, STARTING_GAIN * 4);
    mixer1.gain(1, STARTING_GAIN * 4);
    sine_fm.amplitude(0.0);
    sine_fm.frequency(200);
  }
  else if (ARTEFACT_BEHAVIOUR == FEEDBACK_MODE)
  {
    audio_delay1.delay(0, 0);
    mixer1.gain(0, STARTING_GAIN);
    mixer1.gain(1, STARTING_GAIN);
  }
}

#else // all other species other than Legatus

void setupAudio()
{
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
  if (PEAK_FEATURE_ACTIVE)
  {
    feature_collector.linkPeak(&peak1, P_PEAK_VALS);
#if NUM_PEAK_ANAS > 1
    feature_collector.linkPeak(&right_peak, P_PEAK_VALS);
#endif
  }

#if RMS_FEATURE_ACTIVE
  feature_collector.linkRMS(&rms1, P_RMS_VALS);
#if NUM_RMS_ANAS > 1
  feature_collector.linkRMS(&rms2, P_RMS_VALS);
#endif
#endif

  if (FFT_FEATURES_ACTIVE)
  {
    fft_manager[0].linkFFT(&fft1, "Front");
#if NUM_FFT > 1
    fft_manager[1].linkFFT(&right_fft, "Rear");
#endif

    for (int i = 0; i < num_fft_managers; i++)
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
  }
  Serial.println("Feature collectors have been linked");

#if ARTEFACT_TYPE != EXPLORATOR
  left_amp.gain(STARTING_GAIN);
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
#else
  left_HPF.setHighpass(0, LBQ1_THRESH, LBQ1_Q);
  left_HPF.setLowpass(1, LBQ2_THRESH, LBQ2_Q);
  left_amp.gain(STARTING_GAIN);

#endif

  // feature_collector.testMicrophone();
  Serial.println("Exiting setupAudio()");
  printDivide();
}
#endif // for setupAudio()

#if ARTEFACT_TYPE == SPECULATOR
void speculatorSetup()
{
// setup up some value tracker stuff
#if COLOR_MAP_MODE == COLOR_MAPPING_HSB
  brightness_tracker.setMinMaxUpdateFactor(BGT_MIN_UPDATE_FACTOR, BGT_MAX_UPDATE_FACTOR);
  hue_tracker.setMinMaxUpdateFactor(HUE_MIN_UPDATE_FACTOR, HUE_MAX_UPDATE_FACTOR);
  saturation_tracker.setMinMaxUpdateFactor(SAT_MIN_UPDATE_FACTOR, SAT_MAX_UPDATE_FACTOR);
#endif

  //////////////// User Controls /////////////////////////////
  explainSerialCommands(true);
  // TODO - need to replace with the UIManager

  /////////////// User Controls ////////////////////////////////////////////
  // TODO make buttons do something for the speculators again
#if DISABLE_USER_CONTROLS == false
#if HV_MAJOR == 3

  uimanager.addBut(BUT1_PIN, BUT1_PULLUP, BUT1_LOW_VAL, BUT1_HIGH_VAL, BUT1_LOW_CHANGES, &but_test[0], BUT1_NAME);
  uimanager.addBut(BUT2_PIN, BUT2_PULLUP, BUT2_LOW_VAL, BUT2_HIGH_VAL, BUT2_LOW_CHANGES, &but_test[1], BUT2_NAME);
  uimanager.addBut(BUT3_PIN, BUT3_PULLUP, BUT3_LOW_VAL, BUT3_HIGH_VAL, BUT3_LOW_CHANGES, &but_test[2], BUT3_NAME);
  uimanager.addBut(BUT4_PIN, BUT4_PULLUP, BUT4_LOW_VAL, BUT4_HIGH_VAL, BUT4_LOW_CHANGES, &but_test[3], BUT4_NAME);
  uimanager.addBut(BUT5_PIN, BUT5_PULLUP, BUT5_LOW_VAL, BUT5_HIGH_VAL, BUT5_LOW_CHANGES, &but_test[4], BUT5_NAME);
  uimanager.addBut(BUT6_PIN, BUT6_PULLUP, BUT6_LOW_VAL, BUT6_HIGH_VAL, BUT6_LOW_CHANGES, &but_test[5], BUT6_NAME);
  uimanager.addBut(BUT7_PIN, BUT7_PULLUP, BUT7_LOW_VAL, BUT7_HIGH_VAL, BUT7_LOW_CHANGES, &but_test[6], BUT7_NAME);
  uimanager.addBut(BUT8_PIN, BUT8_PULLUP, BUT8_LOW_VAL, BUT8_HIGH_VAL, BUT8_LOW_CHANGES, &but_test[7], BUT8_NAME);
  uimanager.addBut(BUT9_PIN, BUT9_PULLUP, BUT9_LOW_VAL, BUT9_HIGH_VAL, BUT9_LOW_CHANGES, &but_test[8], BUT9_NAME);
  uimanager.addBut(BUT10_PIN, BUT10_PULLUP, BUT10_LOW_VAL, BUT10_HIGH_VAL, BUT10_LOW_CHANGES, &but_test[9], BUT10_NAME);
  /*
  uimanager.addBut(BUT1_PIN, BUT1_PULLUP, BUT1_LOW_VAL, BUT1_HIGH_VAL, BUT1_LOW_CHANGES, &USE_TARGET_HUE, BUT1_NAME);
  uimanager.addBut(BUT2_PIN, BUT2_PULLUP, BUT2_LOW_VAL, BUT2_HIGH_VAL, BUT2_LOW_CHANGES, &SQUARE_BRIGHTNESS, BUT2_NAME);
  uimanager.addBut(BUT3_PIN, BUT3_PULLUP, BUT3_LOW_VAL, BUT3_HIGH_VAL, BUT3_LOW_CHANGES, &USE_TARGET_BRIGHTNESS, BUT3_NAME);
  uimanager.addBut(BUT4_PIN, BUT4_PULLUP, BUT4_LOW_VAL, BUT4_HIGH_VAL, BUT4_LOW_CHANGES, &REVERSE_HUE, BUT4_NAME);
  uimanager.addBut(BUT5_PIN, BUT5_PULLUP, BUT5_LOW_VAL, BUT5_HIGH_VAL, BUT5_LOW_CHANGES, &REVERSE_SATURATION, BUT5_NAME);
  uimanager.addBut(BUT6_PIN, BUT6_PULLUP, BUT6_LOW_VAL, BUT6_HIGH_VAL, BUT6_LOW_CHANGES, &REVERSE_BRIGHTNESS, BUT6_NAME);
  uimanager.addBut(BUT7_PIN, BUT7_PULLUP, BUT7_LOW_VAL, BUT7_HIGH_VAL, BUT7_LOW_CHANGES, &LED_MAPPING_MODE, BUT7_NAME);
  uimanager.addBut(BUT8_PIN, BUT8_PULLUP, BUT8_LOW_VAL, BUT8_HIGH_VAL, BUT8_LOW_CHANGES, &LED_MAPPING_MODE, BUT8_NAME);
  uimanager.addBut(BUT9_PIN, BUT9_PULLUP, BUT9_LOW_VAL, BUT9_HIGH_VAL, BUT9_LOW_CHANGES, &LED_MAPPING_MODE, BUT9_NAME);
  uimanager.addBut(BUT10_PIN, BUT10_PULLUP, BUT10_LOW_VAL, BUT10_HIGH_VAL, BUT10_LOW_CHANGES, &BOOT_DELAY_ACTIVE, BUT10_NAME);
  */

  uimanager.addPot(POT1_PIN, POT1_REVERSE, POT1_PLAY, &user_brightness_scaler, POT1_NAME);
  uimanager.addPot(POT2_PIN, POT2_REVERSE, POT2_PLAY, &user_brightness_cuttoff, POT2_NAME);
  uimanager.addPot(POT4_PIN, POT4_REVERSE, POT4_PLAY, &ADDED_SATURATION, POT4_NAME);
  uimanager.addPotRange(0, min_user_brightness_scaler, mid_user_brightness_scaler, max_user_brightness_scaler);
  uimanager.addPotRange(1, 0.0, 0.5, 1.0);
  uimanager.addPotRange(2, min_user_brightness_cuttoff, mid_user_brightness_cuttoff, max_user_brightness_cuttoff);

#elif HV_MAJOR == 2
  uimanager.addBut(BUT1_PIN, BUT1_PULLUP, BUT1_LOW_VAL, BUT1_HIGH_VAL, BUT1_LOW_CHANGES, &COLOR_MAP_MODE, BUT1_NAME);
  uimanager.addBut(BUT2_PIN, BUT2_PULLUP, BUT2_LOW_VAL, BUT2_HIGH_VAL, BUT2_LOW_CHANGES, &SQUARE_BRIGHTNESS, BUT2_NAME);
  uimanager.addBut(BUT3_PIN, BUT3_PULLUP, BUT3_LOW_VAL, BUT3_HIGH_VAL, BUT3_LOW_CHANGES, &USE_TARGET_BRIGHTNESS, BUT3_NAME);
  uimanager.addBut(BUT4_PIN, BUT4_PULLUP, BUT4_LOW_VAL, BUT4_HIGH_VAL, BUT4_LOW_CHANGES, &REVERSE_SATURATION, BUT4_NAME);
  uimanager.addBut(BUT5_PIN, BUT5_PULLUP, BUT5_LOW_VAL, BUT5_HIGH_VAL, BUT5_LOW_CHANGES, &REVERSE_HUE, BUT5_NAME);
  uimanager.addBut(BUT6_PIN, BUT6_PULLUP, BUT6_LOW_VAL, BUT6_HIGH_VAL, BUT6_LOW_CHANGES, &BOOT_DELAY_ACTIVE, BUT6_NAME);
#endif
  delay(50);
  uimanager.setup();
  uimanager.printAll();
#endif
  ///////////////////////// Audio //////////////////////////
  setupAudio();
}
#endif

AudioConnection *audio_connections[20];

#if ARTEFACT_TYPE == LEGATUS
void legatusSetup()
{
  if (ARTEFACT_BEHAVIOUR == PLAYBACK_MODE)
  {
    audio_connections[0] = new AudioConnection(i2s1, 0, mixer1, 0);
    audio_connections[1] = new AudioConnection(mixer2, 0, output_usb, 0);
    audio_connections[2] = new AudioConnection(i2s1, 1, mixer1, 1);
    audio_connections[3] = new AudioConnection(audio_player, 0, biquad1, 0);
    audio_connections[4] = new AudioConnection(audio_player, 1, biquad2, 0);
    audio_connections[5] = new AudioConnection(mixer1, 0, output_usb, 1);
    audio_connections[6] = new AudioConnection(mixer1, biquad3);
    audio_connections[7] = new AudioConnection(biquad1, amp1);
    audio_connections[8] = new AudioConnection(biquad2, amp2);
    audio_connections[9] = new AudioConnection(biquad3, biquad4);
    audio_connections[10] = new AudioConnection(amp1, 0, audioOutput, 0);
    audio_connections[11] = new AudioConnection(amp1, 0, mixer2, 0);
    audio_connections[12] = new AudioConnection(amp2, 0, audioOutput, 1);
    audio_connections[13] = new AudioConnection(amp2, 0, mixer2, 1);
    audio_connections[14] = new AudioConnection(biquad4, amp3);
    audio_connections[15] = new AudioConnection(amp3, 0, mixer2, 2);
    audio_connections[16] = new AudioConnection(mixer2, rms1);
    audio_connections[17] = new AudioConnection(mixer2, fft1);
    audio_connections[18] = new AudioConnection(mixer2, peak1);
  }
  else if (ARTEFACT_BEHAVIOUR == FEEDBACK_MODE)
  {
    audio_connections[0] = new AudioConnection(i2s1, 0, mixer1, 0);
    audio_connections[1] = new AudioConnection(i2s1, 1, mixer1, 1);
    audio_connections[2] = new AudioConnection(mixer1, 0, output_usb, 1);
    audio_connections[3] = new AudioConnection(mixer1, biquad3);
    audio_connections[4] = new AudioConnection(biquad3, biquad4);
    audio_connections[5] = new AudioConnection(biquad4, amp3);
    audio_connections[6] = new AudioConnection(amp3, rms1);
    audio_connections[7] = new AudioConnection(amp3, fft1);
    audio_connections[8] = new AudioConnection(amp3, peak1);
    audio_connections[9] = new AudioConnection(amp3, 0, output_usb, 0);
    audio_connections[10] = new AudioConnection(audio_delay1, 0, audioOutput, 0);
    audio_connections[11] = new AudioConnection(audio_delay1, 0, audioOutput, 1);
    audio_connections[12] = new AudioConnection(amp3, 0, audio_delay1, 0);
  }
  else if (ARTEFACT_BEHAVIOUR == FM_FEEDBACK_MODE)
  {
    audio_connections[0] = new AudioConnection(i2s1, 0, mixer1, 0);
    audio_connections[1] = new AudioConnection(i2s1, 1, mixer1, 1);
    audio_connections[2] = new AudioConnection(mixer1, 0, output_usb, 1);
    audio_connections[3] = new AudioConnection(mixer1, biquad3);
    audio_connections[4] = new AudioConnection(biquad3, biquad4);
    audio_connections[5] = new AudioConnection(biquad4, amp3);
    audio_connections[6] = new AudioConnection(amp3, rms1);
    audio_connections[7] = new AudioConnection(amp3, fft1);
    audio_connections[8] = new AudioConnection(amp3, peak1);
    audio_connections[9] = new AudioConnection(amp3, 0, output_usb, 0);
    audio_connections[10] = new AudioConnection(amp3, sine_fm);
    audio_connections[1] = new AudioConnection(sine_fm, 0, audioOutput, 0);
  }
  // setup up some value tracker stuff
  brightness_tracker.setMinMaxUpdateFactor(BGT_MIN_UPDATE_FACTOR, BGT_MAX_UPDATE_FACTOR);
  hue_tracker.setMinMaxUpdateFactor(HUE_MIN_UPDATE_FACTOR, HUE_MAX_UPDATE_FACTOR);
  saturation_tracker.setMinMaxUpdateFactor(SAT_MIN_UPDATE_FACTOR, SAT_MAX_UPDATE_FACTOR);

  //////////////// User Controls /////////////////////////////
  explainSerialCommands(true);
  // TODO - need to replace with the UIManager

  /////////////// User Controls ////////////////////////////////////////////
  uimanager.addBut(BUT1_PIN, BUT1_PULLUP, BUT1_LOW_VAL, BUT1_HIGH_VAL, BUT1_LOW_CHANGES, &but_test[0], BUT1_NAME);
  uimanager.addBut(BUT2_PIN, BUT2_PULLUP, BUT2_LOW_VAL, BUT2_HIGH_VAL, BUT2_LOW_CHANGES, &PLAYBACK_ACTIVE, BUT2_NAME);
  uimanager.addBut(BUT3_PIN, BUT3_PULLUP, BUT3_LOW_VAL, BUT3_HIGH_VAL, BUT3_LOW_CHANGES, &FM_FEEDBACK_ACTIVE, BUT3_NAME);
  uimanager.addBut(BUT4_PIN, BUT4_PULLUP, BUT4_LOW_VAL, BUT4_HIGH_VAL, BUT4_LOW_CHANGES, &FEEDBACK_ACTIVE, BUT4_NAME);
  uimanager.addBut(BUT5_PIN, BUT5_PULLUP, BUT5_LOW_VAL, BUT5_HIGH_VAL, BUT5_LOW_CHANGES, &but_test[4], BUT5_NAME);
  uimanager.addBut(BUT6_PIN, BUT6_PULLUP, BUT6_LOW_VAL, BUT6_HIGH_VAL, BUT6_LOW_CHANGES, &but_test[5], BUT6_NAME);

  uimanager.addPot(POT1_PIN, POT1_REVERSE, POT1_PLAY, &user_brightness_scaler, POT1_NAME);
  uimanager.addPot(POT2_PIN, POT2_REVERSE, POT2_PLAY, &USER_CONTROL_PLAYBACK_GAIN, POT2_NAME);
  uimanager.addPotRange(0, min_user_brightness_scaler, mid_user_brightness_scaler, max_user_brightness_scaler);
  uimanager.addPotRange(1, min_playback_gain, mid_playback_gain, max_playback_gain);

  uimanager.setup();
  uimanager.printAll();
  //////////////////// power control ///////////////////////
  pinMode(PWR_KILL_PIN, OUTPUT);
  digitalWrite(PWR_KILL_PIN, HIGH);

  ///////////////////////// Audio //////////////////////////
  setupAudio();
}
#endif // ARTEFACT_TYPE == LEGATUS

#if ARTEFACT_TYPE == SPECULATOR
void speculatorLoop()
{
#if CALCULATE_DOMINATE_CHANNEL
  dominate_channel = feature_collector.getDominateChannel();
  // Serial.print("dominate channel is : ");
  // Serial.println(dominate_channel);
#endif
#if COLOR_MAP_MODE == COLOR_MAPPING_HSB

  double s = calculateSaturation(&feature_collector, &fft_manager[dominate_channel]);
  double b = calculateBrightness(&feature_collector, &fft_manager[dominate_channel]); // user brightness scaler is applied in this function
  double h = calculateHue(&feature_collector, &fft_manager[dominate_channel]);
  printHSB();
  printRGB();

  s = s * 2;
  if (s > 1.0)
  {
    s = 1.0;
  }

  // if (feature_collector.isActive() == true) {
  neos[0].colorWipeHSB(h, s, b); // now colorWipe the LEDs with the HSB value
  // } else {
  // Serial.println("ERROR - not able to updateNeos() as there is no active audio channels");
  // }

#elif COLOR_MAP_MODE == COLOR_MAPPING_EXPLORATOR
  updateFeedbackLEDs(&fft_manager[dominate_channel]);
  // Serial.println("Finished running updateFeedbackLEDs()");
  // delay(2000);
#elif COLOR_MAP_MODE == COLOR_MAPPING_RGB
  // determine the amount of energy contained in each of the three bands
  float total_energy = fft_manager[0].getFFTTotalEnergy() * 0.25;
  int front_inside_out_order[20] = {16, 17, 18, 19, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  int rear_inside_out_order[20] = {36, 37, 38, 39, 30, 31, 32, 33, 34, 35, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29};
  Serial.print("total energy : ");
  Serial.print(total_energy, 6);
  total_energy = constrainf(total_energy, 0.0, 1.0);
  Serial.print("\t");
  Serial.println(total_energy, 6);

  // map each individual LED to a range of four bins
  for (int i = 0; i < 20; i++)
  {
    int idx_start = (i * 6) + 10;

    // amount of energy contained in the bin multiplied by the number of bins (ignoring the second half)
    double r = pow((idx_start * (fft_manager[0].getFFTRangeByIdx(idx_start, idx_start + 18))), 0.5) * (user_brightness_scaler - user_brightness_scaler * 0.5);
    idx_start += 2;
    double g = pow((idx_start * (fft_manager[0].getFFTRangeByIdx(idx_start, idx_start + 18))), 0.5) * (user_brightness_scaler - user_brightness_scaler * 0.5);
    idx_start += 2;
    double b = pow((idx_start * (fft_manager[0].getFFTRangeByIdx(idx_start, idx_start + 18))), 0.5) * (user_brightness_scaler - user_brightness_scaler * 0.5);

    // scale up the values between 0.0 and 255 for NeoPixel color values
    Serial.print("r: ");
    Serial.print(r);
    Serial.print("\t");
    r = constrainf(r/3, 0.0, 0.75);
    r = r - user_brightness_cuttoff;
    r = constrainf(r, 0.0, 0.75);
    Serial.print(r);
    Serial.print("\t");
    Serial.println(user_brightness_cuttoff);

    g = constrainf(g/3, 0.0, 0.75);
    g = g - user_brightness_cuttoff;
    g = constrainf(g, 0.0, 0.75);

    b = constrainf(b/3, 0.0, 0.75);
    b = b - user_brightness_cuttoff;
    b = constrainf(b, 0.0, 0.75);

    g = (r + g + b)/2.25;
    g = constrainf(g, 0.0, 1.0);

    double h = pow(g, 0.5);
    double s = 1.0 - pow((g), 0.5);
    s = s + ADDED_SATURATION;
    if (s < 1.0){
      s = 1.0 - s;
    } else if (s > 1.0) {
      s = s - 1.0;
    }
    double br = pow(g, 2);

  if ( i == 10) {
    Serial.print(" h,s,b : ");
    Serial.print(h);
    Serial.print("\t");
    Serial.print(s);
    Serial.print("\t");
    Serial.println(br);
  }
    neos[0].colorWipeHSB(h, s, br, user_brightness_scaler, front_inside_out_order[i], front_inside_out_order[i]);
    neos[0].colorWipeHSB(h, s, br, user_brightness_scaler, rear_inside_out_order[i], rear_inside_out_order[i]);
  }
  // use the total energy to influence the brightness
  /*
  total_energy = constrainf((total_energy - 0.02) * 20, 0.0, 1.0);

  Serial.print("\t");
  Serial.println(total_energy, 6);
  Serial.print("total energy : ");
  Serial.print(total_energy, 6);
  // use the total energy to influence the brightness
  total_energy = constrainf((total_energy - 0.02) * 20, 0.0, 1.0);

  Serial.print("\t");
  Serial.println(total_energy, 6);

  Serial.print("r,g,b : ");
  Serial.print(r);
  Serial.print("\t");
  Serial.print(g);
  Serial.print("\t");
  Serial.print(b);

  // inner rings
  neos[0].colorWipe(r, 255 - r, 255 - r, total_energy, 16, 19);
  neos[0].colorWipe(r, 255 - r, 255 - r, total_energy, 36, 39);
  // middle rings
  neos[0].colorWipe(255 - g, g, 255 - g, total_energy, 10, 15);
  neos[0].colorWipe(255 - g, g, 255 - g, total_energy, 30, 35);
  // outer rings
  neos[0].colorWipe(255 - b, 255 - b, b, total_energy, 0, 9);
  neos[0].colorWipe(255 - b, 255 - b, b, total_energy, 20, 29);
  */

#else
  {
    Serial.println("ERROR = that color mode is not implemented in update neos");
  }
#endif // visual feedback
}
#endif // ARTEFACT_TYPE == SPECULATOR

//////////////////////////////////////////////////////////////////////////
////////////////// setup / main loops ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#if ARTEFACT_TYPE == EXPLORATOR
void exploratorSetup()
{

  printMajorDivide("starting exploratorSetup() loop");

#if ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE == WOODPECKER_BODY
  pecker[0].setMotorSpeeds(70, -30);
#endif

  /////////////// User Controls ////////////////////////////////////////////
#if BODY_TYPE == WOODPECKER_BODY || BODY_TYPE == BELL_BODY
  uimanager.addBut(BUT1_PIN, BUT1_PULLUP, BUT1_LOW_VAL, BUT1_HIGH_VAL, BUT1_LOW_CHANGES, &but_test[0], BUT1_NAME);
  uimanager.addBut(BUT2_PIN, BUT2_PULLUP, BUT2_LOW_VAL, BUT2_HIGH_VAL, BUT2_LOW_CHANGES, &but_test[1], BUT2_NAME);
  uimanager.addBut(BUT3_PIN, BUT3_PULLUP, BUT3_LOW_VAL, BUT3_HIGH_VAL, BUT3_LOW_CHANGES, &but_test[2], BUT3_NAME);
  uimanager.addBut(BUT4_PIN, BUT4_PULLUP, BUT4_LOW_VAL, BUT4_HIGH_VAL, BUT4_LOW_CHANGES, &but_test[3], BUT4_NAME);

  uimanager.addPot(POT1_PIN, POT1_REVERSE, POT1_PLAY, &ACTIVITY_LEVEL, POT1_NAME);
  uimanager.addPot(POT2_PIN, POT2_REVERSE, POT2_PLAY, &STRIKE_LENGTH, POT2_NAME);

  uimanager.setup();
  uimanager.printAll();

#elif BODY_TYPE == MB_BODY
  uimanager.addBut(BUT1_PIN, BUT1_PULLUP, BUT1_LOW_VAL, BUT1_HIGH_VAL, BUT1_LOW_CHANGES, &WIND_FORWARD, BUT1_NAME);
  uimanager.addBut(BUT2_PIN, BUT2_PULLUP, BUT2_LOW_VAL, BUT2_HIGH_VAL, BUT2_LOW_CHANGES, &WIND_BACKWARD, BUT2_NAME);

  uimanager.addPot(POT1_PIN, POT1_REVERSE, POT1_PLAY, &ACTIVITY_LEVEL, POT1_NAME);
  uimanager.addPot(POT2_PIN, POT2_REVERSE, POT2_PLAY, &WINDING_RATE, POT2_NAME);
  uimanager.addPotRange(0, MIN_ACTIVITY_LEVEL, MID_ACTIVITY_LEVEL, MAX_ACTIVITY_LEVEL);
  uimanager.addPotRange(1, MIN_WINDING_RATE, MID_WINDING_RATE, MAX_WINDING_RATE);

  uimanager.setup();
  // uimanager.printAll();
#endif // UI controls for the woodpecker and bellbot

  ////////////////////// Audio
  printMinorDivide();
  Serial.println("Starting the Audio system");
  AudioMemory(AUDIO_MEMORY);
  // TODO make this proper
  uint32_t lpf = 14000;
  uint32_t hpf = 200;
  double q = 0.8;

  left_amp.gain(STARTING_GAIN);
#if BODY_TYPE == MB_BODY
  mixer1.gain(0, 1.0);
  mixer1.gain(1, 1.0);
#endif

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

  fft_manager[0].linkFFT(&fft1, "All");
  fft_manager[0].setCalculateCent(true);
  fft_manager[0].setCalculateFlux(false);

  feature_collector.linkPeak(&peak1, P_PEAK_VALS);
  // feature_collector.linkRMS(&rms1, P_PEAK_VALS);

  printMinorDivide();
  Serial.println("Finished setup Loop");
  colorWipeAll(0, 120, 30, 0.25);
  printMinorDivide();
}

void setOutputs()
{
  /////////////// Solenoid Outputs /////////////////////////////////////////
  for (int i = 0; i < NUM_SOLENOIDS; i++)
  {
    pinMode(s_pins[i], OUTPUT);
    digitalWrite(s_pins[i], LOW); // turns them off
  }

#if TEST_SOLENOIDS == true
  Serial.println("TEST_SOLENOIDS is set to true, system will just test solenoids over and over again forever");
  testSolenoids(1000); // let the system settle
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
  delay(2500); // let the system settle
#endif
}
#endif //  ARTEFACT_TYPE == EXPLORATOR

#if (ARTEFACT_TYPE == EXPLORATOR) && TEST_SOLENOIDS == true
void testSolenoids(unsigned int len)
{
  elapsedMillis t = 0;
  Serial.print("Testing Solenoids - ");
  while (t < len)
  {
    for (int i = 0; i < NUM_SOLENOIDS; i++)
    {
      if (sol_active[i] == true)
      {
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
  Serial.println("Starting LuxManager");
  lux_manager.add6030Sensors(2, 25);
  Serial.println("Finished starting LuxManager");
#elif (ARTEFACT_TYPE == SPECULATOR) && (HV_MAJOR == 2)
  Serial.println("Starting LuxManager");
  lux_manager.addSensorTcaIdx("Front", 0);
  lux_manager.addSensorTcaIdx("Rear", 1);
  lux_manager.startTCA7700Sensors(VEML7700_GAIN_1, VEML7700_IT_25MS); // todo add this to config_adv? todo
  Serial.println("Finished starting LuxManager");
#elif (ARTEFACT_TYPE == EXPLORATOR)
  Serial.println("Starting LuxManager");
#if BODY_TYPE == MB_BODY
  lux_manager.add6030Sensors(2, 25);
#else
  lux_manager.add7700Sensor((String) "Eye-Stock");
  lux_manager.start7700Sensor(VEML7700_GAIN_1, VEML7700_IT_25MS); // todo add this to config_adv? todo
#endif
  Serial.println("Finished starting LuxManager");
  printMinorDivide();
#elif ARTEFACT_TYPE == LEGATUS
  Serial.println("Starting LuxManager");
  lux_manager.add6030Sensors(2, 25);
  Serial.println("Finished starting LuxManager");
#endif // HV_MAJOR
  delay(2000);

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

#if WEATHER_MANAGER_PRESENT == true
void extremeHumidityShutdown()
{
  colorWipeAll(0, 0, 0);
#if ARTEFACT_TYPE == LEGATUS
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
#endif // WEATHER_MANAGER_PRESENT == true

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

#if ARTEFACT_TYPE == EXPLORATOR
void configurePlaybackEngine()
{
  // freq, length, onset delay (since last note), velocity
#if BODY_TYPE == BELL_BODY
  rhythm[0].addPitchedNote(500.0, 40, 0, 0.6);
  rhythm[0].addPitchedNote(500.0, 40, 100, 0.8);
  rhythm[0].addPitchedNote(500.0, 30, 150, 1.0);
  rhythm[0].addPitchedNote(1000.0, 30, 400, 1.0);
  rhythm[0].addPitchedNote(1000.0, 20, 1000, 1.0);
  rhythm[0].addPitchedNote(1000.0, 50, 1500, 1.0);

  rhythm[0].addPitchedNote(100.0, 30, 100, 0.3);
  rhythm[0].addPitchedNote(500.0, 40, 600, 0.4);
  rhythm[0].addPitchedNote(1000.0, 30, 150, 0.5);
  rhythm[0].addPitchedNote(100.0, 20, 700, 0.4);
  rhythm[0].addPitchedNote(500.0, 40, 800, 0.3);
  rhythm[0].addPitchedNote(1000.0, 30, 1500, 0.2);

  rhythm[0].addPitchedNote(100.0, 40, 200, 1.0);
  rhythm[0].addPitchedNote(500.0, 30, 70, 1.0);
  rhythm[0].addPitchedNote(1000.0, 40, 90, 1.0);
  rhythm[0].addPitchedNote(100.0, 40, 60, 1.0);
  rhythm[0].addPitchedNote(500.0, 30, 150, 1.0);
  rhythm[0].addPitchedNote(1000.0, 30, 100, 1.0);

  rhythm[1].addPitchedNote(50.0, 50, 0, 1.05);
  rhythm[1].addPitchedNote(150.0, 50, 500, 1.01);
  rhythm[1].addPitchedNote(250.0, 60, 1000, 1.15);
  rhythm[1].addPitchedNote(550.0, 60, 1500, 1.20);
  rhythm[1].addPitchedNote(450.0, 50, 2000, 1.20);
  rhythm[1].addPitchedNote(1550.0, 40, 2500, 1.20);
  rhythm[1].addPitchedNote(1650.0, 30, 3000, 1.20);

  rhythm[2].addPitchedNote(50.0, 40, 0, 1.35);
  rhythm[2].addPitchedNote(150.0, 30, 400, 1.31);
  rhythm[2].addPitchedNote(250.0, 30, 800, 1.35);
  rhythm[2].addPitchedNote(1350.0, 40, 1200, 1.30);
  rhythm[2].addPitchedNote(450.0, 30, 2000, 1.30);
  rhythm[2].addPitchedNote(550.0, 50, 2400, 1.30);
  rhythm[2].addPitchedNote(1650.0, 20, 2600, 1.30);

  rhythm[3].addPitchedNote(150.0, 40, 0, 1.35);
  rhythm[3].addPitchedNote(150.0, 50, 400, 1.31);
  rhythm[3].addPitchedNote(250.0, 60, 600, 1.35);
  rhythm[3].addPitchedNote(1350.0, 40, 700, 1.30);
  rhythm[3].addPitchedNote(450.0, 40, 800, 1.30);
  rhythm[3].addPitchedNote(550.0, 50, 500, 1.30);
  rhythm[3].addPitchedNote(1650.0, 40, 1000, 1.30);

  rhythm_bank.addRhythm(&rhythm[0]);
  rhythm_bank.addRhythm(&rhythm[1]);
  rhythm_bank.addRhythm(&rhythm[2]);
  rhythm_bank.addRhythm(&rhythm[3]);
  * /

      playback_engine.linkBellMechanism(&bells[0]);
  playback_engine.linkBellMechanism(&bells[1]);
  playback_engine.linkBellMechanism(&bells[2]);

  playback_engine.linkNeoGroup(&neos[0]);
  playback_engine.linkNeoGroup(&neos[1]);
  playback_engine.linkNeoGroup(&neos[2]);

#elif BODY_TYPE == WOODPECKER_BODY
  for (int i = 0; i < 10; i++)
  {
    uint32_t quarter = random(120, 750);
    buildPeckRhythm(i, quarter);
    if (random(0, 100))
    {
      rhythm[i].addUnpitchedNote(quarter * 4, 0.75);
      buildPeckRhythm(i, quarter);
    }
    rhythm[i].addMotorMove(1, 70, 100);
    rhythm[i].print();
    rhythm_bank.addRhythm(&rhythm[i]);
  }
  playback_engine.linkMechanism(&pecker[0]);
  playback_engine.linkNeoGroup(&neos[1]);

#endif // BODY_TYPE == BELL_BODY
}

#if BODY_TYPE == WOODPECKER_BODY
void buildPeckRhythm(int idx, uint32_t quarter)
{
  uint32_t t = 0;
  uint32_t eigth = quarter / 2;
  uint32_t triplet = quarter / 3;

  // 30% chance to start with a hard strike
  if (random(0, 100) < 30)
  {
    rhythm[idx].addUnpitchedNote(0, 0.5);
    t = quarter;
  }

  // then 85% chance for a triplet of 3-4
  if (random(0, 100) < 85)
  {
    for (int i = 0; i < (int)random(3, 4); i++)
    {
      rhythm[idx].addUnpitchedNote(t, 0.25);
      t = triplet;
    }
  }

  // then 55% chance for a triplet of 3-4
  if (random(0, 100) < 55)
  {
    for (int i = 0; i < (int)random(3, 4); i++)
    {
      rhythm[idx].addUnpitchedNote(t, 0.25);
      t = triplet;
    }
  }

  // chance for a pause
  if (random(0, 100) < 45)
  {
    t += triplet;
    if (random(0, 100) < 45)
    {
      t += triplet;
    }
  }

  // then 55% chance for a triplet of 3-4
  if (random(0, 100) < 55)
  {
    for (int i = 0; i < (int)random(3, 4); i++)
    {
      rhythm[idx].addUnpitchedNote(t, 0.25);
      t = triplet;
    }
  }

  // chance for a pause
  if (random(0, 100) < 45)
  {
    t += triplet;
    if (random(0, 100) < 45)
    {
      t += eigth;
    }
    if (random(0, 100) < 65)
    {
      t += eigth;
    }
  }

  // 30% chance to start with a hard strike
  if (random(0, 100) < 30)
  {
    rhythm[idx].addUnpitchedNote(t, 0.5);
    t = quarter;
  }
}
#endif // BODY_TYPE == WOODPECKER_BODY
#endif // ARTEFACT_TYPE == EXPLORATOR

elapsedMillis loop_tmr;
const int loop_print_delay = 1000;

#if ARTEFACT_TYPE == LEGATUS
void legatusLoopSamplePlayback()
{
  updateLegatusPassiveLEDs();
  int avg_time = 1000 * 60 * 0.5;
  PLAYBACK_INTERVAL = avg_time - (weather_manager.getTemperature() * (avg_time / 40)) + (weather_manager.getHumidity() * (avg_time / 100)); // uint16_t t = random(45, 150);
  if (loop_tmr > loop_print_delay)
  {
    Serial.print("Awaiting playback (recording is TODO) : ");
    Serial.println(PLAYBACK_INTERVAL - last_playback_tmr);
    loop_tmr = 0;
  }
  if (last_playback_tmr > PLAYBACK_INTERVAL)
  {
    Serial.print("PLAYBACK_INTERVAL is : ");
    Serial.println(PLAYBACK_INTERVAL);
    playFile(audio_file_names[random(0, NUM_AUDIO_FILES)], 0.5);
    // digitalWrite(PWR_KILL_PIN, LOW); // kill the power I think...
    last_playback_tmr = 0;
    Serial.println("last_playback_tmr is reset now to 0");
  }
}

void activateFeedback(float amp, float dur)
{
  amp = amp * 0.125 * USER_CONTROL_PLAYBACK_GAIN;
  if (amp < 0.01)
  {
    amp = 0.01;
  }
  else if (amp > 0.015)
  {
    amp = 0.015;
  }
  Serial.print("activateFeedback(amp, dur) called: ");
  Serial.print(amp);
  Serial.print("\t");
  Serial.println(dur);

  // connect amp to audio output
  for (float i = 0.0; i < amp; i = i + 0.001)
  {
    updateLegatusPassiveLEDs();
    Serial.print("gain is : ");
    Serial.println(i);
    amp3.gain(i);
    delay(10);
  }

  elapsedMillis t;
  while (t < dur)
  {
    /*
    amp = amp * 0.125 * USER_CONTROL_PLAYBACK_GAIN;
    if (amp < 0.01){
      amp = 0.01;
    } else if (amp > 0.04) {
      amp = 0.04;
    }
    uimanager.update();
    amp3.gain(amp);
    */
    updateLegatusPassiveLEDs();
    delay(10);
  }

  delay(dur);

  for (float i = amp; i > 0.0; i = i - 0.001)
  {
    updateLegatusPassiveLEDs();
    amp3.gain(i);
    delay(10);
    Serial.print("gain is : ");
    Serial.println(i);
  }
  amp3.gain(0.0);
}

void legatusLoopFeedback()
{
  updateLegatusPassiveLEDs();
  uimanager.update();
  int avg_time = 1000 * 60 * 2.0;
  float temp = weather_manager.getTemperature();
  float humid = weather_manager.getHumidity();
  PLAYBACK_INTERVAL = avg_time - (temp * (avg_time / 40)) + (humid * (avg_time / 100)); // uint16_t t = random(45, 150);
  if (loop_tmr > loop_print_delay)
  {
    Serial.println(PLAYBACK_INTERVAL - last_playback_tmr);
    loop_tmr = 0;
  }
  if (last_playback_tmr > PLAYBACK_INTERVAL)
  {
    Serial.print("PLAYBACK_INTERVAL is : ");
    Serial.println(PLAYBACK_INTERVAL);
    float factor = random(1, 8);
    float base_freq = factor * ((temp * 2) + (humid * 2));
    activateFeedback(1.0, PLAYBACK_INTERVAL);
    // turn on oscillator
    // let it run for a period of time
    // turn off oscillator

    // digitalWrite(PWR_KILL_PIN, LOW); // kill the power I think...
    last_playback_tmr = 0;
    Serial.println("last_playback_tmr is reset now to 0");
  }
}

void activateFM(int t, float freq, float amp)
{
  if (amp > 1.0)
  {
    amp = 1.0;
  }
  Serial.print("activateFM called (len, freq, amp): ");
  Serial.print(t);
  Serial.print("\t");
  Serial.print(freq);
  Serial.print("\t");
  Serial.println(amp);

  amp3.gain(STARTING_GAIN * amp * USER_CONTROL_PLAYBACK_GAIN);
  sine_fm.frequency(freq);
  for (float i = 0.0; i < 1.0; i = i + 0.0001)
  {
    // make sure we dont pass an amp over 1.0
    if (i > 1.0)
    {
      i = 1.0;
    }
    updateLegatusPassiveLEDs();
    uimanager.update();
    sine_fm.amplitude(i * USER_CONTROL_PLAYBACK_GAIN);
    delay(5);
  }
  for (float i = 0; i < 20; i++)
  {
    updateLegatusPassiveLEDs();
    freq += random(freq * -0.01, freq * 0.01);
    Serial.print("freq:");
    Serial.println(freq);
    sine_fm.frequency(freq);
    float d_time = t * 0.05;
    elapsedMillis m;
    while (m < d_time)
    {
      uimanager.update();
      sine_fm.amplitude(USER_CONTROL_PLAYBACK_GAIN);
      // amp3.gain(STARTING_GAIN * amp * USER_CONTROL_PLAYBACK_GAIN);
      updateLegatusPassiveLEDs();
      delay(20);
    }
  }
  for (float i = 1.0; i > 0.0; i = i - 0.0001)
  {
    updateLegatusPassiveLEDs();
    uimanager.update();
    sine_fm.amplitude(i * USER_CONTROL_PLAYBACK_GAIN);
    delay(10);
  }
  sine_fm.amplitude(0.0);
  amp3.gain(STARTING_GAIN);
}

int avg_time = 1000 * 60 * 2.0;
void legatusLoopFM()
{
  updateLegatusPassiveLEDs();
  uimanager.update();
  float temp = weather_manager.getTemperature();
  float humid = weather_manager.getHumidity();
  PLAYBACK_INTERVAL = avg_time - (temp * (avg_time / 40)) + (humid * (avg_time / 100)); // uint16_t t = random(45, 150);
  if (loop_tmr > loop_print_delay)
  {
    Serial.println(PLAYBACK_INTERVAL - last_playback_tmr);
    loop_tmr = 0;
  }
  if (last_playback_tmr > PLAYBACK_INTERVAL)
  {
    avg_time = 1000 * 60 * random(1, 4);
    Serial.print("PLAYBACK_INTERVAL is : ");
    Serial.println(PLAYBACK_INTERVAL);
    float factor = random(1, 16);
    float base_freq = factor * ((temp * 2) + (humid * 2));
    float amp = 0.5 + random(0, 1000) / 1000;
    activateFM(PLAYBACK_INTERVAL, base_freq, amp);
    // turn on oscillator
    // let it run for a period of time
    // turn off oscillator

    // digitalWrite(PWR_KILL_PIN, LOW); // kill the power I think...
    last_playback_tmr = 0;
    Serial.println("last_playback_tmr is reset now to 0");
  }
}

#endif // ARTEFACT_TYPE == LEGATUS (for all types)

#if SD_PRESENT
void initSD()
{
  SPI.setMOSI(SD_MOSI); // Audio shield has MOSI on pin 7
  SPI.setSCK(SD_CLK);   // Audio shield has SCK on pin 14
  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  if (!card.init(SPI_HALF_SPEED, SD_CS))
  {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    return;
  }
  else
  {
    Serial.println("Wiring is correct and a card is present.");
  }

  // print the type of card
  Serial.print("\nCard type: ");
  switch (card.type())
  {
  case SD_CARD_TYPE_SD1:
    Serial.println("SD1");
    break;
  case SD_CARD_TYPE_SD2:
    Serial.println("SD2");
    break;
  case SD_CARD_TYPE_SDHC:
    Serial.println("SDHC");
    break;
  default:
    Serial.println("Unknown");
  }

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card))
  {
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    return;
  }

  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  Serial.print("\nVolume type is FAT");
  Serial.println(volume.fatType(), DEC);
  Serial.println();

  volumesize = volume.blocksPerCluster(); // clusters are collections of blocks
  volumesize *= volume.clusterCount();    // we'll have a lot of clusters
  if (volumesize < 8388608ul)
  {
    Serial.print("Volume size (bytes): ");
    Serial.println(volumesize * 512); // SD card blocks are always 512 bytes
  }
  Serial.print("Volume size (Kbytes): ");
  volumesize /= 2;
  Serial.println(volumesize);
  Serial.print("Volume size (Mbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);

  Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);

  // list all files in the card with date and size
  root.ls(LS_R | LS_DATE | LS_SIZE);

  // list the length of all audio files
}
#endif // SD card related functions

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
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

  for (int i = 0; i < num_active_led_channels; i++)
  {
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

    neos[i].setOnsetColors(ONSET_RED, ONSET_GREEN, ONSET_BLUE);

    neos[i].setFlashBehaviour(FLASH_DOMINATES);
    Serial.print("NeoGroup flash_behaviour is set to                : \t");
    Serial.println(FLASH_DOMINATES);

    neos[i].changeMapping(LED_MAPPING_MODE);
    Serial.print("LED_MAPPING_MODE is set to                        : \t");
    Serial.println(LED_MAPPING_MODE);

    ///////////////////////////// SD Card //////////////////////////////////
#if SD_PRESENT
    neos[i].colorWipe(12, 0, 0, 1.0);
    initSD();
#endif

#if ARTEFACT_TYPE == SPECULATOR
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
  neos[0].colorWipe(12, 12, 0, 1.0);
  printMinorDivide();
  Serial.println("initalising the weather manager");
  weather_manager.init();
  weather_manager.setPrintReadings(P_WEATHER_MANAGER_READINGS);
  Serial.println("finished initalising the weather manager");
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

  ///////////////////////// Artefact Specific Logic ////////////////////////////
#if ARTEFACT_TYPE == SPECULATOR
  speculatorSetup();
#elif ARTEFACT_TYPE == EXPLORATOR
  exploratorSetup();
#elif ARTEFACT_TYPE == LEGATUS
  legatusSetup();
#endif

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
#if ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE == CLAPPER_BODY
#else
      uimanager.update();
#endif // only update UIManager if it exists
    }
  }
  neos[0].colorWipe(0, 5, 0, 1.0);
  printMajorDivide("Now starting main() loop");
}

int determineLegatusBehavior()
{
  if (FM_FEEDBACK_ACTIVE == true)
  {
    return FM_FEEDBACK_MODE;
  }
  else if (FEEDBACK_ACTIVE == true)
  {
    return FEEDBACK_MODE;
  }
  else if (PLAYBACK_ACTIVE == true)
  {
    return PLAYBACK_MODE;
  }
  else if (MATCH_PITCH_ACTIVE == true)
  {
    return MATCH_PITCH_MODE;
  }
  return 0;
}

void loop()
{
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
  }

  ///////////////// User Controls ////////////////////////////
#if ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE == CLAPPER_BODY
#else
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
#endif

  ///////////////// WeatherManager ///////////////////////////
#if WEATHER_MANAGER_PRESENT == true
  if (weather_manager.update() && P_WEATHER_MANAGER_READINGS)
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
  else
  {
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
#elif ARTEFACT_TYPE == EXPLORATOR && BODY_TYPE == MB_BODY
    exploratorMBLoop();
#elif ARTEFACT_TYPE == EXPLORATOR
  exploratorLoop();
#elif ARTEFACT_TYPE == LEGATUS
  uimanager.update();
  ARTEFACT_BEHAVIOUR = determineLegatusBehavior();
  if (ARTEFACT_BEHAVIOUR == PLAYBACK_MODE)
  {
    legatusLoopSamplePlayback();
  }
  else if (ARTEFACT_BEHAVIOUR == FEEDBACK_MODE)
  {
    legatusLoopFeedback();
  }
  else if (ARTEFACT_BEHAVIOUR == FM_FEEDBACK_MODE)
  {
    legatusLoopFM();
  }
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
  if (millis() > first_dominate_channel_calculation && dominate_channel_update > dominate_channel_update_rate)
  {
    dominate_channel_update = 0;
    feature_collector.calculateDominateChannel(fft_manager);
  }
#endif

#if P_AUDIO_USAGE_MAX == true
  printAudioUsage();
#endif
}
