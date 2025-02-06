/*

Test data string to send: "111111111111111111111111111111111111111111111."

*/

#define RTS_PIN 12 // incoming from RTS, active low
#define CTS_PIN 11 // outgoing to CTS, active low
#define DTR_PIN 10 // incoming from DTR, active low
#define DSR_PIN 9 // outgoing to DSR, active low (labelled "RSD" on my adapter)

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  pinMode(RTS_PIN, INPUT_PULLUP);
  pinMode(CTS_PIN, OUTPUT);
  digitalWrite(CTS_PIN, LOW);

  pinMode(DTR_PIN, INPUT_PULLUP);
  pinMode(DSR_PIN, OUTPUT);
  digitalWrite(DSR_PIN, LOW);

  Serial.begin(1200);
}

uint8_t charsRecvBeforePause = 0;
uint8_t charsRecvAfterPause = 0;
uint8_t sendPauseAt = 30;
bool pauseSent = false;

void loop() {
  // current = millis();

  // if (current > nextReport) {
  //   nextReport = current + 10000;
  //   Serial.print("RTS: ");
  //   Serial.print(digitalRead(RTS_PIN));
  //   Serial.print(", DTR: ");
  //   Serial.println(digitalRead(DTR_PIN));
  // }

  while( Serial.available() > 0 ) {
    char incomingByte = Serial.read();
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    // Serial.print(incomingByte);
    // Serial.print(" 0x");
    // Serial.println(incomingByte, HEX);

    if (incomingByte == '.') {
      Serial.print("charsRecvBeforePause: "); Serial.println(charsRecvBeforePause);
      Serial.print("charsRecvAfterPause: "); Serial.println(charsRecvAfterPause);
      break;
    }

    if (pauseSent) {
      charsRecvAfterPause++;
    } else {
      charsRecvBeforePause++;
    }

    if ((!pauseSent) && (charsRecvBeforePause >= sendPauseAt)) {
      digitalWrite(CTS_PIN, HIGH);
      digitalWrite(DSR_PIN, HIGH);
      Serial.println("Limit reached");
      pauseSent = true;
    }
  }
}
