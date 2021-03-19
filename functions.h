#include "eeprom_helpers.h"

// Declare variables from main sketch
extern byte batteryLEDPin[];
extern byte batteryPin;
extern byte sensorPin;
extern int sensorValue;
extern int sensitivity;

// Battery discharge reference
// https://www.powerstream.com/z/9v-100ma-discharge-tests.png
void displayBatteryLevel() {

  float voltage = (9 * analogRead(batteryPin)) / 1024;
  if (voltage < 6) {
    digitalWrite(batteryLEDPin[0], HIGH);
    digitalWrite(batteryLEDPin[1], LOW);
    digitalWrite(batteryLEDPin[2], LOW);
    digitalWrite(batteryLEDPin[3], LOW);
  } else if (voltage < 7) {
    digitalWrite(batteryLEDPin[0], HIGH);
    digitalWrite(batteryLEDPin[1], HIGH);
    digitalWrite(batteryLEDPin[2], LOW);
    digitalWrite(batteryLEDPin[3], LOW);
  } else if (voltage < 7.5) {
    digitalWrite(batteryLEDPin[0], HIGH);
    digitalWrite(batteryLEDPin[1], HIGH);
    digitalWrite(batteryLEDPin[2], HIGH);
    digitalWrite(batteryLEDPin[3], LOW);
  } else {
    digitalWrite(batteryLEDPin[0], HIGH);
    digitalWrite(batteryLEDPin[1], HIGH);
    digitalWrite(batteryLEDPin[2], HIGH);
    digitalWrite(batteryLEDPin[3], HIGH);
  }
}

void clearBatteryLEDs() {
  digitalWrite(batteryLEDPin[0], LOW);
  digitalWrite(batteryLEDPin[1], LOW);
  digitalWrite(batteryLEDPin[2], LOW);
  digitalWrite(batteryLEDPin[3], LOW);
}

int getSensorValue() {
  // Get a sensor value as an average of 10 readings over the course of 0.1
  // seconds
  int tmpSensor = 0;
  for (int i = 0; i < 10; i++) {
    tmpSensor = tmpSensor + analogRead(sensorPin);
    delay(10);
  }
  sensorValue = tmpSensor / 10;
  return sensorValue;
}

bool wet() {
  if (getSensorValue() > sensitivity) {
    return true;
  } else {
    return false;
  }
}

// set value at which alarm starts
void setSensitivity() {
  unsigned long totalReading = 0;
  int maxReading = 0;
  int minReading = 0;
  int tmpValue = 0;

  for (int i = 0; i < 100; i++) {
    // 100 readings over 10 minutes, 0.1 sec delay
    tmpValue = getSensorValue();
    totalReading = totalReading + tmpValue;
    if (maxReading < tmpValue) {
      maxReading = tmpValue;
    } else if (minReading > tmpValue) {
      minReading = tmpValue;
    }
  }

  int avgReading = totalReading / 100;
  sensitivity = maxReading * 1.1;

  writeIntIntoEEPROM(0, sensitivity);
}

int calcLEDDelay() {
  // above maxSensorValue, the LED blink will not increase
  int maxSensorVal = 1500;
  int minBlinkDelay = 50;
  int maxBlinkDelay = 1000;
  if (sensorValue > maxSensorVal) {
    return 50;
  } else {
    float scale =
        1 - ((sensorValue - sensitivity) / (maxSensorVal - sensitivity));
    return (maxSensorVal - (scale * (maxBlinkDelay - minBlinkDelay)));
  }
}