#ifndef __BELL_MECHANISM_H__
#define __BELL_MECHANISM_H__

#include "Mechanism.h"

// state machine, 0 is inactive
// 1 is dampener is up but solenoid has not struck
// 2 is dampener is up and solenoid has struck
#define BELL_INACTIVE 0
#define BELL_REMOVE_DAMPER 4
#define BELL_DAMPENER_UP 1
#define BELL_STRIKING 2
#define BELL_STRUCK 3

// this is how long in MS to actuate the striking solenoid for the bell
#define BELL_STRIKE_LENGTH 30
#define BELL_RING_LENGTH 30

class BellMechanism {
    public:
      BellMechanism(uint8_t ap, uint8_t dp, uint8_t dd, double f, uint8_t ot);
      void update();
      void queueStrike(double vel, uint32_t len);
      void strike();
      double getFreq() {return freq;};
      void setPrint(bool p){PRINT_MECHANISM_DEBUG = p;};

    private:
      uint8_t dampener_delay = 10;// how long to let the dampener be released for?
      double velocity = 1.0;      // this is really the length of time
      uint32_t note_length = 30;  // how long to leave damper disenguaged after act strikes
      uint8_t state = BELL_INACTIVE; // is it
      uint8_t on_time = 30;
      double freq; // what is the frequency of this mechanism
      uint8_t act_pin;
      uint8_t damp_pin;
      elapsedMillis last_action;
      bool PRINT_MECHANISM_DEBUG = false;
};

BellMechanism::BellMechanism(uint8_t ap, uint8_t dp, uint8_t dd, double f, uint8_t ot) {
  act_pin = ap;
  damp_pin = dp;
  dampener_delay = dd;
  freq = f;
  on_time = ot;
}

void BellMechanism::queueStrike(double vel, uint32_t len) {
    velocity = vel;
    note_length = len;
    dprint(PRINT_MECHANISM_DEBUG, "new strike queued for bell mechanism - vel:");
    dprint(PRINT_MECHANISM_DEBUG, vel);
    dprint(PRINT_MECHANISM_DEBUG, " len: ");
    dprintln(PRINT_MECHANISM_DEBUG, note_length);
}

void BellMechanism::strike() {
    state = BELL_REMOVE_DAMPER;
}

void BellMechanism::update() {
  switch (state) {
    case BELL_INACTIVE:
      // if it has been long enough since the last bell strike and the bell it not currently active
        break;
    case BELL_REMOVE_DAMPER:
        Serial.println("removing bell dampener");
        // turn on the dampener
        digitalWrite(damp_pin, HIGH);
        // reset the last bell activity timer
        last_action = 0;
        // change bell state to reflect status of dampener
        state = BELL_DAMPENER_UP;
        break;
    case BELL_DAMPENER_UP:
      // if the dampener has been up for long enough then strike the solenoid
      if (last_action > dampener_delay) {
        Serial.println("striking the bell");
        if (dampener_delay != 0) {
            digitalWrite(act_pin, HIGH);
        }
        // reset the last bell activity timer
        last_action = 0;
        // change bell state to reflect status of dampener
        state = BELL_STRIKING;
        break;
      }
      break;
    case BELL_STRIKING:
      // if the bell has been struck for long enough deactivate the striking solenoid
      if (last_action > velocity * on_time) {
        Serial.println("stopping bell strike");
        digitalWrite(act_pin, LOW);
        // reset the last bell activity timer
        last_action = 0;
        // change bell state to reflect status of dampener
        state = BELL_STRUCK;
        break;
      }
      break;
    case BELL_STRUCK:
      // if the bell has rings for long enough then allow the dampener to re-engague
      if (last_action > note_length) {
        Serial.println("reapplying the dampener");
        digitalWrite(damp_pin, LOW);
        // reset the last bell activity timer
        last_action = 0;
        // change bell state to inactive
        state = BELL_INACTIVE;
        break;
      }
      break;
  }
}

#endif
