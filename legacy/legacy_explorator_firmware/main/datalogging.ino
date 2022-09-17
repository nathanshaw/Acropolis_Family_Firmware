// evry five minutes
#define DATALOG_RATE 300000
elapsedMillis datalog_timer;

// address locations for the datalogging 
#define EEPROM_RUN_TIME 1000

void updateDatalog() {
  // log how long the program has been running for
  if (datalog_timer > DATALOG_RATE && DATALOG) {
    writeLongToEEPROM(EEPROM_RUN_TIME, millis());
  }
}

void printDatalogOnBoot() {
  if (PRINT_EEPROM_ON_BOOT) {
    Serial.print("The program ran for : ");
    Serial.println(EEPROM.read(EEPROM_RUN_TIME)/60000);
    Serial.println(" minutes");
  }
}

void writeLongToEEPROM(unsigned int addr, unsigned long data) {
  // store the data with the least significant bytes in lower index
  uint8_t bytes[4];
  for (int i = 0; i < 4; i ++) {
    bytes[i] = (data >> 8*i) & 0x000000FF; 
    EEPROM.write(addr + i, bytes[i]);
  }
}

unsigned long readLongFromEEPROM(unsigned int addr) { 
  unsigned long data = EEPROM.read(addr+3);
  data = (data << 8) + EEPROM.read(addr+2);
  data = (data << 8) + EEPROM.read(addr+1);
  data = (data << 8) + EEPROM.read(addr);
  return data;
}

void writeTwoBytesToEEPROM(unsigned int addr, uint16_t data) {
  // store the dat with least significant bytes in lower index
  uint8_t lsb = data & 0x00FF;
  uint8_t msb = data >> 8;
  EEPROM.write(addr, lsb);
  EEPROM.write(addr + 1, msb);
}

uint16_t read16BitsFromEEPROM(unsigned int addr) {
  uint16_t data = EEPROM.read(addr + 1);
  data = (data << 8) + EEPROM.read(addr);
  return data;
}

void writeDoubleToEEPROM(unsigned int addr, double data) {
  // store the dat with least significant bytes in lower index
  uint16_t d = data * 100;
  uint8_t lsb = d & 0x00FF;
  uint8_t msb = d >> 8;
  EEPROM.write(addr, lsb);
  EEPROM.write(addr + 1, msb);
}

double readDoubleFromEEPROM(unsigned int addr) {
  uint16_t data = EEPROM.read(addr + 1);
  data = (data << 8) + EEPROM.read(addr);
  return (double)data / 100;
}
