#ifndef __DATALOG_H__
#define __DATALOG_H__

#include "../Configuration.h"
#include <EEPROM.h>

class Datalog {
    private:
        // to reduce code on the overloaded init functions
        void _setup(String, uint32_t, int, bool);
        // for keeping track of index moving for autolog
        bool moving_index = false;
        // this is the user assigned name to the datalog
        String id = "NO ID ASSIGNED YET";
        // for keeping track of what type of a log it is, there is UPDATING_LOG and ONE_TIME_LOG
        // the updating one will use a timer to update dependent on the amount of time which has passed since the last update
        bool type;
        uint8_t data_type;
        uint8_t value_size;

        // todo ...
        bool autolog_active = true;

        // references to the variables we want to track
        double   *dval;
        uint8_t  *bval;
        uint16_t *sval;
        uint32_t *lval;

        // for keeping track if the log is active
        bool active = true;

        // for keeping track of where to write
        uint32_t addr;
        unsigned int start_addr; // the starting addr
        unsigned int end_addr;   // if multiple values are stored by this log, then this is the end addr.
        uint32_t log_length;

        // writing to EEPROM
        void writeDouble(double data);
        void writeDouble();
        void writeShort(uint16_t data);
        void writeShort();
        void writeLong(uint32_t data);
        void writeLong();

        // reading from EEPROM
        double    readDouble(int);
        double    readDouble();
        uint16_t  readShort(int);
        uint16_t  readShort();
        uint32_t  readLong(int);
        uint32_t  readLong();
    public:
        Datalog();
        ~Datalog();
        Datalog(String _id, uint32_t address, double *val, int length, bool _auto);
        Datalog(String _id, uint32_t address, uint8_t *val, int length, bool _auto);
        Datalog(String _id, uint32_t address, uint16_t *val, int length, bool _auto);
        Datalog(String _id, uint32_t address, uint32_t *val, int length, bool _auto);

        void setAutolog(bool v){ autolog_active = v;};

        bool update(); // could this be a while loop at some point?

        String getName() {return id;};

        bool write(double data);
        bool write(uint8_t data);
        bool write(uint16_t data);
        bool write(uint32_t data);

        bool writeCheck(double);
        bool writeCheck(uint8_t);
        bool writeCheck(uint16_t);
        bool writeCheck(uint32_t);

        void clear();
        void printLog(uint8_t lines);
        void printlog(uint8_t lines);
        void printlog(){printlog(4);};

};
Datalog::Datalog(){};
Datalog::~Datalog(){};

//////////////////////// High Level Methods /////////////////////////////////
Datalog::Datalog(String _id, uint32_t address, double *val, int length, bool move) {
    data_type = type = DATATYPE_DOUBLE;
    value_size = 4;
    dval = val;
    _setup(_id, address, length, move);
}

Datalog::Datalog(String _id, uint32_t address, uint8_t *val, int length, bool move) {
    data_type = type = DATATYPE_BYTE;
    value_size = 1;
    bval = val;
    _setup(_id, address, length, move);
}

Datalog::Datalog(String _id, uint32_t address, uint16_t *val, int length, bool move) {
    data_type = type = DATATYPE_SHORT;
    value_size = 2;
    sval = val;
    _setup(_id, address, length, move);
}

Datalog::Datalog(String _id, uint32_t address, uint32_t *val, int length, bool move) {
    data_type = DATATYPE_LONG;
    type = DATATYPE_LONG;
    value_size = 4;
    lval = val;
    _setup(_id, address, length, move);
    Serial.print("Created a new long datalog with a starting value of : ");Serial.println(*val);
}

// to save code repitition
void Datalog::_setup(String _id, uint32_t address, int length, bool move) {
    start_addr = address;
    addr = address;
    id = _id;
    log_length = length * value_size;
    end_addr = address + log_length;
    moving_index = move;
}

//////////////////////// Writing Methods /////////////////////////////////
// to do this is aweful, need to rewrite todo
// store the dat with least significant bytes in lower index
void Datalog::writeDouble(double data) {
    uint8_t b[4];
    uint32_t d = data * DOUBLE_PRECISION;
    for (int i = 0; i < 4; i++) {
        b[i] = (d >> i * 8) & 0x00FF;
        EEPROM.update(addr + i, b[i]);
    }
}

void Datalog::writeDouble() {
    // to do this is aweful, need to rewrite todo
    // store the dat with least significant bytes in lower index
    uint8_t b[4];
    uint32_t d = *dval * DOUBLE_PRECISION;
    for (int i = 0; i < 4; i++) {
        b[i] = (d >> i * 8) & 0x00FF;
        EEPROM.update(addr + i, b[i]);
    }
}

void Datalog::writeShort(uint16_t data) {
    // store the dat with least significant bytes in lower index
    uint8_t lsb = data & 0x00FF;
    uint8_t msb = data >> 8;
    EEPROM.update(addr, lsb);
    EEPROM.update(addr + 1, msb);
    writeCheck(*sval);
}

void Datalog::writeShort() {
    // store the dat with least significant bytes in lower index
    uint8_t lsb = *sval & 0x00FF;
    uint8_t msb = *sval >> 8;
    EEPROM.update(addr, lsb);
    EEPROM.update(addr + 1, msb);
    writeCheck(*sval);
}

void Datalog::writeLong(uint32_t data) {
    uint8_t b[4];
    for (int i = 0; i < 4; i++) {
        b[i] = data >> 8 * i;
        EEPROM.update(addr + i, b[i]);
    }
    writeCheck(data);
}

void Datalog::writeLong() {
    uint8_t b[4];
    uint32_t v = *lval;
    for (int i = 0; i < 4; i++) {
        b[i] = v >> 8 * i;
        EEPROM.update(addr + i, b[i]);
    }
    writeCheck(v);
}

//////////////////////// Reading Methods /////////////////////////////////
double Datalog::readDouble(int a) {
    uint32_t data = EEPROM.read(a + 3);
    for (int i = 2; i > -1; i--) {
        uint8_t reading = EEPROM.read(a + i);
        // dprint(PRINT_LOG_WRITE, reading);
        // dprint(PRINT_LOG_WRITE, "|");
        data = (data << 8) | reading;
    }
    return (double)data / DOUBLE_PRECISION;
}

double Datalog::readDouble() {
    uint32_t data = EEPROM.read(addr + 3);
    for (int i = 2; i > -1; i--) {
        uint8_t reading = EEPROM.read(addr + i);
        // dprint(PRINT_LOG_WRITE, reading);
        // dprint(PRINT_LOG_WRITE, "|");
        data = (data << 8) | reading;
    }
    return (double)data / DOUBLE_PRECISION;
}

uint16_t Datalog::readShort(int a) {
    int data = EEPROM.read(a + 1);
    data = (data << 8) + EEPROM.read(a);
    return data;
}

uint16_t Datalog::readShort() {
    int data = EEPROM.read(addr + 1);
    data = (data << 8) + EEPROM.read(addr);
    return data;
}

uint32_t Datalog::readLong(int a) {
    uint32_t data = 0;
    for (int i = 0; i < 4; i++) {
        uint32_t n = EEPROM.read(a + i) << 8 * i;
        data = n | data;
    }
    return data;
}

uint32_t Datalog::readLong() {
    uint32_t data = 0;
    for (int i = 0; i < 4; i++) {
        uint32_t n = EEPROM.read(addr + i) << 8 * i;
        data = n | data;
    }
    return data;
}

//////////////////////// Data Checking Methods /////////////////////////
bool Datalog::writeCheck(double data) {
    if (EEPROM_WRITE_CHECK) {
        double temp = readDouble(addr);
        dprint(PRINT_LOG_WRITE, "data check:\t");
        dprint(PRINT_LOG_WRITE, data);
        dprint(PRINT_LOG_WRITE, "\t");
        dprintln(PRINT_LOG_WRITE, temp);
        if (data != temp) {
            return false;
        }
        else {return true;};
    }
    return false;
}

bool Datalog::writeCheck(uint8_t data) {
    if (EEPROM_WRITE_CHECK) {
        uint8_t temp = EEPROM.read(addr);
        dprint(PRINT_LOG_WRITE, "data check:\t");
        dprint(PRINT_LOG_WRITE, data);
        dprint(PRINT_LOG_WRITE, "\t");
        dprintln(PRINT_LOG_WRITE, temp);
        if (data != temp) {
            return false;
        }
        else {return true;};
    }
    return false;
}

bool Datalog::writeCheck(uint16_t data) {
    if (EEPROM_WRITE_CHECK) {
        uint16_t temp = readShort(addr);
        dprint(PRINT_LOG_WRITE, "data check:\t");
        dprint(PRINT_LOG_WRITE, data);
        dprint(PRINT_LOG_WRITE, "\t");
        dprintln(PRINT_LOG_WRITE, temp);
        if (data != temp) {
            return false;
        }
        else {return true;};
    }
    return false;
}

bool Datalog::writeCheck(uint32_t data) {
    if (EEPROM_WRITE_CHECK) {
        uint32_t temp = readLong(addr);
        dprint(PRINT_LOG_WRITE, "data check:\t");
        dprint(PRINT_LOG_WRITE, data);
        dprint(PRINT_LOG_WRITE, "\t");
        dprintln(PRINT_LOG_WRITE, temp);
        if (data != temp) {
            return false;
        }
        else {return true;};
    }
    return false;
}

//////////////////////// Misc. Methods /////////////////////////////////

bool Datalog::update() {
    // if the log is currently active
    dprint(PRINT_LOG_WRITE, id);
    if (active == false) {
        // dprint(PRINT_LOG_WRITE, id);
        dprint(PRINT_LOG_WRITE, " not active, not updating");
        return 0;
    }
    // if we got this far then everything is good for an update
    dprint(PRINT_LOG_WRITE, " updated ");
    // dprint(PRINT_LOG_WRITE, id);
    switch(data_type) {
        case DATATYPE_SHORT:
            writeShort();
            dprintln(PRINT_LOG_WRITE, *sval);
            break;
        case DATATYPE_DOUBLE:
            writeDouble();
            dprintln(PRINT_LOG_WRITE, *dval);
            break;
        case DATATYPE_BYTE:
            EEPROM.update(addr, *bval);
            dprintln(PRINT_LOG_WRITE, *bval);
            break;
        case DATATYPE_LONG:
            writeLong();
            dprintln(PRINT_LOG_WRITE, *lval);
            break;
    }
    // print some feedback if the appropiate flag is set
    /////////// autolog stuff
    if (autolog_active == true && moving_index == true) {
        // increment the addr counter
        addr += value_size;
        // if the current address plus the data_length is greater than the end index and 
        // if the datalogger is on autolog then stop the autologger
        if (addr + value_size > end_addr) {
            autolog_active = false;
            active = false;
            dprintln(PRINT_LOG_WRITE, "Autolog deactivated, allocated memory has been used up");
        }
    }
    return 1;
}

void Datalog::clear() {
    for (unsigned int i = start_addr; i < end_addr; i++) {
        EEPROM.update(i, 0);
    }
    dprint(PRINT_LOG_WRITE, "Cleared the "); dprint(PRINT_LOG_WRITE, id);
    dprintln(PRINT_LOG_WRITE, " Datalog");
}


void Datalog::printlog(uint8_t lines) {
    printLog(lines);
}

void Datalog::printLog(uint8_t lines) {
    //printDivide(PRINT_LOG_WRITE);
    dprint(PRINT_LOG_WRITE, "Printing the ");
    dprint(PRINT_LOG_WRITE, id);
    uint32_t per_line;
    if (end_addr > start_addr) {
        per_line = (end_addr - start_addr) / value_size / lines;
    } else {
        per_line =  1;
    }
    dprint(PRINT_LOG_WRITE, " from start/end addr : ");
    dprint(PRINT_LOG_WRITE, start_addr);dprint(PRINT_LOG_WRITE,"-");
    dprint(PRINT_LOG_WRITE, end_addr);
    if (end_addr - start_addr > 4)  {
        dprintln(PRINT_LOG_WRITE, "\t");
    } else  {
        dprint(PRINT_LOG_WRITE, "\t");
    }
    uint8_t itters = 0;
    double d = 0.0;
    uint32_t l = 0;
    uint16_t iv = 0;
    uint8_t b = 0;

    for (uint16_t i = start_addr; i < end_addr; i += value_size) {
        itters++;
        switch(data_type){
            case DATATYPE_LONG:
                l = readLong(i);
                dprint(PRINT_LOG_WRITE, l);
                break;
            case DATATYPE_BYTE:
                b = EEPROM.read(i);
                dprint(PRINT_LOG_WRITE, b);
                break;
            case DATATYPE_SHORT:
                iv = readShort(i);
                dprint(PRINT_LOG_WRITE, iv);
                break;
            case DATATYPE_DOUBLE:
                d = readDouble(i);
                dprint(PRINT_LOG_WRITE, d);
                break;
        }
        // print tab or line depending on how many prints have occured so far
        if (itters % per_line == 0) {
            dprintln(PRINT_LOG_WRITE);
        } else {
            dprint(PRINT_LOG_WRITE, "\t");
        }
    }
    dprintln(PRINT_LOG_WRITE);
}

#endif // __DATALOG_CONF_H__
