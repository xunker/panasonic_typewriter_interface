#ifndef TEST_MODE_H
#define TEST_MODE_H

#ifdef TEST_MODE

#define ESC_CODE 0x1B // Same character as ESP/P!
// Overstrike test - works but not needed beause built-in underscore support
// char testString[] = {
//   'T', 0x08, '_', 'e', 0x08, '_', 's', 0x08, '_', 't', 0x08, '_', '.', '\r', '\n'
// };

// Bell test - no workie
// char testString[] = { 0x07 };

// Absolute tab - kinda works?
// char testString[] = {
//   ESC_CODE, 0x09, 2, ESC_CODE, 0x09, 4, ESC_CODE, ESC_CODE, 0x09, 0,
// };

// char testString[] = {
//   ESC_CODE, 0x44, // reverse half line, no workie
//   ESC_CODE, 0x55, // forward half line, no workie
//   ' ', ' ',
//   ESC_CODE, 0x1a,0x49  // initialize printer, no workie
// };

// Codes might be same as Panasonic E7000?

//built-in underscore and bold
char testString[] = {
  'P', 'l', 'a', 'i', 'n', ' ',
  ESC_CODE, 0x45, // Bold on, same as ESP/P Legacy Code
  'B', 'o', 'l', 'd',
  ESC_CODE, 0x46, // Bold off, same as ESP/P Legacy Code
  ' ',
  ESC_CODE, 0x2D, 0x31, // Underscore on, similar to ESP/P Code with 0x31 added
  'U', 'n', 'd', 'e', 'r', 'l', 'i', 'n', 'e',
  ESC_CODE, 0x2D, 0x30, // Underscore off, similar to ESP/P Code with 0x30 added
  '\r', '\n'
};

void sendTestString() {
  for (uint8_t = i; i < sizeof(testString); i++) {
    processByte(testString[i]);
    wait(100);
  }

}

#endif
#endif
