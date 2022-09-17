import oscP5.*;
import netP5.*;
import processing.serial.*;

OscP5 oscP5;
NetAddress myRemoteLocation;

// Serial object
Serial teensyPort;
String serial_msg;

float fluxMax = 2.0;
float rmsThresh = 1.0;
float rmsMax = 3.0;

int memory_size = 600;
int num_streams = 4;

int strip_brightness = 0; // for keeping track of the strip brightness
int min_bright = 0;
int max_bright = 120;
float b_weight = 0.5; // the weight for the new brightness signal (higher value will result in quicker change)

float[][] flux_values = new float[num_streams][memory_size];
int[][] flux_flags = new int[num_streams][memory_size];
float[][] rms_values = new float[num_streams][memory_size];
int[][] rms_flags = new int[num_streams][memory_size];
String[] feature_names = new String[num_streams];

int FFT_SIZE = 1024;
float[] fft_magnitudes = new float[FFT_SIZE];

float displayXHop = 0;

float[] addToFIFO(float newVal, float[] buf) {
  for (int i = buf.length-2; i >= 0; i--) {
    buf[i+1] = buf[i];
  }
  buf[0] = newVal;
  return buf;
}

int[] addToFIFO(int newVal, int[] buf) {
  for (int i = buf.length-2; i >= 0; i--) {
    buf[i+1] = buf[i];
  }
  buf[0] = newVal;
  return buf;
}

void oscEvent(OscMessage msg) {
  println("received OSC message");
  if (msg.checkAddrPattern("/rms") == true) {
    float temp1 = constrain(msg.get(0).floatValue(), 0.0, rmsThresh);
    byte b = byte(int(map(temp1*temp1, 0, rmsThresh*rmsThresh, min_bright, max_bright)));
    if (b != 0) {
      sendBrightnessMsg(b);
    }
    println(b);
  } else if (msg.checkAddrPattern("/features") == true) { 
    println("features OSC message received");
    int stream_num = msg.get(0).intValue();
    float temp = msg.get(1).floatValue();
    int flag = msg.get(2).intValue();
    feature_names[stream_num] = msg.get(3).stringValue();
    rms_flags[stream_num] = addToFIFO(flag, rms_flags[stream_num]);
    rms_values[stream_num] = addToFIFO(temp, rms_values[stream_num]);
    // println(rms_values[0][0]);
    // if incomming message has a flag on the first stream msg, then send "click" message to teensy
    byte bright;
    if (stream_num == 0 && flag == 1) {
      // scale the click brightness according to expected threshold and the "max" value
      temp = constrain(temp, rmsThresh, rmsMax);
      bright = byte(int(map(temp, rmsThresh, rmsMax, 200, 255)));
      println("click message: ", bright);
      sendClickMsg(bright);
    }
  } else if (msg.checkAddrPattern("/fft") == true) {
    // plot the magnitudes of all the FFT data below the "features" 
    for (int i = 0; i < FFT_SIZE; i++) {
      fft_magnitudes[i] = msg.get(i).floatValue();
    }
  } else {
    print("received invalid osc message");
  }
  redraw();
}

void sendClickMsg(int brightness) {
  if (brightness < 10) {
    brightness = 10;
  }
  teensyPort.write("1");
  teensyPort.write(brightness); 
  println("sent click message with brightness of ", brightness);
}

void sendBrightnessMsg(int brightness) {
  teensyPort.write("2");
  strip_brightness = int(((strip_brightness * (1.0 - b_weight)) + (brightness * b_weight)) * 0.5);
  teensyPort.write(strip_brightness);
  // println("sent brightness message with value of: ", brightness);
}

void updateDisplay() {
  background(0, 150, 170);
  float sectionHeight = height/num_streams;
  for (int s = 0; s < num_streams; s++) {
    float vHop = (height/num_streams)*s*-1;
    pushMatrix();
    translate(0, vHop);
    for (int i = 0; i < memory_size; i++) {
      float scaled_val = constrain(rms_values[s][i], 0, 1.0) * sectionHeight;// expected flux range is 0-1.5
      // float x_loc = i * displayXHop; // this is the current hop offset for x
      pushMatrix();
      translate(displayXHop*i, 0);
      if (rms_flags[s][i] == 1) {
        fill(255, 0, 0);
      } else if (rms_flags[s][i] == -1) {
        fill(0, 255, 0);
      } else if (scaled_val == sectionHeight) {
        fill(200, 120, 0);
      } else {
        fill(255-rms_values[s][i]*200);
      }
      // 600-scaled_val
      rect(0, height-scaled_val, displayXHop, scaled_val);
      popMatrix();
    }
    textSize(32);
    fill(255, 255, 255);
    text(feature_names[s], 10, height - 100);
    popMatrix();
  }
  // TODO make a bar on the far right which shows the brightness as stored in progam
  fill(strip_brightness, 255-strip_brightness, 0);
  int adj_bright = int(map(strip_brightness, min_bright, max_bright, 0, height));
  // println("adjusted brightness : ", adj_bright, " - ", height);
  rect(width-20, 0 + adj_bright, width, height);
  // rect(0, height, width, 0);
}

void setup() {
  for (int i = 0; i < num_streams; i++) {
    feature_names[i] = "null";
  }
  noStroke();
  frameRate(30);
  size(1200, 900);
  displayXHop = width/memory_size;
  oscP5 = new OscP5(this, 6449);
  myRemoteLocation = new NetAddress("127.0.0.1", 6450);
  // colorMode(HSB, 360); 
  // noCursor();

  String portName = Serial.list()[5];
  println(portName);
  teensyPort = new Serial(this, portName, 57600);
}

void draw() {
  updateDisplay();
}
