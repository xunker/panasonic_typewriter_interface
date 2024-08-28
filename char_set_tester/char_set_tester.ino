/*

Test the difference between what is sent to the Arduino over Serial, and how the
MCU interprets it.

Instructions:
* Upload this on to your device with the Arduino IDE
* After successful upload, open the Serial Console (in the IDE), and set the
  speed to 57600
* Once you see "Ready.", send your text followed by CR/LF.
* Output: `Byte: '225', DEC: 225, HEX: 0xE1, BIN: 0b11100001, OCT: 0o341`
  - Byte: 'nnn': where "nnn" is the character as a byte (uint8_t)
  - DEC: decimal value of character, usually the same as byte value
  - HEX: hexadecimal version of byte value
  - BIN: binary version of byte value
  - OCT: octal version of byte value
* One second after your final input, a record separator ("---") will be printed

This assumes that "Serial" exists for your Arduino. If it is under a different
name (like "Serial0"), change SERIAL_OBJ #define  below.

Find updates to this at https://github.com/xunker/panasonic_typewriter_interface
*/

#define SERIAL_OBJ Serial
// #define SERIAL_OBJ Serial0 // For MCUs with multiple Serial devices

void setup() {
  #ifdef LED_BUILTIN
    pinMode(LED_BUILTIN, OUTPUT);
  #endif

  delay(1000);
  SERIAL_OBJ.begin(57600);

  // wait for serial to come available
  while (!Serial) {
    #ifdef LED_BUILTIN
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    #endif
    delay(100);
  }
  SERIAL_OBJ.println("Ready.");
}

uint8_t incomingByte;

unsigned long insertNextRequestSeparatorAt = 0;
#define REQUEST_SEPARATOR_TIMEOUT 1000

bool skipThisByte = false;
void loop() {
  while( SERIAL_OBJ.available() > 0 ) {
    incomingByte = SERIAL_OBJ.read();

    switch(incomingByte) {
      case 0x0D: // CR
        skipThisByte = true;
        break;
      case 0x0A: // LR
        skipThisByte = true;
        break;
      case 0x20: // space
        skipThisByte = true;
        break;
      default:
        skipThisByte = false;
        break;
    }
    if (skipThisByte)
      continue;

    SERIAL_OBJ.print("Byte: '");
    SERIAL_OBJ.print(incomingByte);
    SERIAL_OBJ.print("', DEC: ");
    SERIAL_OBJ.print(incomingByte, DEC);
    SERIAL_OBJ.print(", HEX: 0x");
    SERIAL_OBJ.print(incomingByte, HEX);
    SERIAL_OBJ.print(", BIN: ");
    SERIAL_OBJ.print(incomingByte, BIN);
    SERIAL_OBJ.print(", OCT: ");
    SERIAL_OBJ.print(incomingByte, OCT);
    SERIAL_OBJ.println("");

    insertNextRequestSeparatorAt = millis() + REQUEST_SEPARATOR_TIMEOUT;
  }

  /* if nothing to read, wait a little bit before trying to read again */
  delay(100);

  if ((insertNextRequestSeparatorAt > 0) && (insertNextRequestSeparatorAt < millis())) {
    insertNextRequestSeparatorAt = 0;
    SERIAL_OBJ.println("---");
  }
}
