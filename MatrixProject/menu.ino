#define SYSTEM_START 0
#define OPTIONS 1

// options
#define START_GAME 2
#define HIGHSCORES 3
#define SETTINGS 4
#define ABOUT 5
#define HOW_TO_PLAY 6

// settings
#define ENTER_NAME 7
#define DIFFICULTY 8
#define LCD_CONTRAST 9
#define LCD_BRIGHTNESS 10
#define MATRIX_SETTINGS 11
#define SOUNDS 12

const byte settingsOffset = 7;
const byte optionsOffset = 2;

const byte lcdContrastStep = 20, lcdBrightnessStep = 20;
const byte lcdContrastOffset = 2, lcdBrightnessOffset = 2;

const byte lcdContrastMin = 0, lcdContrastMax = 240;
const byte lcdBrightnessMin = 0, lcdBrightnessMax = 240;

const byte lcdContrastBoxes = 12, lcdBrightnessBoxes = 12;

const int systemStartTimer = 5*1000;
const byte maxTextLength = 16;
const byte nrOfOptions = 5;
const byte nrOfSettings = 6;

char options[nrOfOptions][maxTextLength] = {
  "Start game",
  "Highscores",
  "Settings",
  "About",
  "How to play"
};

char settings[nrOfSettings][maxTextLength] = {
  "Enter name",
  "Difficulty",
  "LCD contrast",
  "LCD brightness",
  "Matrix settings",
  "Sounds"
};

byte menuState = SYSTEM_START, menuOption = START_GAME;
byte runMainMenu = false;

void menu() {
  if (state != MENU) {
    return;
  }
  if (menuState == SYSTEM_START) {
    runMainMenu = false;
    systemStart();
    if (millis() > systemStartTimer) {
      menuState = OPTIONS;
      lcd.clear();
    }
  }
  if (menuState != SYSTEM_START) {
    runMainMenu = true;
    mainMenu();
  }
  Serial.println("(state, option): " + String(menuState) + " " + String(menuOption));
}

void systemStart() {
  lcd.setCursor(0, 0);
  lcd.print("Welcome! ");
  lcd.print((systemStartTimer - millis()) / 1000 + 1);
  displayImage(images[PLAY_IMAGE]);
}

void mainMenu() {
  if (menuState == OPTIONS) {
    byte option = menuOption - optionsOffset;
    if (option < nrOfOptions - 1) {
      lcd.setCursor(0, 0);
      lcd.write((byte) ARROW_RIGHT_CHAR);
      lcd.setCursor(1, 0);
      lcd.print(options[option]);
      lcd.setCursor(1, 1);
      lcd.print(options[option + 1]);
    }
    else {
      lcd.setCursor(1, 0);
      lcd.print(options[option - 1]);
      lcd.setCursor(0, 1);
      lcd.write((byte) ARROW_RIGHT_CHAR);
      lcd.setCursor(1, 1);
      lcd.print(options[option]);
    }
    displayImage(images[SNAKE_IMAGE]);
  }
  else if (menuState == SETTINGS) {
    byte option = menuOption - settingsOffset;
    if (option < nrOfSettings - 1) {
      lcd.setCursor(0, 0);
      lcd.write((byte) ARROW_RIGHT_CHAR);
      lcd.setCursor(1, 0);
      lcd.print(settings[option]);
      lcd.setCursor(1, 1);
      lcd.print(settings[option + 1]);
    }
    else {
      lcd.setCursor(1, 0);
      lcd.print(settings[option - 1]);
      lcd.setCursor(0, 1);
      lcd.write((byte) ARROW_RIGHT_CHAR);
      lcd.setCursor(1, 1);
      lcd.print(settings[option]);
    }
    displayImage(images[COG_IMAGE]);
  }
  else if (menuState == LCD_CONTRAST) {
    lcd.setCursor(0, 0);
    lcd.print("LCD contrast:");
    byte nr = lcdContrast / lcdContrastStep + lcdContrastOffset;
    for (int col = lcdContrastOffset; col < lcdContrastBoxes + lcdContrastOffset; ++col) {
      lcd.setCursor(col, 1);
      if (col < nr) {
        lcd.write((byte) FULL_BOX_CHAR);
      }
      else {
        lcd.write((byte) EMPTY_BOX_CHAR);
      }
    }
  }
  else if (menuState == LCD_BRIGHTNESS) {
    lcd.setCursor(0, 0);
    lcd.print("LCD brightness:");
    byte nr = lcdBrightness / lcdBrightnessStep + lcdBrightnessOffset;
    for (int col = lcdBrightnessOffset; col < lcdBrightnessBoxes + lcdBrightnessOffset; ++col) {
      lcd.setCursor(col, 1);
      if (col < nr) {
        lcd.write((byte) FULL_BOX_CHAR);
      }
      else {
        lcd.write((byte) EMPTY_BOX_CHAR);
      }
    }
  }

}

byte clamp(byte value, byte minValue, byte maxValue) {
  // byte edge case: 0 - 1 = 255
  byte edgeCase = 0 - 1;
  if (value < minValue || value == edgeCase) {
    return minValue;
  }
  else if (value > maxValue) {
    return maxValue;
  }
  else {
    return value;
  }
}
void moveDownMenu() {
  if (!runMainMenu){
    return;
  }
  lcd.clear();
  if (menuState == OPTIONS) {
    menuOption = clamp(menuOption + 1, optionsOffset, optionsOffset + nrOfOptions - 1);
  }
  else if (menuState == SETTINGS) {
    menuOption = clamp(menuOption + 1, settingsOffset, settingsOffset + nrOfSettings - 1);
  }
  else if (menuState == LCD_CONTRAST) {
    byte checkOverflow = lcdContrast - lcdContrastStep;
    if (checkOverflow < lcdContrast)
    lcdContrast = clamp(lcdContrast - lcdContrastStep, lcdContrastMin, lcdContrastMax);
    analogWrite(lcdContrastPin, lcdContrast);
  }
  else if (menuState == LCD_BRIGHTNESS) {
    byte checkOverflow = lcdBrightness - lcdBrightnessStep;
    if (checkOverflow < lcdBrightness)
    lcdBrightness = clamp(lcdBrightness - lcdBrightnessStep, lcdBrightnessMin, lcdBrightnessMax);
    analogWrite(lcdBrightnessPin, lcdBrightness);
  }
}

void moveUpMenu() {
  if (!runMainMenu){
    return;
  }
  lcd.clear();
  if (menuState == OPTIONS) {
    menuOption = clamp(menuOption - 1, optionsOffset, optionsOffset + nrOfOptions - 1);
  }
  else if (menuState == SETTINGS) {
    menuOption = clamp(menuOption - 1, settingsOffset, settingsOffset + nrOfSettings - 1);
  }
  else if (menuState == LCD_CONTRAST) {
    byte checkOverflow = lcdContrast + lcdContrastStep;
    if (checkOverflow > lcdContrast)
    lcdContrast = clamp(lcdContrast + lcdContrastStep, lcdContrastMin, lcdContrastMax);
    analogWrite(lcdContrastPin, lcdContrast);
  }
  else if (menuState == LCD_BRIGHTNESS) {
    byte checkOverflow = lcdBrightness + lcdBrightnessStep;
    if (checkOverflow > lcdBrightness)
    lcdBrightness = clamp(lcdBrightness + lcdBrightnessStep, lcdBrightnessMin, lcdBrightnessMax);
    analogWrite(lcdBrightnessPin, lcdBrightness);
  }
}

void moveRightMenu() {
  if (!runMainMenu){
    return;
  }
  lcd.clear();
  if (menuState == OPTIONS) {
    if (menuOption == START_GAME) {
      runMainMenu = false;
      newGame();
      state = GAME;
    }
    else if (menuOption == SETTINGS) {
      menuState = SETTINGS;
      menuOption = ENTER_NAME;
    }
  }
  else if (menuState == SETTINGS) {
    if (menuOption == LCD_CONTRAST) {
      menuState = LCD_CONTRAST;
      Serial.println("CONT");
    }
    else if (menuOption == LCD_BRIGHTNESS) {
      menuState = LCD_BRIGHTNESS;
      Serial.println("BRIGHT");
    }
  }
}

void moveLeftMenu() {
  if (!runMainMenu){
    return;
  }
  lcd.clear();
  if (menuState == SETTINGS) {
    menuState = OPTIONS;
    menuOption = SETTINGS;
  }
  else if (menuState == LCD_CONTRAST) {
    menuState = SETTINGS;
    menuOption = LCD_CONTRAST;
  }
  else if (menuState == LCD_BRIGHTNESS) {
    menuState = SETTINGS;
    menuOption = LCD_BRIGHTNESS;
  }
}