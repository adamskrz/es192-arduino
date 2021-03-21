// Adding helper functions
#include "HelperFunctions.cpp"  //remove for actual compilation
#include "HelperFunctions.h"

// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(mainLEDpin,
          OUTPUT);  // initialise digital pin LED_BUILTIN as an output.

  pinMode(sensorPin, INPUT);   // initialise sensor connection as input
  pinMode(batteryPin, INPUT);  // initialise sensor connection as input

  pinMode(batteryLEDPin[0], OUTPUT);  // Battery LEDs
  pinMode(batteryLEDPin[1], OUTPUT);
  pinMode(batteryLEDPin[2], OUTPUT);
  pinMode(batteryLEDPin[3], OUTPUT);

  // Setup main button and its interrupt
  pinMode(resetButtonPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(resetButtonPin), disableAlarm, RISING);

  // set sensitivity from EEPROM
  sensitivity = readIntFromEEPROM(0);
  if (sensitivity == 32767) {
    // if sensitivity never set, calibrate
    sensitivity = calcSensitivity();  // Blocks for 10 minutes!!
    writeIntIntoEEPROM(0, sensitivity);
  }
}

// the loop function runs over and over again forever
void loop() {
  // If the battery indicator button is pressed
  if (digitalRead(batteryButtonPin) == HIGH) {
    // Show current power level for 1 second then clear
    displayBatteryLevel();
    delay(1000);
    clearBatteryLEDs();

    // if still pressed after 1 second, wait another 4 seconds and run
    // calibration if still pressed
    if (digitalRead(batteryButtonPin) == HIGH) {
      delay(4000);
      if (digitalRead(batteryButtonPin) == HIGH) {
        calcSensitivity();
      }
    }
  }

  int sensorValue = getSensorValue();

  int brightness = map(sensorValue, 0, 1023, 0, 255);

  if (alarmOn && sensorValue < sensitivity) {
    alarmOn = false;
  } else if (sensorValue >= sensitivity && !alarmOn) {
    alarmOn = true;
  }

  while (alarmOn) {
    analogWrite(mainLEDpin, brightness);
    int delay_time = calcLEDDelay(sensorValue);
    if (!alarmOn) break;
    delay(delay_time);
    if (!alarmOn) break;
    analogWrite(mainLEDpin, 0);
    if (!alarmOn) break;
    delay(delay_time);
    if (sensorValue < sensitivity) {
      alarmOn = false;
    }
  }
}