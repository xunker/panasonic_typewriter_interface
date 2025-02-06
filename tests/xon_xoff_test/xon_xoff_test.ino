/*

Test data string to send: "111111111111111111111111111111111111111111111."

*/

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.begin(300);
}

uint8_t charsRecvBeforeXoff = 0;
uint8_t charsRecvAfterXoff = 0;
uint8_t sendXoffAt = 30;
bool xoffSent = false;

void loop() {
  while( Serial.available() > 0 ) {
    char incomingByte = Serial.read();
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    // Serial.print(incomingByte);
    // Serial.print(" 0x");
    // Serial.println(incomingByte, HEX);

    if (incomingByte == '.') {
      Serial.print("charsRecvBeforeXoff: "); Serial.println(charsRecvBeforeXoff);
      Serial.print("charsRecvAfterXoff: "); Serial.println(charsRecvAfterXoff);
      break;
    }

    if (xoffSent) {
      charsRecvAfterXoff++;
    } else {
      charsRecvBeforeXoff++;
    }

    if ((!xoffSent) && (charsRecvBeforeXoff >= sendXoffAt)) {
      Serial.write(0x13); //0x13 off, 0x11 on
      Serial.flush();
      xoffSent = true;
    }
  }
}
