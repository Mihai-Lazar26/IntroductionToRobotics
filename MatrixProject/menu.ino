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

#define GAME_OVER_1 13
#define GAME_OVER_2 14

const int textScrollDelay = 300;

const byte difficultyMin = 0, difficultyMax = 2;

const byte settingsOffset = 7;
const byte optionsOffset = 2;

const byte lcdContrastStep = 20, lcdBrightnessStep = 20;
const byte lcdContrastOffset = 2, lcdBrightnessOffset = 2;

const byte lcdContrastMin = 0, lcdContrastMax = 240;
const byte lcdBrightnessMin = 0, lcdBrightnessMax = 240;

const byte lcdContrastBoxes = 12, lcdBrightnessBoxes = 12;

const byte matrixBrightnessStep = 1;
const byte matrixBrightnessBoxes = 15;
const byte matrixBrightnessOffset = 0;

const byte matrixBrightnessMin = 0, matrixBrightnessMax = 15;

const int systemStartTimer = 5*1000;
const byte maxTextLength = 16;
const byte nrOfOptions = 5;
const byte nrOfSettings = 6;

const byte nrOfAboutText = 3;
const byte maxTextLengthBig = 60;

const byte nrOfHowToPlayText = 10;

const byte minCurrentCharacter = 0, maxCurrentCharacter = 2;

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

char aboutText[nrOfAboutText][maxTextLengthBig] = {
  "Game snake",
  "Project realized by Lazar Mihai",
  "https://github.com/Mihai-Lazar26/IntroductionToRobotics"
};

char howToPlayText[nrOfHowToPlayText][maxTextLength] {
  "Use joystick to",
  "move the snake",
  "and eat food to",
  "increase your",
  "score and size.",
  "The game ends",
  "when the snake",
  "collides with",
  "itself or",
  "a wall."
};

byte menuState = SYSTEM_START, menuOption = START_GAME;
byte runMainMenu = false;

byte currentCharacter = 0;
byte editName = false;

byte currentTextToDisplay = 0;
byte textLeft1 = 0, textLeft2 = 0;
unsigned long lastTextScrollUpdate1 = 0, lastTextScrollUpdate2 = 0;


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
    displayImage(images[SETTINGS_IMAGE]);
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
  else if (menuState == MATRIX_SETTINGS) {
    lcd.setCursor(0, 0);
    lcd.print("Brightness:");
    byte nr = matrixBrightness / matrixBrightnessStep + matrixBrightnessOffset;
    for (int col = matrixBrightnessOffset; col < matrixBrightnessBoxes + matrixBrightnessOffset; ++col) {
      lcd.setCursor(col, 1);
      if (col < nr) {
        lcd.write((byte) FULL_BOX_CHAR);
      }
      else {
        lcd.write((byte) EMPTY_BOX_CHAR);
      }
    }
  }
  else if (menuState == ABOUT) {
    if (currentTextToDisplay < nrOfAboutText - 1) {
      lcd.setCursor(0, 0);
      lcd.write((byte) ARROW_RIGHT_CHAR);
      lcd.setCursor(1, 0);
      scroll(15, aboutText[currentTextToDisplay], textLeft1, lastTextScrollUpdate1);
      lcd.setCursor(0, 1);
      lcd.write(' ');
      lcd.setCursor(1, 1);
      char noOverflow[16];
      strncpy(noOverflow, aboutText[currentTextToDisplay + 1], 15);
      lcd.print(noOverflow);
    }
    else if (currentTextToDisplay == nrOfAboutText - 1){
      lcd.setCursor(0, 0);
      lcd.write(' ');
      lcd.setCursor(1, 0);
      char noOverflow[16];
      strncpy(noOverflow, aboutText[currentTextToDisplay - 1], 15);
      lcd.print(noOverflow);
      lcd.setCursor(0, 1);
      lcd.write((byte) ARROW_RIGHT_CHAR);
      lcd.setCursor(1, 1);
      scroll(15, aboutText[currentTextToDisplay], textLeft1, lastTextScrollUpdate1);
    }
  }
  else if (menuState == HOW_TO_PLAY) {
    if (currentTextToDisplay < nrOfHowToPlayText - 1) {
      lcd.setCursor(0, 0);
      lcd.write((byte) ARROW_RIGHT_CHAR);
      lcd.setCursor(1, 0);
      lcd.print(howToPlayText[currentTextToDisplay]);
      lcd.setCursor(1, 1);
      lcd.print(howToPlayText[currentTextToDisplay + 1]);
    }
    else {
      lcd.setCursor(1, 0);
      lcd.print(howToPlayText[currentTextToDisplay - 1]);
      lcd.setCursor(0, 1);
      lcd.write((byte) ARROW_RIGHT_CHAR);
      lcd.setCursor(1, 1);
      lcd.print(howToPlayText[currentTextToDisplay]);
    }
  }
  else if (menuState == DIFFICULTY) {
    lcd.setCursor(0, 0);
    lcd.print("Difficulty:");
    lcd.setCursor(0, 1);
    if (difficulty == EASY) {
      lcd.print("Easy");     
    }
    else if (difficulty == NORMAL) {
      lcd.print("Normal");
    }
    else if (difficulty == HARD) {
      lcd.print("Hard");
    }
  }
  else if (menuState == ENTER_NAME) {
    lcd.setCursor(0, 0);
    lcd.print("Name: ");
    lcd.print(currentName);
    lcd.setCursor(currentCharacter + strlen("Name: "), 1);
    if (editName) {
      lcd.write((byte) ARROW_UP_CHAR);
    }
  }
  else if (menuState == SOUNDS) {
    lcd.setCursor(0, 0);
    lcd.print("Sounds: ");
    if (soundsOn) {
      lcd.print("On");
    }
    else {
      lcd.print("Off");
    }
  }
  else if (menuState == HIGHSCORES) {
    if (savedUsers > 0) {
      if (currentTextToDisplay == 0) {
        lcd.setCursor(0, 0);
        lcd.print("User: ");
        lcd.print(currentName);

        lcd.setCursor(0, 1);
        lcd.print(currentTextToDisplay + 1);
        lcd.print(". ");
        lcd.print(highscores[currentTextToDisplay].name);
        lcd.print(": ");
        lcd.print(highscores[currentTextToDisplay].score);
      }
      else if (currentTextToDisplay < savedUsers){
        lcd.setCursor(0, 0);
        lcd.print(currentTextToDisplay);
        lcd.print(". ");
        lcd.print(highscores[currentTextToDisplay - 1].name);
        lcd.print(": ");
        lcd.print(highscores[currentTextToDisplay - 1].score);

        lcd.setCursor(0, 1);
        lcd.print(currentTextToDisplay + 1);
        lcd.print(". ");
        lcd.print(highscores[currentTextToDisplay].name);
        lcd.print(": ");
        lcd.print(highscores[currentTextToDisplay].score);
      }
    }
    else if (savedUsers == 0) {
      lcd.setCursor(0, 0);
      lcd.print("User: ");
      lcd.print(currentName);
      lcd.setCursor(0, 1);
      lcd.print("No scores saved!");
    }
    displayImage(images[TROPHY_IMAGE]);
  }
  else if (menuState == GAME_OVER_1) {
    lcd.setCursor(0, 0); 
    char score[4];
    itoa(getScore(), score, 10);
    char text[maxTextLengthBig] = "Score reached: ";
    strcat(text, score);
    strcat(text, " Game speed: ");
    itoa(getSpeed(), score, 10);
    strcat(text, score);
    strcat(text, " Difficulty: ");
    if (difficulty == EASY) {
      strcat(text, "Easy");
    }
    else if (difficulty == NORMAL) {
      strcat(text, "Normal");
    }
    else if (difficulty == HARD) {
      strcat(text, "Hard");
    }
    scroll(16, text, textLeft1, lastTextScrollUpdate1);
    lcd.setCursor(0, 1);
    scroll(16, "Press button to continue", textLeft2, lastTextScrollUpdate2);
    displayImage(images[X_IMAGE]);
  }
  else if (menuState == GAME_OVER_2) {
    lcd.setCursor(0, 0);
    byte rank = getUserRank(currentName);
    char text[maxTextLengthBig] = "Best score: ";
    char score[4];
    itoa(highscores[rank - 1].score, score, 10);
    strcat(text, score);
    strcat(text, " Rank: ");
    if (rank > 0) {
      itoa(rank, score, 10);
      strcat(text, score);
    }
    else {
      strcat(text, "unranked");
    }
    scroll(16, text, textLeft1, lastTextScrollUpdate1);
    lcd.setCursor(0, 1);
    scroll(16, "Press button to continue", textLeft2, lastTextScrollUpdate2);
    displayImage(images[X_IMAGE]);
  }
}

void scroll(const byte textLength, const char text[], byte &textLeft, unsigned long &lastTextScrollUpdate) {
  if (strlen(text) <= textLength) {
    lcd.print(text);
  }
  else if (millis() - lastTextScrollUpdate > textScrollDelay) {
    char noOverflow[textLength + 1];
    strncpy(noOverflow, text + textLeft, textLength);
    if (strlen(noOverflow) < textLength) {
      char aux[textLength + 1] = " ";
      strncpy(aux + 1, text, textLength - strlen(noOverflow) - 1);
      strcat(noOverflow, aux);
    }
    lcd.print(noOverflow);
    lastTextScrollUpdate = millis();
    ++textLeft;
    if (strlen(text) < textLeft) {
      textLeft = 0;
    }
  }
}

byte clamp(byte value, byte minValue, byte maxValue) {
  // byte edge case: 0 - 1 = 255 for step = 1
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
  else if (menuState == MATRIX_SETTINGS) {
    matrixBrightness = clamp(matrixBrightness - matrixBrightnessStep, matrixBrightnessMin, matrixBrightnessMax);
    lc.setIntensity(0, matrixBrightness);
  }
  else if (menuState == ABOUT) {
    currentTextToDisplay = clamp(currentTextToDisplay + 1, 0, nrOfAboutText - 1);
    textLeft1 = 0;
    lastTextScrollUpdate1 = 0;
  }
  else if (menuState == HOW_TO_PLAY) {
    currentTextToDisplay = clamp(currentTextToDisplay + 1, 0, nrOfHowToPlayText - 1);
  }
  else if (menuState == DIFFICULTY) {
    difficulty = clamp(difficulty + 1, difficultyMin, difficultyMax);
  }
  else if (menuState == ENTER_NAME) {
    if (editName) {
      if (currentName[currentCharacter] + 1 >= 'A' && currentName[currentCharacter] + 1 <= 'Z'){
        currentName[currentCharacter] += 1;
      }
    }
  }
  else if (menuState == SOUNDS) {
    soundsOn = !soundsOn;
  }
  else if (menuState == HIGHSCORES) {
    currentTextToDisplay = clamp(currentTextToDisplay + 1, 0, savedUsers - 1);
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
  else if (menuState == MATRIX_SETTINGS) {
    matrixBrightness = clamp(matrixBrightness + matrixBrightnessStep, matrixBrightnessMin, matrixBrightnessMax);
    lc.setIntensity(0, matrixBrightness);
  }
  else if (menuState == ABOUT) {
    currentTextToDisplay = clamp(currentTextToDisplay - 1, 0, nrOfAboutText - 1);
    textLeft1 = 0;
    lastTextScrollUpdate1 = 0;
  }
  else if (menuState == HOW_TO_PLAY) {
    currentTextToDisplay = clamp(currentTextToDisplay - 1, 0, nrOfHowToPlayText - 1);
  }
  else if (menuState == DIFFICULTY) {
    difficulty = clamp(difficulty - 1, difficultyMin, difficultyMax);
  }
  else if (menuState == ENTER_NAME) {
    if (editName) {
      if (currentName[currentCharacter] - 1 >= 'A' && currentName[currentCharacter] - 1 <= 'Z'){
        currentName[currentCharacter] -= 1;
      }
    }
  }
  else if (menuState == SOUNDS) {
    soundsOn = !soundsOn;
  }
  else if (menuState == HIGHSCORES) {
    currentTextToDisplay = clamp(currentTextToDisplay - 1, 0, savedUsers - 1);
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
    else if (menuOption == ABOUT) {
      menuState = ABOUT;
      currentTextToDisplay = 0;
      textLeft1 = 0;
      lastTextScrollUpdate1 = 0;
    }
    else if (menuOption == HOW_TO_PLAY) {
      menuState = HOW_TO_PLAY;
      currentTextToDisplay = 0;
    }
    else if (menuOption == HIGHSCORES) {
      menuState = HIGHSCORES;
      currentTextToDisplay = 0;
    }
  }
  else if (menuState == SETTINGS) {
    if (menuOption == LCD_CONTRAST) {
      menuState = LCD_CONTRAST;
    }
    else if (menuOption == LCD_BRIGHTNESS) {
      menuState = LCD_BRIGHTNESS;
    }
    else if (menuOption == MATRIX_SETTINGS) {
      menuState = MATRIX_SETTINGS;
    }
    else if (menuOption == DIFFICULTY) {
      menuState = DIFFICULTY;
    }
    else if (menuOption == ENTER_NAME) {
      menuState = ENTER_NAME;
    }
    else if (menuOption == SOUNDS) {
      menuState = SOUNDS;
    }
  }
  else if (menuState == ENTER_NAME) {
    if (editName) {
      currentCharacter = clamp(currentCharacter + 1, minCurrentCharacter, maxCurrentCharacter);
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
    updateSettingsEEPROM();
  }
  else if (menuState == LCD_CONTRAST) {
    menuState = SETTINGS;
    menuOption = LCD_CONTRAST;
  }
  else if (menuState == LCD_BRIGHTNESS) {
    menuState = SETTINGS;
    menuOption = LCD_BRIGHTNESS;
  }
  else if (menuState == MATRIX_SETTINGS) {
    menuState = SETTINGS;
    menuOption = MATRIX_SETTINGS;
  }
  else if (menuState == ABOUT) {
    menuState = OPTIONS;
    menuOption = ABOUT;
  }
  else if (menuState == HOW_TO_PLAY) {
    menuState = OPTIONS;
    menuOption = HOW_TO_PLAY;
  }
  else if (menuState == DIFFICULTY) {
    menuState = SETTINGS;
    menuOption = DIFFICULTY;
  }
  else if (menuState == ENTER_NAME) {
    if (editName) {
      currentCharacter = clamp(currentCharacter - 1, minCurrentCharacter, maxCurrentCharacter);
    }
    else {
      menuState = SETTINGS;
      menuOption = ENTER_NAME;
    }
  }
  else if (menuState == SOUNDS) {
    menuState = SETTINGS;
    menuOption = SOUNDS;
  }
  else if (menuState == HIGHSCORES) {
    menuState = OPTIONS;
    menuOption = HIGHSCORES;
  }
}

void buttonPress() {
  if (!runMainMenu){
    return;
  }
  lcd.clear();
  if (menuState == ENTER_NAME) {
    editName = !editName;   
  }
  else if (menuState == GAME_OVER_1) {
    menuState = GAME_OVER_2;
    textLeft1 = 0;
    textLeft2 = 0;
    lastTextScrollUpdate1 = 0;
    lastTextScrollUpdate2 = 0; 
  }
  else if (menuState == GAME_OVER_2) {
    menuState = OPTIONS;
  }
}

void gameOverMenu() {
  lcd.clear();
  state = MENU;
  menuState = GAME_OVER_1;
  menuOption = START_GAME;
  textLeft1 = 0;
  textLeft2 = 0;
  lastTextScrollUpdate1 = 0;
  lastTextScrollUpdate2 = 0;
}