#ifndef EEPROM_H
#define EEPROM_H

// #include "config.h"

// TODO: replace this with an eeprom library that does wear leveling
#ifdef ENABLE_EEPROM

  #include <EEPROM.h>

  #define BAUD_IDX_ADDR 0x01
  #define CONFIG_IDX_ADDR 0x02

  void loadEeprom() {
    EEPROM.get(BAUD_IDX_ADDR, serialBaudIdx);
    if (serialBaudIdx >= (sizeof(baudRates)/sizeof(baudRates[0])))
      serialBaudIdx = DEFAULT_BAUD_IDX;
    serialBaud = baudRates[serialBaudIdx];


    EEPROM.get(CONFIG_IDX_ADDR, serialConfigIdx);
    if (serialConfigIdx >= (sizeof(serialConfigs)/sizeof(serialConfigs[0])))
      serialConfigIdx = DEFAULT_SERIAL_CONFIG_IDX;
    serialConfig = serialConfigs[serialConfigIdx].value;
  }

  void eepromSetup() {
    loadEeprom();
  }

  void resetEeprom() {
    EEPROM.put(BAUD_IDX_ADDR, -1);
    EEPROM.put(CONFIG_IDX_ADDR, -1);
  }

#endif
#endif
