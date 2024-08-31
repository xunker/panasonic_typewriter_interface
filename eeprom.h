#ifndef EEPROM_H
#define EEPROM_H

// #include "config.h"

// TODO: replace this with an eeprom library that does wear leveling
#ifdef ENABLE_EEPROM

  #include <EEPROM.h>

  #define BAUD_IDX_ADDR 0x01

  void loadEeprom() {
    EEPROM.get(BAUD_IDX_ADDR, serialBaudIdx);
    if (serialBaudIdx >= (sizeof(baudRates)/sizeof(baudRates[0])))
      serialBaudIdx = DEFAULT_BAUD_IDX;
    serialBaud = baudRates[serialBaudIdx];
  }

  void eeprom_setup() {
    loadEeprom();
  }

  void configWrite(void) {
    EEPROM.put(BAUD_IDX_ADDR, serialBaudIdx);
    configSerial.Print (F("Written.\r\n"));
  }

  void configLoad(void) {
    loadEeprom();
    configSerial.Print (F("Loaded.\r\n"));
  }

#endif
#endif
