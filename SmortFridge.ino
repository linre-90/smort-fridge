#include "mTimer.h"

/**
* 10k ohm thermistor @ 25C
* B: 3977K ± 1%
*/


/* Setup pins */
unsigned int doorInputPIN = 2; // door input from reed relay.
unsigned int doorLedPIN = 3; // door open close led pin
unsigned int buzzerPIN = 4;// buzzer pin
unsigned int thermistorPIN = A0; // Thermistor analog pin

/* Other variable setup */
bool doorOpenedFirstTime = true; // state if door was closed between input changes.
unsigned int doorState = 0; // door state (open/close)
struct mTimer doorOpenTimer = {}; // door open-buzzer timer
float calculatedCelcius = 0; // calculated celcius

/* forward decs */
/* Run door operations. */
void doorOperation();
/* read temperature. */
float tempRead();

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
  Serial.begin(9600);

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
  doorOperation();
  calculatedCelcius = tempRead();
  Serial.println(calculatedCelcius);
}


void doorOperation(){
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

float tempRead(){
  float thermistorRead = 0;
  // Read the thermistor analog val, analog reads values from 0-1023.
  thermistorRead = analogRead(thermistorPIN);
  
  // convert to resistance, 1023 is the arduino analog max. 10k if the nominal value of resistor in front of the thermistor.
  thermistorRead = 1023 / thermistorRead - 1;
  thermistorRead = 10000 / thermistorRead;

  // Convert to celcius
  // 10k is thermistol nominal resistance.
  float celcius = thermistorRead / 10000;
  celcius = log(celcius);
  // 3977 is b coeffiency from thermistor datasheet.
  celcius = celcius / 3977;
  // Nominal temp for thermistor is 25 according to datasheet. 273.15 is kelvin to celcius constant.
  celcius += 1.0 / (25 + 273.15);
  celcius = 1.0 / celcius;
  // Convert kelvin to celcius.
  celcius -= 273.15;

  return celcius;
}
