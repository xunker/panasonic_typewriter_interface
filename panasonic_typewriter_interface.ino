/*
From reference manual from KX-W50TH/W60TH service manual:

10.2.4 Interface Circuit
The interface circuit handles the handshaking needed for communication with a
I/F Adaptor (RP-K100). The RP-K100 allows interfacing with a host computer.
The handshake method is described in the following steps.

Process;
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

Pinout of MiniDIN-8 on Panasonic KX-R435:

Din Pin | X-Over Pin | Source  | Signal   | Direction    | Notes
--------|------------|---------|----------|--------------|---------------------
      1 |          2 | GND     |          |              |
      2 |          1 | GND     |          |              |
      3 |          5 | GND     |          |              |
      4 |          4 | IC1 P16 | ~ACK     | out from MCU | (b)
      5 |          3 | +12V    |          |              | For accessory power?
      6 |          8 | IC1 P24 | TXD      | in to MCU    | (a,b,c)
      7 |          7 | IC1 P18 | ~STB     | in to MCU    | (a,b,c)
      8 |          6 | IC1 P23 | ~ON_LINE | in to MCU    | (a,b,c)
 Shield |     Shield | GND     |          |              |

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

---

character mappings:

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
#define TXD_PIN 6 // Output, active LOW (assumed)

#define GO_PIN A7 // trigger printing to begin

// char message[24] = "Hello, from Panasonic!\n"; // the max. string length is n-1 characters,
                                              // last char is Null as string-terminator
char messages[][54] = {
  "This place is a message,",
  "and part of a system of messages.",
  "Pay attention to it!",
  "",
  "Sending this message was important to us.",
  "We considered ourselves to be a powerful culture.",
  "",
  "This place is not a place of honor.",
  "No highly esteemed deed is commemorated here.",
  "Nothing valued is here.",
  "",
  "What is here was dangerous and repulsive to us.",
  "This message is a warning about danger.",
  "",
  "The danger is in a particular location.",
  "It increases towards a center.",
  "The center of danger is here,",
  "Of a particular size and shape, and below us.",
  "",
  "The danger is still present,",
  "in your time, as it was in ours.",
  "",
  "The danger is to the body, and it can kill.",
  "",
  "The form of the danger is an emanation of energy.",
  "",
  "The danger is unleashed only if you substantially",
  "disturb this place physically.",
  "",
  "This place is best shunned and left uninhabited."
};

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

void onLinePin(bool pinState) {
  digitalWrite(ON_LINE_PIN, pinState);
}

void STBPin(bool pinState) {
  digitalWrite(STB_PIN, pinState);
}

void ACKPin(bool pinState) {
  digitalWrite(ACK_PIN, pinState);
}

void TXDPin(bool pinState) {
  digitalWrite(TXD_PIN, pinState);
}

void toggleTXD() {
  digitalWrite(TXD_PIN, !digitalRead(TXD_PIN));
}

void LEDPin(bool pinState) {
  digitalWrite(LED_BUILTIN, pinState);
}

void toggleLED() {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void setInitialPinStates() {
  onLinePin(HIGH);
  STBPin(HIGH);
  TXDPin(HIGH);
  LEDPin(LOW);
}

void checkPin(String name, uint8_t pin) {
  Serial.print(name);
  Serial.print("\t");
  Serial.println(digitalRead(pin));
}

void checkPinStates() {
  Serial.println(millis());
  checkPin("ON_LINE", ON_LINE_PIN);
  checkPin("STB", STB_PIN);
  checkPin("ACK", ACK_PIN);
  checkPin("TXD", TXD_PIN);
}

void testLoop() {
  checkPinStates();
  delay(1000);
}

void waitForACKToGo(bool pinState) {
  LEDPin(HIGH);
  uint8_t waitCounter = 0;
  delay(1); // wait for typewriter to set ACK and signal to settle
  while(digitalRead(ACK_PIN) == !pinState) {
    delay(10);

    if (waitCounter++ >= 100) {
      // Serial.print(millis());
      Serial.print(" waiting for ACK to go ");
      Serial.println(pinState);
      // checkPinStates();
      waitCounter = 0;
    }
  }
  LEDPin(LOW);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(ON_LINE_PIN, OUTPUT);
  pinMode(STB_PIN, OUTPUT);
  pinMode(ACK_PIN, INPUT_PULLUP);
  pinMode(TXD_PIN, OUTPUT);
  pinMode(GO_PIN, INPUT_PULLUP);
  setInitialPinStates();

  // pinMode(ON_LINE_PIN, INPUT_PULLUP);
  // pinMode(STB_PIN, INPUT_PULLUP);
  // pinMode(ACK_PIN, INPUT_PULLUP);
  // pinMode(TXD_PIN, INPUT_PULLUP);

  delay(1000);
  Serial.begin(57600);
}

void loop() {
  if (digitalRead(GO_PIN) == LOW) {
    printLoop();
    // testLoop();
  } else {
    Serial.print(millis());
    Serial.println(" Waiting for go...");
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

    /*
    Send the bit. It's compatible with whatever character set Arduino uses.
    */
    if (bitRead(outbound, bitPos)) {
      TXDPin(HIGH);
      Serial.print("1");
    } else {
      TXDPin(LOW);
      Serial.print("0");
    }
    delay(5); // wait for level to settle

    /*
    Set STB to low signal to the typewritter to read the current TXD value.
    */
    STBPin(LOW);
    delay(5); // wait for level to settle

    /*
    ACK is kept high while the bit is being processed by the typewriter, and
    will go low once its ready to accept the next bit.
    */
    waitForACKToGo(HIGH);

    /*
    Set STB to HIGH to tell the typewriter to latch the TXD value.
    */
    STBPin(HIGH);

    LEDPin(LOW);

    delay(5);

    TXDPin(LOW); // resest the txd pin, even though I don't think we need to
  }
}

uint8_t messageIdx = 0;


void printLoop() {
  Serial.println(millis());
  Serial.println(messages[messageIdx]);

  for (uint8_t strPos = 0; strPos < sizeof(messages[messageIdx]); strPos++) {
    /*
    ON_LINE goes LOW at the beginning of the BYTE transmission, and remains
    high until all bits of the byte are transmitted.
    */
    onLinePin(LOW);

    Serial.print(messages[messageIdx][strPos]);
    Serial.print(" ");

    if (messages[messageIdx][strPos] == 0b00000000)
      continue;
    sendByte(messages[messageIdx][strPos]);

    Serial.print("\n");
    onLinePin(HIGH); // Signals end of byte
    TXDPin(LOW); // resest the txd pin, even though I don't think we need to

    delay(10); // wait for the printer to actually print the character
  }
  LEDPin(LOW);
  sendByte('\r');
  sendByte('\n');

  delay(1000);

  messageIdx++;
  if (messageIdx > 30) {
    messageIdx = 0;
  }
}
