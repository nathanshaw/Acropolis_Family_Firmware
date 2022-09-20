#ifndef __DLMANAGER_H__
#define __DLMANAGER_H__

#include "../Configuration.h"
#include <Arduino.h>
#include <EEPROM.h>
#include "Datalog.h"

////////////////////////// Struct for One-Off logs ///////////////////////

struct oneofflog {
    String readMsg;
    uint32_t addr;
    uint8_t type;
    uint8_t len;
    bool written = false;
};

typedef struct oneofflog OneOffLog;

//////////////////////// Writing Methods /////////////////////////////////
// to do this is aweful, need to rewrite todo
// store the dat with least significant bytes in lower index
void writeDoubleToEEPROM(uint32_t addr, double data) {
    uint8_t b[4];
    uint32_t d = data * DOUBLE_PRECISION;
    for (int i = 0; i < 4; i++) {
      b[i] = (d >> i * 8) & 0x00FF;
      EEPROM.update(addr + i, b[i]);
    }
}

void writeShortToEEPROM(uint32_t addr, uint16_t data) {
    // store the dat with least significant bytes in lower index
    uint8_t lsb = data & 0x00FF;
    uint8_t msb = data >> 8;
    EEPROM.update(addr, lsb);
    EEPROM.update(addr + 1, msb);
}

void writeLongToEEPROM(uint32_t addr, uint32_t data) {
    uint8_t b[4];
    for (int i = 0; i < 4; i++) {
      b[i] = data >> 8 * i;
      EEPROM.update(addr + i, b[i]);
    }
}

//////////////////////// Reading Methods /////////////////////////////////
double readDoubleFromEEPROM(int a) {
  uint32_t data = EEPROM.read(a + 3);
  for (int i = 2; i > -1; i--) {
    uint8_t reading = EEPROM.read(a + i);
    // dprint(PRINT_LOG_WRITE, reading);
    // dprint(PRINT_LOG_WRITE, "|");
    data = (data << 8) | reading;
  }
  return (double)data / DOUBLE_PRECISION;
}

uint16_t readShortFromEEPROM(int a) {
  int data = EEPROM.read(a + 1);
  data = (data << 8) + EEPROM.read(a);
  return data;
}

uint32_t readLongFromEEPROM(int a) {
  uint32_t data = 0;
  for (int i = 0; i < 4; i++) {
    uint32_t n = EEPROM.read(a + i) << 8 * i;
    data = n | data;
  }
  return data;
}

class DLManager {
    public:
        // DLManager();
        DLManager();
        DLManager(String _id);
        ~DLManager();
        // DLManager(Datalog &l[], uint8_t);

        void logSetupConfigByte(String str, uint8_t data);
        void logSetupConfigShort(String str, uint16_t data);
        void logSetupConfigLong(String str, uint32_t data);
        void logSetupConfigDouble(String str, double data);

        void addAutolog(String, uint8_t, double *);
        void addAutolog(String, uint8_t, uint32_t *);

        void addStaticLog(String, uint8_t, double *);
        void addStaticLog(String, uint8_t, uint8_t *);
        void addStaticLog(String, uint8_t, uint16_t *);
        void addStaticLog(String, uint8_t, uint32_t  *);

        bool configureAutolog();
        void configureAutoTimer(uint8_t num, uint32_t start_delay, uint32_t log_time, uint32_t logs_requested);
        void configureStaticTimer(uint8_t num, uint32_t start_delay, uint32_t logging_rate);
        void update();

        void printAutologs();
        void printOneOffLogs();
        void printOneOffLog(OneOffLog log);
        void printAllLogs();

        uint32_t getTimerStart(uint8_t which) {return start_delays[which];};
        uint32_t getTimerEnd(uint8_t which) {
            return start_delays[which] + (remaining_logs[which] * log_refresh_length[which]);
        };
        uint32_t getTimerRate(uint8_t which) {return log_refresh_length[which];};

        void printTimerConfigs();
        void clearLogs();

        uint16_t getNextFreeAutologIdx() {return autolog_write_idx;};

    private:
        String id = "";
        // for keeping track of the datalogging shizzz
        uint32_t total_log_size = EEPROM_LOG_SIZE;

        void addLog(Datalog *, uint8_t);
        void addLog(Datalog, uint8_t);
        
        ///////////////////// Static Logs //////////////////
        void _addStaticLog(uint32_t log_size, String _id);

        ///////////////////// One-off Logs /////////////////
        // These are logs which are only intended to be written to once
        // It includes things such as setup configurations, software and hardware versions
        // and similar things, it is not for logs in which a single value is updated over the
        // runtime of the program, only for setup configurations.

        // how much memory there is to store the single value logs
        uint32_t one_off_start_idx = 0;
        uint32_t one_off_log_size = EEPROM_WRITE_ONCE_LOG_SIZE;
        // how much unallocated single value space is left?
        uint32_t remaining_one_off_size = one_off_log_size;
        // what is the next unallocated space?
        uint32_t next_one_off_start_idx = 0;

        // the structs which track the one off log data
        OneOffLog one_off_logs[EEPROM_WRITE_ONCE_LOG_SIZE/2];
        // what is the next open index for the once-off log
        uint16_t one_off_write_idx = 0;

        // the number of one_off_logs which have actually been stored
        uint16_t active_one_off_logs = 0;

        //////////// autolog ////////////////////
        uint32_t autolog_start_idx = one_off_log_size;
        uint32_t autolog_write_idx = autolog_start_idx;
        uint32_t remaining_autolog_space = total_log_size - one_off_log_size;
        void _addAutoLog(uint32_t log_size, String _id);

        // Start and stop indexes for each log
        Datalog logs[DATALOG_MANAGER_MAX_LOGS];
        bool first_reading[DATALOG_MANAGER_MAX_LOGS];
        uint8_t log_timer_map[DATALOG_MANAGER_MAX_LOGS]; // what timer is associated with the log
        uint8_t active_logs = 0;
        // for keeping track of which is the next index for a log to add
        uint8_t add_log_idx = 0;

        // for handling pointer logs
        Datalog *logs_p[DATALOG_MANAGER_MAX_LOGS];
        bool first_reading_p[DATALOG_MANAGER_MAX_LOGS];
        uint8_t log_timer_map_p[DATALOG_MANAGER_MAX_LOGS]; // what timer is associated with the log
        uint8_t active_logs_p = 0;
        // for keeping track of which is the next index for a log to add
        uint8_t add_log_idx_p = 0;

        // for keeping track of the timers / triggers
        elapsedMillis log_timers[DATALOG_MANAGER_TIMER_NUM];
        uint32_t log_refresh_length[DATALOG_MANAGER_TIMER_NUM]; // how long each timer lasts for
        uint32_t start_delays[DATALOG_MANAGER_TIMER_NUM];
        long remaining_logs[DATALOG_MANAGER_TIMER_NUM];
};

///////////////////////// constructors ///////////////////////////////////////////
DLManager::DLManager(){};
DLManager::~DLManager(){};
/*
DatalogManager::DatalogManager(Datalog &l[], uint8_t num_logs) {
    id = _id;
    for (int i = 0; i < DATALOG_MANAGER_MAX_LOGS; i++ ) {
        first_reading[i] = true;
    }
    for (int i = 0; i < num_logs; i++ ) {
        logs[i] = &l[i];
    }
}
*/

DLManager::DLManager(String _id) {
    id = _id;
    for (int i = 0; i < DATALOG_MANAGER_MAX_LOGS; i++ ) {
        first_reading[i] = true;
    }
}

/////////////////////// One-off logs ////////////////////////////////////////

void DLManager::logSetupConfigByte(String str, uint8_t data) {
    // if there is enough room in the log to track another value
    uint8_t dlen = 1;
    if (one_off_write_idx + dlen < one_off_start_idx + one_off_log_size) {
        // write the data to eeprom according to type
        uint8_t _data = EEPROM.read(one_off_write_idx);
        EEPROM.update(one_off_write_idx, data);
        // update a new struct with the log data
        one_off_logs[active_one_off_logs] = {str, one_off_write_idx, DATATYPE_BYTE, dlen, true};
        // incrment the one off timer write idx
        one_off_write_idx += dlen;
        // update the stored one off log active structs idx
        active_one_off_logs++;
        // dprint
        Serial.print("idx:");
        Serial.print(one_off_write_idx);
        Serial.print(" ");
        Serial.print("old/new ");
        Serial.print(str);
        Serial.print("\t");
        Serial.print(_data);
        Serial.print("\t");
        Serial.println(data);
    } else {
        Serial.println("ERROR - Sorry can't create any more one-off logs, there is not enough allocated space on the EEPROM for one-off logging");
    }
}

void DLManager::logSetupConfigShort(String str, uint16_t data) {
    // if there is enough room in the log to track another value
    uint8_t dlen = 2;
    if (one_off_write_idx + dlen < one_off_start_idx + one_off_log_size) {
        // write the data to eeprom according to type
        uint16_t _data = readShortFromEEPROM(one_off_write_idx);
        writeShortToEEPROM(one_off_write_idx, data);
        // update a new struct with the log data
        one_off_logs[active_one_off_logs] = {str, one_off_write_idx, DATATYPE_SHORT, dlen, true};
        // incrment the one off timer write idx
        one_off_write_idx += dlen;
        // update the stored one off log active structs idx
        active_one_off_logs++;
        // dprint
        Serial.print(one_off_write_idx);
        Serial.print("\t");
        Serial.print("old/new ");
        Serial.print(str);
        Serial.print("\t");
        Serial.print(_data);
        Serial.print("\t");
        Serial.println(data);
    } else {
        Serial.println("ERROR - Sorry can't create any more one-off logs, there is not enough allocated space on the EEPROM for one-off logging");
    }
}

void DLManager::logSetupConfigLong(String str, uint32_t data) {
    // if there is enough room in the log to track another value
    uint8_t dlen = 4;
    if (one_off_write_idx + dlen < one_off_start_idx + one_off_log_size) {
        // write the data to eeprom according to type.
        uint32_t _data = readLongFromEEPROM(one_off_write_idx);
        writeLongToEEPROM(one_off_write_idx, data);
        // update a new struct with the log data
        one_off_logs[active_one_off_logs] = {str, one_off_write_idx, DATATYPE_LONG, dlen, true};
        // incrment the one off timer write idx
        one_off_write_idx += dlen;
        // update the stored one off log active structs idx
        active_one_off_logs++;
        // dprint
        Serial.print(one_off_write_idx);
        Serial.print("\t");
        Serial.print("old/new ");
        Serial.print(str);
        Serial.print("\t");
        Serial.print(_data);
        Serial.print("\t");
        Serial.println(data);
    } else {
        Serial.println("ERROR - Sorry can't create any more one-off logs, there is not enough allocated space on the EEPROM for one-off logging");
    }
}

void DLManager::logSetupConfigDouble(String str, double data) {
    // if there is enough room in the log to track another value
    uint8_t dlen = 4;
    if (one_off_write_idx + dlen < one_off_start_idx + one_off_log_size) {
        // write the data to eeprom according to type
        double _data = readDoubleFromEEPROM(one_off_write_idx);
        writeDoubleToEEPROM(one_off_write_idx, data);
        // update a new struct with the log data
        one_off_logs[active_one_off_logs] = {str, one_off_write_idx, DATATYPE_DOUBLE, dlen, true};
        // incrment the one off timer write idx
        one_off_write_idx += dlen;
        // update the stored one off log active structs idx
        active_one_off_logs++;
        // dprint
        Serial.print(one_off_write_idx);
        Serial.print("\t");
        Serial.print("old/new ");
        Serial.print(str);
        Serial.print("\t");
        Serial.print(_data);
        Serial.print("\t");
        Serial.println(data);
    } else {
        Serial.println("ERROR - Sorry can't create any more one-off logs, there is not enough allocated space on the EEPROM for one-off logging");
    }
}

 /////////////////////////////// Static Logs ////////////////////////////////////////////
void DLManager::_addStaticLog(uint32_t log_size, String _id){
        autolog_write_idx += log_size;
        remaining_autolog_space -= log_size;
        printMinorDivide();
        Serial.println(_id);
        Serial.print("Adding a new static log with size       :\t");Serial.println(log_size);
        Serial.print("remaining static log (autolog) space    :\t");Serial.println(remaining_autolog_space);
        Serial.print("starting EEPROM idx                     :\t");Serial.println(autolog_write_idx - log_size);
        printMinorDivide();
}

void DLManager::addStaticLog(String _id,  uint8_t _timer, double*_val) {
    uint32_t log_size = 4;
    if (log_size < remaining_autolog_space) {
        Datalog log = Datalog(_id, autolog_write_idx, _val, 1, false);
        addLog(log, _timer);
        log.printLog(1);
        _addStaticLog(log_size, _id);
    } else {
        Serial.println("ERROR - sorry the autolog is not initiated due to a lack of remaining EEPROM space.");
    }
}

void DLManager::addStaticLog(String _id,  uint8_t _timer, uint8_t*_val) {
    uint32_t log_size = 1;
    if (log_size < remaining_autolog_space) {
        Datalog log = Datalog(_id, autolog_write_idx, _val, 1, false);
        addLog(log, _timer);
        _addStaticLog(log_size, _id);
    } else {
        Serial.println("ERROR - sorry the autolog is not initiated due to a lack of remaining EEPROM space.");
    }
}

void DLManager::addStaticLog(String _id,  uint8_t _timer, uint16_t*_val) {
    uint32_t log_size = 2;
    if (log_size < remaining_autolog_space) {
        Datalog log = Datalog(_id, autolog_write_idx, _val, 1, false);
        addLog(log, _timer);
        _addStaticLog(log_size, _id);
    } else {
        Serial.println("ERROR - sorry the autolog is not initiated due to a lack of remaining EEPROM space.");
    }
}

void DLManager::addStaticLog(String _id,  uint8_t _timer, uint32_t*_val) {
    uint32_t log_size = 4;
    if (log_size < remaining_autolog_space) {
        Datalog log = Datalog(_id, autolog_write_idx, _val, 1, false);
        addLog(log, _timer);
        _addStaticLog(log_size, _id);
    } else {
        Serial.println("ERROR - sorry the autolog is not initiated due to a lack of remaining EEPROM space.");
    }
}

/////////////////////////////// Adding External Datalog Objects ////////////////////////////////

void DLManager::addLog(Datalog log, uint8_t timer_num) {
    if (add_log_idx >= DATALOG_MANAGER_MAX_LOGS) {
        Serial.print("ERROR, Datalog Manager can only handle ");
        Serial.print(DATALOG_MANAGER_MAX_LOGS);
        Serial.println(" logs at a time, resetting index to 0");
        add_log_idx = 0;
    }
    logs[add_log_idx] = log;
    log_timer_map[add_log_idx] = timer_num;
    active_logs++;
    active_logs = max(add_log_idx, active_logs);
    add_log_idx++;
    add_log_idx = min(add_log_idx, DATALOG_MANAGER_MAX_LOGS);
    dprint(PRINT_LOG_WRITE, "Added log to the datamanager under timer ");
    dprint(PRINT_LOG_WRITE, timer_num);dprint(PRINT_LOG_WRITE, " active_logs now: ");dprintln(PRINT_LOG_WRITE, active_logs);
    dprint(PRINT_LOG_WRITE, "log written under index :");
    dprint(PRINT_LOG_WRITE, add_log_idx - 1);
    dprint(PRINT_LOG_WRITE, " and the contents of the log (from the last runtime) is as follows: ");
    logs[active_logs - 1].printLog(1);
}

void DLManager::addLog(Datalog *log, uint8_t timer_num) {
    if (add_log_idx_p >= DATALOG_MANAGER_MAX_LOGS) {
        Serial.print("ERROR, Datalog Manager can only handle ");
        Serial.print(DATALOG_MANAGER_MAX_LOGS);
        Serial.println(" logs at a time, resetting index to 0");
        add_log_idx_p = 0;
    }
    logs_p[add_log_idx_p] = log;
    log_timer_map_p[add_log_idx_p] = timer_num;
    active_logs_p = max(add_log_idx_p, active_logs_p++);
    add_log_idx_p++;
    add_log_idx_p = min(add_log_idx_p, DATALOG_MANAGER_MAX_LOGS);
    dprint(PRINT_LOG_WRITE, "Added log to the datamanager under timer ");
    dprint(PRINT_LOG_WRITE, timer_num);
}

/////////////////////////////// Auto Logging ////////////////////////////////
void DLManager::_addAutoLog(uint32_t log_size, String _id) {
        autolog_write_idx += log_size;
        remaining_autolog_space -= log_size;
        Serial.print("Adding a new AutoLog ");Serial.print(_id);Serial.print(" with size:\t");
        Serial.println(log_size);
        Serial.print("remaining autolog space       :\t");Serial.println(remaining_autolog_space);
        Serial.print("starting EEPROM idx           :\t");Serial.println(autolog_write_idx - log_size);
        printMinorDivide();
}

void DLManager::addAutolog(String _id,  uint8_t _timer, double *_val) {
    uint32_t log_size = remaining_logs[_timer] * 4;
    if (log_size < remaining_autolog_space) {
        addLog(Datalog(_id, autolog_write_idx, _val, remaining_logs[_timer], true), _timer);
        _addAutoLog(log_size, _id);
    } else {
        Serial.println("ERROR - sorry the autolog is not initiated due to a lack of remaining EEPROM space.");
    }
}

void DLManager::addAutolog(String _id,  uint8_t _timer, uint32_t *_val) {
    uint32_t log_size = remaining_logs[_timer] * 4;
    if (log_size < remaining_autolog_space) {
        addLog(Datalog(_id, autolog_write_idx, _val, remaining_logs[_timer], true), _timer);
        _addAutoLog(log_size, _id);
    } else {
        Serial.println("ERROR - sorry the autolog is not initiated due to a lack of remaining EEPROM space.");
    }
}

/////////////////////////////// Update Functions ////////////////////////////////
void DLManager::update() {
    // update each timer one at a time
    for (int timer_num = 0; timer_num < DATALOG_MANAGER_TIMER_NUM; timer_num++) {
        // take the elapsedd millis value at start of loop to ensure that
        // all related logs either update or dont update at the same time
        unsigned long u_time = log_timers[timer_num];
        // if it is the first reading then only update if it has been longer than the start delay
        if ((first_reading[timer_num] == true && u_time > start_delays[timer_num]) || (first_reading[timer_num] == false )) {
            first_reading[timer_num] = false;
            // if it is time to update these logs then do so
            uint8_t updates = 0;
            for (int log = 0; log < active_logs; log++) {
                 if (log_timer_map[log] == timer_num && u_time > log_refresh_length[timer_num]) {
                    // Serial.print("Updating : ");Serial.println(logs[log].getName());
                    logs[log].update();
                    updates++;
                 }
            }
            for (int log = 0; log < active_logs_p; log++) {
                 if (log_timer_map_p[log] == timer_num && u_time > log_refresh_length[timer_num]) {
                    // Serial.print("Updating : ");Serial.println(logs[log].getName());
                    logs_p[log]->update();
                    updates++;
                 }
            }
            if (updates > 0) {
                log_timers[timer_num] = 0;
                remaining_logs[timer_num]--;
            }
        }
    }
}
//
/////////////////////////////////// Utility Functions /////////////////////////////////////////
void DLManager::configureAutoTimer(uint8_t num, uint32_t start_delay, uint32_t log_time, uint32_t logs_requested) {
    log_refresh_length[num] = log_time / logs_requested;
    start_delays[num] = start_delay;
    remaining_logs[num] = logs_requested;
}

void DLManager::configureStaticTimer(uint8_t num, uint32_t start_delay, uint32_t logging_rate) {
    log_refresh_length[num] = logging_rate;
    start_delays[num] = start_delay;
    remaining_logs[num] = 100000;// the number of writes which are allowed before the EEPROM turns bad
}

void DLManager::clearLogs() {
    for (int i = 0; i <= active_logs; i++) {
        logs[i].clear();
    }
    for (int i = 0; i <= active_logs_p; i++) {
        logs_p[i]->clear();
    }
}

////////////////////////////////////// Printing Functions //////////////////////////////
void DLManager::printOneOffLog(OneOffLog log) {
    Serial.print(log.addr);printTab();
    Serial.print(log.readMsg);printTab();
    if (log.type == DATATYPE_BYTE) {
        Serial.print(EEPROM.read(log.addr));
    } else if (log.type == DATATYPE_SHORT) {
        Serial.print(readShortFromEEPROM(log.addr));
    } else if (log.type == DATATYPE_LONG) {
        Serial.print(readLongFromEEPROM(log.addr));
    } else if (log.type == DATATYPE_DOUBLE) {
        Serial.print(readDoubleFromEEPROM(log.addr));
    }
    printTab();
    Serial.print("written = ");
    Serial.println(log.written);
}

void DLManager::printOneOffLogs() {
    for (int i = 0; i < active_one_off_logs; i++) {
        printOneOffLog(one_off_logs[i]);
    }
    Serial.println();
}

void DLManager::printAutologs() {
    // note this also prints static logs
    for (int i = 0; i < active_logs; i++) {
        Serial.print("Log idx ");
        Serial.print(i);printTab();
        logs[i].printLog(4);
    }
    for (int i = 0; i < active_logs_p; i++) {
        Serial.print(i);Serial.print("p\t");
        logs_p[i]->printLog(8);
    }
    Serial.println();
}

void DLManager::printAllLogs() {
    printMajorDivide("All Logs Stored in EEPROM");
    Serial.println("----------- Printing One-off Logs --------------");
    printOneOffLogs();
    printMinorDivide();
    Serial.println("----------- Printing Auto and Static Logs --------------");
    printAutologs();
    printMajorDivide("Finished Printing EEPROM Contents");
}

void DLManager::printTimerConfigs()  {
    printMinorDivide();
    Serial.println("Printing the Timer Configurations for the Datalog Manager");
    for (int i = 0; i < DATALOG_MANAGER_TIMER_NUM; i++) {
        Serial.print(i);Serial.print(" current: ");Serial.print(log_timers[i]);Serial.print("\tmax\t");
        Serial.println(log_refresh_length[i]);
    }

    Serial.print("start_delays           :\t");
    for (int i = 0; i < DATALOG_MANAGER_TIMER_NUM; i++) {
        Serial.print(start_delays[i]);printTab();
    }
    Serial.println();

    Serial.print("remaining_logs        :\t");
    for (int i = 0; i < DATALOG_MANAGER_TIMER_NUM; i++) {
        Serial.print(remaining_logs[i]);printTab();

    }
    Serial.println();

    Serial.print("log_refresh_length   :\t");
    for (int i = 0; i < DATALOG_MANAGER_TIMER_NUM; i++) {
        Serial.print(log_refresh_length[i]);printTab();

    }
    Serial.println();

    Serial.print("log_timer_map       :\t");
    for (int i = 0; i < DATALOG_MANAGER_MAX_LOGS; i++) {
        Serial.print(log_timer_map[i]);Serial.print("\t");
    }
    Serial.println();
    printMinorDivide();
}
#endif // datalog_manager_h
