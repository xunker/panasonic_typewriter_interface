#ifndef MODE_BUTTON_H
#define MODE_BUTTON_H

// #include "config.h"

#ifdef ENABLE_MODE_BUTTON
  #include <AceButton.h> // https://github.com/bxparks/AceButton/
  using namespace ace_button;

  AceButton modeButton(MODE_PIN);

  void handleModeButton(AceButton* button, uint8_t eventType, uint8_t buttonState) {
    switch (eventType) {
    // case AceButton::kEventPressed:
    case AceButton::kEventLongPressed:
      // TODO: in future, run demo mode here instead
      testLeds();
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