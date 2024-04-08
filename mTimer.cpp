#include "mTimer.h"
#include <Arduino.h>

void timerStart(struct mTimer* timer, unsigned int customInterval){
  if(timer->nextInvoke == 0){
    if(customInterval > 0){
      timer->nextInvoke = millis() + customInterval;
    }else{
      timer->nextInvoke = millis() + timer->delayMs;
    }
  }
}

void timerRun(struct mTimer* timer){
  if(millis() > timer->nextInvoke && timer->nextInvoke != 0){
    timer->timerCallback();
    timer->nextInvoke = 0;
  }
}

void timerRunCyclic(struct mTimer* timer){
  if(millis() > timer->nextInvoke && timer->nextInvoke != 0){
    timer->timerCallback();
    timer->nextInvoke = millis() + timer->delayMs;
  }
}

void timerForceStop(struct mTimer* timer){
  timer->nextInvoke = 0;
}