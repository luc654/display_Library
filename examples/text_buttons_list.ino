#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <DisplayLib.h>
#include <vector>

// Define screen
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
DisplayLib dC(&display); 


// Define buttons
#define BUTTON_A 28
#define BUTTON_B 27
#define BUTTON_C 26

// Define a vector with struct ButtonDef
std::vector<ButtonDef> btnList = {
  {"Home", []() {
    dC.loadScreen("home");
  }},
    {"Sleep", []() {
    dC.loadScreen("sleep");
  }},
    {"Settings", []() {
    dC.loadScreen("Settings");
  }}
};


// Define vector with const char*
std::vector<const char *> textList = { "One", "Two", "Three"};

void setup() {
  Serial.begin(9600);
  dC.begin();

  dC.safeScreen("sleep");

  dC.addText("Homepage 0.2", 2,0);

  dC.addBtnList( 2,50,5,false, btnList);
  dC.addList(2,15,15, false, textList);

  dC.safeScreen("home");
  
  dC.loadScreen("home");

  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
}

unsigned long buttonPressStart = 0;
char currentHold = '\0';
bool sleepMode = false;

void loop() {
  dC.updateScreen();


  if(sleepMode == false){

  // Check sleep mode
  if(digitalRead(BUTTON_A) && digitalRead(BUTTON_C)){
    sleepMode = true;
    dC.loadScreen("sleep");
    delay(100);
  }

  if (digitalRead(BUTTON_A)) {
    handleHold('A', []() { dC.cycle(true); }, 100);
  } else if (digitalRead(BUTTON_B)) {
    handleHold('B', []() { dC.flush();}, 50);
  } else if (digitalRead(BUTTON_C)) {
    handleHold('C', []() { dC.cycle(false); }, 100);
  } else {
    currentHold = '\0';  
  }
    } 
    else 
    {
      if(digitalRead(BUTTON_A) && digitalRead(BUTTON_C)){
        sleepMode = false;
        dC.loadScreen("home");
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
