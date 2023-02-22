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

#define CALIBRATION_PIN 5
#define DOUBLE_RATE_PIN 6
#define SINE_PIN 7
#define TRI_PIN 8
#define SQR_PIN 9
#define SAW_PIN 10
#define RSAW_PIN 11
#define RAND_PIN 12

#define SINE 0
#define TRI 1
#define SQR 2
#define SAW 3
#define RSAW 4
#define RAND 5
#define NO_WAVE -1



DigiPot pot(2, 3, 4);
int rateValue = 0;

int depthPin = A1;    // select the input pin for the potentiometer
int depthValue = 0;



long millisPeriod = 0;

int potValue = 0;
int sampleReadingTime = 0;
int lastSampleReadingMicros = 0;
int lastSampleReadingIdx = 0;

uint16_t waveSample = 0;

//0 = sine, 1=square, 2=triangle, 3=sawtooh, 4=reverse saw, 5=random
int selectedWave = 1;

void setup() {
  Serial.begin(115200);

  //CONFIGURE INPUT_PULLUP SWITCHES
  pinMode(CALIBRATION_PIN, INPUT_PULLUP);
  pinMode(DOUBLE_RATE_PIN, INPUT_PULLUP);
  pinMode(SINE_PIN, INPUT_PULLUP);
  pinMode(TRI_PIN, INPUT_PULLUP);
  pinMode(SQR_PIN, INPUT_PULLUP);
  pinMode(SAW_PIN, INPUT_PULLUP);
  pinMode(RSAW_PIN, INPUT_PULLUP);
  pinMode(RAND_PIN, INPUT_PULLUP);

  pinMode(LED_BUILTIN, OUTPUT);

}

//returns the highest HIGH pin
int getSelectedWave() {
  if (digitalRead(RAND_PIN) == HIGH) {
    return RAND;
  } else if (digitalRead(RSAW_PIN) == HIGH) {
    return RSAW;
  }
  else if (digitalRead(SAW_PIN) == HIGH) {
    return SAW;
  }
  else if (digitalRead(SQR_PIN) == HIGH) {
    return SQR;
  }
  else if (digitalRead(TRI_PIN) == HIGH) {
    return TRI;
  }  else if (digitalRead(SINE_PIN) == HIGH) {
    return SINE;
  }
  return NO_WAVE;
}


int isCalibrationMode() {
  return digitalRead(CALIBRATION_PIN) == HIGH;
}

int doubleRate() {
  return 0;// digitalRead(DOUBLE_RATE_PIN) == HIGH;
}


void loop() {
  if (isCalibrationMode()) {
    pot.set(100);
    digitalWrite(LED_BUILTIN, LOW);
    //Serial.print("calibration mode : "); Serial.println(100);
  } else {
    digitalWrite(LED_BUILTIN, HIGH);
    int currentMicros = micros();
    rateValue = analogRead(A0);
    Serial.println(rateValue);
    int minPeriod = doubleRate() ? 500000 : 1000000;
    int maxPeriod = doubleRate() ? 50000 : 100000;
    millisPeriod = map(rateValue, 0, 1023, minPeriod, maxPeriod);
    bool guard = currentMicros - lastSampleReadingMicros >= millisPeriod / 256;
    if (getSelectedWave() == RAND) {
      guard = currentMicros - lastSampleReadingMicros >= millisPeriod / 32;
    }

    //Serial.print("doubleRate() = "); Serial.println(doubleRate());
    //Serial.print(rateValue); Serial.print("=>"); Serial.println(millisPeriod);

    if (guard) {
      switch (getSelectedWave())
      {
        case SINE:
          //Serial.println("SINE");
          waveSample = pgm_read_word(&sine256[lastSampleReadingIdx]);
          break;
        case TRI:
          //Serial.println("TRI");
          waveSample = pgm_read_word(&square256[lastSampleReadingIdx]);
          break;
        case SQR:
          //Serial.println("SQR");
          waveSample = pgm_read_word(&square256[lastSampleReadingIdx]);
          break;
        case SAW:
          //Serial.println("SAW");
          waveSample = pgm_read_word(&saw256[lastSampleReadingIdx]);
          break;
        case RSAW:
          //Serial.println("RSAW");
          waveSample = pgm_read_word(&rsaw256[lastSampleReadingIdx]);
          break;
        case RAND:
          //Serial.println("RAND");

          waveSample = random(0, 255);
          break;

        default:
          waveSample = 0;
          break;
      }
      int depthValue = analogRead(A1);
      depthValue = map(depthValue, 0, 1023, 0, 90);
      potValue = map(waveSample, 0, 255, depthValue, 100);
      pot.set(potValue);
      Serial.print("Rate: ");
      Serial.print(rateValue);
      Serial.print(" depthValue: ");
      Serial.print(depthValue);
      Serial.print(" getSelectedWave(): ");
      Serial.print(getSelectedWave());
      Serial.print(" waveSample: ");
      Serial.print(waveSample);
      Serial.print(" potValue : ");
      Serial.print(potValue);
      Serial.print("lastSampleReadingIdx : ");
      Serial.println(lastSampleReadingIdx);

      lastSampleReadingMicros = currentMicros;
      if (lastSampleReadingIdx < 256) {
        lastSampleReadingIdx++;
      }
      else {
        lastSampleReadingIdx = 0;
      }
    }

  }
}
