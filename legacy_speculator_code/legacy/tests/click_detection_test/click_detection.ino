#include <WS2812Serial.h>
#include <elapsedMillis.h>
const int numled = 12;
const int pin = 5;

// Usable pins:
//   Teensy LC:   1, 4, 5, 24
//   Teensy 3.2:  1, 5, 8, 10, 31   (overclock to 120 MHz for pin 8)
//   Teensy 3.5:  1, 5, 8, 10, 26, 32, 33, 48
//   Teensy 3.6:  1, 5, 8, 10, 26, 32, 33

byte drawingMemory[numled*3];         //  3 bytes per LED
DMAMEM byte displayMemory[numled*12]; // 12 bytes per LED

WS2812Serial leds(numled, displayMemory, drawingMemory, pin, WS2812_GRB);

#define RED    0xFF0000
#define GREEN  0x00FF00
#define BLUE   0x0000FF
#define YELLOW 0xFFFF00
#define PINK   0xFF1088
#define ORANGE 0xE05800
#define WHITE  0xFFFFFF

// Less intense...
/*
#define RED    0x160000
#define GREEN  0x001600
#define BLUE   0x000016
#define YELLOW 0x101400
#define PINK   0x120009
#define ORANGE 0x100400
#define WHITE  0x101010
*/

// to keep track of the flash
uint16_t flash_delay = 0;
bool flash_on = false;
elapsedMillis last_click_flash;
unsigned long click_length = 20;

////////////////////////////////////////////////////////////////////////////////

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            i2s1;           //xy=55,122.00000190734863
AudioInputUSB            usb2;           //xy=55,258.00000190734863
AudioAmplifier           click_input_amp;           //xy=216,116.00000190734863
AudioAnalyzeRMS          rms_input;           //xy=230.00000381469727,258.99999046325684
AudioAnalyzePeak         peak_input;          //xy=234,290.99999141693115
AudioFilterBiquad        click_biquad;        //xy=381.00000762939453,116.00000190734863
AudioAmplifier           click_mid_amp;           //xy=464,50
AudioFilterBiquad        click_biquad2;        //xy=568.0000228881836,116.00000190734863
AudioAmplifier           click_post_amp;           //xy=677.2000198364258,50
AudioOutputUSB           usb1;           //xy=857.0000228881836,172.00000190734863
AudioAnalyzeRMS          click_rms;           //xy=865.200023651123,28.000003814697266
AudioAnalyzePeak         click_peak;          //xy=869.2000198364258,60.000003814697266
AudioConnection          patchCord1(i2s1, 0, rms_input, 0);
AudioConnection          patchCord2(i2s1, 0, peak_input, 0);
AudioConnection          patchCord3(i2s1, 0, click_input_amp, 0);
AudioConnection          patchCord4(click_input_amp, click_biquad);
AudioConnection          patchCord5(click_biquad, click_mid_amp);
AudioConnection          patchCord6(click_mid_amp, click_biquad2);
AudioConnection          patchCord7(click_biquad2, click_post_amp);
AudioConnection          patchCord8(click_post_amp, click_rms);
AudioConnection          patchCord9(click_post_amp, click_peak);
AudioConnection          patchCord10(click_post_amp, 0, usb1, 0);
AudioConnection          patchCord11(click_post_amp, 0, usb1, 1);
// GUItool: end automatically generated code

/*
   TODO
   -----------
   Map the rms of the "song" spectrum to LED brightness
   When click occurs, change LED color to Blue/White instead of red

   Determine how to detect clicks (perhaps by comparing either the "peak" of that channel
   or perhaps by comparing its peak with another band?
*/

double click_rms_delta = 0.0;
double click_rms_val = 0.0;
double last_click_rms_val = 0.0;
double click_peak_val = 0.0;

uint8_t song_rms_weighted = 0; // 0 -255 depending on the RMS of the song band...

elapsedMillis lastUsagePrint = 0;
#define AUDIO_MEMORY 16

void setup() {
  click_biquad.setHighpass(0, 1200, 0.75);
  click_biquad.setHighpass(1, 1200, 0.75);
  click_biquad.setHighpass(2, 1200, 0.75);
  click_biquad.setLowShelf(3, 1200, -12);
  
  click_biquad2.setLowpass(0, 2500, 0.75);
  click_biquad2.setLowpass(1, 2500, 0.75);
  click_biquad2.setLowpass(2, 2500, 0.75);
  click_biquad2.setHighShelf(3, 2500, -12);

  /*
  song_biquad.setLowpass(0, 16000, 0.75);
  song_biquad.setLowpass(1, 14000, 0.75);
  song_biquad.setLowpass(2, 16000, 0.75);
  song_biquad.setLowpass(3, 14000, 0.75);

  song_biquad2.setHighpass(0, 3500, 0.75);
  song_biquad2.setHighpass(1, 3000, 0.75);       
  song_biquad2.setHighpass(2, 3500, 0.75);
  song_biquad2.setHighpass(3, 3000, 0.75);


    click_biquad2.setLowShelf(0, 3000, 1.0, 1.0);
    click_biquad2.setLowShelf(1, 2000, 1.0, 1.0);
    click_biquad2.setHighShelf(2, 1200, 1.0, 1.0);
    click_biquad2.setHighShelf(3, 1000, 1.0, 1.0);
  */

  Serial.begin(57600);
  AudioMemory(AUDIO_MEMORY);
  click_input_amp.gain(2);
  click_mid_amp.gain(2);
  click_post_amp.gain(2);
  // Start the LEDs
  leds.begin();
  delay(1000);
  Serial.println("Finshed Setup Loop");
  Serial.println("------------------");
  delay(500);
}

void printClickStats() {
  if (click_rms.available() && click_peak.available()) {
    Serial.print("Click | rms: ");
    Serial.print(click_rms_val * 1000);
    Serial.print(" delta: ");
    Serial.print(click_rms_delta * 1000);
    Serial.print("\t peak: ");
    Serial.println(click_peak_val * 1000);
  }
}

void printAudioUsage() {
  // print the audio usage every second or so
  if (lastUsagePrint > 5000) {
    Serial.print("memory usage: ");
    Serial.print(AudioMemoryUsageMax());
    Serial.print(" out of ");
    Serial.println(AUDIO_MEMORY);
    lastUsagePrint = 0;
    AudioMemoryUsageMaxReset();
  }
}

void calculateAudioFeatures() {
  if (click_rms.available()) {
    last_click_rms_val = click_rms_val;
    click_rms_val = click_rms.read();
    click_rms_delta = last_click_rms_val - click_rms_val;
  }

  if ( click_peak.available() ) {
    click_peak_val = click_peak.read();
  }

  if (click_rms_delta > 0.01) {
    Serial.print("-------------- CLICK DETECTED --------------------- ");
    // If a click is detected set the flash timer to 20ms, if flash timer already set increase count by 1
    if (flash_delay <= 0) {
      flash_delay += click_length;
    } else {
      flash_delay++;
    }
    Serial.println(flash_delay);
  }
}

void clickFlashes() {
  // check to see if light is already on
  // TODO ////
  if (flash_delay > 0) { // if there is time remaining in the flash
    if (flash_on == false) { //and the light is not currently on
      flash_on = true; // turn the light on along with the flag
      colorWipe(WHITE, 0);
      last_click_flash = 0; // reset the elapsed millis variable as the light was just turned on
      Serial.println("Turned WHITE flash on");
    } else {
      // if the light is already on
      // subtract the number of ms which have gone by since the last check
      // TODO optimize below lines
      flash_delay = flash_delay - last_click_flash;
      if (flash_delay < 0) {flash_delay = 0;}
            
      last_click_flash = 0;
      if (flash_delay == 0) {
        // if there is no more time left then turn off the light and turn flag to false
        flash_on = false;
        colorWipe(0x000000, 0);
        Serial.println("Turned WHITE flash off");
      }
    }
  } else {
    last_click_flash = 0;
  }
}

uint8_t calcSongRMSWeighted() {
  uint8_t song_rms_weighted = 10;
  // TODO take the Peak signal level scale it from 0-255
  // conduct further scaling depending on the brightness detected from VEML7700
  return song_rms_weighted;
}

void songDisplay() {
  // if the click WHITE light is on, then ignore the song rms
  if (flash_on == true) {
    return;
  }
  // if not then adjust the brightness of the LEDs according to the RMS
  uint32_t color = 0;
  // TODO add compensation to the brightness according to the ambiant brightness
  // TODO calculate the song_rms_weighted
  color = calcSongRMSWeighted() << 16; // TODO make sure this shifting is correct
  colorWipe(color, 0);
}

void loop() {
  calculateAudioFeatures();
  clickFlashes();
  songDisplay();
  // printClickStats();
  // put your main code here, to run repeatedly:
  /*
    if ( post_gain_rms.available() && post_gain_peak.available()) {
    Serial.print(rms_input.read());
    Serial.print(" - ");
    Serial.println(peak_input.read());
    Serial.print(post_gain_rms.read());
    Serial.print(" - ");
    Serial.println(post_gain_peak.read());
    Serial.println("---------------------------");
    }
  */
  printAudioUsage(); // print audio usage every five seconds
}


void colorWipe(int color, int wait) {
  for (int i=0; i < leds.numPixels(); i++) {
    leds.setPixel(i, color);
    leds.show();
    delayMicroseconds(wait);
  } 
}
