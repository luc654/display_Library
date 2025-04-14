#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <DisplayLib.h>

// Define screen
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

DisplayLib displayController(&display);


// Define buttons
#define BUTTON_A 28
#define BUTTON_B 27
#define BUTTON_C 26


void setup() {
  Serial.begin(9600);
  displayController.begin();


  // sleep screen


  displayController.safeScreen("sleep");
  //  Home screen
  displayController.addText("Advanced controls", 0, 0);
  displayController.addText("A & C = sleep", 0, 20);
  displayController.addButton("Example screen", 2, 50, [](void) {
    // '1' as parameter to set the second clickabe element as active
    displayController.loadScreen("checks", 1);
  });

  displayController.safeScreen("home");
  delay(20);


  // Checkbox screen
  displayController.addText("Back", 0, 10);
  displayController.addButton("<", 30, 10,  [](void) {
    displayController.loadScreen("home");
  });
  displayController.addCheckbox(10, 30, 10, false, nullptr);
  displayController.addCheckbox(25, 30, 10, false, nullptr);
  displayController.addCheckbox(40, 30, 10, false, nullptr);
  displayController.addCheckbox(55, 30, 10, true, nullptr);
  displayController.addCheckbox(70, 30, 15, true, nullptr);

  displayController.addText("Loads of checkboxes", 0, 50);
  displayController.safeScreen("checks");




  displayController.loadScreen("home");


  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
}

unsigned long buttonPressStart = 0;
char currentHold = '\0';
bool sleepMode = false;

void loop() {
  displayController.updateScreen();


  if(sleepMode == false){

  // Check sleep mode
  if(digitalRead(BUTTON_A) && digitalRead(BUTTON_C)){
    sleepMode = true;
    displayController.loadScreen("sleep");
    delay(100);
  }

  if (digitalRead(BUTTON_A)) {
    handleHold('A', []() { displayController.cycle(true); }, 100);
  } else if (digitalRead(BUTTON_B)) {
    handleHold('B', []() { displayController.flush();}, 50);
  } else if (digitalRead(BUTTON_C)) {
    handleHold('C', []() { displayController.cycle(false); }, 100);
  } else {
    currentHold = '\0';  
  }
    } 
    else 
    {
      if(digitalRead(BUTTON_A) && digitalRead(BUTTON_C)){
        sleepMode = false;
        displayController.loadScreen("home");
      delay(100);

    }
  }
  delay(10);
}

void handleHold(char buttonId, void (*action)(), const int holdTime) {
  if (currentHold != buttonId) {
    currentHold = buttonId;
    buttonPressStart = millis();
    return;
  }

  if (millis() - buttonPressStart >= holdTime) {
    action();
    currentHold = '\0';  
  }
}
