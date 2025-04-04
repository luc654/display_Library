#include "Arduino.h"
#include "DisplayLib.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define MAX_ELEMENTS 10

enum ElementType {
  TEXT
};

struct TextParams {
  const char* text;
  int x;
  int y;
};

struct DisplayElement {
  ElementType type;
  union {
    TextParams textParams;
  } data;
};

DisplayElement screenElements[MAX_ELEMENTS];
int elementCount = 0;

DisplayLib::DisplayLib(Adafruit_SH1106G* displayObject) {
  _display = displayObject;
}

void DisplayLib::addText(const char* text, int xPos, int yPos) {
  if (elementCount >= MAX_ELEMENTS) return;

  screenElements[elementCount].type = TEXT;
  screenElements[elementCount].data.textParams = { text, xPos, yPos };
  elementCount++;
}

void DisplayLib::begin() {
  _display->begin(0x3c, true);
  _display->clearDisplay();
  _display->setTextSize(1);
  _display->setTextColor(SH110X_WHITE);
}

void DisplayLib::showText(const char* text, int xPos, int yPos) {
  _display->setCursor(xPos, yPos);
  _display->println(text);
}

void DisplayLib::updateScreen() {
  _display->clearDisplay();

  for (int i = 0; i < elementCount; i++) {
    handleElement(screenElements[i]);
  }

  _display->display();
}

void DisplayLib::handleElement(DisplayElement el) {
  switch (el.type) {
    case TEXT:
      showText(el.data.textParams.text, el.data.textParams.x, el.data.textParams.y);
      break;
    default:
      break;
  }
}
