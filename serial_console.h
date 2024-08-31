#ifndef ENABLE_CONSOLE_H
#define ENABLE_CONSOLE_H

// #include "config.h"

#ifdef ENABLE_CONSOLE

#include <SerialCmd.h> // https://github.com/gpb01/SerialCmd

#include <MemoryUsage.h> // https://github.com/Locoduino/MemoryUsage

#define SERIALCMD_FORCEUC 0 // do not force uppercase commands
#define SERIALCMD_CR      0x0D // command terminator, Carriage Return (char)
#define SERIALCMD_SPACE   " " // Use space as command->separator
#define LINE_ENDING "\r\n"

SerialCmd consoleSerial( Serial, SERIALCMD_CR, SERIALCMD_SPACE );

bool consoleEnabled = true; // to be moved elsewhere, trigged by button or something
bool consoleEnMessageSent = false;

int8_t ret;

void configHelp(void) {
    consoleSerial.Print(F("Commands:\r\n"));
    consoleSerial.Print(F("baud [rate]\tget or set the Serial Baud rate\r\n"));
    consoleSerial.Print(F("show\t\tprint current config\r\n"));
    #ifdef ENABLE_EEPROM
      consoleSerial.Print(F("load\t\tload config from EEPROM\r\n"));
      consoleSerial.Print(F("write\t\twrite current config to EEPROM\r\n"));
    #endif
    consoleSerial.Print(F("?\t\tlist commands\r\n"));
  }

  void configShow(void) {
    consoleSerial.Print(F("Current config:\r\n"));
    consoleSerial.Print(F("\tbaud\t")); consoleSerial.Print (serialBaud); consoleSerial.Print(F(" (index: "));
    consoleSerial.Print(serialBaudIdx); consoleSerial.Print(F(")\r\n"));

    consoleSerial.Print(F("\tconfig\t"));
    consoleSerial.Print(serialConfigs[serialConfigIdx].label);
     consoleSerial.Print(F(" (index: "));
     consoleSerial.Print(serialConfigIdx);
    consoleSerial.Print(F(")\r\n"));

    consoleSerial.Print(F("\r\n"));
  }

  void configInfo(void) {
    consoleSerial.Print(F("Compiled with:\r\n"));
    #ifdef ENABLE_DEBUGGING
      consoleSerial.Print(F("\tserial debugging\r\n"));
    #endif

    #ifdef ENABLE_CONSOLE
      consoleSerial.Print(F("\tserial console\r\n"));
    #endif

    #ifdef ENABLE_EEPROM
      consoleSerial.Print(F("\teeprom support\r\n"));
    #endif

    consoleSerial.Print(F("Memory:\r\n"));

    consoleSerial.Print(F("\tStack size: ")); consoleSerial.Print((int) RAMEND - (int)SP); consoleSerial.Print(F("\r\n"));

    consoleSerial.Print(F("\tFree ram: ")); consoleSerial.Print((int) SP - (int) (__brkval == 0 ? (int)&__heap_start : (int)__brkval)); consoleSerial.Print(F("\r\n"));

    consoleSerial.Print(F("\tSRAM size: ")); consoleSerial.Print((int) RAMEND - (int) &__data_start); consoleSerial.Print(F("\r\n"));
  }

  void configBaud(void) {
    char * sParam;
    sParam = consoleSerial.ReadNext();
    if (!( sParam == NULL )) {
      uint32_t newSerialBaud = strtoul ( sParam, NULL, 10 ); // (str, str_end char, base)
      short newSerialBaudIdx = -1;
      for (uint8_t idx = 0; idx < (sizeof(baudRates)/sizeof(baudRates[0])); idx++) {
        if (baudRates[idx] == newSerialBaud) {
          newSerialBaudIdx = idx;
        }
      }

      if (newSerialBaudIdx < 0) {
        consoleSerial.Print (F("Error: valid baud rates are:\r\n"));
        for (uint8_t idx = 0; idx < (sizeof(baudRates)/sizeof(baudRates[0])); idx++) {
          consoleSerial.Print(baudRates[idx]); consoleSerial.Print(F(" "));
        }
        consoleSerial.Print(F("\r\n"));

        return;
      }

      serialBaud = baudRates[newSerialBaudIdx];
      serialBaudIdx = newSerialBaudIdx;
    }

    consoleSerial.Print(serialBaud);
    consoleSerial.Print(F(LINE_ENDING));
  }

  void configConfig(void) {
    char * sParam;
    sParam = consoleSerial.ReadNext();
    if (!( sParam == NULL )) {
      short newConfigIdx = -1;
      for (uint8_t idx = 0; idx < (sizeof(serialConfigs)/sizeof(serialConfigs[0])); idx++) {
        if (strcmp(serialConfigs[idx].label, sParam) == 0) {
          newConfigIdx = idx;
        }
      }

      if (newConfigIdx < 0) {
        consoleSerial.Print (F("Error: valid configs are:\r\n"));
        for (uint8_t idx = 0; idx < (sizeof(serialConfigs)/sizeof(serialConfigs[0])); idx++) {
          consoleSerial.Print(serialConfigs[idx].label); consoleSerial.Print(F(" "));
        }
        consoleSerial.Print(F("\r\n"));

        return;
      }

      serialConfig = serialConfigs[newConfigIdx].value;
      serialConfigIdx = newConfigIdx;
    }

    consoleSerial.Print(serialConfigs[serialConfigIdx].label);
    consoleSerial.Print(F(LINE_ENDING));
  }

  void configWrite(void) {
    EEPROM.put(BAUD_IDX_ADDR, serialBaudIdx);
    EEPROM.put(CONFIG_IDX_ADDR, serialConfigIdx);

    #ifdef ENABLE_CONSOLE
      consoleSerial.Print (F("Written.\r\n"));
    #endif
  }

  void configLoad(void) {
    loadEeprom();
    #ifdef ENABLE_CONSOLE
      consoleSerial.Print (F("Loaded.\r\n"));
    #endif
  }

  void configReset(void) {
    resetEeprom();
    #ifdef ENABLE_CONSOLE
      consoleSerial.Print (F("Reset.\r\n"));
    #endif
  }

  void serialConsoleSetup() {
    consoleSerial.AddCmd ( F ( "?" ) , SERIALCMD_FROMALL, configHelp );
    consoleSerial.AddCmd ( F ( "show" ) , SERIALCMD_FROMALL, configShow );
    consoleSerial.AddCmd ( F ( "info" ) , SERIALCMD_FROMALL, configInfo );
    consoleSerial.AddCmd ( F ( "baud" ) , SERIALCMD_FROMALL, configBaud );
    consoleSerial.AddCmd ( F ( "config" ) , SERIALCMD_FROMALL, configConfig );
    consoleSerial.AddCmd ( F ( "write" ) , SERIALCMD_FROMALL, configWrite );
    consoleSerial.AddCmd ( F ( "reset" ) , SERIALCMD_FROMALL, configReset );
    consoleSerial.AddCmd ( F ( "load" ) , SERIALCMD_FROMALL, configLoad );
  }

  void serialConsoleLoop() {
    if (!consoleEnMessageSent) {
      consoleSerial.Print ( F("\r\nConfig console active.\r\n") );
      consoleEnMessageSent = true;
    }

    ret = consoleSerial.ReadSer();
    if ( ret == 0 ) {
      consoleSerial.Print ( F("ERROR: Urecognized command. \r\n") );
    }
    delay(100);
  }

#endif
#endif
