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
  elapsedMillis led_off_len[2];          // keeps track of how uint32_t the LEDs are turned off, it will reset when colorWipe is called with a color of 0

  // to keep track of how often the leds are on/off
  elapsedMillis led_switch_timer[2];// each time color wipe is called this value is reset
  uint32_t led_on_times[2] = {1, 1};
  uint32_t led_off_times[2] = {1, 1};
  double led_on_ratio[2];
*/
#include <WS2812Serial.h>
#include "../Configuration.h"
#include "../Macros.h"
#include "../PrintUtils/PrintUtils.h"

#ifndef PRINT_CLICK_DEBUG
#define PRINT_CLICK_DEBUG 0
#endif

#ifndef PRINT_LED_DEBUG 
#define PRINT_LED_DEBUG 0
#endif

#ifndef PRINT_LED_ON_RATIO_DEBUG
#define PRINT_LED_ON_RATIO_DEBUG 0
#endif

#ifndef PRINT_COLOR_WIPE_DEBUG
#define PRINT_COLOR_WIPE_DEBUG 0
#endif

#ifndef FLASH_DEBOUNCE_TIME
#define FLASH_DEBOUNCE_TIME 50
#endif

#ifndef FLASH_DOMINATES
#define FLASH_DOMINATES 0
#endif

#ifndef PRINT_LUX_DEBUG 
#define PRINT_LUX_DEBUG 0
#endif

#ifndef UPDATE_ON_OFF_RATIOS 
#define UPDATE_ON_OFF_RATIOS 1
#endif

#ifndef MIN_BRIGHTNESS 
#define MIN_BRIGHTNESS 0
#endif

#ifndef MAX_BRIGHTNESS
#define MAX_BRIGHTNESS 255
#endif


uint32_t packColors(uint8_t &red, uint8_t &green, uint8_t &blue, double scaler) {
    /*
     * TODO write a function summary
     * 
     * */
  uint32_t color = 0;
  red = red * scaler;
  green = green * scaler;
  blue = blue * scaler;
  red = min(red, MAX_BRIGHTNESS);
  green = min(green, MAX_BRIGHTNESS);
  blue = min(blue, MAX_BRIGHTNESS);
  if (red < MIN_BRIGHTNESS) {red = 0;};
  if (green < MIN_BRIGHTNESS) {green = 0;};
  if (blue < MIN_BRIGHTNESS) {blue = 0;};
  color = (red << 16) + (green << 8) + (blue);
  return color;
}

// todo add on/off bools for the datalogging shiz
class NeoGroup {
    /* TODO add a class summary
     *
     */
  public:
    NeoGroup(WS2812Serial *neos, int start_idx, int end_idx, String _id, uint32_t f_min, uint32_t f_max);
    NeoGroup(WS2812Serial *neos, int start_idx, int end_idx, String _id);

    /////////////////////////////// Datatracking /////////////////////////////
    double red_avg, green_avg, blue_avg;
    void resetRGBAverageTracker();
    bool getLedsOn() {return leds_on;}; // for externally determining if the LEDs are currently on

    /////////////////////////////// Flashes //////////////////////////////////
    uint32_t getOnOffLen() {return on_off_len;};
    uint32_t getShdnTimer() {return shdn_timer;};
    uint32_t getNumFlashes() {return num_flashes;};
    void setFlashOn(bool val) {flash_on = val;};
    bool getFlashOn() {return flash_on;};
    uint32_t num_flashes = 0;// these are public so they can be tracked by the datalog manager
    uint32_t total_flashes = 0;
    double fpm;
    double getFPM();
    void resetFPM();
    void addToRemainingFlashDelay(long i);
    void setRemainingFlashDelay(long d);

    ////////////////////////////// SHDN Timer ///////////////////////////////
    uint32_t getShdnLen();
    bool isInShutdown();
    void updateAvgBrightnessScaler();
    void resetAvgBrightnessScaler();
    void setBrightnessScaler(double scaler) {brightness_scaler = scaler;updateAvgBrightnessScaler();};
    double getOnRatio() {return on_ratio;};
    double on_ratio = 0.5;

    ////////////////////////// Brightness Scaler //////////////////////////////
    double getBrightnessScaler() {return brightness_scaler;};
    double getAvgBrightnessScaler();
    double getAvgBrightness(String type);
    long getRemainingFlashDelay() {return remaining_flash_delay;};

    ///////////////////////////////// HSB Colors //////////////////////////////
    void updateHSB(double h, double s, double b);
    double getHue(){return hsb[0];};
    double getSat(){return hsb[1];};
    double getBright(){return hsb[2];};

    //////////////////////////////// ColorWipes ///////////////////////////////
    void colorWipe(uint8_t red, uint8_t green, uint8_t blue, double bs);
    void colorWipe(uint8_t red, uint8_t green, uint8_t blue);
    void colorWipe(int colors);
    void colorWipeHSB(double h, double s, double b);
    void colorWipeAdd(uint8_t red, uint8_t green, uint8_t blue);
    void colorWipeAdd(uint8_t red, uint8_t green, uint8_t blue, double bs);

    //////////////////////////////// Flashes //////////////////////////////////
    bool flashOn(uint8_t red, uint8_t green, uint8_t blue); // perhaps add time for flash to flashOn
    bool flashOn();
    void flashOff();
    void update();
    void setFlashColors(uint8_t red, uint8_t green, uint8_t blue);
    void powerOn(); // force a power on, overriding any shdn_timer

    /////////////////////////////// Printing /////////////////////////////////
    void printGroupConfigs();
    void printColors();

    ///////////////////////////// Misc //////////////////////////////////////
    bool shutdown(uint32_t len);
    String getName() { return id;};

    void setExtremeLuxShdn(bool e){extreme_lux_shdn = e;};
    bool getLuxShdn() { return extreme_lux_shdn;};

  private:
    double hsb[3]; // limited from 0 - 255
    uint8_t rgb[3]; // limited from 0.0 - 1.0
    double hue2rgb(double p, double q, double t);
    void RgbToHsb(uint8_t red, uint8_t green, uint8_t blue);
    void HsbToRgb(double hue, double saturation, double lightness);
    void updateColorLog(uint8_t red, uint8_t green, uint8_t blue);
    bool extreme_lux_shdn = false;

    ////////////////////////////// Flashes //////////////////////////////////
    uint8_t flash_red = 0;
    uint8_t flash_green = 0;
    uint8_t flash_blue = 255;
    long remaining_flash_delay = 0;// negative values expected, can not be a variable
    bool flash_on = false;
    long flash_min_time;  // how long is the shortest flash?
    long flash_max_time;  // how about the longest?
    elapsedMillis fpm_timer;

    // a linked neogroup for sharing flash info?

    //////////////////////////// Data Tracking /////////////////////////////
    uint64_t red_tot, green_tot, blue_tot;
    uint64_t red_readings, green_readings, blue_readings;
    // related to auto-calibration and datalogging
    bool update_on_off_ratios = UPDATE_ON_OFF_RATIOS;
    String id;

    //////////////////////////  LED Linking ////////////////////////////////
    WS2812Serial *leds;
    int idx_start;
    int idx_end;
    int num_pixels;
    elapsedMillis shdn_timer; // if this is below a certain threshold then shutdown everything
    uint32_t shdn_len = 0;
    bool leds_on = false;
    elapsedMillis last_flash; // the last time a flash message was received
    elapsedMillis on_off_len; // this is reset every time the leds shutdown or turn on (length of time on or off)
    elapsedMillis last_flash_update;

    //////////////////////// On Ratio /////////////////////////////////////
    uint64_t on_time = 1;
    uint64_t off_time = 1;
    uint32_t last_on_ratio_update = 0;
    void updateOnRatio(int color);

    /////////////////////// Brightness Scaler ////////////////////////////
    double brightness_scaler = 1.0;
    double brightness_scaler_total;
    double brightness_scaler_changes;
    void updateBrightnessScalerTotals();
    void resetOnOffRatioCounters();
};

void NeoGroup::resetFPM() {
  num_flashes = 0;
  fpm_timer = 0;
}

double NeoGroup::getFPM() {
  fpm = (double)num_flashes / (double)fpm_timer * 1000 * 60;
  return fpm;
}

NeoGroup::NeoGroup(WS2812Serial *neos, int start_idx, int end_idx, String _id) {
  // todo
  num_pixels = end_idx - start_idx + 1;
  idx_start = start_idx;
  idx_end = end_idx;
  leds = neos;
  id = _id;
}

NeoGroup::NeoGroup(WS2812Serial *neos, int start_idx, int end_idx, String _id, uint32_t f_min, uint32_t f_max) {
  // todo
  flash_min_time = f_min;
  flash_max_time = f_max;
  num_pixels = end_idx - start_idx + 1;
  idx_start = start_idx;
  idx_end = end_idx;
  leds = neos;
  id = _id;
}

bool NeoGroup::shutdown(uint32_t len) {
  // return 0 if lux shutdown not a success, 1 if it is
  if (!isInShutdown()) {
    dprint(PRINT_LUX_DEBUG,millis());dprint(PRINT_LUX_DEBUG, "\tSHUTTING DOWN GROUP ");
    dprintln(PRINT_LUX_DEBUG, id);
    shdn_len = len;
    colorWipe(0, 0, 0);
    shdn_timer = 0;
    leds_on = false;
    return 0;
  }
  return 1;
}

void NeoGroup::printColors() {
    Serial.print(id);
    Serial.print("  red  "); Serial.print(rgb[0]);
    Serial.print("\tgreen  "); Serial.print(rgb[1]);
    Serial.print("\tblue  ");
    Serial.print(rgb[2]);
    Serial.print("  =  "); Serial.print(rgb[0] + rgb[1] + rgb[0]);
    Serial.print("\thue "); Serial.print(hsb[0]);
    Serial.print("\tsat "); Serial.print(hsb[1]);
    Serial.print("\tbgt "); Serial.println(hsb[2]);
}

void NeoGroup::powerOn() {
  dprintln(PRINT_LUX_DEBUG, "POWER ON MESSAGE RECEIVED");
  shdn_timer += shdn_len;
}

uint32_t NeoGroup::getShdnLen() {
  if (shdn_timer <= shdn_len) {
    return shdn_len - shdn_timer;
  } else  {
    return 0;
  }
}

void NeoGroup::setFlashColors(uint8_t red, uint8_t green, uint8_t blue) {
  flash_red = red;
  flash_green = green;
  flash_blue = blue;
}

void NeoGroup::updateHSB(double h, double s, double b) {
    hsb[0] = h;
    hsb[1] = s;
    hsb[2] = b;
}

double NeoGroup::hue2rgb(double p, double q, double t)
{
	if (t < 0) t += 1;
	if (t > 1) t -= 1;
	if (t < 1 / 6.0) return p + (q - p) * 6 * t;
	if (t < 1 / 2.0) return q;
	if (t < 2 / 3.0) return p + (q - p) * (2 / 3.0 - t) * 6;
	return p;
}

void NeoGroup::HsbToRgb(double hue, double saturation, double lightness)
{
	double r, g, b = 0.0;

	if (saturation == 0)
	{
		r = g = b = lightness; // achromatic
	}
	else
	{
		auto q = lightness < 0.5 ? lightness * (1 + saturation) : lightness + saturation - lightness * saturation;
		auto p = 2 * lightness - q;
		r = hue2rgb(p, q, hue + 1 / 3.0);
		g = hue2rgb(p, q, hue);
		b = hue2rgb(p, q, hue - 1 / 3.0);
	}


	uint8_t r8  =  static_cast<uint8_t>(r * 255);
	uint8_t g8  = static_cast<uint8_t>(g * 255);
	uint8_t b8  = static_cast<uint8_t>(b * 255);
    updateColorLog(r8, g8, b8);
}

void NeoGroup::RgbToHsb(uint8_t red, uint8_t green, uint8_t blue)
{
/***************************************************
Copyright (c) 2017 Luis Llamas
(www.luisllamas.es)

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License
 ****************************************************/
	auto rd = static_cast<double>(red) / 255;
	auto gd = static_cast<double>(green) / 255;
	auto bd = static_cast<double>(blue) / 255;
	auto ma = max(rd, max(gd, bd));
    auto mi = min(rd, min(gd, bd));
	hsb[2]= ma;

	auto d = ma - mi;
	hsb[1] = ma == 0 ? 0 : d / ma;

	hsb[0] = 0;
	if (ma != mi)
	{
		if (ma == rd)
		{
			hsb[0] = (gd - bd) / d + (gd < bd ? 6 : 0);
		}
		else if (ma == gd)
		{
			hsb[0] = (bd - rd) / d + 2;
		}
		else if (ma == bd)
		{
			hsb[0] = (rd - gd) / d + 4;
		}
		hsb[0] /= 6;
	}
}

void NeoGroup::resetRGBAverageTracker() {
    red_tot = 0;
    green_tot = 0;
    blue_tot = 0;
    red_readings = 0;
    green_readings = 0;
    blue_readings = 0;
}

void NeoGroup::colorWipeHSB(double h, double s, double b) {
  updateHSB(h, s, b);
  HsbToRgb(h, s, b);
  colorWipe(rgb[0], rgb[1], rgb[2]);
}

void NeoGroup::colorWipeAdd(uint8_t red, uint8_t green, uint8_t blue, double bs) {
  red += rgb[0];
  red = constrain(red, 0, 255);
  green += rgb[1];
  green = constrain(green, 0 , 255);
  blue += rgb[2];
  blue = constrain(blue, 0, 255);
  colorWipe(red, green, blue, bs);
}


void NeoGroup::colorWipeAdd(uint8_t red, uint8_t green, uint8_t blue) {
  red += rgb[0];
  red = constrain(red, 0, 255);
  green += rgb[1];
  green = constrain(green, 0 , 255);
  blue += rgb[2];
  blue = constrain(blue, 0, 255);
  colorWipe(red, green, blue, brightness_scaler);
}

void NeoGroup::colorWipe(uint8_t red, uint8_t green, uint8_t blue) {
  colorWipe(red, green, blue, brightness_scaler);
}

void NeoGroup::updateColorLog(uint8_t red, uint8_t green, uint8_t blue) {

  // for each ms this value has been present add the value of the last color
  //  to the running total
  for (uint64_t i = 0; i < on_off_len; i++) {
      red_tot += rgb[0];
      green_tot += rgb[1];
      blue_tot += rgb[2];
      red_readings++;
      green_readings++;
      blue_readings++;
  }
  // then update the array which tracks the currently displayed color
  rgb[0] = red;
  rgb[1] = green;
  rgb[2] = blue;

  // update the average (perhaps should only do this every second or so?
  if (millis() + 1000 > last_on_ratio_update){
  red_avg = red_tot / (double)red_readings;
  green_avg = green_tot / (double)green_readings;
  blue_avg = blue_tot / (double)blue_readings;
  if (update_on_off_ratios) {
    updateOnRatio(red + green + blue);
  }
  last_on_ratio_update = millis();
  }
}

void NeoGroup::colorWipe(uint8_t red, uint8_t green, uint8_t blue, double bs) {
  // TODO this logic is broken...
  if (extreme_lux_shdn == true) {
    dprintln(PRINT_COLOR_WIPE_DEBUG, " colorWipe returning due extreme lux conditions");
    return;
  }
  if (shdn_timer < shdn_len) {
    // if the LEDs are in shutdown mode than simply exit without changing the LEDs
    dprint(PRINT_COLOR_WIPE_DEBUG, " colorWipe returning due to shdn_timer : "); 
    dprintln(PRINT_COLOR_WIPE_DEBUG, shdn_timer);
    updateColorLog(0, 0, 0);
    return;
  }

  if (flash_on == true) {
      // if the flash is on then add the flash colors to the color wipe colors
      if (FLASH_DOMINATES == false) {
          dprintln(PRINT_COLOR_WIPE_DEBUG, " Flash blocked colorWipe");
          red += flash_red;
          green += flash_green;
          blue += flash_blue;
      }
      else {
          dprintln(PRINT_COLOR_WIPE_DEBUG, " Flash blocked colorWipe");
          red += flash_red;
          green += flash_green;
          blue += flash_blue;
      }
  }

  int colors = packColors(red, green, blue, bs);

  dprint(PRINT_COLOR_WIPE_DEBUG, id);
  dprint(PRINT_COLOR_WIPE_DEBUG, " Starting colorWipe in NeoGroup - ");
  dprint(PRINT_COLOR_WIPE_DEBUG, " num_pixels: ");
  dprint(PRINT_COLOR_WIPE_DEBUG, num_pixels); 
  dprint(PRINT_COLOR_WIPE_DEBUG, " - ");

  for (int i = 0; i < num_pixels; i++) {
      leds->setPixel(idx_start + i, colors);
      dprint(PRINT_COLOR_WIPE_DEBUG, idx_start+i);
      dprint(PRINT_COLOR_WIPE_DEBUG, ": ");
      dprint(PRINT_COLOR_WIPE_DEBUG, colors); 
      dprint(PRINT_COLOR_WIPE_DEBUG, "\t");
  }
  leds->show();
  dprint(PRINT_COLOR_WIPE_DEBUG, " finished updating the neopixels");

  // if the LEDs are on set "led_on" to true, otherwise turn "led_on" to false
  // also reset led_off_len if the leds  were just turned off
  if (colors == 0) {
    leds_on = false;
    dprint(PRINT_COLOR_WIPE_DEBUG, " setting leds_on to false");
  } else {
    leds_on = true;
    dprint(PRINT_COLOR_WIPE_DEBUG, " setting leds_on to true");
  }
  updateColorLog(red, green, blue);
}

void NeoGroup::colorWipe(int colors) {
  // TODO this logic is broken...
  Serial.println("WARNING COLOR WIPE DOES NOT DO ITS OWN BRIGHTNESS SCALING WHEN SINGLE INT IS PASSED TO IT");
  if (shdn_timer < shdn_len) {
    // if the LEDs are in shutdown mode than simply exit without changing the LEDs
    return;
  }
  for (int i = 0; i < num_pixels; i++) {
    leds->setPixel(idx_start + i, colors);
    dprint(PRINT_COLOR_WIPE_DEBUG, idx_start + i); dprintTab(PRINT_COLOR_WIPE_DEBUG);
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
    dprint(PRINT_CLICK_DEBUG, id);
    dprint(PRINT_CLICK_DEBUG, " FlashOff : ");
    dprintln(PRINT_CLICK_DEBUG, last_flash);
    flash_on = false;
    leds_on = false;
    colorWipe(0, 0, 0);
    remaining_flash_delay = 0;
    // last_flash = 0;
  }
}

bool NeoGroup::flashOn(uint8_t red, uint8_t green, uint8_t blue) {
  // if it has been uint32_t enough since the last flash occured
  if (last_flash > FLASH_DEBOUNCE_TIME) {
    if (red + green + blue > 0 && shdn_timer > shdn_len) {
      // if a flash is not currently on
      if ( (flash_on == false) || (leds_on == false) ) {
        remaining_flash_delay = flash_min_time;
        colorWipe(red, green, blue, brightness_scaler * 1.5); // has to be on first as flash_on will block the colorWipe
        flash_on = true; // turn the light on along with the flag
        leds_on = true;
        last_flash = 0; // reset the elapsed millis variable as the light was just turned on
        num_flashes = num_flashes  + 1;
        total_flashes++;
        getFPM();
        dprint(PRINT_CLICK_DEBUG, id);
        dprint(PRINT_CLICK_DEBUG, " FLASH ON #");
        dprint(PRINT_CLICK_DEBUG, num_flashes);
        dprint(PRINT_CLICK_DEBUG, " Flashed "); dprint(PRINT_CLICK_DEBUG, remaining_flash_delay);
        dprint(PRINT_CLICK_DEBUG, " FPM "); dprintln(PRINT_CLICK_DEBUG, fpm);
      } else { // if a flash is on then increment the remaining_flash_Delay
        addToRemainingFlashDelay(1);
        if (remaining_flash_delay > flash_max_time) {
          remaining_flash_delay = flash_max_time;
        }
      }
      return true;
    }
  } else {
    dprint(PRINT_CLICK_DEBUG, "Flash skipped due to FLASH_DEBOUNCE_TIME : ");
    dprintln(PRINT_CLICK_DEBUG, last_flash);
  }
  return false;
}

bool NeoGroup::flashOn() {
  return flashOn(flash_red, flash_green, flash_blue);
}

//////////////////////////////////////////////////////
////////////// Update Methods/////////////////////////
//////////////////////////////////////////////////////
void NeoGroup::update() {
  // if there is time remaining in the flash it either needs to be turned
  // on or the timer needs to increment
  if (remaining_flash_delay > 0) {
    dprintln(PRINT_CLICK_DEBUG, "-------------------------------------");
    dprint(PRINT_CLICK_DEBUG, "flash delay "); dprint(PRINT_CLICK_DEBUG, id); dprint(PRINT_CLICK_DEBUG, " : ");
    dprint(PRINT_CLICK_DEBUG, remaining_flash_delay); dprintTab(PRINT_CLICK_DEBUG);
    dprint(PRINT_CLICK_DEBUG, last_flash_update); dprintTab(PRINT_CLICK_DEBUG);
    // if the flash is not currently on, turn the flash on
    if (flash_on < 1) { //and the light is not currently on
      dprintln(PRINT_CLICK_DEBUG, "-- Turning the Flash ON --");
      flashOn(flash_red, flash_green, flash_blue);// flash on
    }
    // if the flash is already on subtract from the timer
    else {
      dprintln(PRINT_CLICK_DEBUG, "- - - - - - - - - - - - - - - - - - -");
      dprint(PRINT_CLICK_DEBUG, "last_flash :\t"); dprintln(PRINT_CLICK_DEBUG, last_flash);
      dprint(PRINT_CLICK_DEBUG, "remaining_flash_delay "); 
      dprint(PRINT_CLICK_DEBUG, id); dprint(PRINT_CLICK_DEBUG, ":\t");
      dprint(PRINT_CLICK_DEBUG, remaining_flash_delay); dprint(PRINT_CLICK_DEBUG, "\t");
      remaining_flash_delay = remaining_flash_delay - last_flash_update;
      remaining_flash_delay = max(remaining_flash_delay, 0);
      dprintln(PRINT_CLICK_DEBUG, remaining_flash_delay);
      if (remaining_flash_delay == 0) {
        dprint(PRINT_CLICK_DEBUG, "Click time over, turning off flash "); dprintln(PRINT_CLICK_DEBUG, id);
        flashOff(); // turn off the NeoPixels
      }
    }
  }
  // this is so the timer does not get reset 
  // if it has been running for less than one ms
  if (last_flash_update != 0) {
    last_flash_update = 0;
    dprintln(PRINT_CLICK_DEBUG, "updated last_flash_upate to 0");
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
  dprint(PRINT_LED_ON_RATIO_DEBUG, "updated led on/off ratio "); dprint(PRINT_LED_ON_RATIO_DEBUG, " :\t");
  dprint(PRINT_LED_ON_RATIO_DEBUG, on_ratio); dprint(PRINT_LED_ON_RATIO_DEBUG, "\t=\t"); dprint(PRINT_LED_ON_RATIO_DEBUG, on_time);
  dprint(PRINT_LED_ON_RATIO_DEBUG, "\t"); dprintln(PRINT_LED_ON_RATIO_DEBUG, off_time);
}

void NeoGroup::addToRemainingFlashDelay(long i) {
  remaining_flash_delay += i;
  if (remaining_flash_delay > flash_max_time) {
    remaining_flash_delay = flash_max_time;
  }
};

void NeoGroup::setRemainingFlashDelay(long d) {
  remaining_flash_delay = d;
};

bool NeoGroup::isInShutdown() {
  if (shdn_timer < shdn_len) {
    return true;
  }
  return false;
};

#endif // __LEDS_H__
