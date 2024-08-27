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

/* Uncomment TEST_MODE to make this interface work in a demo mode that will
   print various test strings */
#define TEST_MODE

void togglePin(uint8_t pinNum) { digitalWrite(pinNum, !digitalRead(pinNum)); }
void toggleLED() { togglePin(LED_BUILTIN); }

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
    #ifdef TEST_MODE
      testLoop();
    #else
      relayLoop();
    #endif

  } else {
    Serial.print(millis());
    Serial.println(F(" Waiting for go..."));
    delay(1000);
  }
}

void sendByte(char outbound) {
  Serial.print(outbound);
  Serial.print(F(" "));
  /*
  ON_LINE goes LOW at the beginning of the BYTE transmission, and remains
  high until all bits of the byte are transmitted.
  */
  onLinePin(LOW);

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
      Serial.print(F("1"));
    } else {
      TXDPin(LOW);
      Serial.print(F("0"));
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
  delay(CHARACTER_PRINT_DELAY); // wait for the printer to actually print the character
}

char incomingByte;

void processByte(byte incomingByte) {
  if (incomingByte == 0b00000000)
    return;

  sendByte(incomingByte);

  Serial.print(F("\n"));

  LEDPin(LOW);
}

void relayLoop() {
  while( Serial.available() > 0 ) {
    incomingByte = Serial.read();
    processByte(incomingByte);
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
