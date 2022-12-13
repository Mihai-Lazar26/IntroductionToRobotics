#include "LedControl.h" //  need the library

#define EMPTY 0
#define SNAKE 1
#define FOOD 2
#define WALL 3

#define SEGMENT_ROW 0
#define SEGMENT_COL 1
#define SEGMENT_HEAD 0

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

const unsigned int foodBlinkOn = 500, foodBlinkOff = 500;
const unsigned int headBlinkOn = 50, headBlinkOff = 50;

const byte maxSnakeSegments = 64;
byte lastSegment;
byte snakeSegments[2][maxSnakeSegments];

byte matrixState[matrixSize][matrixSize];
byte foodRow, foodCol;
byte score;

unsigned long lastHeadBlink = 0, lastFoodBlink = 0;

void boardSetup() {
  if (difficulty == EASY) {
    for (int row = 0; row < matrixSize; ++row){
      for (int col = 0; col < matrixSize; ++col){
        matrixState[col][row] = EMPTY;
      }
    }
  }
  else if (difficulty == NORMAL) {
    for (int row = 0; row < matrixSize; ++row){
      for (int col = 0; col < matrixSize; ++col){
        if (row == 0 || row == matrixSize - 1 || col == 0 || col == matrixSize - 1) {
          matrixState[col][row] = WALL;
        }
        else {
          matrixState[col][row] = EMPTY;
        }
      }
    }
  }
  else if (difficulty == HARD) {
    for (int row = 0; row < matrixSize; ++row){
      for (int col = 0; col < matrixSize; ++col){
        if (row == 0 || row == matrixSize - 1 || col == 0 || col == matrixSize - 1) {
          matrixState[col][row] = WALL;
        }
        else {
          matrixState[col][row] = EMPTY;
        }
      }
    }
    matrixState[1][5] = WALL;
    matrixState[1][6] = WALL;
    matrixState[2][5] = WALL;
    matrixState[2][6] = WALL;
    matrixState[3][1] = WALL;
    matrixState[3][2] = WALL;
    matrixState[5][4] = WALL;
    matrixState[5][5] = WALL;
  }
}

void gameSetup() {
  boardSetup();
  byte snakeRow = matrixSize / 2;
  byte snakeCol = matrixSize / 2 - 1;
  snakeSegments[SEGMENT_ROW][SEGMENT_HEAD] = snakeRow;
  snakeSegments[SEGMENT_COL][SEGMENT_HEAD] = snakeCol;
  lastSegment = 0;
  score = 0;
  addSnakeSegment(snakeRow, snakeCol + 1);

  matrixState[snakeRow][snakeCol] = SNAKE;
  generateRandomFood();
  matrixState[foodRow][foodCol] = FOOD;
}

void gameLoop() {
  gameUpdate();
}

void addSnakeSegment(byte row, byte col) {
  ++lastSegment;
  moveSnakeSegments(row, col, false);
}

void moveSnakeSegments(byte row, byte col, byte turnOffLastSegment) {
  if (turnOffLastSegment) {
    matrixState[snakeSegments[SEGMENT_ROW][lastSegment]][snakeSegments[SEGMENT_COL][lastSegment]] = EMPTY;
  }
  for (int segment = lastSegment; segment > 0; --segment) {
    snakeSegments[SEGMENT_ROW][segment] = snakeSegments[SEGMENT_ROW][segment - 1];
    snakeSegments[SEGMENT_COL][segment] = snakeSegments[SEGMENT_COL][segment - 1];
  }
  snakeSegments[SEGMENT_ROW][SEGMENT_HEAD] = row;
  snakeSegments[SEGMENT_COL][SEGMENT_HEAD] = col;
  matrixState[snakeSegments[SEGMENT_ROW][SEGMENT_HEAD]][snakeSegments[SEGMENT_COL][SEGMENT_HEAD]] = SNAKE;
}

void eatFood() {
  if (difficulty == EASY) {
    score += 1;
  }
  else if (difficulty == NORMAL){
    score += 5;
  }
  else if (difficulty == HARD) {
    score += 9;
  }
  matrixState[foodRow][foodCol] = EMPTY;
  addSnakeSegment(foodRow, foodCol);
  generateRandomFood();
  matrixState[foodRow][foodCol] = FOOD;
}

void generateRandomFood() {
  do {
    foodRow = random(matrixSize);
    foodCol = random(matrixSize);
  } while (matrixState[foodRow][foodCol] != EMPTY);
}

byte wrapAround(byte value, byte minValue, byte maxValue) {
  // byte edge case: 0 - 1 = 255
  byte edgeCase = 0 - 1;
  if (value < minValue || value == edgeCase) {
    return maxValue;
  }
  else if (value > maxValue) {
    return minValue;
  }
  else {
    return value;
  }
}

void moveLeft() {
  byte snakeRow = snakeSegments[SEGMENT_ROW][SEGMENT_HEAD];
  byte snakeCol = snakeSegments[SEGMENT_COL][SEGMENT_HEAD];
  snakeCol = wrapAround(snakeCol - 1, 0, matrixSize - 1);
  if (matrixState[snakeRow][snakeCol] == FOOD) {
    eatFood();
  }
  else if (matrixState[snakeRow][snakeCol] == EMPTY || (snakeRow == snakeSegments[SEGMENT_ROW][lastSegment] && snakeCol == snakeSegments[SEGMENT_COL][lastSegment])) {
    moveSnakeSegments(snakeRow, snakeCol, true);
  }
  else {
    // printColiziune(snakeRow, snakeCol);
    gameOver();
  }
}

void moveRight() {
  byte snakeRow = snakeSegments[SEGMENT_ROW][SEGMENT_HEAD];
  byte snakeCol = snakeSegments[SEGMENT_COL][SEGMENT_HEAD];
  snakeCol = wrapAround(snakeCol + 1, 0, matrixSize - 1);
  if (matrixState[snakeRow][snakeCol] == FOOD) {
    eatFood();
  }
  else if (matrixState[snakeRow][snakeCol] == EMPTY || (snakeRow == snakeSegments[SEGMENT_ROW][lastSegment] && snakeCol == snakeSegments[SEGMENT_COL][lastSegment])) {
    moveSnakeSegments(snakeRow, snakeCol, true);
  }
  else {
    // printColiziune(snakeRow, snakeCol);
    gameOver();
  }
}

void moveUp() {
  byte snakeRow = snakeSegments[SEGMENT_ROW][SEGMENT_HEAD];
  byte snakeCol = snakeSegments[SEGMENT_COL][SEGMENT_HEAD];
  snakeRow = wrapAround(snakeRow - 1, 0, matrixSize - 1);
  if (matrixState[snakeRow][snakeCol] == FOOD) {
    eatFood();
  }
  else if (matrixState[snakeRow][snakeCol] == EMPTY || (snakeRow == snakeSegments[SEGMENT_ROW][lastSegment] && snakeCol == snakeSegments[SEGMENT_COL][lastSegment])) {
    moveSnakeSegments(snakeRow, snakeCol, true);
  }
  else {
    // printColiziune(snakeRow, snakeCol);
    gameOver();
  }
}

void moveDown() {
  byte snakeRow = snakeSegments[SEGMENT_ROW][SEGMENT_HEAD];
  byte snakeCol = snakeSegments[SEGMENT_COL][SEGMENT_HEAD];
  snakeRow = wrapAround(snakeRow + 1, 0, matrixSize - 1);
  if (matrixState[snakeRow][snakeCol] == FOOD) {
    eatFood();
  }
  else if (matrixState[snakeRow][snakeCol] == EMPTY || (snakeRow == snakeSegments[SEGMENT_ROW][lastSegment] && snakeCol == snakeSegments[SEGMENT_COL][lastSegment])) {
    moveSnakeSegments(snakeRow, snakeCol, true);
  }
  else {
    // printColiziune(snakeRow, snakeCol);
    gameOver();
  }
}

void gameOver() {
  UserScore user;
  user.score = score;
  user.setName(currentName);
  addUser(user);
  gameOverMenu();
}

void newGame() {
  lcd.clear();
  setDefaultDelay();
  joyStickGameSetup();
  gameSetup();
  // state = GAME;
}

void gameUpdate() {
  for (int row = 0; row < matrixSize; ++row) {
    for (int col = 0; col < matrixSize; ++col) {
      if (matrixState[col][row] == FOOD){
        lc.setLed(0, row, col, ledBlink(foodBlinkOn, foodBlinkOff, lastFoodBlink));
      }
      else if (col == snakeSegments[SEGMENT_ROW][SEGMENT_HEAD] && row == snakeSegments[SEGMENT_COL][SEGMENT_HEAD]) {
        lc.setLed(0, row, col, ledBlink(headBlinkOn, headBlinkOff, lastHeadBlink));
      }
      else {
        lc.setLed(0, row, col, matrixState[col][row]);
      }
    }
  }
}

byte ledBlink(unsigned int delayOn, unsigned int delayOff, unsigned long &lastLedBlink) {
  unsigned long timeDif = millis() - lastLedBlink;
  if (timeDif <= delayOn) {
    return HIGH;
  }
  else if (timeDif > delayOn && timeDif <= delayOn + delayOff) {
    return LOW;
  }
  else {
    lastLedBlink = millis();
  }
  return LOW;
}

void lcdGame() {
  lcd.setCursor(0, 0);
  lcd.print("Score: ");
  lcd.print(score);
  lcd.setCursor(0, 1);
  lcd.print("Game speed: ");
  lcd.print(getSpeed());
}

byte getScore() {
  return score;
}