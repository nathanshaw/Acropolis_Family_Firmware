#include "MAX14870Motors.h"

// Constructors ////////////////////////////////////////////////////////////////

MAX14870Motors::MAX14870Motors(uint8_t M1DIR,
        uint8_t M1PWM,
        uint8_t M2DIR,
        uint8_t M2PWM,
        uint8_t M3DIR,
        uint8_t M3PWM,
        uint8_t M1EN,
        uint8_t M2EN,
        uint8_t M3EN,
        uint8_t M1FAULT,
        uint8_t M2FAULT,
        uint8_t M3FAULT)
{
    _M1DIR = M1DIR;
    _M1PWM = M1PWM;
    _M2DIR = M2DIR;
    _M2PWM = M2PWM;
    _M3DIR = M1DIR;
    _M3PWM = M3PWM;
    _M1EN = M1EN;
    _M1FAULT = M1FAULT;
    _M2EN = M2EN;
    _M2FAULT = M2FAULT;
    _M3EN = M3EN;
    _M3FAULT =  M3FAULT;
}


void MAX14870Motors::initPinsAndMaybeTimer()
{
    pinMode(_M1PWM, OUTPUT);
    digitalWrite(_M1PWM, LOW);
    pinMode(_M2PWM, OUTPUT);
    digitalWrite(_M2PWM, LOW);
    pinMode(_M3PWM, OUTPUT);
    digitalWrite(_M3PWM, LOW);
    pinMode(_M1DIR, OUTPUT);
    digitalWrite(_M1DIR, LOW);
    pinMode(_M2DIR, OUTPUT);
    digitalWrite(_M2DIR, LOW);
    pinMode(_M3DIR, OUTPUT);
    digitalWrite(_M3DIR, LOW);

    pinMode(_M1EN, OUTPUT);
    digitalWrite(_M1EN, LOW); // default to on
    pinMode(_M2EN, OUTPUT);
    digitalWrite(_M2EN, LOW); // default to on
    pinMode(_M3EN, OUTPUT);
    digitalWrite(_M3EN, LOW); // default to on

    pinMode(_M1FAULT, INPUT_PULLUP);
    pinMode(_M2FAULT, INPUT_PULLUP);
    pinMode(_M3FAULT, INPUT_PULLUP);
}

void MAX14870Motors::setM1Speed(int16_t speed)
{
    init(); // initialize if necessary

    boolean reverse = 0;

    if (speed < 0 )
    {
        speed = -speed; // make speed a positive quantity
        reverse = 1;    // preserve the direction
    }
    if (speed > 1023)  // max
    {
        speed = 1023;
    }
    analogWrite(_M1PWM, speed); 

    if (reverse ^ _flipM1) {// flip if speed was negative or _flipM1 setting is active, but not both
        digitalWrite(_M1DIR, HIGH);
    }
    else
    {
        digitalWrite(_M1DIR, LOW);
    }
}

void MAX14870Motors::setM2Speed(int16_t speed)
{
    init(); // initialize if necessary

    boolean reverse = 0;

    if (speed < 0)
    {
        speed = -speed;  // make speed a positive quantity
        reverse = 1;  // preserve the direction
    }
    if (speed > 1023)  // max PWM duty cycle
    {
        speed = 1023;
    }
    analogWrite(_M2PWM, speed); 
    if (reverse ^ _flipM2) // flip if speed was negative or _flipM2 setting is active, but not both
    {
        digitalWrite(_M2DIR, HIGH);
    }
    else
    {
        digitalWrite(_M2DIR, LOW);
    }
}

void MAX14870Motors::setM3Speed(int16_t speed)
{
    init(); // initialize if necessary

    boolean reverse = 0;

    if (speed < 0)
    {
        speed = -speed;  // make speed a positive quantity
        reverse = 1;  // preserve the direction
    }
    if (speed > 1023)  // max PWM duty cycle
    {
        speed = 1023;
    }
    analogWrite(_M3PWM, speed); 

    if (reverse ^ _flipM3) // flip if speed was negative or _flipM3 setting is active, but not both
    {
        digitalWrite(_M3DIR, HIGH);
    }
    else
    {
        digitalWrite(_M3DIR, LOW);
    }
}

// set speed for both motors
void MAX14870Motors::setSpeeds(int16_t m1Speed, int16_t m2Speed, int16_t m3Speed)
{
    setM1Speed(m1Speed);
    setM2Speed(m2Speed);
    setM3Speed(m3Speed);
}

void MAX14870Motors::flipM1(bool flip)
{
    _flipM1 = flip;
}

void MAX14870Motors::flipM2(bool flip)
{
    _flipM2 = flip;
}

void MAX14870Motors::flipM3(bool flip)
{
    _flipM3 = flip;
}

bool MAX14870Motors::getFault()
{
    return !digitalRead(_M1FAULT) || !digitalRead(_M2FAULT) || !digitalRead(_M3FAULT);
}
void MAX14870Motors::enableDrivers(uint8_t which)
{
    if (which == 0) {
        digitalWrite(_M1EN, LOW);
    } else if (which == 1) {
        digitalWrite(_M2EN, LOW);
    } else if (which == 2) {
        digitalWrite(_M3EN, LOW);
    }
}

void MAX14870Motors::disableDrivers(uint8_t which)
{
    if (which == 0) {
        digitalWrite(_M1EN, HIGH);
    } else if (which == 1) {
        digitalWrite(_M2EN, HIGH);
    } else if (which == 2) {
        digitalWrite(_M3EN, HIGH);
    }
}

void MAX14870Motors::enableDrivers()
{
    digitalWrite(_M1EN, LOW);
    digitalWrite(_M2EN, LOW);
    digitalWrite(_M3EN, LOW);
}

void MAX14870Motors::disableDrivers()
{
    digitalWrite(_M1EN, HIGH);
    digitalWrite(_M2EN, HIGH);
    digitalWrite(_M3EN, HIGH);
}
