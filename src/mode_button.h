/*

Defines behavior of the "Mode" button.

If TEST_MODE is #define'd, long-pressing the Mode button will print out a short
test string, defined in test_mode.h

If TEST_MODE is not #define'd, long-pressing the Mode button will run the LED
test.

TODO:
  * If Mode is pressed during boot, EEPROM is set to defaults
  * If Mode is pressed while in RUN mode, serial flow is paused
    * this requires flow control to be working, which it isn't

*/
#ifndef MODE_BUTTON_H
#define MODE_BUTTON_H

#ifdef ENABLE_MODE_BUTTON
  #include <AceButton.h> // https://github.com/bxparks/AceButton/
  using namespace ace_button;

  AceButton modeButton(MODE_PIN);

  void handleModeButton(AceButton* button, uint8_t eventType, uint8_t buttonState) {
    switch (eventType) {
    // case AceButton::kEventPressed:
    case AceButton::kEventLongPressed:
      #ifdef TEST_MODE
        if (inRunMode()) {
          sendTestString();
        } else {
          testLeds();
        }
      #else
        testLeds();
      #endif
      break;
  }
  }

  void modeButtonSetup() {
    pinMode(MODE_PIN, INPUT_PULLUP);

    modeButton.setEventHandler(handleModeButton);

    ButtonConfig* modeButtonConfig = modeButton.getButtonConfig();
    // enable long-press
    modeButtonConfig->setFeature(ButtonConfig::kFeatureLongPress);
  };

  void modeButtonLoop() {
    modeButton.check();
  }
#else
  // nothing
#endif
#endif