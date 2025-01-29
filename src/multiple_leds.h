#ifndef MULTIPLE_LEDS_H
#define MULTIPLE_LEDS_H

// #include "config.h"

// built-in led is always enabled
void StatusLed(bool pinState) { digitalWrite(LED_BUILTIN, pinState);  }

#ifdef ENABLE_MULTIPLE_LEDS
  void onLineLed(bool pinState) { digitalWrite(ON_LINE_LED, pinState); }
  void STBLed(bool pinState) { digitalWrite(STB_LED, pinState); }
  void ACKLed(bool pinState) { digitalWrite(ACK_LED, pinState); }
  void TXDLed(bool pinState) { digitalWrite(TXD_LED, pinState); }

  void testLeds() {
    const uint8_t LedPins[] = { LED_BUILTIN, ON_LINE_LED, STB_LED, TXD_LED, ACK_LED };

    for (uint8_t i = 0; i < 5; i++) {
      digitalWrite(LedPins[i], HIGH);
      wait(100);
    }

    for (uint8_t i = 0; i < 5; i++) {
      digitalWrite(LedPins[i], LOW);
      wait(100);
    }
  }

  void multipleLedSetup() {
    pinMode(ON_LINE_LED, OUTPUT);
    pinMode(STB_LED, OUTPUT);
    pinMode(ACK_LED, OUTPUT);
    pinMode(TXD_LED, OUTPUT);

    testLeds();
  }
#else
  void onLineLed(bool pinState) { }
  void STBLed(bool pinState) { }
  void ACKLed(bool pinState) { }
  void TXDLed(bool pinState) { }

  void testLeds() {
    for (uint8_t i = 0; i < 5; i++) {
      StatusLed(HIGH);
      wait(100);
      StatusLed(LOW);
      wait(100);
    }
  }

  void multipleLedSetup() {
    // No LEDs to set up, but we still want a delay before we initialize Serial,
    // just in case we b0rked something, we we have time to upload some new code
    testLeds();
  }
#endif

void ledSetup() {
  pinMode(LED_BUILTIN, OUTPUT);
  multipleLedSetup();
}

#endif
