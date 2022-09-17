///////////////////////// WASHBOARD / PECKER MODULE /////////////////////////////
// 0 is off, 1 is forward, -1 is backward
/*
int8_t hbridge_state = 0;
uint16_t hbridge_forward_remaining = 0;
uint16_t hbridge_backward_remaining = 0;

elapsedMillis hbridge_update;
#define H_BRIDGE_F_SPEED 250
#define H_BRIDGE_R_SPEED -200

void shakeWashboard(uint16_t len) {
  hbridge_forward_remaining = len * 0.7;
  hbridge_backward_remaining = len * 0.3;
  Serial.print("Shake washboard: ");
  Serial.print(hbridge_forward_remaining);
  Serial.print(" - ");
  Serial.println(hbridge_backward_remaining);
  hbridge_state = 1;
  hbridge_update = 0;
  motor.enableDrivers();
  updateHBridge();
}

void updateHBridge() {
  if (hbridge_state == 0) {
    dbPrintln("wb state == 0", 3);
    return;
  } else if (hbridge_state == 1) {
    dbPrintln("wb state = 1", 3);
    if (hbridge_update > hbridge_forward_remaining) {
      hbridge_state = -1;
      hbridge_update = 0;
    } else {
      motor.setM1Speed(H_BRIDGE_F_SPEED);  
      dbPrintln("forward", 2);
    }
  } else if (hbridge_state == -1) {
    dbPrintln("wb state = -1", 3);
    if (hbridge_update > hbridge_backward_remaining) {
      hbridge_state = 0;
      hbridge_update = 0;
      motor.setM1Speed(0);
      motor.disableDrivers();
    }
    else {
      dbPrintln("reverse", 2);
      motor.setM1Speed(H_BRIDGE_R_SPEED);
    }
  }
}
*/
