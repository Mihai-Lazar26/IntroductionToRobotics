const int redPotPin = A0;
const int greenPotPin = A1;
const int bluePotPin = A2;
const int redLedPin = 9;
const int greenLedPin = 10;
const int blueLedPin = 11;

const int minAnalogReadValue = 0;
const int maxAnalogReadValue = 1023;
const int minAnalogWriteValue = 0;
const int maxAnalogWriteValue = 255;

void setup() {
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
  pinMode(redPotPin, INPUT);
  pinMode(greenPotPin, INPUT);
  pinMode(bluePotPin, INPUT);
  Serial.begin(9600);
}

void loop() {
  int redPotValue = analogRead(redPotPin);
  int greenPotValue = analogRead(greenPotPin);
  int bluePotValue = analogRead(bluePotPin);

  setColorAnalogRead(redPotValue, greenPotValue, bluePotValue);
}

void setColorAnalogRead(int redPotValue, int greenPotValue, int bluePotValue){

  int mappedRedPotValue = map(redPotValue, minAnalogReadValue, maxAnalogReadValue, minAnalogWriteValue, maxAnalogWriteValue);
  int mappedGreenPotValue = map(greenPotValue, minAnalogReadValue, maxAnalogReadValue, minAnalogWriteValue, maxAnalogWriteValue);
  int mappedBluePotValue = map(bluePotValue, minAnalogReadValue, maxAnalogReadValue, minAnalogWriteValue, maxAnalogWriteValue);

  analogWrite(redLedPin, mappedRedPotValue);
  analogWrite(greenLedPin, mappedGreenPotValue);
  analogWrite(blueLedPin, mappedBluePotValue);

  Serial.println("RGB(" + String(mappedRedPotValue) + ", " + String(mappedGreenPotValue) + ", " + String(mappedBluePotValue) + ")");
}

