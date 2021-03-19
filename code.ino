// Adding helper functions
#include "eeprom_helpers.h"
#include "functions.h"

// Global
const byte sensorPin = A0;
// const byte sensitivtyPin = A1;
const byte batteryPin = A1;
int sensorValue = 0;
int sensitivity;
const byte batteryLEDPin[] = {8, 9, 10, 11};
const byte batteryButtonPin = 2;
const byte resetButtonPin = 3;

// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(LED_BUILTIN,
          OUTPUT);  // initialise digital pin LED_BUILTIN as an output.

  pinMode(sensorPin, INPUT);   // initialise sensor connection as input
  pinMode(batteryPin, INPUT);  // initialise sensor connection as input

  pinMode(batteryLEDPin[0], OUTPUT);  // Battery LEDs
  pinMode(batteryLEDPin[1], OUTPUT);
  pinMode(batteryLEDPin[2], OUTPUT);
  pinMode(batteryLEDPin[3], OUTPUT);

  float sensorValue = 0;
  const byte sensorPin = A0;

  // set sensitivity from EEPROM
  sensitivity = readIntFromEEPROM(0);
  if (sensitivity == 32767) {
    // if sensitivity never set, calibrate
    setSensitivity();
  }
}

// the loop function runs over and over again forever
void loop() {
  if (digitalRead(batteryButtonPin) == HIGH) {
    displayBatteryLevel();
    delay(1000);
    clearBatteryLEDs();
    if (digitalRead(batteryButtonPin) == HIGH) {
      delay(3000);
      if (digitalRead(batteryButtonPin) == HIGH) {
        setSensitivity();
      }
    }
  }

  if (wet()) {
    digitalWrite(LED_BUILTIN, HIGH);
    int delay_time = calcLEDDelay();
    delay(delay_time);
    digitalWrite(LED_BUILTIN, LOW);
    delay(delay_time);
  }
}



