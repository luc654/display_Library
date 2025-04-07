#include "Arduino.h"
#include "DisplayLib.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define MAX_ELEMENTS 10
#define MAX_SCREENS 10

enum ElementType {
  TEXT,
  BUTTON
};

struct TextParams {
  int id;
  const char* text;
  int x;
  int y;
};

struct ButtonParams {
  int id;
  const char* text;
  int x;
  int y;
  boolean active;
  void (*action)(); 
};

struct DisplayElement {
  ElementType type;
  union {
    TextParams textParams;
    ButtonParams buttonParams;
  } data;
};

DisplayElement screenElements[MAX_ELEMENTS];
DisplayElement clickableElements[MAX_ELEMENTS];


struct ScreenLoaded {
  const char* name;
  DisplayElement screenElements[MAX_ELEMENTS];
  DisplayElement clickableElements[MAX_ELEMENTS];
  int elementCount;
  int clickableCount;
};


ScreenLoaded screens[MAX_SCREENS];

int elementCount = 0;
int clickableCount = 0;
int screenCount = 0;

DisplayLib::DisplayLib(Adafruit_SH1106G* displayObject) {
  _display = displayObject;
}


// 
// Public functions
// 

void DisplayLib::addText(const char* text, int xPos, int yPos) {
  if (elementCount >= MAX_ELEMENTS) return;

  screenElements[elementCount].type = TEXT;
  screenElements[elementCount].data.textParams = { elementCount, text, xPos, yPos };
  elementCount++;
}

void DisplayLib::addButton(const char* text, int xPos, int yPos, boolean active, void(*callback)()){
  if (elementCount >= MAX_ELEMENTS) return;

  ButtonParams btn = { elementCount, text, xPos, yPos, active, callback };

  screenElements[elementCount].type = BUTTON;
  screenElements[elementCount].data.buttonParams = btn;

  clickableElements[clickableCount].type = BUTTON;
  clickableElements[clickableCount].data.buttonParams = btn;

  clickableCount++;
  elementCount++;
}




void DisplayLib::begin() {
  _display->begin(0x3c, true);
  _display->clearDisplay();
  _display->setTextSize(1);
  _display->setTextColor(SH110X_WHITE);
}

void DisplayLib::cycle(boolean forward) {
  for (int i = 0; i < clickableCount; i++) {
    clickableElements[i].data.buttonParams.active = false;
  }

  for (int i = 0; i < clickableCount; i++) {
    DisplayElement currIndex = clickableElements[i];

    if (screenElements[currIndex.data.buttonParams.id].data.buttonParams.active == true) {
      if (forward) {
        if (i == clickableCount - 1) {
          screenElements[clickableElements[0].data.buttonParams.id].data.buttonParams.active = true;
        } else {
          screenElements[clickableElements[i + 1].data.buttonParams.id].data.buttonParams.active = true;
        }
      } else {
        if (i == 0) {
          screenElements[clickableElements[clickableCount - 1].data.buttonParams.id].data.buttonParams.active = true;
        } else {
          screenElements[clickableElements[i - 1].data.buttonParams.id].data.buttonParams.active = true;
        }
      }

      
      screenElements[currIndex.data.buttonParams.id].data.buttonParams.active = false;
      break; 
    }
  }
}


void DisplayLib::flush() {
  void (*action)() = nullptr;
  for (int i = 0; i < clickableCount; i++) {
    int id = clickableElements[i].data.buttonParams.id;
    if (screenElements[id].data.buttonParams.active) {
      Serial.print("Active ID ");
      Serial.println(id);

      action = screenElements[id].data.buttonParams.action;
      if (action != nullptr) {

        action();  
  }
      break;
    } else {
      Serial.print("Not active ID ");
      Serial.println(id);
    }
  }
}


void DisplayLib::back(){
  Serial.println("We're back");
}

void DisplayLib::safeScreen(const char* name){
  screens[screenCount].name = name;
  screens[screenCount].elementCount = elementCount;
  screens[screenCount].clickableCount = clickableCount;

  for (int i = 0; i < elementCount; i++) {
    screens[screenCount].screenElements[i] = screenElements[i];
  }

  for (int i = 0; i < clickableCount; i++) {
    screens[screenCount].clickableElements[i] = clickableElements[i];
  }

  screenCount++;

  elementCount = 0;
  clickableCount = 0;
}


void DisplayLib::loadScreen(const char* name){
  for (int i = 0; i < screenCount; i++) {
    if (strcmp(screens[i].name, name) == 0) {
      elementCount = screens[i].elementCount;
      clickableCount = screens[i].clickableCount;

      for (int j = 0; j < elementCount; j++) {
        screenElements[j] = screens[i].screenElements[j];
      }

      for (int j = 0; j < clickableCount; j++) {
        clickableElements[j] = screens[i].clickableElements[j];
      }

      updateScreen();
      return;
    }
  }

  // fallback
  updateScreen();
}








// 
// Private functions
// 

void DisplayLib::showText(const char* text, int xPos, int yPos) {
  _display->setCursor(xPos, yPos);
  _display->println(text);
}

void DisplayLib::showButton(const char* text, int xPos, int yPos, boolean active){

  if(active){
    _display->setTextColor(SH110X_BLACK, SH110X_WHITE); 
  }
  _display->setCursor(xPos, yPos);
  _display->println(text);
  _display->setTextColor(SH110X_WHITE);


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
    case BUTTON:
      showButton(el.data.buttonParams.text, el.data.buttonParams.x, el.data.buttonParams.y, el.data.buttonParams.active);
      break;
    default:
      break;
  }



}
