#ifndef _CONFIGURATION_ADV_H_
#define _CONFIGURATION_ADV_H_

// 1 is v 1.0 (april, 2020 with the 9 solenoid and 3 motor outputs)
#define PCB_REVISION          1

// Body types
#define BELL_BOT              0
#define WOODPEECKER_BOT       1
// the body type determines the actuator configuration
// for Bell Bot six solenoids are used, with solenoids 0, 3, 6 as the strikers and 1, 4, 7 as the dampeners
#define BODY_TYPE             BELL_BOT



#endif
