#include <Arduino.h>

class MAX14870Motors
{
    public:
        MAX14870Motors(uint8_t M1DIR,
                uint8_t M1PWM,
                uint8_t M1EN,
                uint8_t M1FAULT,
                uint8_t M2DIR,
                uint8_t M2PWM,
                uint8_t M2EN,
                uint8_t M2FAULT,
                uint8_t M3DIR,
                uint8_t M3PWM,
                uint8_t M3EN,
                uint8_t M3FAULT);

        void setM1Speed(int16_t speed);
        void setM2Speed(int16_t speed);
        void setM3Speed(int16_t speed);
        void setSpeeds(int16_t m1Speed, int16_t m2Speed, int16_t m3Speed);
        void flipM1(bool flip);
        void flipM2(bool flip);
        void flipM3(bool flip);
        bool getFault();
        void enableDrivers();
        void disableDrivers();
        void enableDrivers(uint8_t which);
        void disableDrivers(uint8_t which);

    private:
        void initPinsAndMaybeTimer();

        uint8_t _M1EN;
        uint8_t _M2EN;
        uint8_t _M3EN;
        uint8_t _M1DIR;
        uint8_t _M2DIR;
        uint8_t _M3DIR;
        uint8_t _M1PWM;

        static const uint8_t _M1PWM_TIMER1_PIN = 9;
        uint8_t _M2PWM;
        static const uint8_t _M2PWM_TIMER1_PIN = 10;
        uint8_t _M3PWM;
        static const uint8_t _M3PWM_TIMER1_PIN = 11;// TODO - not sure what this is really doing

        uint8_t _M1FAULT;
        uint8_t _M2FAULT;
        uint8_t _M3FAULT;

        bool _flipM1 = false;
        bool _flipM2 = false;
        bool _flipM3 = false;

        bool initialized = false;

        inline void init()
        {
            if (!initialized)
            {
                initialized = true;
                initPinsAndMaybeTimer();
            }
        }
};
