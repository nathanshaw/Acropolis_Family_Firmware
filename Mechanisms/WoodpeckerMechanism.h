#ifndef __WOODPECKER_MECHANISM_H__
#define __WOODPECKER_MECHANISM_H__

#include "../MAX14870Motors/MAX14870Motors.cpp"

#define STATE_INACTIVE                  0
#define STATE_SOL_ACTIVE                1
#define STATE_SOL_ACTIVATE              2
#define STATE_MOVING_MOTOR_FORWARD      4
#define STATE_MOVING_MOTOR_BACKWARD     5
#define STATE_MOVE_MOTOR                6

class Woodpecker {
    public:
        Woodpecker(uint8_t sol, MAX14870Motors * m, uint16_t m_num, double min_on, double max_on, double between_time);

        void setMotorSpeeds(int f, int b);
        void update();
        void queueStrike(double vel);
        bool strike();
        bool strike(double vel);
        void rotate(int len);

        void setPrint(bool p){PRINT_MECHANISM_DEBUG = p;};

    private:
        elapsedMillis last_action;
        // there are  a few states
        // the first is about to strike, the second
        uint8_t state = 0;
        // on a scale of 0.0 - 1.0 how strong to make the solenoid strike?
        double velocity = 0.0;

        double sol_min_on_time = 0;
        double sol_max_on_time = 0;
        double sol_min_time_between_strikes = 0;

        uint16_t forward_remaining = 0;
        uint16_t backward_remaining = 0;

        int forward_speed = 70;
        int backward_speed = -30;


        // for the solenoid component
        uint8_t sol_pin;
        // for the motor component
        uint8_t mot_num;
        MAX14870Motors * motors;
        bool PRINT_MECHANISM_DEBUG = true;
};

Woodpecker::Woodpecker(uint8_t sol, MAX14870Motors * m, uint16_t m_num, double min_on, double max_on, double between_time) {
    mot_num = m_num;
    sol_pin = sol;
    motors = m;
    sol_min_on_time = min_on;
    sol_max_on_time = max_on;
    sol_min_time_between_strikes = between_time;
}

void Woodpecker::setMotorSpeeds(int f, int b) {
    forward_speed = f;
    backward_speed = b;
}

void Woodpecker::queueStrike(double vel) {
    vel = (vel * (sol_max_on_time - sol_min_on_time)) + sol_min_on_time;
    if (vel > sol_max_on_time) {
        dprint(PRINT_MECHANISM_DEBUG, "WARNING - velocity for queueStrike is too high, setting to max_on_time:");
        dprint(PRINT_MECHANISM_DEBUG, vel);
        vel = sol_max_on_time;
    } else if (vel < sol_min_on_time) {
        dprint(PRINT_MECHANISM_DEBUG, "WARNING - velocity for queueStrike is too low, setting to min_on_time:");
        dprint(PRINT_MECHANISM_DEBUG, vel);
        vel = sol_min_on_time;
    }
    velocity = vel;
    dprint(PRINT_MECHANISM_DEBUG, "\nnew strike queued for woodpecker mechanism - vel:");
    dprint(PRINT_MECHANISM_DEBUG, vel);
}

// TODO - overload to call queue rotate as well
void Woodpecker::rotate(int len) {
    // this is the basic function which will cause the motor to rotate the wood disk
    forward_remaining = (int)((float)len * 0.7);
    backward_remaining = (int)((float)len * 0.3);
    dprint(PRINT_MECHANISM_DEBUG, "forward/backward remaining on rotate call is: ");
    dprint(PRINT_MECHANISM_DEBUG, forward_remaining);
    dprint(PRINT_MECHANISM_DEBUG, "\t");
    dprintln(PRINT_MECHANISM_DEBUG, backward_remaining);
    state = STATE_MOVE_MOTOR;
}

bool Woodpecker::strike(double vel) {
    queueStrike(vel);
    return strike();
}

bool Woodpecker::strike() {
    // by changing the state to STATE_SOL_ACTIVE, the update loop will handle the
    // solenoid strike given the current class values
    update();
    if (state == STATE_INACTIVE) {
        state = STATE_SOL_ACTIVATE;
        return true;
    }else {
        dprint(PRINT_MECHANISM_DEBUG, "WARNING - strike() message ignored by Woodpecker as current state is ");
        dprintln(PRINT_MECHANISM_DEBUG, state);
        return false;
    }
}

void Woodpecker::update() {
    switch (state) {
        case STATE_INACTIVE:
            // dprint(PRINT_MECHANISM_DEBUG, ".");
            digitalWrite(sol_pin, LOW);
            break;
        case STATE_SOL_ACTIVATE:
            if (last_action > sol_min_time_between_strikes) {
                digitalWrite(sol_pin, HIGH);
                state = STATE_SOL_ACTIVE;
                dprint(PRINT_MECHANISM_DEBUG, last_action);
                dprint(PRINT_MECHANISM_DEBUG, " STATE_SOL_ACTIVE\t");
                last_action = 0;
            }
            break;

        case STATE_SOL_ACTIVE:
            if (last_action > velocity) {
                digitalWrite(sol_pin, LOW);
                state = STATE_INACTIVE;
                dprint(PRINT_MECHANISM_DEBUG, last_action);
                dprint(PRINT_MECHANISM_DEBUG, " STATE_INACTIVE\t");
                last_action = 0;
            }
            break;

        case STATE_MOVE_MOTOR:
            // this is for when the motor is flagged to move forward but is not moving yet
            last_action = 0;
            motors->enableDrivers(mot_num);
            motors->setSpeed(mot_num, forward_speed);
            state = STATE_MOVING_MOTOR_FORWARD;
            dprintln(PRINT_MECHANISM_DEBUG, " STATE_MOVING_MOTOR_FORWARD\t");
            break;

        case STATE_MOVING_MOTOR_FORWARD:
            if (last_action > forward_remaining) {
                motors->setSpeed(mot_num, backward_speed);
                last_action = 0;
                state = STATE_MOVING_MOTOR_BACKWARD;
                dprint(PRINT_MECHANISM_DEBUG, " STATE_MOVING_MOTOR_BACKWARD\t");
            } else {
                dprint(PRINT_MECHANISM_DEBUG, "still moving forward\t");
                dprint(PRINT_MECHANISM_DEBUG, last_action);
                dprint(PRINT_MECHANISM_DEBUG, "\t");
                dprintln(forward_remaining);
            }
            break;

        case STATE_MOVING_MOTOR_BACKWARD:
            if (last_action > backward_remaining) {
                motors->setSpeed(mot_num, 0);
                motors->disableDrivers(mot_num);
                last_action = 0;
                dprintln(PRINT_MECHANISM_DEBUG, " STATE_INACTIVE\t");
                state = STATE_INACTIVE;
            }
            break;
    }
}

#endif
