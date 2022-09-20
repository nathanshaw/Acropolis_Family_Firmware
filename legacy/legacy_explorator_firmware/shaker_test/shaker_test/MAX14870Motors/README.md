# Arduino library for the Pololu Dual MAX14870 Motor Driver Shield

Version: 2.0.0 <br>
Release date: 2018-04-11 <br>
[![Build Status](https://travis-ci.org/pololu/dual-max14870-motor-shield.svg?branch=master)](https://travis-ci.org/pololu/dual-max14870-motor-shield) <br>
[www.pololu.com](https://www.pololu.com/)

## Summary

This is a library for the Arduino IDE that interfaces with the
[Pololu Dual MAX14870 Motor Driver Shield for Arduino](https://www.pololu.com/catalog/product/2519).
It makes it simple to drive two brushed, DC motors.

## Supported platforms

This library is designed to work with the Arduino IDE versions 1.8.x
or later; we have not tested it with earlier versions.  This library
should support any Arduino-compatible board, including the
[Pololu A-Star 32U4 controllers](https://www.pololu.com/category/149/a-star-programmable-controllers).

## Getting started

### Hardware

The
[Pololu Dual MAX14870 Motor Driver Shield for Arduino](https://www.pololu.com/catalog/product/2519)
can be purchased from Pololu's website. Before continuing, careful
reading of the product page is recommended.

This library configures Timer 1 on ATmega168, ATmega328P, ATmega328PB, and
ATmega32U4 microcontrollers to generate a 20 kHz PWM frequency for the motors.
The library uses `analogWrite()` on any board that does not use one of these
microcontrollers. On the Arduino Mega 2560 and Arduino Due, `analogWrite()`
generates 489 Hz and 1 kHz PWM frequencies, respectively, with the
default timer configuration.

### Software

If you are using version 1.6.2 or later of the
[Arduino software (IDE)](https://www.arduino.cc/en/Main/Software), you can use
the Library Manager to install this library:

1. In the Arduino IDE, open the "Sketch" menu, select "Include Library", then
   "Manage Libraries...".
2. Search for "DualMAX14870MotorShield".
3. Click the DualMAX14870MotorShield entry in the list.
4. Click "Install".

If this does not work, you can manually install the library:

1. Download the
   [latest release archive from GitHub](https://github.com/pololu/dual-max14870-motor-shield/releases)
   and decompress it.
2. Rename the folder "dual-max14870-motor-shield-xxxx" to "DualMAX14870MotorShield".
3. Drag the "DualMAX14870MotorShield" folder into the "libraries" directory
   inside your Arduino sketchbook directory. You can view your sketchbook
   location by opening the "File" menu and selecting "Preferences" in the
   Arduino IDE. If there is not already a "libraries" folder in that location,
   you should make the folder yourself.
4. After installing the library, restart the Arduino IDE.

## Example

An example sketch is available that shows how to use the library.  You
can access it from the Arduino IDE by opening the "File" menu,
selecting "Examples", and then selecting "DualMAX14870MotorShield".  If
you cannot find this example, the library was probably installed
incorrectly and you should retry the installation instructions above.

### Demo

The demo enables the drivers and ramps motor 1 from stopped to full speed
forward, ramps down to full speed reverse, and back to stopped. Then, it does
the same with the other motor. After motor 2 is stopped, sleep mode is entered
for 500 ms before the demo enables the drivers again. If a driver fault is
detected, the demo blinks the yellow LED rapidly.

## Documentation

- `DualMax14870MotorShield()` <br> Default constructor, selects the
  default pins as connected by the motor shield.
- `DualMax14870MotorShield(uint8_t char M1DIR, uint8_t M1PWM, uint8_t
   M2DIR, uint8_t M2PWM, uint8_t nEN, uint8_t nFAULT)` <br> Alternate
   constructor for shield connections remapped by user. If M1PWM and
   M2PWM are remapped, it will try to use analogWrite instead of
   timer1.
- `void setM1Speed(int16_t speed)` <br> Set speed and direction for
  motor 1. Speed should be between -400 and 400. The motors brake at 0
  speed. Positive speeds correspond to motor current flowing from M1A
  to M1B. Negative speeds correspond to motor current flowing from M1B
  to M1A.
- `void setM2Speed(int16_t speed)` <br> Set speed and direction for
  motor 2. Speed should be between -400 and 400. The motors brake at 0
  speed. Positive speeds correspond to motor current flowing from M2A
  to M2B. Negative speeds correspond to motor current flowing from M2B
  to M2A.
- `void setSpeeds(int16_t m1Speed, int16_t m2Speed)` <br> Set speed and
  direction for motor 1 and 2.
- `void flipM1(bool flip)` <br> Flip the direction meaning of the
  speed passed to the setSpeeds function for motor 1.  The default
  direction corresponds to flipM1(false) having been called.
- `void flipM2(bool flip)` <br> Flip the direction meaning of the
  speed passed to the setSpeeds function for motor 2.  The default
  direction corresponds to flipM2(false) having been called.
- `bool getFault()` <br> Returns 1 if there is a motor driver fault, 0 if no
  fault.
- `void enableDrivers()` <br> Enables the motor drivers.
- `void disableDrivers()` <br> Disables the motor drivers.
    
## Version history

* 2.0.0 (2018-04-11): Allow PWM remapping (use analogWrite if PWM pins
  remapped).
* 1.0.0 (2018-03-30): Original release.
