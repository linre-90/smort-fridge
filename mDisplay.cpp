#include <Arduino.h>
#include "mDisplay.h"


//Display pins, {NORTH, NE, SE, SOUTH, SW, NW, MIDDLE, DOT }
unsigned int displayPins[8] = {22, 23, 24, 25, 26, 27, 28, 29};

/** draw output bits to screen. from left to right. */
void mdpScreenItem(const byte output_bits);

void mdpSetupDisplayPins(){
  for(int i = 0; i < 8; i++){
    pinMode(displayPins[i], OUTPUT);
    digitalWrite(displayPins[i], LOW);
  }
}

void mdpDraw(const mdpCharacter character){
  mdpScreenItem(character);
}

void mdpScreenItem(const byte output_bits){
  byte out = output_bits;
  for(int i = 0; i < 8; i++){
    if(out & 1){
      digitalWrite(displayPins[i], HIGH);
    }else{
      digitalWrite(displayPins[i], LOW);
    }
    out >>= 1;
  }
}

mdpCharacter mdpIntToDisplayCharacter(int i){
  switch (i) {
  case 0:
    return ZERO;
  case 1:
    return ONE;
  case 2:
    return TWO;
  case 3:
    return THREE;
  case 4:
    return FOUR;
  case 5:
    return FIVE;
  case 6:
    return SIX;
  case 7:
    return SEVEN;
  case 8:
    return EIGHT;
  case 9:
    return NINE;
  default:
    return MDPNAN;
  }
}




