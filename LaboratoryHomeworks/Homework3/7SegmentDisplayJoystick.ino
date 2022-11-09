#define STATE_1 1
#define STATE_2 2
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

const int pinA = 4;
const int pinB = 5;
const int pinC = 6;
const int pinD = 7;
const int pinE = 8;
const int pinF = 9;
const int pinG = 10;
const int pinDP = 11;

const int segSize = 8;

const int ledBlinkOn = 500;
const int ledBlingOff = 500;

const byte nrMoves = 4;
const byte offsetPinInSegments = 4;
const byte NotAMove = -1;

// joystick pins
const int joyXPin = A0;
const int joyYPin = A1;
const int joySWPin = 12;

const int joyLeftThreshold = 400;
const int joyRightThreshold = 600;
const int joyDownThreshold = 400;
const int joyUpThreshold = 600;

bool joyIsNeutral = true;
bool joySWState = HIGH;
bool joyPrevSWState = HIGH;

unsigned long lastSWStateChange = 0;
unsigned int debounceDelay = 50;

unsigned long lastPressTime = 0;
unsigned int longPressTime = 1000 * 3;
byte toggle = false;

byte lastSWState = HIGH;
byte currentSWState = HIGH;

bool commonAnode = false; 

byte segmentOn = HIGH;
int segments[segSize] = { 
  pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP
};

bool segmentsState[segSize] = {
  0, 0, 0, 0, 0, 0, 0, 0
};

int segmentMoves[segSize][nrMoves] = {
  // UP,            DOWN,                     LEFT,                           RIGHT
  {NotAMove, pinG - offsetPinInSegments, pinF - offsetPinInSegments, pinB - offsetPinInSegments },                    // a
  {pinA - offsetPinInSegments, pinG - offsetPinInSegments, pinF - offsetPinInSegments, NotAMove},                     // b
  {pinG - offsetPinInSegments, pinD - offsetPinInSegments, pinE - offsetPinInSegments, pinDP - offsetPinInSegments},  // c
  {pinG - offsetPinInSegments, NotAMove, pinE - offsetPinInSegments, pinC - offsetPinInSegments},                     // d
  {pinG - offsetPinInSegments, pinD - offsetPinInSegments, NotAMove, pinC - offsetPinInSegments},                     // e
  {pinA - offsetPinInSegments, pinG - offsetPinInSegments, NotAMove, pinB - offsetPinInSegments},                     // f
  {pinA - offsetPinInSegments, pinD - offsetPinInSegments, NotAMove, NotAMove},                                       // g
  {NotAMove, NotAMove, pinC - offsetPinInSegments, NotAMove}                                                          // dp
};

byte currentSeg = segSize - 1;
byte currentState = STATE_1;

unsigned long lastLedBlink = 0;

void setup() {
  if (commonAnode == true) {
    segmentOn = !segmentOn;
  }

  for (int i = 0; i < segSize; i++) {
    pinMode(segments[i], OUTPUT);
    digitalWrite(segments[i], !segmentOn);
  }

  pinMode(joySWPin, INPUT_PULLUP);
  pinMode(joyXPin, INPUT);
  pinMode(joyYPin, INPUT);

  Serial.begin(9600);
}

void loop() {
  byte reading = digitalRead(joySWPin);
  byte pressed = debounceButton(reading, currentSWState, lastSWState, lastSWStateChange, debounceDelay);
  lastSWState = reading;

  if (pressed) {
    if (currentState == STATE_1) {
      currentState = STATE_2;
    }
    else {
      currentState = STATE_1;
    }

    toggle = true;
    lastPressTime = millis();
  }

  if (toggle && millis() - lastPressTime > longPressTime && currentSWState == LOW && currentState == STATE_2){
    toggle = false;
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
  for (int i = 0; i < segSize; i++) {
    segmentsState[i] = 0;
  }

  currentSeg = segSize - 1;
  currentState = STATE_1;
}

void stateHandler() {
  int joyY = analogRead(joyYPin);
  int joyX = analogRead(joyXPin);
  if (currentState == STATE_1) {
    byte lastSeg = currentSeg;
    if(joyY < joyLeftThreshold && joyIsNeutral) {
      joyIsNeutral = false;
      currentSeg = segmentMoves[currentSeg][LEFT];
    }
    if(joyY > joyRightThreshold && joyIsNeutral) {
      joyIsNeutral = false;
      currentSeg = segmentMoves[currentSeg][RIGHT];
    }
    if(joyX < joyDownThreshold && joyIsNeutral) {
      joyIsNeutral = false;
      currentSeg = segmentMoves[currentSeg][DOWN];
    }
    if(joyX > joyUpThreshold && joyIsNeutral) {
      joyIsNeutral = false;
      currentSeg = segmentMoves[currentSeg][UP];
    }

    if (currentSeg == NotAMove) 
      currentSeg = lastSeg;

  }
  else if (currentState == STATE_2) {
    if((joyX < joyDownThreshold || joyX > joyUpThreshold) && joyIsNeutral) {
      joyIsNeutral = false;
      segmentsState[currentSeg] = !segmentsState[currentSeg];
    }
  }

  if(joyLeftThreshold <= joyY && joyY <= joyRightThreshold && joyDownThreshold <= joyX && joyX <= joyUpThreshold) {
    joyIsNeutral = true;
  }
}

void runState() {
  for (int i = 0; i < segSize; i++) {
    if (currentSeg != i) {
      digitalWrite(segments[i], segmentsState[i] ^ commonAnode);
    }
    else {
      if (currentState == STATE_1) {
        ledBlink(segments[i], ledBlinkOn, ledBlingOff);
      }
      else{
        digitalWrite(segments[i], segmentsState[i] ^ commonAnode);
      }
    }
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
