/*
https://github.com/xunker/panasonic_typewriter_interface

# Printing to a Panasonic KX-R435 (and compatible) electronic typewriter from a
computer

Sort-of emulates what the KX-R60, RP-K100, or RP-K105 interface adapters do,
but without the useful part of being seen as a Centronics-compatible printer.

## Theory of Operation

From reference manual from KX-W50TH/W60TH service manual:

  10.2.4 Interface Circuit
  The interface circuit handles the handshaking needed for communication with a
  I/F Adaptor (RP-K100). The RP-K100 allows interfacing with a host computer.
  The handshake method is described in the following steps.

  Process:
  (1) The RP-K100 changes the ON LINE signal from H to L indicating that data
  transmission has started . This ON LINE signal remains Low during the
  transmission of 1 byte.
  (2) The RP-K100 first sends the LSB (DO) of a transmitted byte to the TXD line
  and changes the STB signal from H to L. This STB signal is sent to P51 of the
  CPU which is the interruption.
  (3) In the interruption state, the CPU receives a TXD signal and changes the
  ACK signal from L to H. This ACK signal is sent to the RP-K100.
  (4) After the RP-K100 has received the ACK signal (L level), the STB signal
  changes from L to H.
  (5) When the STB signal (High) is sent from the RP-K100, the thermalwriter
  sends the ACK signal (High) to the RP-K100.
  (6) When the ACK signal is High, the RP-K100 starts to send the next bit of
  data.
  (7) Once the RP-K100 sends 1 byte of data (8 bits) to the CPU , the ON LINE
  signal changes from L to H.

---

## Pinout of MiniDIN-8 on Panasonic KX-R435

"Direction" is relative to the Typewriter itself.

Din Pin | X-Over Pin | Source  | Signal   | Direction | Notes
--------|------------|---------|----------|-----------|---------------------
      1 |          2 | GND     |          |           |
      2 |          1 | GND     |          |           |
      3 |          5 | GND     |          |           |
      4 |          4 | IC1 P16 | ~ACK     | out       | (b)
      5 |          3 | +12V    |          |           | For accessory power?
      6 |          8 | IC1 P24 | TXD      | in        | (a,b,c)
      7 |          7 | IC1 P18 | ~STB     | in        | (a,b,c)
      8 |          6 | IC1 P23 | ~ON_LINE | in        | (a,b,c)
 Shield |     Shield | GND     |          |           |

Notes:
(a) Routed to MCU pin through a 100-ohm resistor
(b) Has 1.5K pull-up to +5v
(c) Decoupled via 103Z ceramic cap (10K pF, +80%/-20% tolerance) to ground
(b) Has 10K pull-down to ground

"X-Over Pin": If using a Macintosh-style printer cable, be aware that it is
probably a "cross-over" or "null-modem" cable and several pairs of pins have
been swapped:

Connector | Cable
----------|---------
 (HSKo) 1 | 2 (HSKi)
 (TXD-) 3 | 5 (RXD-)
 (TXD+) 6 | 8 (RXD+)

!! IMPORTANT !!

PIN 5 may carry 12V! That voltage can COMPLETELY RUIN your microcontroller!
Verify the voltages of ALL PINS before connecting typerwiter to your device.

*/

// Version information
#define FW_VERSION "1.0.0"

// Enable serial configuration console
#define ENABLE_CONSOLE

// #define ON_LINE_PIN 5 // Output, active LOW
// #define STB_PIN 7  // Output, active LOW
// #define ACK_PIN 2 // Input, active LOW
// #define TXD_PIN 6 // Output; HIGH = 1, LOW = 0

// For adapter PCB
#define ON_LINE_PIN A0 // Output, active LOW
#define STB_PIN     A2 // Output, active LOW
#define ACK_PIN     A3 // Input, active LOW
#define TXD_PIN     A1 // Output; HIGH = 1, LOW = 0

#define GO_PIN A6 // trigger printing to begin when this is pulled low

#define MODE_PIN A7 // trigger printing to begin when this is pulled low

/* uncomment to have seprate LEDs for each typewriter signal, otherwise only
   LED_BUILTIN will be enabled */
#define ENABLE_MULTIPLE_LEDS

/*
SIGNAL_SETTLE_DELAY: Used in waitForSignalToSettle(), see that function for
explaination.
*/
#define SIGNAL_SETTLE_DELAY 2

/*
How many MILLIseconds to wait after sending a byte to the typewriter, to give it
a chance to type the character
*/
#define CHARACTER_PRINT_DELAY 5

/* Uncomment TEST_MODE to make this interface work in a demo mode that will
   print various test strings */
// #define TEST_MODE

// Enable serial debugging
#define ENABLE_DEBUGGING

/*
 serialBaud is the baud for Serial.begin. Default is 300 baud (YES I'M SERIOUS),
 and while you can set it higher I DO NO RECOMMEND IT because typewriters are
 fantasically slow! Also, the atMega238 only has a 64 byte serial buffer out of
 the box and 300 baud will fill that in literally TWO SECONDS.

 Possible values are:

 300, 600, 1200, 2400, 4800, 9600, 19200, 31250, 38400, and 57600.
 Speeds of 748800 and 115200 are possible but not recommened on the ATmega.
*/
#define DEFAULT_BAUD_IDX 0
const uint32_t baudRates[] ={
  300, 600, 1200, 2400, 4800, 9600, 19200, 31250, 38400, 57600
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

/*
  Automatically send a CR-LF when a line reaches the maximum length,
  either DEFAULT_MAXIMUM_LINE_LENGTH or the setting in EEPROM.
*/
#define ENABLE_AUTOMATIC_CRLF

#define DEFAULT_MAXIMUM_LINE_LENGTH 62 // characters
uint8_t maximumLineLength = DEFAULT_MAXIMUM_LINE_LENGTH;

/* Enable upper-ascii character translation. THIS IS CURRENTLY BROKEN. */
// #define ENABLE_CHARACTER_TRANSLATION

#define ENABLE_EEPROM

#ifdef ENABLE_MULTIPLE_LEDS
  /* these are for the kicad adapter board */
  #define ON_LINE_LED 8
  #define STB_LED     6
  #define ACK_LED     2
  #define TXD_LED     3
  #define LED_BUILTIN 13

  // /* these are for the breadboard test */
  // #define ON_LINE_LED D12
  // #define STB_LED     D11
  // #define ACK_LED     D10
  // #define TXD_LED     D9
  // #define LED_BUILTIN D13
#endif

#define ENABLE_MODE_BUTTON

#ifdef ENABLE_AUTOMATIC_CRLF
  uint8_t currentLineLength = 0;
#endif

#include "eeprom.h"
#include "debugging.h"
#include "serial_console.h"
#include "conversion.h"
#include "multiple_leds.h"
#include "mode_button.h"

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
bool readACKPin() {
  bool pinState = digitalRead(ACK_PIN);
  ACKLed(!pinState); // Signal is Active Low
  return pinState;
}
void setTXDPin(bool pinState) {
  digitalWrite(TXD_PIN, pinState);
  TXDLed(pinState);
}

void waitForACKToGo(bool pinState) {
  StatusLed(HIGH);

  uint8_t waitCounter = 0;
  while(readACKPin() == !pinState) {
    waitForSignalToSettle();

    if (waitCounter++ >= 100) {
      debug(millis());
      debugf(" waiting for ACK to go ");
      debugln(pinState);
      waitCounter = 0;
    }
  }
  StatusLed(LOW);
}

/*
waitForSignalToSettle() is a separate method only to explain what's happening
without adding a comment everywhere we delay for this reason.
We will wait SIGNAL_SETTLE_DELAY MILLIseconds after we set an outbound pin
state, but before reading the next dependent signal. This is to give the
typewriter a chance to process the signal and for any levels to settle. For
example, this delay is used between us setting ~STB and reading the ~ACK value
from the typewriter.
*/
void waitForSignalToSettle() {
  delay(SIGNAL_SETTLE_DELAY);
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

bool inRunMode() {
  return (digitalRead(GO_PIN) == LOW);
}

bool inHaltMode() {
  return (!inRunMode());
}

void loop() {
  #ifndef ENABLE_CONSOLE
    currentMillis = millis();
  #endif

  #ifdef ENABLE_MODE_BUTTON
    modeButtonLoop();
  #endif

  if (inRunMode()) {
    #ifdef TEST_MODE
      testLoop();
    #else
      relayLoop();
    #endif

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

char translatedChar = ' ';

#ifndef ENABLE_CHARACTER_TRANSLATION
  char translateCharacter(char incoming) {
    return incoming;
  }
#endif

void sendByte(char outbound) {
  translatedChar = translateCharacter(outbound);
  debug(translatedChar);
  debugf(" ");

  /*
  ON_LINE goes LOW at the beginning of the BYTE transmission, and remains
  high until all bits of the byte are transmitted.
  */
  setOnLinePin(LOW);

  for(uint8_t bitPos = 0;  bitPos < 8; bitPos++) {
    /*
    Wait for ACK to be high. The process outline at the beginning of this
    file has this check *after* sending a bit, but I'm assuming ACK will be
    LOW from the get-go
    */
    waitForACKToGo(LOW);

    /* Send the bit. Compatible with whatever character set Arduino uses. */
    if (bitRead(translatedChar, bitPos)) {
      setTXDPin(HIGH);
      debugf("1");
    } else {
      setTXDPin(LOW);
      debugf("0");
    }
    waitForSignalToSettle();

    /* Set STB to low, signaling typewritter to read the current TXD value. */
    setSTBPin(LOW);
    waitForSignalToSettle();

    /*
    ACK is kept high while the bit is being processed by the typewriter, and
    will go low once its ready to accept the next bit.
    */
    waitForACKToGo(HIGH);

    /* Set STB to HIGH to tell the typewriter to latch the TXD value. */
    setSTBPin(HIGH);

    StatusLed(LOW);

    waitForSignalToSettle();

    setTXDPin(LOW); // resest the txd pin, just to be sure
  }

  setOnLinePin(HIGH); // Signals end of byte
  delay(CHARACTER_PRINT_DELAY); // wait for the printer to actually print the character

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

  /* if nothing to read, wait a little bit before trying to read again */
  delay(100);
}

#define ESC_CODE 0x1B
// Overstrike test - works but not needed beause built-in underscore support
// char testString[] = {
//   'T', 0x08, '_', 'e', 0x08, '_', 's', 0x08, '_', 't', 0x08, '_', '.', '\r', '\n'
// };

// Bell test - no workie
// char testString[] = { 0x07 };

// Absolute tab - kinda works?
// char testString[] = {
//   ESC_CODE, 0x09, 2, ESC_CODE, 0x09, 4, ESC_CODE, ESC_CODE, 0x09, 0,
// };

//built-in underscore and bold
char testString[] = {
  'P', 'l', 'a', 'i', 'n', ' ',
  ESC_CODE, 0x45, // Bold on
  'B', 'o', 'l', 'd',
  ESC_CODE, 0x46, // Bold off
  ' ',
  ESC_CODE, 0x2D, 0x01, // Underscore on
  'U', 'n', 'd', 'e', 'r', 'l', 'i', 'n', 'e',
  ESC_CODE, 0x2D, 0x00, // Underscore off
  '\r', '\n'
};

uint8_t testStringIdx = 0;
void testLoop() {
  processByte(testString[testStringIdx]);

  testStringIdx++;
  if (testStringIdx >= sizeof(testString))
    testStringIdx = 0;

  delay(500);
}
