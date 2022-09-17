#include <DualMAX14870MotorShield.h>

/*
 * This example uses the DualMAX14870MotorShield library to drive each motor
 * with the Pololu Dual MAX14870 Motor Driver Shield for Arduino forward, then
 * backward. The yellow user LED is on when a motor is set to a positive speed
 * and off when a motor is set to a negative speed; it blinks rapidly if the
 * drivers report a fault.
 */

#define LED_PIN 13
uint8_t fault = 2;
uint8_t mot_dir = 20;
uint8_t mot_pwm = 22; // 15 in the pcb, but 15 is not a teensy 3.2 PWM pin =(
uint8_t mot_en = 21;  
DualMAX14870MotorShield motors(mot_dir, mot_pwm, 31, 32, mot_en, fault);

void stopIfFault()
{
  if (motors.getFault())
  {
    while (1)
    {
      Serial.println("Fault detected");
      digitalWrite(LED_PIN, HIGH);
      delay(100);
      digitalWrite(LED_PIN, LOW);
      delay(100);
    }
  }
}

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(57600);
  // uncomment one or both of the following lines if your motors' directions
  // need to be flipped
  //motors.flipM1(true);
  //motors.flipM2(true);
}

void loop()
{ 
  Serial.println("starting motor loop");
  motors.enableDrivers();

  // run M1 motor with positive speed

  digitalWrite(LED_PIN, HIGH);

  for (int speed = 0; speed <= 400; speed++)
  {
    motors.setM1Speed(speed);
    stopIfFault();
    delay(1);
  }

  for (int speed = 400; speed >= 0; speed--)
  {
    motors.setM1Speed(speed);
    stopIfFault();
    delay(1);
  }

  // run M1 motor with negative speed

  digitalWrite(LED_PIN, LOW);

  for (int speed = 0; speed >= -400; speed--)
  {
    motors.setM1Speed(speed);
    stopIfFault();
    delay(1);
  }

  for (int speed = -400; speed <= 0; speed++)
  {
    motors.setM1Speed(speed);
    stopIfFault();
    delay(1);
  }

  // run M2 motor with positive speed

  digitalWrite(LED_PIN, HIGH);

  motors.disableDrivers();
  delay(500);
}
