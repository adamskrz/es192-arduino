// Adding helper functions
#include "HelperFunctions.cpp"  //remove for actual compilation
#include "HelperFunctions.h"

// the setup function runs once when you press reset or power the board
void setup() {
  // initialise I/O pins.
  pinMode(mainLEDpin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(sensorPin, INPUT);
  pinMode(batteryPin, INPUT);
  pinMode(batteryButtonPin, INPUT);
  pinMode(LEDBrightnessPin, INPUT);
  pinMode(buzzerVolumePin, INPUT);

  // Battery LEDs
  pinMode(batteryLEDPin[0], OUTPUT);
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

  // Map the user settings of brightness and voulme to values
  int LEDbrightness = map(analogRead(LEDBrightnessPin), 0, 1023, 0, 255);
  int buzzerVolume = map(analogRead(buzzerVolumePin), 0, 1023, 0, 255);

  if (alarmOn && sensorValue < sensitivity) {
    alarmOn = false;
  } else if (sensorValue >= sensitivity && !alarmOn) {
    alarmOn = true;
  }

  while (alarmOn) {
    sensorValue = getSensorValue();
    analogWrite(mainLEDpin, LEDbrightness);
    analogWrite(mainLEDpin, buzzerVolume);
    int delay_time = calcLEDDelay(sensorValue);
    if (!alarmOn) break;
    delay(delay_time);
    if (!alarmOn) break;
    analogWrite(mainLEDpin, 0);
    if (!alarmOn) break;
    delay(delay_time);
    if (sensorValue < sensitivity) {
      disableAlarm();
    }
  }
}