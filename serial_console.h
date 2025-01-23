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

#define consolePrint(msg) consoleSerial.Print(msg)

// when you just want a new line
void consolePrintln() {
  consoleSerial.Print(F(LINE_ENDING));
}

void consolePrintln(char * msg) {
  consoleSerial.Print(msg);
  consolePrintln();
}

void consolePrintln(const __FlashStringHelper *msg) {
  consoleSerial.Print(msg);
  consolePrintln();
}


bool consoleEnabled = true; // to be moved elsewhere, trigged by button or something
bool consoleEnMessageSent = false;

int8_t ret;

const uint8_t maxBannerWidth = 23;
// void currentCommandBanner(char label[16], uint8_t width = 20, char bannerChar = '=') {
void currentCommandBanner(char *label, uint8_t width = maxBannerWidth, char bannerChar = '=') {
  char banner[width+1];
  banner[width] = '\0'; // null terminate the array

  for (uint8_t i = 0; i < width; i++) {
    banner[i] = bannerChar;
  }

  uint8_t labelStartIdx = (width - strlen(label)) / 2;
  for (uint8_t i = 0; i < strlen(label); i++) {
    banner[labelStartIdx + i] = label[i];
  }

  consolePrintln(banner);
}

// Same as above, but for use with `F("")` macro
// From https://stackoverflow.com/a/69139335
void currentCommandBanner(const __FlashStringHelper *label, uint8_t width = 20, char bannerChar = '=') {
  char buffer[maxBannerWidth+1];
  strncpy_P(buffer, (const char*)label, maxBannerWidth);  // _P is the version to read from program space
  currentCommandBanner(buffer);
}

void configHelp(void) {
  currentCommandBanner(F("Help"));

  consolePrintln(F("Commands:"));
  consolePrintln(F("\tbaud [rate]\tget or set the Serial Baud rate"));
  consolePrintln(F("\tconfig [value]\tget or set the Serial configuration"));
  consolePrintln(F("\tshow\t\tprint current settings"));
  #ifdef ENABLE_EEPROM
    consolePrintln(F("\tload\t\tload config from EEPROM"));
    consolePrintln(F("\twrite\t\twrite current config to EEPROM"));
    consolePrintln(F("\treset\t\treset EEPROM to defaults"));
  #endif
  consolePrintln(F("\tinfo\t\tsystem information"));
  consolePrintln(F("\t?\t\tlist commands (this screen)"));
}

void configShow(void) {
  currentCommandBanner(F("Show Settings"));

  consolePrint(F("\tbaud\t"));
  consolePrint (serialBaud);
  // consolePrint(F(" index: "));
  // consolePrint(serialBaudIdx);
  consolePrintln();

  consolePrint(F("\tconfig\t"));
  consolePrint(serialConfigs[serialConfigIdx].label);
  // consolePrint(F(" index: "));
  // consolePrint(serialConfigIdx);
  consolePrintln();
}

void configInfo(void) {
  currentCommandBanner(F("System Info"));
  consolePrint(F("Firmware version: "));
  consolePrint(F(FW_VERSION));
  consolePrintln();

  consolePrintln(F("Licensed under GNU GPL V3\r\n"));

  consolePrintln(F("Compiled with:"));
  #ifdef ENABLE_DEBUGGING
    consolePrintln(F("\tserial debugging"));
  #endif

  #ifdef ENABLE_CONSOLE
    consolePrintln(F("\tserial console"));
  #endif

  #ifdef ENABLE_EEPROM
    consolePrintln(F("\teeprom support"));
  #endif

  consolePrintln(F("Memory:"));

  consolePrint(F("\tStack size:\t")); consoleSerial.Print((int) RAMEND - (int)SP); consolePrintln();

  consolePrint(F("\tFree ram:\t")); consoleSerial.Print((int) SP - (int) (__brkval == 0 ? (int)&__heap_start : (int)__brkval)); consolePrintln();

  consolePrint(F("\tSRAM size:\t")); consoleSerial.Print((int) RAMEND - (int) &__data_start); consolePrintln();

  consolePrint(F("Uptime: ")); consoleSerial.Print(millis()); consolePrintln();
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
      consolePrintln(F("Error: valid baud rates are:"));
      for (uint8_t idx = 0; idx < (sizeof(baudRates)/sizeof(baudRates[0])); idx++) {
        consolePrint(baudRates[idx]); consolePrint(F(" "));
      }
      consolePrintln();

      return;
    }

    serialBaud = baudRates[newSerialBaudIdx];
    serialBaudIdx = newSerialBaudIdx;
  }

  currentCommandBanner(F("Serial Baud"));
  consoleSerial.Print(serialBaud);
  consolePrintln();
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
        consoleSerial.Print(serialConfigs[idx].label); consolePrint(F(" "));
      }
      consolePrintln();

      return;
    }

    serialConfig = serialConfigs[newConfigIdx].value;
    serialConfigIdx = newConfigIdx;
  }

  currentCommandBanner(F("Serial Config"));
  consoleSerial.Print(serialConfigs[serialConfigIdx].label);
  consolePrintln();
}

void configWrite(void) {
  EEPROM.put(BAUD_IDX_ADDR, serialBaudIdx);
  EEPROM.put(CONFIG_IDX_ADDR, serialConfigIdx);

  #ifdef ENABLE_CONSOLE
    currentCommandBanner(F("EEPROM Write"));
    consolePrintln(F("Written."));
  #endif
}

void configLoad(void) {
  loadEeprom();
  #ifdef ENABLE_CONSOLE
    currentCommandBanner(F("EEPROM Load"));
    consolePrintln(F("Loaded."));
  #endif
}

void configReset(void) {
  resetEeprom();
  #ifdef ENABLE_CONSOLE
    currentCommandBanner(F("EEPROM Reset"));
    consolePrintln(F("Reset."));
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
    consolePrintln( F("\r\nConfig console active.") );
    consoleEnMessageSent = true;
  }

  ret = consoleSerial.ReadSer();
  if ( ret == 0 ) {
    consolePrintln( F("ERROR: Urecognized command.") );
  }
  delay(100);
}

#endif
#endif
