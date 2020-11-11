#ifndef __BELL_MECHANISM_H__
#define __BELL_MECHANISM_H__

class Mechanism {
    public:
        Mechanism();
        virtual void update();

    private:
        virtual elapsedMillis last_action;
}

Mechanism::Mechanism() {

}

#endif

