/*
  Modified on Nov 16, 2020
  Modified by MehranMaleki from Arduino Examples
  Home
*/

/*
   For this example, connect your X9C103P (or the like) as follows:
   1 - INC - Arduino pin 2
   2 - U/D - Arduino pin 3
   3 - VH  - 5V
   4 - VSS - GND
   5 - VW  - Output: Arduino pin A0 for analogRead
   6 - VL  - GND
   7 - CS  - Arduino pin 4
   8 - VCC - 5V
*/
#include "avr/pgmspace.h"

#include <DigiPotX9Cxxx.h>
#include "waves.h"

DigiPot pot(2, 3, 4);
int ratePin = A0;    // select the input pin for the potentiometer
int rateValue = 0;

int depthPin = A1;    // select the input pin for the potentiometer
int depthValue = 0;


long millisPeriod = 0;

int potValue = 0;
int sampleReadingTime = 0;
int lastSampleReadingMicros = 0;
int lastSampleReadingIdx = 0;

int waveSample = 0;

//0 = sine, 1=square, 2=triangle, 3=sawtooh, 4=reverse saw, 5=random
int selectedWave = 1;

void setup() {
  Serial.begin(2000000);
}

void loop() {
  int currentMicros = micros();
  rateValue = analogRead(ratePin);
  millisPeriod = map(rateValue, 0, 1023, 1000000, 100000);
  bool guard = currentMicros - lastSampleReadingMicros >= millisPeriod / 256;
  if (selectedWave == 4) {
    guard = currentMicros - lastSampleReadingMicros >= millisPeriod/32;
  }

  Serial.print(rateValue); Serial.print("=>"); Serial.println(millisPeriod);

  if (guard) {
    switch (selectedWave)
    {
      case 0:
        waveSample = sine256[lastSampleReadingIdx];
        break;

      case 1:
        waveSample = square256[lastSampleReadingIdx];
        break;
      case 2:
        waveSample = saw256[lastSampleReadingIdx];
        break;
      case 3:
        waveSample = rsaw256[lastSampleReadingIdx];
        break;
      case 4:
        waveSample = random(0, 255);
        break;

      default:
        break;
    }
    potValue = map(waveSample, 0, 255, 0, 100);
    pot.set(potValue);
    lastSampleReadingMicros = currentMicros;
    if (lastSampleReadingIdx < 256) {
      lastSampleReadingIdx++;
    }
    else {
      lastSampleReadingIdx = 0;
    }
  }

  Serial.println(potValue);
}
