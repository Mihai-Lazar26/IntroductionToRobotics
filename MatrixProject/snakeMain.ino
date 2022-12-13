#include <EEPROM.h>
#include <LiquidCrystal.h>
#include "LedControl.h"

#define MENU 0
#define GAME 1

#define PLAY_IMAGE 0
#define SETTINGS_IMAGE 1
#define TROPHY_IMAGE 2
#define X_IMAGE 3
#define SNAKE_IMAGE 4

#define EASY 0
#define NORMAL 1
#define HARD 2

#define ARROW_RIGHT_CHAR 0
#define ARROW_UP_CHAR 1
#define EMPTY_BOX_CHAR 2
#define FULL_BOX_CHAR 3

const byte offsetEEPROM = 0;

const byte matrixBrightnessAddress = 0 + offsetEEPROM;
const byte lcdBrightnessAddress = 1 + offsetEEPROM;
const byte lcdContrastAddress = 2 + offsetEEPROM;
const byte soundsOnAddress = 3 + offsetEEPROM;
const byte difficultyAddress = 4 + offsetEEPROM;
const byte currentNameAddress = 5 + offsetEEPROM;
const byte savedUsersAddress = 9 + offsetEEPROM;
const byte highscoresAddress = 10 + offsetEEPROM;

const byte buzzerPin = 9;

const uint64_t images[] = {
  0x0008183838180800, //play
  0x00243c18183c2400, //cog
  0x003c18183c7e7e00, //trophy
  0xc3e77e3c3c7ee7c3, //X
  0x1d3f36381c4c3c18  //snake
};

const byte dinPin = 12;
const byte clockPin = 11;
const byte loadPin = 10;
const byte matrixSize = 8;

const byte rs = 1;
const byte en = 8;
const byte d4 = 7;
const byte d5 = 3;
const byte d6 = 2;
const byte d7 = 4;

const byte lcdBrightnessPin = 5;
const byte lcdContrastPin = 6; 
 
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);

byte state = MENU;

byte arrowRight[] = {
  B00000,
  B01000,
  B01100,
  B01110,
  B01100,
  B01000,
  B00000,
  B00000
};

byte arrowUp[] = {
  B00000,
  B00000,
  B00100,
  B01110,
  B11111,
  B00000,
  B00000,
  B00000
};

byte emptyBox[] = {
  B11111,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
  B11111
};

byte fullBox[] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};


byte matrixBrightness;
byte lcdBrightness, lcdContrast;
byte soundsOn;
char currentName[4] = "AAA";
byte difficulty;
byte savedUsers;

// 5 byte of memory;
struct UserScore {
public:
  char name[4];
  byte score;

  void setName(const char name[4]) {
    strcpy(this->name, name);
  }
};

const byte maxNoHighscores = 5;
UserScore highscores[maxNoHighscores];

 
void lcdSetup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.createChar(ARROW_RIGHT_CHAR, arrowRight);
  lcd.createChar(ARROW_UP_CHAR, arrowUp);
  lcd.createChar(EMPTY_BOX_CHAR, emptyBox);
  lcd.createChar(FULL_BOX_CHAR, fullBox);

  pinMode(rs, OUTPUT);
  pinMode(en, OUTPUT);
  pinMode(d4, OUTPUT);
  pinMode(d5, OUTPUT);
  pinMode(d6, OUTPUT);
  pinMode(d7, OUTPUT);

  pinMode(lcdBrightnessPin, OUTPUT);
  pinMode(lcdContrastPin, OUTPUT);

  analogWrite(lcdBrightnessPin, lcdBrightness);
  analogWrite(lcdContrastPin, lcdContrast);
}

void matrixSetup() {
  // the zero refers to the MAX7219 number, it is zero for 1 chip
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.setIntensity(0, matrixBrightness); // sets brightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen
}

void displayImage(uint64_t image) {
  for (int i = 0; i < 8; i++) {
    byte row = (image >> i * 8) & 0xFF;
    for (int j = 0; j < 8; j++) {
      lc.setLed(0, j, i, bitRead(row, j));
    }
  }
}

void updateSettingsEEPROM() {
  EEPROM.update(matrixBrightnessAddress, matrixBrightness);
  EEPROM.update(lcdBrightnessAddress, lcdBrightness);
  EEPROM.update(lcdContrastAddress, lcdContrast);
  EEPROM.update(soundsOnAddress, soundsOn);
  EEPROM.put(currentNameAddress, currentName);
  EEPROM.update(difficultyAddress, difficulty);
}

void updateHighscoreEEPROM() {
  EEPROM.update(savedUsersAddress, savedUsers);
  for (int i = 0; i < savedUsers; ++i) {
    EEPROM.put(highscoresAddress + sizeof(UserScore) * i, highscores[i]);
  }
}

byte getUserRank(const char userName[]) {
  for (int i = 0; i < savedUsers; ++i) {
    if (strcmp(highscores[i].name, userName) == 0) {
      return i + 1;
    }    
  }
  return 0;
}

void addUser(const UserScore &val) {
  byte newUserPos = 0;
  byte userRank = getUserRank(val.name);
  if (userRank) {
    if (highscores[userRank - 1].score < val.score) {
      highscores[userRank - 1].score = val.score;
      newUserPos = userRank - 1;
    }
  }
  else {
    if (savedUsers < maxNoHighscores) {
      ++savedUsers;
      highscores[savedUsers - 1].setName(val.name);
      highscores[savedUsers - 1].score = val.score;
      newUserPos = savedUsers - 1;
    }
    else {
      byte min = highscores[0].score;
      byte minPos = 0;

      for (int i = 1; i < savedUsers; ++i) {
        if (highscores[i].score < min) {
          min = highscores[i].score;
          minPos = i;
        }
      }

      if (val.score > min) {
        highscores[minPos].score = val.score;
        highscores[minPos].setName(val.name);
        newUserPos = minPos;
      }
    }
  }

  while(newUserPos > 0 && highscores[newUserPos].score > highscores[newUserPos - 1].score) {
    UserScore aux;
    aux.score = highscores[newUserPos].score;
    aux.setName(highscores[newUserPos].name);

    highscores[newUserPos].score = highscores[newUserPos - 1].score;
    highscores[newUserPos].setName(highscores[newUserPos - 1].name);

    highscores[newUserPos - 1].score = aux.score;
    highscores[newUserPos - 1].setName(aux.name);
    --newUserPos;
  }

  updateHighscoreEEPROM();
}

void setup() {
  
  matrixBrightness = EEPROM.read(matrixBrightnessAddress);
  lcdBrightness = EEPROM.read(lcdBrightnessAddress);
  lcdContrast = EEPROM.read(lcdContrastAddress);
  soundsOn = EEPROM.read(soundsOnAddress);
  EEPROM.get(currentNameAddress, currentName);
  savedUsers = EEPROM.read(savedUsersAddress);
  difficulty = EEPROM.read(difficultyAddress);

  for (int i = 0; i < savedUsers; ++i) {
    EEPROM.get(highscoresAddress + sizeof(UserScore) * i, highscores[i]);
  }

  pinMode(buzzerPin, OUTPUT);
  randomSeed(analogRead(A2));
  matrixSetup();
  lcdSetup();
  joystickSetup();
}
 
void loop() {
  if (state == GAME) {
    runGame();
  }
  if (state == MENU) {
    runMenu();
  }
}

void runMenu() {
  joystickMenuLoop();
  menu();
}

void runGame() {
  joystickGameLoop();
  gameLoop();
  lcdGame();
}