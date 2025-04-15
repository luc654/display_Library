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

DisplayLib displayController(&display);


// Define buttons
#define BUTTON_A 28
#define BUTTON_B 27
#define BUTTON_C 26

// Define vector with const char*
std::vector<const char *> cars = { "Volvo", "BMW", "Ford", "Mazda" };

void setup() {
  Serial.begin(9600);
  displayController.begin();
  displayController.addList(0, 0, 10, cars);

  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
}

void loop() {
  displayController.updateScreen();

  if (digitalRead(BUTTON_A)) {
    displayController.cycle(true);
  }
  if (digitalRead(BUTTON_B)) {
    displayController.flush();
  }

  if (digitalRead(BUTTON_C)) {
    displayController.cycle(false);
  }


  delay(200);
}