#include <EEPROM.h>

// Global
const byte sensorPin = A0;
// const byte sensitivtyPin = A1;
const byte batteryPin = A1;
int sensorValue = 0;
int sensitivity;

// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(LED_BUILTIN, OUTPUT); // initialise digital pin LED_BUILTIN as an output.
  
  pinMode(sensorPin, INPUT); // initialise sensor connection as input
  pinMode(batteryPin, INPUT); // initialise sensor connection as input 

  pinMode(0, OUTPUT); // Battery LEDs
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);

  float sensorValue = 0;
  const byte sensorPin = A0;

  // set sensitivity from EEPROM
  sensitivity = readIntFromEEPROM(0);
  if (sensitivity == 32767) {
    // if sensitivity never set, calibrate
    setSensitivity();
  }
}

// Source: https://roboticsbackend.com/arduino-store-int-into-eeprom/
void writeIntIntoEEPROM(int address, int number) {
  byte byte1 = number >> 8;
  byte byte2 = number & 0xFF;
  EEPROM.update(address, byte1);
  EEPROM.update(address + 1, byte2);
}

int readIntFromEEPROM(int address) {
  byte byte1 = EEPROM.read(address);
  byte byte2 = EEPROM.read(address + 1);
  return (byte1 << 8) + byte2;
}

// Battery discharge reference https://www.powerstream.com/z/9v-100ma-discharge-tests.png
void displayBatteryLevel() {
  float voltage = (9 * analogRead(batteryPin)) / 1024;
  if (voltage < 6) {
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(LED_BUILTIN, LOW);
  } else if (voltage < 7) {
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(LED_BUILTIN, LOW);
  } else if (voltage < 7.5) {
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(LED_BUILTIN, LOW);
  } else {
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(LED_BUILTIN, HIGH);
  }

  delay (1000);
}


int getSensorValue() {
  // Get a sensor value as an average of 10 readings over the course of 0.1
  // secondsr
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

// // set value at which alarm starts (between 0 and 506 on input)
// void setSensitivity() {
//   int val = analogRead(sensitivtyPin);
//   sensitivity = val / 4;
// }

// set value at which alarm starts (between 0 and 506 on input)
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

// the loop function runs over and over again forever
void loop() {
  if (wet()) {
    digitalWrite(LED_BUILTIN, HIGH);
    int delay_time = calcLEDDelay();
    delay(delay_time);
    digitalWrite(LED_BUILTIN, LOW);
    delay(delay_time);
  }
}