#define STATE_1 1
#define STATE_2 2
#define STATE_3 3
#define STATE_4 4

const int ledPinRedCars = 8;
const int ledPinYellowCars = 9;
const int ledPinGreenCars = 10;

const int ledPinRedPeople = 11;
const int ledPinGreenPeople = 12;

const int buzzerPin = 5;
const int buttonPin = 2;

const int buzzerFrequency = 500;

const int buzzerBeepOn1 = 1000;
const int buzzerBeepOff1 = 200;
const int buzzerBeepOn2 = 500;
const int buzzerBeepOff2 = 200;

const int ledBlinkOn = 500;
const int ledBlinkOff = 200;

const unsigned int delayState1State2 = 1000 * 8;
const unsigned int delayState2State3 = 1000 * 3;
const unsigned int delayState3State4 = 1000 * 8;
const unsigned int delayState4State1 = 1000 * 4;

const unsigned int debounceDelay = 50;

int currentState = STATE_1;

byte buttonState = LOW;

unsigned long lastButtonPress = 0;

byte executeSequence = false;

unsigned long lastBuzzerBeep = 0;
unsigned long lastLedBlink = 0;

volatile unsigned long lastButtonStateChange = 0;

void setup() {
  pinMode(ledPinRedCars, OUTPUT);
  pinMode(ledPinYellowCars, OUTPUT);
  pinMode(ledPinGreenCars, OUTPUT);
  pinMode(ledPinRedPeople, OUTPUT);
  pinMode(ledPinGreenPeople, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonISR, CHANGE);

  states(currentState);
}

void loop() {
  byte reading = digitalRead(buttonPin);
  byte pressed = debounceButton(reading);

  if(executeSequence) {
    stateHandler();
  }
  else {
    if (pressed) {
      lastButtonPress = millis();
      executeSequence = true;
    }
  }

  states(currentState);
}

void buttonISR() {
  lastButtonStateChange = millis();
}

byte debounceButton(byte reading) {
  if (millis() - lastButtonStateChange > debounceDelay && reading != buttonState) {
    buttonState = reading;
    if (buttonState == LOW) {
      return true;
    }
  }
  return false;
}

void stateHandler() {
  unsigned long timeDif = millis() - lastButtonPress;
  if (timeDif <= delayState1State2) {
    currentState = STATE_1;
  }
  else if (timeDif > delayState1State2 && timeDif <= delayState1State2 + delayState2State3) {
    currentState = STATE_2;
  }
  else if (timeDif > delayState1State2 + delayState2State3 && timeDif <= delayState1State2 + delayState2State3 + delayState3State4) {
    currentState = STATE_3;
  }
  else if (timeDif > delayState1State2 + delayState2State3 + delayState3State4 && timeDif <= delayState1State2 + delayState2State3 + delayState3State4 + delayState4State1) {
    currentState = STATE_4;
  }
  else {
    currentState = STATE_1;
    executeSequence = false;
  }
}

void states(int &currentState) {
  if(currentState == STATE_1) {
    noTone(buzzerPin);

    digitalWrite(ledPinGreenCars, HIGH);
    digitalWrite(ledPinRedCars, LOW);

    digitalWrite(ledPinGreenPeople, LOW);
    digitalWrite(ledPinRedPeople, HIGH);
  }
  else if (currentState == STATE_2) {
    digitalWrite(ledPinGreenCars, LOW);
    digitalWrite(ledPinYellowCars, HIGH);

    digitalWrite(ledPinRedPeople, HIGH);
  }
  else if (currentState == STATE_3) {
    digitalWrite(ledPinYellowCars, LOW);
    digitalWrite(ledPinRedCars, HIGH);

    digitalWrite(ledPinRedPeople, LOW);
    digitalWrite(ledPinGreenPeople, HIGH);

    buzzerBeep(buzzerPin, buzzerFrequency, buzzerBeepOn1, buzzerBeepOff1);
  }
  else if (currentState == STATE_4) {
    digitalWrite(ledPinRedCars, HIGH);

    ledBlink(ledPinGreenPeople, ledBlinkOn, ledBlinkOff);
    buzzerBeep(buzzerPin, buzzerFrequency, buzzerBeepOn2, buzzerBeepOff2);
  }
}

void buzzerBeep(int buzzerPin, int frequency, unsigned int delayOn, unsigned int delayOff) {
  unsigned long timeDif = millis() - lastBuzzerBeep;
  if (timeDif <= delayOn) {
    tone(buzzerPin, frequency);
  }
  else if (timeDif > delayOn && timeDif <= delayOn + delayOff) {
    noTone(buzzerPin);
  }
  else {
    lastBuzzerBeep = millis();
  }
}

void ledBlink(int ledPin, unsigned int delayOn, unsigned int delayOff) {
  unsigned long timeDif = millis() - lastLedBlink;
  if (timeDif <= delayOn) {
    digitalWrite(ledPin, HIGH);
  }
  else if (timeDif > delayOn && timeDif <= delayOn + delayOff) {
    digitalWrite(ledPin, LOW);
  }
  else {
    lastLedBlink = millis();
  }
}
