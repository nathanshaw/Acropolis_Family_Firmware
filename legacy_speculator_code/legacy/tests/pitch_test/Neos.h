#ifndef __NEOS_H__
#define __NEOS_H__
/*
   This is a general purpose file which contains helper functions for dealing with using NeoPixel LEDs
   with the WS2812Serial library
*/
/*
  // all these are declared as an array just in case dual-sided lights are used
  int16_t remaining_flash_delay[2];           // to keep track of the flash
  bool flash_on[2] = {false, false};              // to keep track of the flash


  elapsedMillis last_click_flash[2]; // ensures that the LEDs are not turned on when they are in shutdown mode to allow for an accurate LUX reading
  elapsedMillis led_shdn_timer[2];
  bool leds_on[2] = {false, false};               // set to false if the LEDs are turned off and true if the leds are on...
  elapsedMillis led_off_len[2];          // keeps track of how long the LEDs are turned off, it will reset when colorWipe is called with a color of 0

  // to keep track of how often the leds are on/off
  elapsedMillis led_switch_timer[2];// each time color wipe is called this value is reset
  long led_on_times[2] = {1, 1};
  long led_off_times[2] = {1, 1};
  double led_on_ratio[2];
*/
#include <WS2812Serial.h>
#include "Configuration.h"
#include <PrintUtils.h>
// #include <elapsedMillis.h>

uint32_t packColors(uint8_t red, uint8_t green, uint8_t blue, double scaler) {
  // TODO change code to account for front and back sides
  uint32_t color = 0;
  // Serial.print("red: ");
  // Serial.print(red);
  red = red * scaler;
  // Serial.print("\t");
  // Serial.println(red);
  green = green * scaler;
  blue = blue * scaler;
  red = min(red, MAX_BRIGHTNESS);
  green = min(green, MAX_BRIGHTNESS);
  blue = min(blue, MAX_BRIGHTNESS);
  color = (red << 16) + (green << 8) + (blue);
  return color;
}

class NeoGroup {
    // class to control a sub group of neopixels easily
    // init should cover
  public:
    NeoGroup(WS2812Serial *neos, int start_idx, int end_idx, String n, long f_min, long f_max);

    // setting and getting functions
    void setFlashOn(bool val) {
      flash_on = val;
    };
    bool getFlashOn() {
      return flash_on;
    };
    bool getLedsOn() {
      return leds_on;
    };
    long getOnOffLen() {
      return on_off_len;
    };
    long getShdnTimer() {
      return shdn_timer;
    };
   
    unsigned long getShdnLen();

    bool isInShutdown() {
      if (shdn_timer < shdn_len) {
        return true;
      }
      return false;
    };

    void updateAvgBrightnessScaler();
    void resetAvgBrightnessScaler();

    void setBrightnessScaler(double scaler) {
      brightness_scaler = scaler;
      updateAvgBrightnessScaler();
    };

    double getOnRatio() {
      return on_ratio;
    };

    double getBrightnessScaler() {
      return brightness_scaler;
    };
    double getAvgBrightnessScaler();
    double getAvgBrightness(String type);
    long getRemainingFlashDelay() {
      return remaining_flash_delay;
    };
    void addToRemainingFlashDelay(long i) {
      remaining_flash_delay += i;
      if (remaining_flash_delay > flash_max_time) {
        remaining_flash_delay = flash_max_time;
      }
    };
    void setRemainingFlashDelay(long d) {
      remaining_flash_delay = d;
    };

    void colorWipe(uint8_t red, uint8_t green, uint8_t blue, double bs);
    void colorWipe(uint8_t red, uint8_t green, uint8_t blue);
    void colorWipe(int colors);
    // void wipeAll(uint8_t red, uint8_t green, uint8_t blue);
    bool flashOn(uint8_t red, uint8_t green, uint8_t blue); // perhaps add time for flash to flashOn
    bool flashOn();
    void flashOff();
    void updateFlash();

    void printGroupConfigs();

    bool shutdown(long len);
    void powerOn();
    void updateFlashColors(uint8_t red, uint8_t green, uint8_t blue);

    double getFlashPerMinuteAvg();
    void resetFlashPerMinuteAvg();

  private:
    // related to the flash command
    uint8_t flash_red = 0;
    uint8_t flash_green = 0;
    uint8_t flash_blue = 255;
    long remaining_flash_delay = 0;// negative values expected, can not be a variable
    
    bool flash_on = false;
    long flash_min_time;
    long flash_max_time;
    long num_flashes;
    elapsedMillis fpm_timer;

    // related to auto-calibration and datalogging
    bool update_on_off_ratios = UPDATE_ON_OFF_RATIOS;
    String gname;

    // controling leds
    WS2812Serial *leds;
    int idx_start;
    int idx_end;
    int num_pixels;

    elapsedMillis shdn_timer; // if this is below a certain threshold then shutdown everything
    unsigned long shdn_len = 0;

    bool leds_on = false;

    elapsedMillis last_flash; // the last time a flash message was received
    elapsedMillis on_off_len; // this is reset every time the leds shutdown or turn on (length of time on or off)
    elapsedMillis last_flash_update;

    long on_time = 1;
    long off_time = 1;
    double on_ratio = 0.5;
    double brightness_scaler = 1.0;
    double brightness_scaler_total;
    double brightness_scaler_changes;
    // functions

    void updateOnRatio(int color);
    void updateBrightnessScalerTotals();
    void resetOnOffRatioCounters();
};

void NeoGroup::resetFlashPerMinuteAvg() {
  num_flashes = 0;
  fpm_timer = 0;
}

double NeoGroup::getFlashPerMinuteAvg() {
  return (double)num_flashes / (double)fpm_timer;
}

NeoGroup::NeoGroup(WS2812Serial *neos, int start_idx, int end_idx, String id, long f_min, long f_max) {
  // todo
  flash_min_time = f_min;
  flash_max_time = f_max;
  num_pixels = end_idx - start_idx + 1;
  idx_start = start_idx;
  idx_end = end_idx;
  leds = neos;
  gname = id;
}

bool NeoGroup::shutdown(long len) {
  // return 0 if lux shutdown not a success, 1 if it is
  if (!isInShutdown()) {
    dprint(PRINT_LUX_DEBUG,millis());dprint(PRINT_LUX_DEBUG, "\tSHUTTING DOWN GROUP ");
    dprintln(PRINT_LUX_DEBUG, gname);
    shdn_len = len;
    colorWipe(0, 0, 0);
    shdn_timer = 0;
    leds_on = false;
    return 0;
  }
  return 1;
}

void NeoGroup::powerOn() {
  dprintln(PRINT_LUX_DEBUG, "POWER ON MESSAGE RECEIVED");
  shdn_timer += shdn_len;
}

unsigned long NeoGroup::getShdnLen() {
  if (shdn_timer <= shdn_len) {
    return shdn_timer;
  } else  {
    return 0;
  }
}

void NeoGroup::updateFlashColors(uint8_t red, uint8_t green, uint8_t blue) {
  flash_red = red;
  flash_green = green;
  flash_blue = blue;
}

void NeoGroup::colorWipe(uint8_t red, uint8_t green, uint8_t blue) {
  colorWipe(red, green, blue, brightness_scaler);
}

void NeoGroup::colorWipe(uint8_t red, uint8_t green, uint8_t blue, double bs) {
  // TODO this logic is broken...
  // Serial.println("Starting colorWipe in NeoGroup");
  int colors = packColors(red, green, blue, bs);
  if (update_on_off_ratios) {
    updateOnRatio(colors);
  }
  if (shdn_timer < shdn_len) {
    // if the LEDs are in shutdown mode than simply exit without changing the LEDs
    dprint(PRINT_LED_DEBUG, "colorWipe returning due to shdn_timer : "); dprintln(PRINT_LED_DEBUG, shdn_timer);
    return;
  }
  if (flash_on == true) {
    dprintln(PRINT_LED_DEBUG, "Flash blocked colorWipe");
    return;
  }
  // Serial.print("num_pixels :\t");Serial.println(num_pixels);
  for (int i = 0; i < num_pixels; i++) {
    leds->setPixel(idx_start + i, colors);
    // Serial.print(i);printTab();Serial.print(idx_start + i);printTab();
  }
  leds->show();
  // Serial.println(" displaying");
  // TODO how toa dapt this to new array variables?
  // if the LEDs are on set "led_on" to true, otherwise turn "led_on" to false
  // also reset led_off_len if the leds  were just turned off
  if (colors == 0) {
    leds_on = false;
  } else {
    leds_on = true;
  }
}

void NeoGroup::colorWipe(int colors) {
  // TODO this logic is broken...
  Serial.println("WARNING COLOR WIPE DOES NOT DO ITS OWN BRIGHTNESS SCALING WHEN SINGLE INT IS PASSED TO IT");
  updateOnRatio(colors);
  if (shdn_timer < shdn_len) {
    // if the LEDs are in shutdown mode than simply exit without changing the LEDs
    return;
  }
  for (int i = 0; i < num_pixels; i++) {
    leds->setPixel(idx_start + i, colors);
    dprint(PRINT_LED_DEBUG, idx_start + i); dprintTab(PRINT_LED_DEBUG);
  }
  leds->show();
  // TODO how toa dapt this to new array variables?
  // if the LEDs are on set "led_on" to true, otherwise turn "led_on" to false
  // also reset led_off_len if the leds  were just turned off
  if (colors == 0) {
    leds_on = false;
  } else {
    leds_on = true;
  }
}

////////////// General Purpose Helper Functions /////////////////////////
void NeoGroup::flashOff() {
  // if the flash is allowed to be turned off
  if (remaining_flash_delay <= 0) {
    dprint(PRINT_CLICK_DEBUG, gname);
    dprint(PRINT_CLICK_DEBUG, " FlashOff : ");
    dprintln(PRINT_CLICK_DEBUG, last_flash);
    flash_on = false;
    leds_on = false;
    colorWipe(0, 0, 0);
    remaining_flash_delay = 0;
    last_flash = 0;
  }
}

bool NeoGroup::flashOn(uint8_t red, uint8_t green, uint8_t blue) {
  // if it has been long enough since the last flash occured
  if (last_flash > FLASH_DEBOUNCE_TIME) {
    dprint(PRINT_CLICK_DEBUG, "FLASH ON");
    if (red + green + blue > 0 && shdn_timer > shdn_len) {
      // if a flash is not currently on
      if ( (flash_on == false) || (leds_on == false) ) {
        remaining_flash_delay = flash_min_time;
        colorWipe(red, green, blue, brightness_scaler * 1.5); // has to be on first as flash_on will block the colorWipe
        flash_on = true; // turn the light on along with the flag
        leds_on = true;
        last_flash = 0; // reset the elapsed millis variable as the light was just turned on
        dprint(PRINT_CLICK_DEBUG, " Flashed "); dprintln(PRINT_CLICK_DEBUG, remaining_flash_delay);
        num_flashes++;
      } else { // if a flash is on then increment the remaining_flash_Delay
        addToRemainingFlashDelay(1);
        if (remaining_flash_delay > flash_max_time) {
          remaining_flash_delay = flash_max_time;
        }
      }
      return true;
    }
  } else {
    dprintln(PRINT_CLICK_DEBUG, "Flash skipped due to FLASH_DEBOUNCE_TIME");
    return false;
  }
}

bool NeoGroup::flashOn() {
  return flashOn(flash_red, flash_green, flash_blue);
}

void NeoGroup::updateFlash() {
  // if there is time remaining in the flash it either needs to be turned on or the timer needs to increment
  if (remaining_flash_delay > 0) {
    dprint(PRINT_CLICK_DEBUG, "flash delay "); dprint(PRINT_CLICK_DEBUG, gname); dprint(PRINT_CLICK_DEBUG, " : ");
    dprint(PRINT_CLICK_DEBUG, remaining_flash_delay); dprintTab(PRINT_CLICK_DEBUG);
    dprint(PRINT_CLICK_DEBUG, last_flash_update); dprintTab(PRINT_CLICK_DEBUG);
    if (flash_on < 1) { //and the light is not currently on
      dprintln(PRINT_CLICK_DEBUG, "-- Turning the Flash ON --");
      flashOn(flash_red, flash_green, flash_blue);// flash on
    }
    else {
      // if the light is already on subtract the number of ms which have gone by since the last check
      // Serial.print("last_click_flash :"); Serial.println(last_click_flash[i]);
      // Serial.print("remaining_flash_delay["); Serial.print(i); Serial.print("] :\t");
      // Serial.print(remaining_flash_delay[i]); Serial.print("\t");
      remaining_flash_delay = remaining_flash_delay - last_flash_update;
      remaining_flash_delay = max(remaining_flash_delay, 0);
      dprintln(PRINT_CLICK_DEBUG, remaining_flash_delay);
      if (remaining_flash_delay == 0) {
        dprint(PRINT_CLICK_DEBUG, "Click time over, turning off flash "); dprintln(PRINT_CLICK_DEBUG, gname);
        flashOff(); // turn off the NeoPixels
      }
    }
  }
  // this is so the timer does not get reset if it has been running for less than one ms
  if (last_flash_update != 0) {
    last_flash_update = 0;
  }
}

void NeoGroup::updateAvgBrightnessScaler() {
  brightness_scaler_total += brightness_scaler;
  brightness_scaler_changes++;
}

double NeoGroup::getAvgBrightnessScaler() {
  return brightness_scaler_total / brightness_scaler_changes;
}

void NeoGroup::resetAvgBrightnessScaler() {
  brightness_scaler_total = 0;
  brightness_scaler_changes = 0;
}

void NeoGroup::resetOnOffRatioCounters() {
  on_ratio = (double)on_time / (double)(on_time + off_time);
  on_time = 0;
  off_time = 0;
  dprintln(PRINT_LED_DEBUG, "reset the led on/off ratio counters");
}

// mode 0 is just front, mode 1 is just rear, mode 2 is both (using combined values?), mode 3 is both using independent values
void NeoGroup::updateOnRatio(int color) {
  // when color wipe is called it should take care of this for  us
  // to keep track of on/off times
  if (color > 0) {
    on_time += on_off_len;
  } else {
    off_time += on_off_len;
  }
  if (on_time > 0 && off_time > 0) {
    on_ratio = (double)on_time / (double)(on_time + off_time);
    on_off_len = 0;
  }
  dprint(PRINT_LED_DEBUG, "updated led on/off ratio "); dprint(PRINT_LED_DEBUG, " :\t");
  dprint(PRINT_LED_DEBUG, on_ratio); dprint(PRINT_LED_DEBUG, "\t=\t"); dprint(PRINT_LED_DEBUG, on_time);
  dprint(PRINT_LED_DEBUG, "\t"); dprintln(PRINT_LED_DEBUG, off_time);
}

#endif // __LEDS_H__
