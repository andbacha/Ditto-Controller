#include <LiquidCrystal.h>

/* CONSTANTS ============================================================== */

#define BTN_LEFT 8
#define BTN_RIGHT 9
#define BTN_ACTION 10
#define BTN_OPTIONS 13
#define SYNC 11
#define TRIGGER_ACTION 12

#define OPTION_MAIN 0
#define OPTION_MEASURE 1
#define OPTION_CALIBRATE 2

/* VARIABLES =============================================================== */

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
byte currentLoop = 1;
bool isSyncPulseOdd = true;
byte currentOption = 0;

/*  BOARD SETUP ============================================================ */

void initializeIO() {
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_ACTION, INPUT_PULLUP);
  pinMode(BTN_OPTIONS, INPUT_PULLUP);
  pinMode(SYNC, INPUT_PULLUP);
  pinMode(TRIGGER_ACTION, OUTPUT);
}

/* GENERIC FUNCTIONS (display) ============================================== */

void clearLine(byte line) {
  lcd.setCursor(0, line);
  lcd.print("                ");
}

void printLine(byte line, String text) {
  clearLine(line);
  lcd.setCursor(0, line);
  lcd.print(text);
}

/* DISPLAY =================================================================== */

void helloScreen() {
  int i = 0;
  while (i < 3) {
    lcd.setCursor(0, 0);
    lcd.print("Ditto+ CTRL");
    delay(300);
    lcd.clear();
    delay(300);
    i++;
  }
}

void printLoopNumber() {
  lcd.setCursor(5, 0);
  lcd.print("   ");
  lcd.setCursor(5, 0);
  lcd.print(currentLoop);
}

void printSyncIndicator() {
  if (digitalRead(SYNC) == HIGH){
    lcd.setCursor(5, 1);
    lcd.print("*");
    delay(250);
    lcd.setCursor(5, 1);
    lcd.print(" ");
  }
}

void printActionMsg() {
  lcd.setCursor(5, 1);
  lcd.print("WAITING...");
}

void printTriggerSuccessMsg() {
  lcd.setCursor(5, 1);
  lcd.print("TRIGGERED!  ");
  delay(500);
  lcd.setCursor(5, 1);
  lcd.print("          ");
}

/* BUTTON HANDLING =========================================================== */

void waitForButtonRelease(byte button) {
  while(true) {
    if (digitalRead(button) == HIGH) {
      break;
    }
  }
}

void handleButtonClick(byte button) {
  printLoopNumber();
  waitForButtonRelease(button);
}

void handleActionButtonClick() {
  printActionMsg();
  waitForButtonRelease(BTN_ACTION);
  if (waitForSync()) triggerLooper('A');
  printTriggerSuccessMsg();
}

void handleOptionsButtonClick() {
  waitForButtonRelease(BTN_OPTIONS);
  currentOption++;
  if (currentOption == 3) currentOption = 0;
  
  switch (currentOption) {
    case OPTION_MAIN:
      viewMain();
    case OPTION_MEASURE:
      viewMeasure();
    case OPTION_CALIBRATE:
      viewCalibrate();
  }
}

/* VIEWS ============================================== */

void viewMain() {
  printLine(0, "LOOP");
  printLoopNumber();
  printLine(1, "SYNC");
}

void viewMeasure() {
  printLine(0, "OPTIONS");
  printLine(1, "Measure");
}

void viewCalibrate() {
  printLine(0, "OPTIONS");
  printLine(1, "Calibrate");
}

/* ACTIONS ============================================== */


void triggerLooper(char button) {
//  switch(button) {
//    case 'A':
        digitalWrite(TRIGGER_ACTION, HIGH);
        delay(100);
        digitalWrite(TRIGGER_ACTION, LOW);
//  }
}

bool waitForSync() {
  while(true){
      if (digitalRead(SYNC) == HIGH) {
        isSyncPulseOdd = !isSyncPulseOdd;
        return true;
    }
  }
}

/* SETUP / LOOP ============================================== */
 
void setup() {
  initializeIO();
  lcd.begin(16, 2);
  helloScreen();
  viewMain();
}
 
void loop() {
  if (digitalRead(BTN_ACTION) == LOW) {
    handleActionButtonClick();
  } else if (digitalRead(BTN_LEFT) == LOW) {
    currentLoop--;
    handleButtonClick(BTN_LEFT);
  } else if (digitalRead(BTN_RIGHT) == LOW) {
    currentLoop++;
    handleButtonClick(BTN_RIGHT);
  } else if (digitalRead(BTN_OPTIONS) == LOW) {
    handleOptionsButtonClick();
  }
  
  if (currentOption == OPTION_MAIN) printSyncIndicator();
}
