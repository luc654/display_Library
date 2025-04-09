#include "DisplayLib.h"
#include "Arduino.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <string>
#include <vector>

#define MAX_ELEMENTS 10
#define MAX_SCREENS 10

bool checkboxStates[MAX_ELEMENTS] = {false};

enum ElementType { TEXT, BUTTON, CHECKBOX };

struct TextParams {
  const char *text;
  int x;
  int y;  
};

struct ButtonParams {
  const char* text;
  int x;
  int y;
};

struct CheckParams {
  int x;
  int y;
  int size;
  bool clicked;
};


struct DisplayElement {
  int id;
  ElementType type;
  bool active;
  void (*action)();

  union {
    TextParams textParams;
    ButtonParams buttonParams;
    CheckParams checkParams;
  } data;
};

DisplayElement screenElements[MAX_ELEMENTS];
DisplayElement clickableElements[MAX_ELEMENTS];
std::vector<std::string> history;


struct ScreenLoaded {
  const char *name;
  DisplayElement screenElements[MAX_ELEMENTS];
  DisplayElement clickableElements[MAX_ELEMENTS];
  int elementCount;
  int clickableCount;
};

ScreenLoaded screens[MAX_SCREENS];

int elementCount = 0;
int clickableCount = 0;
int screenCount = 0;

DisplayLib::DisplayLib(Adafruit_SH1106G *displayObject) {
  _display = displayObject;
}

//
// Public functions
//

void DisplayLib::addText(const char *text, int xPos, int yPos) {
  if (elementCount >= MAX_ELEMENTS)
    return;

  screenElements[elementCount].type = TEXT;
  screenElements[elementCount].data.textParams = {text, xPos, yPos};
  screenElements[elementCount].id = elementCount;
  elementCount++;
}

void DisplayLib::addButton(const char *text, int xPos, int yPos, boolean active,
                           void (*callback)()) {
  if (elementCount >= MAX_ELEMENTS)
    return;

  ButtonParams btn = {text, xPos, yPos};

  screenElements[elementCount].type = BUTTON;
  screenElements[elementCount].data.buttonParams = btn;
  screenElements[elementCount].active = active;
  screenElements[elementCount].action = callback;
  screenElements[elementCount].id = elementCount;

  clickableElements[clickableCount].type = BUTTON;
  clickableElements[clickableCount].data.buttonParams = btn;
  clickableElements[clickableCount].active = active;
  clickableElements[clickableCount].action = callback;
  clickableElements[clickableCount].id = elementCount;

  clickableCount++;
  elementCount++;
}

void DisplayLib::addCheckbox(int xPos, int yPos, int size,
                             boolean active, boolean clicked, void (*callback)()) {
  if (elementCount >= MAX_ELEMENTS)
    return;
  CheckParams checkbox = {xPos, yPos, size, clicked};

  screenElements[elementCount].type = CHECKBOX;
  screenElements[elementCount].data.checkParams = checkbox;
  screenElements[elementCount].active = active;
  screenElements[elementCount].action = callback;
  screenElements[elementCount].id = elementCount;

  clickableElements[clickableCount].type = CHECKBOX;
  clickableElements[clickableCount].data.checkParams = checkbox;
  clickableElements[clickableCount].active = active;
  clickableElements[clickableCount].action = callback;
  clickableElements[clickableCount].id = elementCount;


  checkboxStates[elementCount] = clicked; 
  
  
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
    clickableElements[i].active = false;
  }

  for (int i = 0; i < clickableCount; i++) {
    DisplayElement currElement = clickableElements[i];
    int id = currElement.id;

    if (screenElements[id].active) {
      int nextIndex = forward ? (i + 1) % clickableCount
                              : (i == 0 ? clickableCount - 1 : i - 1);

      int nextId = clickableElements[nextIndex].id;

      screenElements[nextId].active = true;
      screenElements[id].active = false;
      break;
    }
  }
}


void DisplayLib::flush() {
  void (*action)() = nullptr;
  for (int i = 0; i < clickableCount; i++) {
    int id = clickableElements[i].id;
    if (screenElements[id].active) {

      if (screenElements[id].type == CHECKBOX) {
        checkboxStates[id] = !checkboxStates[id]; 
      
  
        screenElements[id].data.checkParams.clicked = checkboxStates[id];
        clickableElements[i].data.checkParams.clicked = checkboxStates[id];
      }
      

      action = screenElements[id].action;
      if (action != nullptr) {

        action();
      }
      break;
    }
  }
}

void DisplayLib::safeScreen(const char *name) {
  screens[screenCount].name = name;
  screens[screenCount].elementCount = elementCount;
  screens[screenCount].clickableCount = clickableCount;

  for (int i = 0; i < elementCount; i++) {
    screens[screenCount].screenElements[i] = screenElements[i];
  
    if (screenElements[i].type == CHECKBOX) {
      int id = screenElements[i].id;
      screens[screenCount].screenElements[i].data.checkParams.clicked = checkboxStates[id];
    }
  }
  

  for (int i = 0; i < clickableCount; i++) {
    screens[screenCount].clickableElements[i] = clickableElements[i];
  }

  screenCount++;

  elementCount = 0;
  clickableCount = 0;
}




void DisplayLib::back() {
  if (history.size() < 2) {
    return;
  }

  history.pop_back();
  const std::string &name = history.back();
  this->loadScreen(name.c_str());
}

void DisplayLib::loadScreen(const char *name) {
  for (int i = 0; i < screenCount; i++) {
    if (strcmp(screens[i].name, name) == 0) {
      elementCount = screens[i].elementCount;
      clickableCount = screens[i].clickableCount;

      for (int j = 0; j < elementCount; j++) {
        screenElements[j] = screens[i].screenElements[j];
      
        if (screenElements[j].type == CHECKBOX) {
          int id = screenElements[j].id;
          screenElements[j].data.checkParams.clicked = checkboxStates[id];
        }
      }
      
      for (int j = 0; j < clickableCount; j++) {
        clickableElements[j] = screens[i].clickableElements[j];
      
        if (clickableElements[j].type == CHECKBOX) {
          int id = clickableElements[j].id;
          clickableElements[j].data.checkParams.clicked = checkboxStates[id];
        }
      }
      
      history.push_back(name);
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

void DisplayLib::showText(const char *text, int xPos, int yPos) {
  _display->setCursor(xPos, yPos);
  _display->println(text);
}

void DisplayLib::showButton(const char *text, int xPos, int yPos,
                            boolean active) {
  if (active) {
    _display->setTextColor(SH110X_BLACK, SH110X_WHITE);
  }
  _display->setCursor(xPos, yPos);
  _display->println(text);
  _display->setTextColor(SH110X_WHITE);
}

void DisplayLib::showCheckbox(int xPos, int yPos, int size, boolean active, boolean clicked) {
    
  _display->setCursor(xPos, yPos);
  _display->drawRect(xPos, yPos, size, size, SH110X_WHITE);

  
  
  // Since there are 4 states a checkbox can be in we need 4 styles
  if (active && clicked){
  _display->drawRect(xPos - 1, yPos - 1, size + 2, size + 2, SH110X_WHITE);
  _display->drawLine(xPos, yPos+ size - 1, xPos + size - 1, yPos, SH110X_WHITE);
  _display->drawLine(xPos, yPos, xPos + size - 1, yPos + size - 1, SH110X_WHITE);
} else if (clicked){
  
  _display->drawLine(xPos, yPos+ size - 1, xPos + size - 1, yPos, SH110X_WHITE);
  _display->drawLine(xPos, yPos, xPos + size - 1, yPos + size - 1, SH110X_WHITE);

} else if (active){
    _display->drawRect(xPos - 1, yPos - 1, size + 2, size + 2, SH110X_WHITE);
  }
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
    showText(el.data.textParams.text, el.data.textParams.x,
             el.data.textParams.y);
    break;
  case BUTTON:
    showButton(el.data.buttonParams.text, el.data.buttonParams.x,
               el.data.buttonParams.y, el.active);
    break;
  case CHECKBOX:
  showCheckbox(el.data.checkParams.x, el.data.checkParams.y, el.data.checkParams.size, el.active, el.data.checkParams.clicked);
  default:
    break;
  }
}
