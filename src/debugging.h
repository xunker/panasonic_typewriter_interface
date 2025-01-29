#ifndef DEBUGGING_H
#define DEBUGGING_H

#ifdef ENABLE_DEBUGGING
  #define debugf(msg) Serial.print(F(msg))
  #define debug(msg) Serial.print(msg)
  #define debugfln(msg) Serial.println(F(msg))
  #define debugln(msg) Serial.println(msg)
  #define debugfmt(msg, fmt) Serial.print(msg, fmt)
  #define debuglnfmt(msg, fmt) Serial.println(msg, fmt)

  void debugPaddedBinary(byte b, bool newlineAtEnd = false) {
    Serial.print("0b");
    for(int i = 7; i >= 0; i--)
      Serial.print(bitRead(b,i));
    if (newlineAtEnd)
      Serial.println();
  }

#else
  #define debugf(msg)
  #define debug(msg)
  #define debugfmt(msg, fmt)
  #define debugln(msg)
  #define debugfln(msg)
  #define debuglnfmt(msg, fmt)

  #define debugPaddedBinary(b, newlineAtEnd)
#endif

#endif
