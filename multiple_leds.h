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

  void multiple_led_setup() {
    pinMode(ON_LINE_LED, OUTPUT);
    pinMode(STB_LED, OUTPUT);
    pinMode(ACK_LED, OUTPUT);
    pinMode(TXD_LED, OUTPUT);

    // test the LEDS
    const uint8_t LedPins[] = { LED_BUILTIN, ON_LINE_LED, STB_LED, ACK_LED, TXD_LED };

    for (uint8_t i = 0; i < 5; i++) {
      digitalWrite(LedPins[i], HIGH);
      delay(100);
    }

    for (uint8_t i = 0; i < 5; i++) {
      digitalWrite(LedPins[i], LOW);
      delay(100);
    }
  }
#else
  void onLineLed(bool pinState) { }
  void STBLed(bool pinState) { }
  void ACKLed(bool pinState) { }
  void TXDLed(bool pinState) { }

  void multiple_led_setup() {
    // No LEDs to set up, but we still want a delay before we initialize Serial,
    // just in case we b0rked something, we we have time to upload some new code

    for (uint8_t i = 0; i < 5; i++) {
      StatusLed(HIGH);
      delay(100);
      StatusLed(LOW);
      delay(100);
    }
  }
#endif

void led_setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  multiple_led_setup();
}

#endif
