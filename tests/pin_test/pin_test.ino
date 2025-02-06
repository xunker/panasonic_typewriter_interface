/*

# Typewriter off

14 ON_LINE_PIN 0
16 STB_PIN 0
17 ACK_PIN 0
15 TXD_PIN 0
25 GO_PIN 0
26 MODE_PIN 0

# Typewriter on, regular mode
14 ON_LINE_PIN 1
16 STB_PIN 1
17 ACK_PIN 0
15 TXD_PIN 1
25 GO_PIN 0
26 MODE_PIN 0

# Typewriter on, ON LINE mode

14 ON_LINE_PIN 1
16 STB_PIN 1
17 ACK_PIN 0
15 TXD_PIN 1
25 GO_PIN 0
26 MODE_PIN 0

*/
#define ON_LINE_PIN A0 // Output, active LOW
#define STB_PIN     A2 // Output, active LOW
#define ACK_PIN     A3 // Input, active LOW
#define TXD_PIN     A1 // Output; HIGH = 1, LOW = 0
#define GO_PIN A6 // trigger printing to begin when this is pulled low
#define MODE_PIN A7 // trigger printing to begin when this is pulled low


void reportPinState(uint8_t pin, String label) {
  Serial.print(pin, DEC);
  Serial.print("\t" + label + " ");
  Serial.println(digitalRead(pin));
}

void reportPinStates() {
  Serial.println(millis());
  reportPinState(ON_LINE_PIN, "ON_LINE_PIN");
  reportPinState(STB_PIN, "STB_PIN");
  reportPinState(ACK_PIN, "ACK_PIN");
  reportPinState(TXD_PIN, "TXD_PIN");
  reportPinState(GO_PIN, "GO_PIN");
  reportPinState(MODE_PIN, "MODE_PIN");
  Serial.println();
}

#include <AceButton.h> // https://github.com/bxparks/AceButton/
using namespace ace_button;

AceButton modeButton(MODE_PIN);

// Forward reference to prevent Arduino compiler becoming confused.
void handleEvent(AceButton*, uint8_t, uint8_t);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  // Pin Modes for typewriter
  pinMode(ON_LINE_PIN, INPUT);
  pinMode(STB_PIN, INPUT);
  pinMode(ACK_PIN, INPUT);
  pinMode(TXD_PIN, INPUT);
  pinMode(GO_PIN, INPUT_PULLUP);
  pinMode(MODE_PIN, INPUT_PULLUP);

  modeButton.setEventHandler(handleEvent);

  ButtonConfig* modeButtonConfig = modeButton.getButtonConfig();
  // // enable long-press
  // modeButtonConfig->setFeature(ButtonConfig::kFeatureLongPress);

  Serial.begin(57600);
}

void loop() {
  modeButton.check();
}

void handleEvent(AceButton* /* button */, uint8_t eventType,
    uint8_t /* buttonState */) {
  switch (eventType) {
    case AceButton::kEventPressed:
      reportPinStates();
      break;
    // case AceButton::kEventReleased:
    //   digitalWrite(LED_PIN, LED_OFF);
    //   break;
  }
}
