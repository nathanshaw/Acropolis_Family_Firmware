///////////////////////// WASHBOARD / PECKER MODULE /////////////////////////////
// 0 is off, 1 is forward, -1 is backward
#if HV_MAJOR ==1
int8_t hbridge_state[NUM_MOTORS] = {0,0,0};
uint16_t hbridge_forward_remaining[NUM_MOTORS] = {0,0,0};
uint16_t hbridge_backward_remaining[NUM_MOTORS] = {0,0,0};

elapsedMillis hbridge_update[NUM_MOTORS];

#define H_BRIDGE_F_SPEED 250
#define H_BRIDGE_R_SPEED -200

// TODO - need to add fault checking as well....
void shakeWashboard(int w, uint16_t len) {
  hbridge_forward_remaining[w] = len * 0.7;
  hbridge_backward_remaining[w] = len * 0.3;
  Serial.print("Shake washboard: ");
  Serial.print(hbridge_forward_remaining[w]);
  Serial.print(" - ");
  Serial.println(hbridge_backward_remaining[w]);
  hbridge_state[w] = 1;
  hbridge_update[w] = 0;
  motors[w].enableDrivers();
  updateHBridge(w);
}

void updateHBridge(int w) {
  if (hbridge_state[w] == 0) {
    // dbPrintln("wb state == 0", 3);
    return;
  } else if (hbridge_state[w] == 1) {
    // dbPrintln("wb state = 1", 3);
    if (hbridge_update[w] > hbridge_forward_remaining[w]) {
      hbridge_state[w] = -1;
      hbridge_update[w] = 0;
    } else {
      motors[w].setM1Speed(H_BRIDGE_F_SPEED);  
      // dbPrintln("forward", 2);
    }
  } else if (hbridge_state[w] == -1) {
    // dbPrintln("wb state = -1", 3);
    if (hbridge_update[w] > hbridge_backward_remaining[w]) {
      hbridge_state[w] = 0;
      hbridge_update[w] = 0;
      motors[w].setM1Speed(0);
      motors[w].disableDrivers();
    }
    else {
      // dbPrintln("reverse", 2);
      motors[w].setM1Speed(H_BRIDGE_R_SPEED);
    }
  }
}
#endif // HV_MAJOR == 1
