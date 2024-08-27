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

## character mappings

Only lower-ascii is a direct match. Upper-ascii is befunged. Sample mappings
I've found

typewriter | ascii char
-----------|-----------
í          | é
¼          | •
½          |
ß          | ≠
Ù          | ˚

*/
#define ON_LINE_PIN 5 // Output, active LOW
#define STB_PIN 7  // Output, active LOW
#define ACK_PIN 2 // Input, active LOW
#define TXD_PIN 6 // Output, active LOW

#define GO_PIN A7 // trigger printing to begin when this is pulled low

/*
SIGNAL_SETTLE_DELAY: Used in waitForSignalToSettle(), see that function for
explaination.
*/
#define SIGNAL_SETTLE_DELAY 2

/*
How many MILLIseconds to wait after sending a byte to the typewriter, to give it a chance to type
the character
*/
#define CHARACTER_PRINT_DELAY 5

/*
character mappings:

Only lower-ascii is a direct match. Upper-ascii is befunged.

typewriter | ascii char
-----------|-----------
í          | é
¼          | •
½          |
ß          | ≠
Ù          | ˚
*/

// The code for Esc/Escape (decimal 27), for processing Epson ESC/P Codes
#define ESC_CODE 0b00011011

bool inUnderlineMode = false;
void toggleUnderlineMode(bool state) {
  Serial.print(F("Setting Underline Mode to "));
  Serial.println(state);
  inUnderlineMode = state;
}
void toggleUnderlineModeOn() { toggleUnderlineMode(true); }
void toggleUnderlineModeOff() { toggleUnderlineMode(false); }

bool inDoubleStrikeMode = false;
void toggleDoubleStrikeMode(bool state) {
  Serial.print(F("Setting Double-Strike Mode to "));
  Serial.println(state);
  inDoubleStrikeMode = state;
}
void toggleDoubleStrikeModeOn() { toggleDoubleStrikeMode(true); }
void toggleDoubleStrikeModeOff() { toggleDoubleStrikeMode(false); }

typedef struct {
  byte first;
  uint8_t (*hFunc) ();
} EscHandler;

const EscHandler escHandlers[] {
    {52, &toggleUnderlineModeOn},  // actually italic, using underscore instead
    {53, &toggleUnderlineModeOff}, // actually italic, using underscore instead
    {69, &toggleDoubleStrikeModeOn}, // semi-bold
    {70, &toggleDoubleStrikeModeOff}, //semi-bold
    {71, &toggleDoubleStrikeModeOn},
    {72, &toggleDoubleStrikeModeOff},
};

/*

Other misc codes to test:
8   backspace
7   beep or bell
10  line feed (new line)
11  down tab as set? What's this?
12  form feed
19  deselect printer (will that exit offline mode?)

...if we change this code to buffer whole lines (and not print characters as
soon as they are received), codes like this could be useful:
127 Empty the print buffer/erase buffer from last character
24  erase buffer from last line


*/

// Are we currently in Escape-code mode?
bool currentEscMode = false;

void togglePin(uint8_t pinNum) {
  digitalWrite(pinNum, !digitalRead(pinNum));
}

void toggleLED() {
  togglePin(LED_BUILTIN);
}

void onLinePin(bool pinState) { digitalWrite(ON_LINE_PIN, pinState); }
void STBPin(bool pinState) { digitalWrite(STB_PIN, pinState); }
void ACKPin(bool pinState) { digitalWrite(ACK_PIN, pinState); }
void TXDPin(bool pinState) { digitalWrite(TXD_PIN, pinState); }
void LEDPin(bool pinState) { digitalWrite(LED_BUILTIN, pinState); }

void waitForACKToGo(bool pinState) {
  LEDPin(HIGH);

  uint8_t waitCounter = 0;
  while(digitalRead(ACK_PIN) == !pinState) {
    waitForSignalToSettle();

    if (waitCounter++ >= 100) {
      Serial.print(millis());
      Serial.print(F(" waiting for ACK to go "));
      Serial.println(pinState);
      waitCounter = 0;
    }
  }
  LEDPin(LOW);
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
  onLinePin(HIGH);
  STBPin(HIGH);
  TXDPin(HIGH);
  LEDPin(LOW);

  delay(1000);
  Serial.begin(57600);
}

void loop() {
  if (digitalRead(GO_PIN) == LOW) {
    relayLoop();
  } else {
    Serial.print(millis());
    Serial.println(F(" Waiting for go..."));
    delay(1000);
  }
}

void sendByte(char outbound) {
  for(uint8_t bitPos = 0;  bitPos < 8; bitPos++) {
    /*
    Wait for ACK to be high. The process outline at the beginning of this
    file has this check *after* sending a bit, but I'm assuming ACK will be
    LOW from the get-go
    */
    waitForACKToGo(LOW);

    /* Send the bit. Compatible with whatever character set Arduino uses. */
    if (bitRead(outbound, bitPos)) {
      TXDPin(HIGH);
      Serial.print("1");
    } else {
      TXDPin(LOW);
      Serial.print("0");
    }
    waitForSignalToSettle();

    /* Set STB to low, signaling typewritter to read the current TXD value. */
    STBPin(LOW);
    waitForSignalToSettle();

    /*
    ACK is kept high while the bit is being processed by the typewriter, and
    will go low once its ready to accept the next bit.
    */
    waitForACKToGo(HIGH);

    /* Set STB to HIGH to tell the typewriter to latch the TXD value. */
    STBPin(HIGH);

    LEDPin(LOW);

    waitForSignalToSettle();

    TXDPin(LOW); // resest the txd pin, just to be sure
  }

  onLinePin(HIGH); // Signals end of byte
}

char incomingByte;
bool lastEscCodeHandled = false;

void relayLoop() {
  while( Serial.available() > 0 ) {
    incomingByte = Serial.read();

    if (incomingByte == 0b00000000)
      continue;

    if (currentEscMode) {
      // We're in escape mode
      // Find the handler for this key code
      for (uint8_t idx; idx < sizeof(escHandlers); idx++) {
        if (escHandlers[idx].first == incomingByte) {
          escHandlers[idx].hFunc();
          lastEscCodeHandled = true;
          break;
        }
      }

      if (!lastEscCodeHandled) {
        Serial.print(F("No handler for "));
        Serial.println(incomingByte);
      }
      lastEscCodeHandled = false;

      currentEscMode = false;
      continue;
    }

    // We got an "escape code", so toggle the mode and loop again
    if (incomingByte == ESC_CODE) {
      currentEscMode = true;
      continue;
    }

    sendByte(incomingByte);

    Serial.print("\n");

    delay(CHARACTER_PRINT_DELAY); // wait for the printer to actually print the character

    LEDPin(LOW);
  }

  /* if nothing to read, wait a little bit before trying to read again */
  delay(100);
}
