#include <DualMAX14870MotorShield.h>

/*
 * This example uses the DualMAX14870MotorShield library to drive each motor
 * with the Pololu Dual MAX14870 Motor Driver Shield for Arduino forward, then
 * backward. The yellow user LED is on when a motor is set to a positive speed
 * and off when a motor is set to a negative speed; it blinks rapidly if the
 * drivers report a fault.
 */

#define LED_PIN 13

DualMAX14870MotorShield motors;

void stopIfFault()
{
  if (motors.getFault())
  {
    while (1)
    {
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

  // uncomment one or both of the following lines if your motors' directions
  // need to be flipped
  //motors.flipM1(true);
  //motors.flipM2(true);
}

void loop()
{
  motors.enableDrivers();

  // run M1 motor with positive speed

  digitalWrite(LED_PIN, HIGH);

  for (int speed = 0; speed <= 400; speed++)
  {
    motors.setM1Speed(speed);
    stopIfFault();
    delay(2);
  }

  for (int speed = 400; speed >= 0; speed--)
  {
    motors.setM1Speed(speed);
    stopIfFault();
    delay(2);
  }

  // run M1 motor with negative speed

  digitalWrite(LED_PIN, LOW);

  for (int speed = 0; speed >= -400; speed--)
  {
    motors.setM1Speed(speed);
    stopIfFault();
    delay(2);
  }

  for (int speed = -400; speed <= 0; speed++)
  {
    motors.setM1Speed(speed);
    stopIfFault();
    delay(2);
  }

  // run M2 motor with positive speed

  digitalWrite(LED_PIN, HIGH);

  for (int speed = 0; speed <= 400; speed++)
  {
    motors.setM2Speed(speed);
    stopIfFault();
    delay(2);
  }

  for (int speed = 400; speed >= 0; speed--)
  {
    motors.setM2Speed(speed);
    stopIfFault();
    delay(2);
  }

  // run M2 motor with negative speed

  digitalWrite(LED_PIN, LOW);

  for (int speed = 0; speed >= -400; speed--)
  {
    motors.setM2Speed(speed);
    stopIfFault();
    delay(2);
  }

  for (int speed = -400; speed <= 0; speed++)
  {
    motors.setM2Speed(speed);
    stopIfFault();
    delay(2);
  }

  motors.disableDrivers();
  delay(500);
}