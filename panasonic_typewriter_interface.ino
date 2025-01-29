/*
https://github.com/xunker/panasonic_typewriter_interface

Printing to a Panasonic KX-R435 (and compatible) electronic typewriter from a
computer. Also probbably compatible with other KX-R Daisywheel
typewriters, KX-W word processors, RK-T "CupWheel" typewriters, or KX-WD55
daisywheel printers.

Emulates the Serial function of the KX-R60, RP-K100, or RP-K105 interface
adapters, but without the useful part of being seen as a Centronics-compatible
printer.

## Default settings

Serial: 300 baud, 8N1

## Pinout of MiniDIN-8 on Panasonic KX-R435

For pinout information, see:
https://github.com/xunker/panasonic_typewriter_interface/blob/main/PINOUT.md

## Theory of Operation

See:
https://github.com/xunker/panasonic_typewriter_interface/blob/main/README.md#theory-of-operation

*/

/* --- BEGIN CONFIGURATION SECTION --- */

/*
ENABLE_CONSOLE

Enable serial configuration console, available when in HALT mode */
#define ENABLE_CONSOLE

/*
ENABLE_EEPROM

Enable storing some firmware settings in EEPROM so they can be changed
on-the-fly via software. Only makes sense if ENABLE_CONSOLE is enabled */
#define ENABLE_EEPROM

/*
ENABLE_MODE_BUTTON

Enables a button connected to MODE_PIN, behaviour defined in mode_button.h */
#define ENABLE_MODE_BUTTON

/*
TEST_MODE

Enables a demo mode when Mode button is long-pressed while in RUN mode */
#define TEST_MODE

/*
ENABLE_DEBUGGING

Enable debugging information over serial
WARNING: this will slow down data transfter because it shares the same
serial connection, so only enable it while ACTIVELY debugging a problem */
// #define ENABLE_DEBUGGING

/* Pin Assignments, To/From Typewriter */
#define ON_LINE_PIN A0 // Output, active LOW
#define STB_PIN     A2 // Output, active LOW
#define ACK_PIN     A3 // Input, active LOW
#define TXD_PIN     A1 // Output; HIGH = 1, LOW = 0

/* Pin Assignments, Switches and Buttons on this device */
#define GO_PIN A6 // trigger printing to begin when this is pulled low
#define MODE_PIN A7 // See mode_button.h

/*
  Pin Assignments, Optional extra LEDs, Enabled by default
  Disabled by commenting-out `#define ENABLE_MULTIPLE_LEDS` below */
#define ON_LINE_LED 8
#define STB_LED     6
#define ACK_LED     2
#define TXD_LED     3

#ifndef LED_BUILTIN
  #define LED_BUILTIN 13
#endif

/*
ENABLE_MULTIPLE_LEDS

uncomment to have seprate LEDs for each typewriter signal, otherwise only
LED_BUILTIN will be enabled */
#define ENABLE_MULTIPLE_LEDS

/*
ENABLE_AUTOMATIC_CRLF

Automatically send a CR-LF when a line reaches the maximum length,
either DEFAULT_MAXIMUM_LINE_LENGTH or the setting in EEPROM.

THis is NOT RECOMMENDED, because the typewriter seems to handle long lines just
fine by itself.
*/
// #define ENABLE_AUTOMATIC_CRLF

#define DEFAULT_MAXIMUM_LINE_LENGTH 80 // characters

/*
ENABLE_CHARACTER_TRANSLATION

Enable upper-ascii character translation. THIS IS CURRENTLY BROKEN. */
// #define ENABLE_CHARACTER_TRANSLATION

/* --- END CONFIGURATION SECTION --- */

// Version information, please don't touch this unless you are me
#define FW_VERSION "1.0.0"

/*
 serialBaud is the baud for Serial.begin. Default is 300 baud (YES I'M SERIOUS),
 and while you can set it higher I DO NO RECOMMEND IT because typewriters are
 fantasically slow! Also, the atMega238 only has a 64 byte serial buffer out of
 the box and 300 baud will fill that in literally TWO SECONDS.

 Possible values are:

 110, 150, 300, 600, 1200, 2400, 4800, 9600, 19200, 38400, and 57600.
 Speeds of 748800 and 115200 are possible but not recommened on the ATmega.
*/
#define DEFAULT_BAUD_IDX 2
const uint32_t baudRates[] ={
  110, 150, 300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600
};
uint8_t serialBaudIdx = DEFAULT_BAUD_IDX;
uint32_t serialBaud = baudRates[DEFAULT_BAUD_IDX];

typedef struct {
  char *label;
  uint8_t value;
} serialConfigOption;

const serialConfigOption serialConfigs[] = {
  { "5n1", SERIAL_5N1 },
  { "6n1", SERIAL_6N1 },
  { "7n1", SERIAL_7N1 },
  { "8n1", SERIAL_8N1 }, // default
  { "5n2", SERIAL_5N2 },
  { "6n2", SERIAL_6N2 },
  { "7n2", SERIAL_7N2 },
  { "8n2", SERIAL_8N2 },
  { "5e1", SERIAL_5E1 },
  { "6e1", SERIAL_6E1 },
  { "7e1", SERIAL_7E1 },
  { "8e1", SERIAL_8E1 },
  { "5e2", SERIAL_5E2 },
  { "6e2", SERIAL_6E2 },
  { "7e2", SERIAL_7E2 },
  { "8e2", SERIAL_8E2 },
  { "5o1", SERIAL_5O1 },
  { "6o1", SERIAL_6O1 },
  { "7o1", SERIAL_7O1 },
  { "8o1", SERIAL_8O1 },
  { "5o2", SERIAL_5O2 },
  { "6o2", SERIAL_6O2 },
  { "7o2", SERIAL_7O2 },
  { "8o2", SERIAL_8O2 }
};

#define DEFAULT_SERIAL_CONFIG_IDX 3
uint8_t serialConfigIdx = DEFAULT_SERIAL_CONFIG_IDX;
uint8_t serialConfig = serialConfigs[serialConfigIdx].value;

#ifdef ENABLE_AUTOMATIC_CRLF
  uint8_t maximumLineLength = DEFAULT_MAXIMUM_LINE_LENGTH;
#endif

#ifdef ENABLE_AUTOMATIC_CRLF
  uint8_t currentLineLength = 0;
#endif

// A non-blocking replacement for delay()
unsigned long waitUntilMS = 0;
void waitMS(uint16_t delayMS) {
  waitUntilMS = millis() + delayMS;
  while (millis() < waitUntilMS) {
    // no-op
  }
}

// A non-blocking replacement for delayMicroseconds()
unsigned long waitUntilUS = 0;
void waitUS(uint16_t delayUS) {
  waitUntilUS = micros() + delayUS;
  while (micros() < waitUntilUS) {
    // no-op
  }
}

void wait(uint16_t delayMS) {
  waitMS(delayMS);
}

#include "src/eeprom.h"
#include "src/debugging.h"
#include "src/serial_console.h"
#include "src/conversion.h"
#include "src/multiple_leds.h"
#include "src/test_mode.h"
#include "src/mode_button.h"

void togglePin(uint8_t pinNum) { digitalWrite(pinNum, !digitalRead(pinNum)); }
void toggleLED() { togglePin(LED_BUILTIN); }

void setOnLinePin(bool pinState) {
  digitalWrite(ON_LINE_PIN, pinState);
  onLineLed(!pinState); // Signal is Active Low
}
void setSTBPin(bool pinState) {
  digitalWrite(STB_PIN, pinState);
  STBLed(!pinState); // Signal is Active Low
}

void setTXDPin(bool pinState) {
  digitalWrite(TXD_PIN, pinState);
  TXDLed(pinState);
}

void waitForACKToGo(bool pinState) {
  #ifdef ENABLE_DEBUGGING
    uint8_t waitCounter = 0;
  #endif
  while(digitalRead(ACK_PIN) == !pinState) {
    #ifdef ENABLE_DEBUGGING
      if (waitCounter++ >= 100) {
        debug(millis());
        debugf(" waiting for ACK to go ");
        debugln(pinState);
        waitCounter = 0;
      }
    #endif
  }
  ACKLed(!pinState);  // Signal is Active Low
}

bool inRunMode() {
  return (digitalRead(GO_PIN) == LOW);
}

bool inHaltMode() {
  return (!inRunMode());
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  // Pin Modes for typewriter
  pinMode(ON_LINE_PIN, OUTPUT);
  pinMode(STB_PIN, OUTPUT);
  pinMode(ACK_PIN, INPUT_PULLUP);
  pinMode(TXD_PIN, OUTPUT);
  pinMode(GO_PIN, INPUT_PULLUP);

  // Set initial pin states for good measure
  setOnLinePin(HIGH);
  setSTBPin(HIGH);
  setTXDPin(HIGH);
  StatusLed(LOW);

  #ifdef ENABLE_EEPROM
    eepromSetup();
  #endif

  #ifdef ENABLE_CONSOLE
    serialConsoleSetup();
  #endif

  #ifdef ENABLE_MODE_BUTTON
    modeButtonSetup();
  #endif

  ledSetup();

  Serial.begin(serialBaud, serialConfig);
}

#ifndef ENABLE_CONSOLE
  unsigned long currentMillis = 0;
  unsigned long nextSerialConsoleStatus = 0;
  #define SEND_SERIAL_CONSOLE_STATUS_EVERY 1000 // milliseconds
#endif

void loop() {
  #ifndef ENABLE_CONSOLE
    currentMillis = millis();
  #endif

  #ifdef ENABLE_MODE_BUTTON
    modeButtonLoop();
  #endif

  if (inRunMode()) {
    relayLoop();
  } else {
    #ifdef ENABLE_CONSOLE
      serialConsoleLoop();
    #else
      if (nextSerialConsoleStatus < currentMillis) {
        nextSerialConsoleStatus = currentMillis + SEND_SERIAL_CONSOLE_STATUS_EVERY;

        debug(currentMillis);
        debugfln(" Waiting for go...");
      }
    #endif
  }
}

void sendByte(char outbound) {
  #ifdef ENABLE_CHARACTER_TRANSLATION
    outbound = translateCharacter(outbound);
  #endif

  debug(outbound);
  debugf(" ");

  /*
  ON_LINE goes LOW at the beginning of the BYTE transmission, and remains
  high until all bits of the byte are transmitted.
  */
  setOnLinePin(LOW);

  for(uint8_t bitPos = 0;  bitPos < 8; bitPos++) {

    /* Send the bit. Compatible with whatever character set Arduino uses. */
    if (bitRead(outbound, bitPos)) {
      setTXDPin(HIGH);
      debugf("1");
    } else {
      setTXDPin(LOW);
      debugf("0");
    }
    waitUS(50);

    /* Set STB to low, signaling typewritter to read the current TXD value. */
    setSTBPin(LOW);

    /*
    ACK is kept high while the bit is being processed by the typewriter, and
    will go low once its ready to accept the next bit.
    */
    waitForACKToGo(HIGH);

    /* Set STB to HIGH to tell the typewriter to latch the TXD value. */
    setSTBPin(HIGH);

    waitForACKToGo(LOW);
  }

  setOnLinePin(HIGH); // Signals end of byte

  #ifdef ENABLE_AUTOMATIC_CRLF
    if ((outbound == '\r') || (outbound == '\n')) {
      currentLineLength = 0;
    }
  #endif

  debugf("\n");
}

char incomingByte;

void processByte(char incomingByte) {
  if (incomingByte == 0b00000000)
    return;

  sendByte(incomingByte);
  StatusLed(LOW);
}

void relayLoop() {
  while( Serial.available() > 0 ) {
    incomingByte = Serial.read();
    processByte(incomingByte);

    #ifdef ENABLE_AUTOMATIC_CRLF
      if (currentLineLength++ >= maximumLineLength) {
        // Send CR/LF because we're reached the end of the line.
        // snedByte() will reset currentLineLength for us.
        debugf("maximumLineLength ");
        debug(maximumLineLength);
        debugf(" reached, sending crlf.\n");
        sendByte('\r');
        sendByte('\n');

        waitForACKToGo(LOW);
      };
    #endif
  }
}
