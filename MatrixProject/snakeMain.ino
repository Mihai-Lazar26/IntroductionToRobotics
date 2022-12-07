#include <LiquidCrystal.h>
#include "LedControl.h" //  need the library

#define MENU 0
#define GAME 1

#define PLAY_IMAGE 0
#define COG_IMAGE 1
#define TROPHY_IMAGE 2
#define X_IMAGE 3
#define SNAKE_IMAGE 4

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
// const byte matrixSize = 8;
// pin 12 is connected to the MAX7219 pin 1
// pin 11 is connected to the CLK pin 13
// pin 10 is connected to LOAD pin 12
// 1 as we are only using 1 MAX7219
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1); //DIN, CLK, LOAD, No. DRIVER
byte matrixBrightness = 2;

const byte rs = 9;
const byte en = 8;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 2;
const byte d7 = 4;

const byte lcdBrightnessPin = 5;
const byte lcdContrastPin = 3; 
 
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

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

byte lcdBrightness = 240, lcdContrast = 60;

#define ARROW_RIGHT_CHAR 0
#define ARROW_UP_CHAR 1
#define EMPTY_BOX_CHAR 2
#define FULL_BOX_CHAR 3
 
void lcdSetup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.createChar(ARROW_RIGHT_CHAR, arrowRight);
  lcd.createChar(ARROW_UP_CHAR, arrowUp);
  lcd.createChar(EMPTY_BOX_CHAR, emptyBox);
  lcd.createChar(FULL_BOX_CHAR, fullBox);

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

void setup() {
  randomSeed(analogRead(A2));
  matrixSetup();
  lcdSetup();
  joystickSetup();
  Serial.begin(9600);
 
}
 
void loop() {
  if (state == GAME) {
    runGame();
  }
  if (state == MENU) {
    runMenu();
  }
  Serial.println("(contrast, brightness): " + String(lcdContrast) + " " + String(lcdBrightness));
  
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