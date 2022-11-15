//DS= [D]ata [S]torage - data
//STCP= [ST]orage [C]lock [P]in latch
//SHCP= [SH]ift register [C]lock [P]in clock

#define STATE_1 1
#define STATE_2 2
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

// joystick pins
const int joyXPin = A0;
const int joyYPin = A1;
const int joySWPin = 9;

const int joyLeftThreshold = 400;
const int joyRightThreshold = 600;
const int joyDownThreshold = 400;
const int joyUpThreshold = 600;

const int latchPin = 11; // STCP to 12 on Shift Register
const int clockPin = 10; // SHCP to 11 on Shift Register
const int dataPin = 12; // DS to 14 on Shift Register

const int segD1 = 7;
const int segD2 = 6;
const int segD3 = 5;
const int segD4 = 4;

const byte regSize = 8; // 1 byte aka 8 bits

const int blinkOnDelay = 500;
const int blinkOffDelay = 500;

bool joyIsNeutral = true;
bool joySWState = HIGH;
bool joyPrevSWState = HIGH;

unsigned long lastSWStateChange = 0;
unsigned int debounceDelay = 50;

unsigned long lastPressTime = 0;
unsigned int longPressTime = 1000 * 3;
byte resetPossible = false;

byte lastSWState = HIGH;
byte currentSWState = HIGH;

int displayDigits[] = {
  segD1, segD2, segD3, segD4
};
const int displayCount = 4;
const int encodingsNumber = 16;

int byteEncodings[encodingsNumber] = {
//A B C D E F G DP 
  B11111100, // 0 
  B01100000, // 1
  B11011010, // 2
  B11110010, // 3
  B01100110, // 4
  B10110110, // 5
  B10111110, // 6
  B11100000, // 7
  B11111110, // 8
  B11110110, // 9
  B11101110, // A
  B00111110, // b
  B10011100, // C
  B01111010, // d
  B10011110, // E
  B10001110  // F
};

byte displayValues[] = {
  0, 0, 0, 0
};

byte currentState = STATE_1;
byte currentDisplay = 0;

unsigned long lastLedBlink = 0;

void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  for (int i = 0; i < displayCount; i++) {
    pinMode(displayDigits[i], OUTPUT);
    digitalWrite(displayDigits[i], LOW);
  }  

  pinMode(joyXPin, INPUT);
  pinMode(joyYPin, INPUT);
  pinMode(joySWPin, INPUT_PULLUP);

  Serial.begin(9600);
}

void loop() {
  byte reading = digitalRead(joySWPin);
  byte pressed = debounceButton(reading, currentSWState, lastSWState, lastSWStateChange, debounceDelay);
  lastSWState = reading;

  if (pressed) {
    if (currentState == STATE_1) {
      currentState = STATE_2;
      resetPossible = true;
      lastPressTime = millis();
    }
    else {
      resetPossible = false;
      currentState = STATE_1;
    }
  }

  if (resetPossible && millis() - lastPressTime > longPressTime && currentSWState == LOW){
    resetPossible = false;
    reset();
  }

  stateHandler();
  runState();
}

byte debounceButton(byte &reading, byte &buttonState, byte &lastButtonState, unsigned long &lastDebounceTime, unsigned int &debounceDelay) {
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        return true;
      }
    }
  }
  return false;
}

void reset() {
  for (int i = 0; i < displayCount; i++) {
    displayValues[i] = 0;
  }
  currentDisplay = 0;
  currentState = STATE_1;
}

void stateHandler() {
  int joyY = analogRead(joyYPin);
  int joyX = analogRead(joyXPin);
  if (currentState == STATE_1) {
    if(joyY < joyLeftThreshold && joyIsNeutral) {
      joyIsNeutral = false;
      currentDisplay = constrain(currentDisplay + 1, 0, displayCount - 1);
    }
    if(joyY > joyRightThreshold && joyIsNeutral) {
      joyIsNeutral = false;
      currentDisplay = constrain(currentDisplay - 1, 0, displayCount - 1);
    }
  }
  else if (currentState == STATE_2) {
    if(joyX < joyDownThreshold && joyIsNeutral) {
      joyIsNeutral = false;
      displayValues[currentDisplay] = constrain(displayValues[currentDisplay] - 1, 0, encodingsNumber - 1);
    }
    if(joyX > joyUpThreshold && joyIsNeutral) {
      joyIsNeutral = false;
      displayValues[currentDisplay] = constrain(displayValues[currentDisplay] + 1, 0, encodingsNumber - 1);
    } 
  }

  if(joyLeftThreshold <= joyY && joyY <= joyRightThreshold && joyDownThreshold <= joyX && joyX <= joyUpThreshold) {
    joyIsNeutral = true;
  }
}

void runState() {
  for (int i = 0; i < displayCount; i++) {
    if (i != currentDisplay) {
      writeReg(byteEncodings[displayValues[i]]);
    }
    else {
      if (currentState == STATE_1) {
        writeReg(blinkEncodingDP(byteEncodings[displayValues[i]], blinkOnDelay, blinkOffDelay));
      }
      else {
        writeReg(bitFlip(byteEncodings[displayValues[i]], 0));
      }
    }
    activateDisplay(i);
    delay(5);
  }
}

void writeReg(int digit) {
    // ST_CP LOW to keep LEDs from changing while reading serial data
    digitalWrite(latchPin, LOW);
    // Shift out the bits
    shiftOut(dataPin, clockPin, MSBFIRST, digit);
    /* ST_CP on Rising to move the data from shift register
     * to storage register, making the bits available for output.
     */
    digitalWrite(latchPin, HIGH);
}

void activateDisplay(int displayNumber) { 
  // first, disable all the display digits
  for (int i = 0; i < displayCount; i++) {
    digitalWrite(displayDigits[i], HIGH);
  }
  // then enable only the digit you want to use now
  digitalWrite(displayDigits[displayNumber], LOW);
}

byte blinkEncodingDP(byte encoding, unsigned int delayOn, unsigned int delayOff) {
  unsigned long timeDif = millis() - lastLedBlink;
  if (timeDif <= delayOn) {
    return bitFlip(encoding, 0);
  }
  else if (timeDif > delayOn && timeDif <= delayOn + delayOff) {
    return encoding;
  }
  else {
    lastLedBlink = millis();
  }
  return encoding;
}

byte bitFlip(byte number, byte pos) {
  byte mask = 1 << pos;
  return number ^ mask;
}
