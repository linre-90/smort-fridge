#ifndef MY_DISPLAY
#define MY_DISPLAY

//Display pins reversed to bit mask, { DOT, MIDDLE, NW, SW, SOUTH, SE, NE, NORTH }
typedef enum {
  ZERO = 0b10111111,
  ONE = 0b10000110,
  TWO = 0b11011011,
  THREE = 0b11001111,
  FOUR = 0b11100110,
  FIVE = 0b11101101,
  SIX = 0b11111101,
  SEVEN = 0b10000111,
  EIGHT = 0b11111111,
  NINE = 0b11101111,
  MDPHIGH = 0b01110110,
  MDPLOW = 0b00111000,
  MDPNAN = 0b01000000,
  MDPOK = 0b00000000,
} mdpCharacter;

/** Initialize or clear display related pins. */
void mdpSetupDisplayPins();

/** Draw character on display. */
void mdpDraw(const mdpCharacter);

/** Convert integer value to mdpCharacter. Converts digits from 0-9, others result in MDPNAN. */
mdpCharacter mdpIntToDisplayCharacter(int);

#endif