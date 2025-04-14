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
#define BUTTON_A  28
#define BUTTON_B  27
#define BUTTON_C  26


void setup() {
  Serial.begin(9600);
  displayController.begin(); 

//  Home screen 
  displayController.addText("Hello, World!!", 10, 30, "change");
  displayController.addButton("one", 2, 50, [](void){
    displayController.setText("change", "Example of removing text");
  });
  displayController.addButton("print", 45, 50, [](void){
    print();
  });
  displayController.addButton("next", 90, 50, [](void) {
      displayController.loadScreen("second");
  });
  displayController.safeScreen("home");
  delay(20);

  // second screen

  displayController.addText("Cycle between 2 & 3", 10, 30, "editText");
  displayController.addButton("Back", 2, 50, []() {
      displayController.back();
  });
  displayController.addButton("two", 45, 50, [](void){
    displayController.setText("editText", "Hello");
  });
  displayController.addButton("three", 90, 50, [](void){
    displayController.setText("editText", "World!");
    displayController.setText("change", "sub page secret?!");
  });
  displayController.safeScreen("second");
  delay(20);

  // set screen to home screen
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


  if (digitalRead(BUTTON_A)) {
    handleHold('A', []() { displayController.cycle(true); }, 100);
  } else if (digitalRead(BUTTON_B)) {
    handleHold('B', []() { displayController.flush();}, 50);
  } else if (digitalRead(BUTTON_C)) {
    handleHold('C', []() { displayController.cycle(false); }, 100);
  } else {
    currentHold = '\0';  
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

void print(){
  Serial.println("Hello");
}