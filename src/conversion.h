#ifndef CONVERSION_H
#define CONVERSION_H

// #include "config.h"

#ifdef ENABLE_CHARACTER_TRANSLATION
  typedef struct {
    char incoming; // coming in from serial
    char translated; // sent to printer
  } charTranslation;

  // From R435 Manual, Page 51, Section C
  // First character is what we receive on serial interface,
  // second is what we will send the the typewriter.
  const charTranslation charTranslations[] = {
    { 'ß', 0xA0 },
    { '½', 0xA1 },
    { '¼', 0xA2 },
    { 'ç', 0xA8 },
    { '¡', 0xAA },
    { '¿', 0xAB },
    { '¢', 0xAC },
    { '£', 0xAD },

    { 'Ä', 0xB1 },
    { 'Ë', 0xB2 },
    { 'Ï', 0xB3 },
    { 'Ö', 0xB4 },
    { 'Ü', 0xB5 },
    { 'ä', 0xB6 },
    { 'ë', 0xB7 },
    { 'ï', 0xB8 },
    { 'ö', 0xB9 },
    { 'ü', 0xBA },

    { 'Á', 0xBC },
    { 'É', 0xBD },
    { 'Í', 0xBE },
    { 'Ó', 0xBF },
    { 'Ú', 0xC0 },
    { 'á', 0xC1 },
    { 'é', 0xC2 },
    { 'í', 0xC3 },
    { 'ó', 0xC4 },
    { 'ú', 0xC5 },

    { 'À', 0xC7 },
    { 'È', 0xC8 },
    { 'Ì', 0xC9 },
    { 'Ò', 0xCA },
    { 'Ù', 0xCB },
    { 'à', 0xCC },
    { 'è', 0xCD },
    { 'ì', 0xCE },
    { 'ò', 0xCF },
    { 'ù', 0xD0 },

    { 'Â', 0xD2 },
    { 'Ê', 0xD3 },
    { 'Î', 0xD4 },
    { 'Ô', 0xD5 },
    { 'Û', 0xD6 },
    { 'â', 0xD7 },
    { 'ê', 0xD8 },
    { 'î', 0xD9 },
    { 'ô', 0xDA },
    { 'û', 0xDB },

    { 'Ã', 0xFA },
    { 'Õ', 0xFB },
    { 'Ñ', 0xFC },
    { 'ã', 0xFD },
    { 'õ', 0xFE },
    { 'ñ', 0xF5 },

    { 0xC1, 0xFA }, // 'Ã'
    { 0xD5, 0xFB }, // 'Õ'
    { 0xD1, 0xFC }, // 'Ñ'
    { 0xE3, 0xFD }, // 'ã'
    { 0xF5, 0xFE }, // 'õ'
    { 0xF1, 0xF5 }, // 'ñ'
  };

  // character translations -- these are all currently broken, don't know why
  uint8_t testString[] = {
    'ß', '½', '¼', 'ç', '¡', '¿', '¢', '£',
    '\r', '\n',
    'Ä', 'Ë', 'Ï', 'Ö', 'Ü', 'ä', 'ë', 'ï', 'ö', 'ü',
    '\r', '\n',
    'Á', 'É', 'Í', 'Ó', 'Ú', 'á', 'é', 'í', 'ó', 'ú',
    '\r', '\n',
    'À', 'È', 'Ì', 'Ò', 'Ù', 'à', 'è', 'ì', 'ò', 'ù',
    '\r', '\n',
    'Â', 'Ê', 'Î', 'Ô', 'Û', 'â', 'ê', 'î', 'ô', 'û',
    '\r', '\n',
    'Ã', 'Õ', 'Ñ', 'ã', 'õ', 'ñ',
    '\r', '\n'
  };

    char translateCharacter(char incoming) {
    if (incoming >= 0xA0) {
      debugf("translating ");
      debug(incoming);
      debugf(" (");
      debug(incoming, DEC);
      debugf("): ");
      translatedChar = ' ';
      // translate the incoming byte in to requested charater for typewriter.
      for (uint8_t idx; idx < sizeof(charTranslations); idx++) {
        if (charTranslations[idx].incoming == incoming)
          translatedChar = charTranslations[idx].translated;
      }

      if (translatedChar == ' ') {
        debugfln("not found");
        translatedChar = incoming;
      } else {
        debugln(translatedChar);
      }
    }
    return translatedChar;
  }

#endif
#endif