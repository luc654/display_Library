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
  displayController.addText("Finally, checkboxes!!", 10, 30);
  displayController.addButton("checkboxes", 2, 50, true, [](void) {
      displayController.loadScreen("checks");
  });
  displayController.addButton("settings", 65, 50, false, [](void) {
      displayController.loadScreen("settings");
  });
  displayController.safeScreen("home");
  delay(20);

  // Settings screen
  displayController.addText("nuthing here", 10, 30);
  displayController.addButton("Back", 2, 50, true, []() {
      displayController.back();
  });
  displayController.addButton("two", 45, 50, false, nullptr);
  displayController.addButton("three", 90, 50, false, nullptr);
  displayController.safeScreen("settings");
  delay(20);

  // Checkbox screen
  displayController.addText("back", 30, 10);
  displayController.addCheckbox(10, 30, 10, true, false, nullptr);
  displayController.addCheckbox(25, 30, 10, false, false, []() {
    displayController.back();
  });
  displayController.addCheckbox(40, 30, 10, false, false, nullptr);
  displayController.addCheckbox(55, 30, 10, false, false, nullptr);
  displayController.safeScreen("checks");





  displayController.loadScreen("home");


  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);

}

void loop() {
  displayController.updateScreen();

    if(digitalRead(BUTTON_A)){
    displayController.cycle(true);
  } 
  if(digitalRead(BUTTON_B)){
    displayController.flush();
  } 

  if(digitalRead(BUTTON_C)){
    displayController.cycle(false);
  } 
  
  
  delay(200);
}