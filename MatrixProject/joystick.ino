#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

const int joyXPin = A0;
const int joyYPin = A1;
const int joySWPin = 13;

const int joyLeftThreshold = 300;
const int joyRightThreshold = 700;
const int joyDownThreshold = 300;
const int joyUpThreshold = 700;

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

unsigned long lastMoveTime = 0;
const unsigned int delayMove = 500;

byte direction;
byte lastDirection;

// void setDirection(byte val) {
//   direction = val;
// }

// byte getDirection() {
//   return direction;
// }

void joystickSetup() {
  pinMode(joyXPin, INPUT);
  pinMode(joyYPin, INPUT);
  pinMode(joySWPin, INPUT_PULLUP);
}

void joyStickGameSetup() {
  direction = RIGHT;
  lastDirection = RIGHT;
}

void joystickGameLoop() {
  int joyX = analogRead(joyXPin);
  int joyY = analogRead(joyYPin);

  if(joyY < joyLeftThreshold && lastDirection != LEFT) {
    direction = RIGHT;
  }
  if(joyY > joyRightThreshold && lastDirection != RIGHT) {
    direction = LEFT;
  }
  if(joyX < joyDownThreshold && lastDirection != DOWN) {
    direction = UP;
  }
  if(joyX > joyUpThreshold && lastDirection != UP) {
    direction = DOWN;
  }

  if (millis() - lastMoveTime > delayMove) {

    if (direction == LEFT) {
      lastDirection = LEFT;
      moveLeft();
    }
    if (direction == RIGHT) {
      lastDirection = RIGHT;
      moveRight();
    }
    if (direction == DOWN) {
      lastDirection = DOWN;
      moveDown();
    }
    if (direction == UP) {
      lastDirection = UP;
      moveUp();
    }

    lastMoveTime = millis();
  }
}

void joystickMenuLoop() {
  int joyX = analogRead(joyXPin);
  int joyY = analogRead(joyYPin);

  if(joyX < joyDownThreshold && joyIsNeutral) {
    moveUpMenu();
    joyIsNeutral = false;
  }
  if(joyX > joyUpThreshold && joyIsNeutral) {
    moveDownMenu();
    joyIsNeutral = false;
  }
  if(joyY < joyLeftThreshold && joyIsNeutral) {
    moveRightMenu();
    joyIsNeutral = false;
  }
  if(joyY > joyRightThreshold && joyIsNeutral) {
    moveLeftMenu();
    joyIsNeutral = false;
  }

  if(joyLeftThreshold <= joyY && joyY <= joyRightThreshold && joyDownThreshold <= joyX && joyX <= joyUpThreshold) {
    joyIsNeutral = true;
  }
}

void debounceSW() {
  byte reading = digitalRead(joySWPin);
  byte pressed = debounceButton(reading, currentSWState, lastSWState, lastSWStateChange, debounceDelay);
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

