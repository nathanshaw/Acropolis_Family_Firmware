/* Mechatronic Creatures
   "Bowl Bot" Genus
   using the Adafruit Huzzah ESP8266 Microcontroller


*/
#include "SparkFun_SHTC3.h"
#include <DualMAX14870MotorShield.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define LED_PIN_1 5
#define LED_PIN_1 8
#define LED_PIN_1 10

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT_1 10
#define LED_COUNT_2 10
#define LED_COUNT_3 10

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN_1, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

// Solenoids
const int s_pins[] = {12, 11, 7, 6, 4, 3};

// H-Bridge Motor (MAX14870)
#define LED_PIN 13
uint8_t mot_dir = 21;
uint8_t mot_pwm = 22;
uint8_t mot_en = 20;
DualMAX14870MotorShield motor(mot_dir, mot_pwm, 31, 32, mot_en, 33); // 31, 32, and 33 are unused pins

// pots
uint8_t pot = 14;
uint16_t pot_val = 0;

// jumper button
uint8_t jump_but = 6;

// LDR ??!!??!!
uint8_t ldr1 = 15;
uint8_t ldr2 = 16;
uint16_t ldr1_val = 0;
uint16_t ldr2_val = 0;

// temp and humidity
SHTC3 mySHTC3;
double temp = 0.0;
double humidity = 0.0;

void setup() {
  Serial.begin(115200);
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
  delay(2000);
  // put your setup code here, to run once:
  pinMode(s_pins[0], OUTPUT);
  pinMode(s_pins[1], OUTPUT);
  pinMode(s_pins[2], OUTPUT);
  pinMode(s_pins[3], OUTPUT);
  pinMode(s_pins[4], OUTPUT);
  pinMode(s_pins[5], OUTPUT);

  digitalWrite(s_pins[0], LOW);
  digitalWrite(s_pins[1], LOW);
  digitalWrite(s_pins[2], LOW);
  digitalWrite(s_pins[3], LOW);
  digitalWrite(s_pins[4], LOW);
  digitalWrite(s_pins[5], LOW);

  pinMode(ldr1, INPUT);
  pinMode(ldr2, INPUT);
  
  // temp and humidity sensor
  errorDecoder(mySHTC3.begin());
  Wire.setClock(400000);
  if (mySHTC3.passIDcrc)                                      // Whenever data is received the associated checksum is calculated and verified so you can be sure the data is true
  { // The checksum pass indicators are: passIDcrc, passRHcrc, and passTcrc for the ID, RH, and T readings respectively
    Serial.print("ID Passed Checksum. ");
    Serial.print("Device ID: 0b");
    Serial.print(mySHTC3.ID, BIN);                       // The 16-bit device ID can be accessed as a member variable of the object
  }
  else
  {
    Serial.print("ID Checksum Failed. ");
  }

  //motor.flipM1(true); // if the direction of the motor needs to be flipped
  Serial.println("Finshed Setting up");
  Serial.println("||||||||||||||||||||||||||||||||||||||||||||||||||||");
  Serial.println("Welcome to the Parrot Test Program");
  Serial.println("0-5 to pulse solenoids 1-4");
  Serial.println("b to move motor backwards");
  Serial.println("f to move motor forwards");
  Serial.println("6 to read and print pot values");
  Serial.println("7 to read and print the LDR values");
  Serial.println("8 to read and print the temp/humidity sensor values");
  Serial.println("9 to read and print the jumper button value");
  Serial.println("||||||||||||||||||||||||||||||||||||||||||||||||||||");
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
    int incByte = Serial.read() - 48;
    Serial.print("incbyte : ");
    Serial.println(incByte);
    if (incByte > -1 && incByte < 6) {
      Serial.print("pulsing solenoid output #");
      Serial.println(incByte);
      digitalWrite(s_pins[incByte], HIGH);
      strip.setPixelColor(incByte, strip.Color(255, 0, 0));
      strip.show();
      delay(50);
      strip.setPixelColor(incByte, strip.Color(0, 0, 0));
      strip.show();
      digitalWrite(s_pins[incByte], LOW);
    }
    else if (incByte == 50) {
      Serial.println("pulse motor backwards");
      motor.enableDrivers();
      // pulse motor "backwards"
      for (int speed = 0; speed >= -400; speed--)
      {
        motor.setM1Speed(speed);
        delay(1);
      }

      for (int speed = -400; speed <= 0; speed++)
      {
        motor.setM1Speed(speed);
        delay(1);
      }
      motor.disableDrivers();
    } else if (incByte == 54) {
      // pulse motor "forward"
      Serial.println("pulse motor forwards");
      motor.enableDrivers();
      for (int speed = 0; speed <= 400; speed++)
      {
        motor.setM1Speed(speed);
        delayMicroseconds(300);
      }

      for (int speed = 400; speed >= 0; speed--)
      {
        motor.setM1Speed(speed);
        delayMicroseconds(300);
      }
      motor.disableDrivers();
    } else if (incByte == 6) { // pot values
      pot_val = analogRead(pot);
      Serial.print("Pot value: ");
      Serial.print(pot_val);
    } else if (incByte == 7) { // LDR values
      Serial.print("LDR : ");
      Serial.print(analogRead(ldr1));
      Serial.print("\t");
      Serial.println(analogRead(ldr2));
    } else if (incByte == 8) { // temp/humidity sensor
      SHTC3_Status_TypeDef result = mySHTC3.update();             // Call "update()" to command a measurement, wait for measurement to complete, and update the RH and T members of the object
      printSHTCInfo();                                                // This function is used to print a nice little line of info to the serial port
      Serial.print("Temp:");
      Serial.print(temp);
      Serial.print("\t Humidity:");
      Serial.println(humidity);
    } else if (incByte == 9) { // Jumper button
      Serial.print("Jumper button value is: ");
      Serial.println(digitalRead(jump_but));
    }
  }
}

///////////////////////
// SHTC3 Utility Functions //
///////////////////////
void printSHTCInfo()
{
  if (mySHTC3.lastStatus == SHTC3_Status_Nominal)             // You can also assess the status of the last command by checking the ".lastStatus" member of the object
  {
    humidity = mySHTC3.toPercent();
    Serial.print("RH = ");
    Serial.print(humidity);                   // "toPercent" returns the percent humidity as a floating point number
    Serial.print("% (checksum: ");
    if (mySHTC3.passRHcrc)                                    // Like "passIDcrc" this is true when the RH value is valid from the sensor (but not necessarily up-to-date in terms of time)
    {
      Serial.print("pass");
    }
    else
    {
      Serial.print("fail");
    }
    Serial.print("), T = ");
    temp = mySHTC3.toDegF();
    Serial.print(temp);                        // "toDegF" and "toDegC" return the temperature as a flaoting point number in deg F and deg C respectively
    Serial.print(" deg F (checksum: ");
    if (mySHTC3.passTcrc)                                       // Like "passIDcrc" this is true when the T value is valid from the sensor (but not necessarily up-to-date in terms of time)
    {
      Serial.print("pass");
    }
    else
    {
      Serial.print("fail");
    }
    Serial.println(")");
  }
  else
  {
    Serial.print("Update failed, error: ");
    errorDecoder(mySHTC3.lastStatus);
    Serial.println();
  }
}

void errorDecoder(SHTC3_Status_TypeDef message)                             // The errorDecoder function prints "SHTC3_Status_TypeDef" resultsin a human-friendly way
{
  switch (message)
  {
    case SHTC3_Status_Nominal : Serial.print("Nominal"); break;
    case SHTC3_Status_Error : Serial.print("Error"); break;
    case SHTC3_Status_CRC_Fail : Serial.print("CRC Fail"); break;
    default : Serial.print("Unknown return code"); break;
  }
}
