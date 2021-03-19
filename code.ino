// Adding helper functions
#include "HelperFunctions.h"
#include "HelperFunctions.cpp" //remove for actual compilation

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
    sensitivity = calcSensitivity(); // Blocks for 10 minutes!!
    writeIntIntoEEPROM(0, sensitivity);
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
        calcSensitivity();
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



