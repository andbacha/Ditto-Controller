#include <LiquidCrystal.h>

/* CONSTANTS ============================================================== */

#define BTN_LEFT 8
#define BTN_RIGHT 9
#define BTN_ACTION 10
#define BTN_OPTIONS 13
#define SYNC 11
#define TRIGGER_ACTION 12

#define OPTION_MAIN 0
#define OPTION_DIVIDER 1
#define OPTION_MEASURE 2
#define OPTION_CALIBRATE 3

#define MIN_LOOP 1
#define MAX_LOOP 99

/* VARIABLES =============================================================== */

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
byte currentLoop = 1;
byte currentOption = 0;
byte pulsesPerBarArray[] = {1, 4, 8};
byte pulsesPerBarIndex = 2;
byte pulsesPerBar = pulsesPerBarArray[pulsesPerBarIndex];
// bool isSyncPulseOdd = true;



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
  for (byte i = 0; i < 16; i++){
    lcd.print(" ");
  }
}

void printLine(byte line, const char* text) {
  clearLine(line);
  lcd.setCursor(0, line);
  lcd.print(text);
}

/* DISPLAY =================================================================== */

void helloScreen() {
  for (byte i = 0; i < 3; i++) {
    printLine(0, "* Ditto+  CTRL *");
    printLine(1, "*    v. 0.1    *");
    delay(300);
    printLine(0, "  Ditto+  CTRL  ");
    printLine(1, "     v. 0.1     ");
    delay(300);
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
  currentOption++;
  waitForButtonRelease(BTN_OPTIONS);
  if (currentOption == 4) currentOption = 0;
  
  switch (currentOption) {
    case OPTION_MAIN:
      viewMain();
      break;
    case OPTION_DIVIDER:
      viewDivider();
      break;
    case OPTION_MEASURE:
      viewMeasure();
      break;
    case OPTION_CALIBRATE:
      viewCalibrate();
      break;
    default:
      viewMain();
      break;
  }
}

/* VIEWS ============================================== */

void viewMain() {
  printLine(0, "LOOP");
  printLoopNumber();
  printLine(1, "SYNC");
}

void viewDivider() {
  printLine(0, "OPTIONS --------");
  printLine(1, "| Pulses/bar:   ");
  while(true) {
    if (digitalRead(BTN_LEFT) == LOW) {
      waitForButtonRelease(BTN_LEFT);
      if (pulsesPerBar != 1) pulsesPerBarIndex--;
    } if (digitalRead(BTN_RIGHT) == LOW) {
      waitForButtonRelease(BTN_RIGHT);
      if (pulsesPerBar != 8) pulsesPerBarIndex++;
    } if (digitalRead(BTN_OPTIONS) == LOW) {
      break;
    }
    pulsesPerBar = pulsesPerBarArray[pulsesPerBarIndex];
    lcd.setCursor(14, 1);
    lcd.print(pulsesPerBar);
  }
}

void viewMeasure() {
  printLine(0, "OPTIONS --------");
  printLine(1, "| Measure: 4/4");
  while(true) {
    if (digitalRead(BTN_OPTIONS) == LOW) {
      break;
    }
  }
}

void viewCalibrate() {
  printLine(0, "OPTIONS --------");
  printLine(1, "| Calibrate < >");
  while(true) {
    if (digitalRead(BTN_OPTIONS) == LOW) {
      break;
    }
  }
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
        // isSyncPulseOdd = !isSyncPulseOdd;
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
    if (currentLoop != MIN_LOOP) currentLoop--;
    handleButtonClick(BTN_LEFT);
  } else if (digitalRead(BTN_RIGHT) == LOW) {
    if (currentLoop != MAX_LOOP) currentLoop++;
    handleButtonClick(BTN_RIGHT);
  } else if (digitalRead(BTN_OPTIONS) == LOW) {
    handleOptionsButtonClick();
  }
  
  if (currentOption == OPTION_MAIN) printSyncIndicator();
}
