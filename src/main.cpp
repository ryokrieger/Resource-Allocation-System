#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define BTN_R1     2
#define BTN_R2     3
#define BTN_PROC   4
#define BTN_OK     5
#define BTN_RESET  6

#define LED_R1A 8
#define LED_R1B 9
#define LED_R2A 10
#define LED_R2B 11

#define BUZZER 12

enum State { 
  CHOOSE,
  RUN_ORDER,
  SHOW_SAFE,
  EXECUTE,
  FINISHED
};
State currentState = CHOOSE;

int totalR1 = 1;
int totalR2 = 1;
int runIndex = 0;
int execIndex = 0;
int selectedProc = 0;
bool showingProcess = false;

int getScenario() {
  if (totalR1 == 2 && totalR2 == 2) return 0;
  if (totalR1 == 2 && totalR2 == 1) return 1;
  if (totalR1 == 1 && totalR2 == 2) return 2;
  return 3;
}

const int safeSeq[4][3] = {
  {1, 0, 2},
  {2, 0, 1},
  {1, 2, 0},
  {0, 1, 2}
};

const byte fullLED[4][4] = {
  {1, 1, 1, 1},
  {1, 1, 1, 0},
  {1, 0, 1, 1},
  {0, 1, 0, 1}
};

const byte runLEDBefore[4][3][4] = {
  { {1,1,1,1}, {1,1,1,1}, {0,1,0,1} },
  { {1,1,1,0}, {1,1,1,0}, {1,1,1,0} },
  { {1,0,1,1}, {1,0,1,1}, {1,0,0,1} },
  { {0,1,0,1}, {0,1,0,1}, {0,1,0,1} }
};

const byte runLEDAfter[4][3][4] = {
  { {1,1,1,1}, {0,1,0,1}, {0,1,0,1} },
  { {1,1,1,0}, {1,1,1,0}, {0,1,1,0} },
  { {1,0,1,1}, {1,0,0,1}, {1,0,0,1} },
  { {0,1,0,1}, {0,1,0,1}, {0,1,0,1} }
};

bool pressed(int pin) {
  if (digitalRead(pin) == LOW) {
    delay(200);
    while (digitalRead(pin) == LOW);
    return true;
  }
  return false;
}

void clearLEDs() {
  digitalWrite(LED_R1A, LOW);
  digitalWrite(LED_R1B, LOW);
  digitalWrite(LED_R2A, LOW);
  digitalWrite(LED_R2B, LOW);
}

void showLED(const byte led[4]) {
  digitalWrite(LED_R1A, led[0] ? HIGH : LOW);
  digitalWrite(LED_R1B, led[1] ? HIGH : LOW);
  digitalWrite(LED_R2A, led[2] ? HIGH : LOW);
  digitalWrite(LED_R2B, led[3] ? HIGH : LOW);
}

void playErrorBuzz() {
  tone(BUZZER, 1000, 300);
  delay(350);
}

void playSuccessBeep() {
  tone(BUZZER, 2000, 100);
  delay(150);
}

void displayChoose() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  
  display.println("Choose Resources");
  display.println();
  display.print("R1: ");
  display.println(totalR1);
  display.print("R2: ");
  display.println(totalR2);
  display.println();
  display.println("Press OK to confirm");
  
  display.display();
}

void displayRun() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  
  display.println("Run Processes");
  display.println("(In Order)");
  display.println();
  display.setTextSize(2);
  display.print("P");
  display.println(runIndex);
  display.setTextSize(1);
  display.println();
  display.println("Press OK");
  
  display.display();
}

void displaySafeSequence() {
  int s = getScenario();
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  
  display.println("Safe Sequence:");
  display.println();
  display.setTextSize(2);
  display.print("P");
  display.print(safeSeq[s][0]);
  display.print(">");
  display.print("P");
  display.print(safeSeq[s][1]);
  display.print(">");
  display.print("P");
  display.println(safeSeq[s][2]);
  
  display.display();
}

void displayExecute() {
  int s = getScenario();
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  
  display.println("Safe Sequence:");
  display.print("P");
  display.print(safeSeq[s][0]);
  display.print(" > P");
  display.print(safeSeq[s][1]);
  display.print(" > P");
  display.println(safeSeq[s][2]);
  display.println();
  
  display.print("Next: P");
  display.println(safeSeq[s][execIndex]);
  display.println();
  
  display.setTextSize(2);
  display.print("Sel: P");
  display.println(selectedProc);
  
  display.display();
}

void displayFinished() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  
  display.setCursor(35, 0);
  display.println("Yay!");
  
  display.setCursor(10, 20);
  display.println("Processes");
  display.setCursor(10, 40);
  display.println("Finished!");
  
  display.display();
}

void setup() {
  pinMode(BTN_R1, INPUT_PULLUP);
  pinMode(BTN_R2, INPUT_PULLUP);
  pinMode(BTN_PROC, INPUT_PULLUP);
  pinMode(BTN_OK, INPUT_PULLUP);
  pinMode(BTN_RESET, INPUT_PULLUP);

  pinMode(LED_R1A, OUTPUT);
  pinMode(LED_R1B, OUTPUT);
  pinMode(LED_R2A, OUTPUT);
  pinMode(LED_R2B, OUTPUT);
  
  pinMode(BUZZER, OUTPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (1) {
      digitalWrite(LED_R1A, HIGH);
      delay(100);
      digitalWrite(LED_R1A, LOW);
      delay(100);
    }
  }
  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.clearDisplay();
  
  display.setCursor(0, 0);
  display.println("Resource Allocation");
  display.println("System");
  display.println();
  display.println("Banker's Algorithm");
  display.display();
  delay(2000);
  
  clearLEDs();
}

void loop() {
  int s = getScenario();

  if (pressed(BTN_RESET)) {
    currentState = CHOOSE;
    totalR1 = 1;
    totalR2 = 1;
    runIndex = 0;
    execIndex = 0;
    selectedProc = 0;
    showingProcess = false;
    clearLEDs();
    displayChoose();
    delay(300);
    return;
  }

  if (currentState == CHOOSE) {
    if (pressed(BTN_R1)) {
      totalR1 = (totalR1 == 1) ? 2 : 1;
      playSuccessBeep();
    }
    
    if (pressed(BTN_R2)) {
      totalR2 = (totalR2 == 1) ? 2 : 1;
      playSuccessBeep();
    }

    showLED(fullLED[getScenario()]);
    displayChoose();

    if (pressed(BTN_OK)) {
      runIndex = 0;
      showingProcess = false;
      currentState = RUN_ORDER;
      playSuccessBeep();
    }
  }

  else if (currentState == RUN_ORDER) {
    if (!showingProcess) {
      showLED(runLEDBefore[s][runIndex]);
      displayRun();
      showingProcess = true;
    }

    if (pressed(BTN_OK)) {
      playSuccessBeep();
      
      showLED(runLEDAfter[s][runIndex]);
      delay(800);
      
      runIndex++;
      showingProcess = false;
      
      if (runIndex > 2) {
        execIndex = 0;
        selectedProc = 0;
        currentState = SHOW_SAFE;
        displaySafeSequence();
        delay(2000);
        currentState = EXECUTE;
      }
    }
  }

  else if (currentState == EXECUTE) {
    displayExecute();

    if (pressed(BTN_PROC)) {
      selectedProc = (selectedProc + 1) % 3;
      playSuccessBeep();
      displayExecute();
    }

    if (pressed(BTN_OK)) {
      if (selectedProc == safeSeq[s][execIndex]) {
        playSuccessBeep();
        showLED(fullLED[s]);
        delay(500);
        
        execIndex++;
        
        if (execIndex > 2) {
          currentState = FINISHED;
        } else {
          displayExecute();
        }
      } else {
        playErrorBuzz();
      }
    }
  }

  else if (currentState == FINISHED) {
    showLED(fullLED[s]);
    displayFinished();
  }
}