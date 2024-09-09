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
  }
#else
  void onLineLed(bool pinState) { }
  void STBLed(bool pinState) { }
  void ACKLed(bool pinState) { }
  void TXDLed(bool pinState) { }

  void multiple_led_setup() { }
#endif

void led_setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  multiple_led_setup();
}

#endif
