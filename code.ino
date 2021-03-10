// Global
const byte sensorPin = A0;
const byte sensitivtyPin = A1;
float sensorValue = 0;
int sensitivity;
// the setup function runs once when you press reset or power the board
void setup()
{
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(A0, INPUT);
  float sensorValue = 0;
  const byte sensorPin = A0;
}

void blink()
{
  digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
  delay(1000);                     // wait for a second
  digitalWrite(LED_BUILTIN, LOW);  // turn the LED off by making the voltage LOW
  delay(1000);                     // wait for a second
}

bool wet()
{
  for (int i = 0; i < 10; i++)
  {
    sensorValue = sensorValue + analogRead(sensorPin);
    delay(1);
  }
  sensorValue = sensorValue / 10;

  if (sensorValue > sensitivity)
  {
    return true;
  }
  else
  {
    return false;
  }
}

// set value at which alarm starts (between 0 and 506 on input)
void setSensitivity()
{
  int val = analogRead(sensitivtyPin);
  sensitivity = val / 4;
}

int calcLEDDelay()
{
  // above maxSensorValue, the LED blink will not increase
  int maxSensorVal = 1500;
  int minBlinkDelay = 50;
  int maxBlinkDelay = 1000;
  if (sensorValue > maxSensorVal)
  {
    return 50;
  }
  else
  {
    float scale = 1 - ((sensorValue - sensitivity)/(maxSensorVal-sensitivity));
    return (maxSensorVal - (scale*(maxBlinkDelay-minBlinkDelay)));
  }
}

// the loop function runs over and over again forever
void loop()
{
  setSensitivity();

  if (wet())
  {
    digitalWrite(LED_BUILTIN, HIGH);
    int delay_time = calcLEDDelay();
    delay(delay_time);                     // wait for a second
    digitalWrite(LED_BUILTIN, LOW);  // turn the LED off by making the voltage LOW
    delay(delay_time);
  }

  digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
  delay(1000);                     // wait for a second
  digitalWrite(LED_BUILTIN, LOW);  // turn the LED off by making the voltage LOW
  delay(1000);                     // wait for a second
}