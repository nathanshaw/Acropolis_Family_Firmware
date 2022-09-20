#ifndef __CONFIGURATION_DATALOGGING_H__
#define __CONFIGURATION_DATALOGGING_H__

////////////////////////////////////////////////////////////////////////////
///////////////////////// Datalog Settings /////////////////////////////////
////////////////////////////////////////////////////////////////////////////
double runtime;
bool data_logging_active =              true;

// TODO
// if this is set to true the hardware settings in the Configuration_hardawre.h file will be writtent o EEPROM
// if this is set to false then those settings will be read from the EEPROM 
// this should include things like the number of lux sensors, number of neopixels, etc
#define WRITE_HARDWARE_SETTINGS         true

// does the autolog get written over each time?
#define CLEAR_EEPROM_CONTENTS           1

// how long will each of the four different auto-log options be? 
// // a -1 means that the log will keep updating forever
#define DATALOG_1_LENGTH                20
#define DATALOG_2_LENGTH                20
#define DATALOG_3_LENGTH                100000
#define DATALOG_4_LENGTH                100000

// record the run time // last value is number of minutes
#define DATALOG_TIMER_1                 (1000*60*30)
#define DATALOG_TIMER_2                 (1000*60*60*1)
#define DATALOG_TIMER_3                 (1000*60*0.5)
#define DATALOG_TIMER_4                 (1000*60*10)

// how long the program runs for before the datalog starts logging
#define DATALOG_START_DELAY_1           (1000*60*0.1)
#define DATALOG_START_DELAY_2           (1000*60*0.1)
#define DATALOG_START_DELAY_3           (1000*60*0.1)
#define DATALOG_START_DELAY_4           (1000*60*0.1)

// how long the data logging  will last for
#define DATALOG_TIME_FRAME_1            (1000*60*60*1) // two hours
#define DATALOG_TIME_FRAME_2            (1000*60*60*1) // two hours
// #define DATALOG_TIME_FRAME_3         (1000*60*60*0.1)
// #define DATALOG_TIME_FRAME_4         (1000*60*60*1)

// refresh rates for the static logs
#define STATICLOG_RATE_FAST             (1000*60*0.25)
#define STATICLOG_RATE_SLOW             (1000*60*12)

////////////////////////////////////////////////////////////////////////////
/////////////////////////// auto logging ///////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// will the lux readings be logged?
#define AUTOLOG_LUX_F                   1
#define AUTOLOG_LUX_R                   1
#define AUTOLOG_LUX_TIMER               1

// the ratio of on vs off time for the neopixels
#define AUTOLOG_LED_ON_OFF              1
#define AUTOLOG_LED_ON_OFF_TIMER        1

// the number of values to store in the logging process
#define AUTOLOG_FLASHES                 0
#define AUTOLOG_FLASHES_TIMER           0

// the number of values to store in the logging process
#define AUTOLOG_FPM                     0
#define AUTOLOG_FPM_TIMER               0

// the brightness scaler avg log
#define AUTOLOG_BRIGHTNESS_SCALER       1
#define AUTOLOG_BRIGHTNESS_SCALER_TIMER 0

////////////////////////////////////////////////////////////////////////////
/////////////////////////// static logging /////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#define STATICLOG_MASTER_GAIN           1
#define STATICLOG_LUX_VALUES            1
#define STATICLOG_FLASHES               1
#define STATICLOG_RUNTIME               1
#define STATICLOG_RGB_AVG_VALS          1

#define STATICLOG_LUX_MIN_MAX_TIMER     1
#define STATICLOG_ONSET_GAIN_TIMER      1
#define STATICLOG_SONG_GAIN_TIMER       1
#define STATICLOG_FLASHES_TIMER         1
#define STATICLOG_RUNTIME_TIMER         1
#define STATICLOG_RGB_AVG_VALS_TIMER    1

////////////////////////////////////////////////////////////////////////////
/////////////////////////// Datalog Manager ////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// calculate the actual start and end times based on this
#define EEPROM_LOG_SIZE                 2000
// How much space will be allocated for the write once logs
#define EEPROM_WRITE_ONCE_LOG_SIZE      400
#define AUTO_LOG_SPACE                  (EEPROM_LOG_SIZE - WRITE_ONCE_LOG_SPACE)

#define DATALOG_MANAGER_MAX_LOGS        50
#define DATALOG_MANAGER_TIMER_NUM       4
uint8_t datalog_timer_num =             DATALOG_MANAGER_TIMER_NUM;
uint32_t datalog_timer_lens[4] =        {DATALOG_TIMER_1, DATALOG_TIMER_2, DATALOG_TIMER_3, DATALOG_TIMER_4};
#define DOUBLE_PRECISION                ((double)100000.0)

#endif // __CONFIGURATION_DATALOGGING_H__
