#include "mTimer.h"

/* Setup pins */
unsigned int doorInputPIN = 7; // door input from reed relay.
unsigned int buzzerPIN = 8;// buzzer pin
unsigned int doorLedPIN = 11; // door open close led pin

/* Other variable setup */
bool doorOpenedFirstTime = true; // state if door was closed between input changes.
unsigned int doorState = 0; // door state (open/close)
struct mTimer doorOpenTimer = {}; // door open-buzzer timer

// doorOpenTimer timer finished callback.
void onDoorOpenTimerFinished(){
  // do some buzzing with buzzer.
  for(int i = 0; i < 80; i++){
    digitalWrite(buzzerPIN, HIGH);
    digitalWrite(buzzerPIN, LOW);
    delay(1);
  }
}

void setup() {
  // Serial.begin(9600);

  // Setup pin input output
  pinMode(doorInputPIN, INPUT);
  pinMode(doorLedPIN, OUTPUT);
  pinMode(buzzerPIN, OUTPUT);

  // initialize door timer struct values
  doorOpenTimer.delayMs = 1500;
  doorOpenTimer.nextInvoke = 0;
  doorOpenTimer.timerCallback = &onDoorOpenTimerFinished;
}

void loop() {
  // read doorState
  doorState = digitalRead(doorInputPIN);

  // Handle door open/close detection
  if(doorState == HIGH){
    // Door is closed
    digitalWrite(doorLedPIN, HIGH);
    timerForceStop(&doorOpenTimer);
    doorOpenedFirstTime = true;
  }else{ 
    // Door is open
    digitalWrite(doorLedPIN, LOW);
    if(doorOpenedFirstTime){
      // Use longer delay if door was not previously open, 
      // buzzer does not immediatly start screaming.
      timerStart(&doorOpenTimer, 10000);
      doorOpenedFirstTime = false;
    }else{
      // Use timer interval value
      timerStart(&doorOpenTimer, 0);
    }
  }

  // Run doortimer
  timerRun(&doorOpenTimer);  
}
