#include <math.h>
#include "mTimer.h"
#include "mDisplay.h"

/**
* Thermistor details
* 10k ohm thermistor @ 25C
* B: 3977K ± 1%
* min...max, -40C...+105C
*/

#define TEMP_SMOOTHING 64

/* Setup pins */
unsigned int doorLedPIN = 11; // door open close led pin
unsigned int doorInputPIN = 12; // door input from reed relay.
unsigned int buzzerPIN = 10;// buzzer pin
unsigned int thermistorPIN = A0; // Thermistor analog pin

/* Other variable setup */
bool doorOpenedFirstTime = true; // state if door was closed between input changes.
unsigned int doorState = 0; // door state (open/close)
struct mTimer doorOpenTimer = {}; // door open-buzzer timer
struct mTimer displayTimer = {}; // Timer for wirting new info to display.
float calculatedCelcius = 0; // calculated celcius
float celciusHistory[TEMP_SMOOTHING] = {0}; // Track last TEMP_SMOOTHING reads.
int celciusHistoryIterator = 0; // celcius history iterator.
mdpCharacter displayStatus = MDPNAN;
int buzzing = 0;

/* forward decs */
/* Run door operations. */
void doorOperation();

/* read temperature. */
float tempRead();

/** Check thermistor calculation results. Returns non 0 value if value is not usable.*/
mdpCharacter checkTemperatureResult();

/** Calculates temperature from celcius history. */
float calculateTemperature();

// doorOpenTimer timer finished callback.
void onDoorOpenTimerFinished(){
  if(buzzing == 1){
    digitalWrite(buzzerPIN, HIGH);
    buzzing = 0;
    return;
  }
  buzzing = 1;
  digitalWrite(buzzerPIN, LOW);
}

// displayTimer timer finished callback.
void onDisplayTimerFinished(){
  Serial.println("New value to display");
  mdpDraw(displayStatus);
}

void setup() {
  Serial.begin(9600);

  // Setup pin input output
  pinMode(doorInputPIN, INPUT);
  pinMode(doorLedPIN, OUTPUT);
  pinMode(buzzerPIN, OUTPUT);

  mdpSetupDisplayPins();

  // initialize door timer struct values
  doorOpenTimer.delayMs = 500;
  doorOpenTimer.nextInvoke = 0;
  doorOpenTimer.timerCallback = &onDoorOpenTimerFinished;

  // initialize display timer values
  displayTimer.delayMs = 20000;
  displayTimer.nextInvoke = 20000;
  displayTimer.timerCallback = &onDisplayTimerFinished;
}

void loop() {

  // Door operation
  doorOperation();

  // Temperature stuff
  calculatedCelcius = tempRead();
  mdpCharacter status = checkTemperatureResult();

  // Can read temps to history
  if(status == MDPOK){
    celciusHistory[celciusHistoryIterator] = calculatedCelcius;
    if(celciusHistoryIterator + 1 < TEMP_SMOOTHING){
      celciusHistoryIterator++;
    }else{
      calculatedCelcius = calculateTemperature();
      celciusHistoryIterator = 0;
      displayStatus = mdpIntToDisplayCharacter((int) calculatedCelcius);
    }
  }else{
    // Reading is outside of the range. React immediatly.
    displayStatus = status;
    mdpDraw(displayStatus);
  }

  // Run display timer endlesly
  timerRunCyclic(&displayTimer);
  
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
    digitalWrite(buzzerPIN, LOW);
  }else{ 
    // Door is open
    digitalWrite(doorLedPIN, LOW);
    if(doorOpenedFirstTime){
      // Use longer delay if door was not previously open, 
      // buzzer does not immediatly start screaming.
      timerStart(&doorOpenTimer, 15000);
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

mdpCharacter checkTemperatureResult(){
  // range check inclusive 0...9
  if(calculatedCelcius >= 0 && calculatedCelcius <= 9){
    return MDPOK;
  }

  // range check exclusive -10...0
  if(calculatedCelcius < 0 && calculatedCelcius > -10){
    return MDPLOW;
  }

  // Range check exclusive 9...50
  if(calculatedCelcius > 9 && calculatedCelcius < 50){
    return MDPHIGH;
  }

  return MDPNAN;
}

float calculateTemperature(){
  // Calculate average of history temps.
  float sum = 0;
  for(int i = 0; i <= celciusHistoryIterator; i++){
    sum += celciusHistory[i];
  }

  return roundf(sum / celciusHistoryIterator);
}
