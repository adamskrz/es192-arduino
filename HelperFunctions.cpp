#include "HelperFunctions.h"
#include "eeprom_helpers.h"

// Constants (pins)
const byte sensorPin = A0;
const byte batteryPin = A1;
const byte buzzerVolumePin = A2;
const byte LEDBrightnessPin = A3;
const byte batteryLEDPin[] = {8, 9, 10, 11};
const byte batteryButtonPin = 2;
const byte resetButtonPin = 3;
const byte mainLEDpin = 5;
const byte buzzerPin = 6;

// Very rarely set so keeping as global variables
int sensitivity;
boolean alarmOn = false;

// Battery discharge reference
// https://www.powerstream.com/z/9v-100ma-discharge-tests.png
void displayBatteryLevel() {
  // Voltage divider halves the input voltage from 9V to 4.5V. As the analogue
  // input operates on a scale from 0V to 5V, we need to adjust the input to be
  // able to make direct comparisons to battery voltage:
  float voltage = (9 * (5 / 4.5) * analogRead(batteryPin)) / 1024;
  // Show battery level roughly equivalent to percentage on the LEDs
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
  // Disable all the battery LEDs
  digitalWrite(batteryLEDPin[0], LOW);
  digitalWrite(batteryLEDPin[1], LOW);
  digitalWrite(batteryLEDPin[2], LOW);
  digitalWrite(batteryLEDPin[3], LOW);
}

int getSensorValue() {
  // Get a sensor value as an average of 10 readings over the course of 0.1
  // seconds. This is used rather than reading
  int tmpSensor = 0;
  for (int i = 0; i < 10; i++) {
    tmpSensor = tmpSensor + analogRead(sensorPin);
    delay(10);
  }

  return (tmpSensor / 10);
}

int calcSensitivity() {
  // Calculate sensitivity (value for alarm to start over 10 minutes)
  unsigned long totalReading = 0;
  int maxReading = 0;
  int tmpValue = 0;
  int sensitivity = 0;

  digitalWrite(batteryLEDPin[0], HIGH);

  for (int i = 0; i < 100; i++) {
    // Take 100 readings over 10 minutes, 0.1 sec delay
    tmpValue = getSensorValue();
    if (maxReading < tmpValue) maxReading = tmpValue;
    delay(100);
    // Flash an LED to indicate the reading taking place
    digitalWrite(batteryLEDPin[0], !digitalRead(batteryLEDPin[0]));
  }
  digitalWrite(batteryLEDPin[0], LOW);

  // The sensitivity is set as 1.2x the maximum observed value during the
  // calculation time
  sensitivity = maxReading * 1.2;
  return sensitivity;
}

int calcLEDDelay(int sensorValue) {
  // Use linear interpolation to dynamically generate required delay between LED
  // blinks in alarm
  // above maxSensorValue, the LED blink speed will not increase
  int maxSensorVal = 950;
  int minBlinkDelay = 50;
  int maxBlinkDelay = 1000;
  extern int sensitivity;

  if (sensorValue > maxSensorVal) {
    return 50;
  } else {
    float scale =
        1 - ((sensorValue - sensitivity) / (maxSensorVal - sensitivity));
    return (maxSensorVal - (scale * (maxBlinkDelay - minBlinkDelay)));
  }
}

void disableAlarm() {
  // Function used as ISR to disable the alarms when the button is pressed
  alarmOn = false;
  digitalWrite(mainLEDpin, LOW);
  digitalWrite(buzzerPin, LOW);
}