#include "DisplayLib.h"
#include "Arduino.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <string>

#define MAX_ELEMENTS 10
#define MAX_SCREENS 10

#define MAX_HISTORY 10

bool checkboxStates[MAX_ELEMENTS] = {false};

enum ElementType { TEXT, BUTTON, CHECKBOX };

struct TextParams {
  const char *text;
  int x;
  int y;  
  const char *identifier;
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
  int globId;
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
std::string history[MAX_HISTORY];
int historyIndex = 0;

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

void DisplayLib::addText(const char *text, int xPos, int yPos, const char *identifier) {
  if (elementCount >= MAX_ELEMENTS)
    return;

  screenElements[elementCount].type = TEXT;
  screenElements[elementCount].data.textParams = {text, xPos, yPos, identifier};
  screenElements[elementCount].id = elementCount;
  elementCount++;
}


void DisplayLib::setText(const char *identifier, const char *text, boolean all){
  int elemId;
  // Get id of current active element
  for(int i = 0; i < elementCount; i++){
    if(screenElements[i].active){
      elemId  = screenElements[i].id;
      Serial.println("Selected> " + elemId);
    }
  }

  // Loop through all screens
  for (int i = 0; i < screenCount; i++){
    // Loop through all elements in screen
    for(int j = 0; j < screens[i].elementCount; j++){
      // Check each element if type is text
      if(screens[i].screenElements[j].type == TEXT){
        // Check if element has given identifier
        if (strcmp(screens[i].screenElements[j].data.textParams.identifier, identifier) == 0){
          // Update text
          screens[i].screenElements[j].data.textParams.text = text;
          
          // If not update all with identifier then return
          if (!all){
            this->loadScreen(historyBack().c_str(), elemId, false);
            return;
          }
        }
      }
    }
  }
  this->loadScreen(historyBack().c_str(), elemId, false);
  return;
}


void DisplayLib::addButton(const char *text, int xPos, int yPos,
                           void (*callback)()) {
  if (elementCount >= MAX_ELEMENTS)
    return;

  ButtonParams btn = {text, xPos, yPos};

  screenElements[elementCount].type = BUTTON;
  screenElements[elementCount].data.buttonParams = btn;
  screenElements[elementCount].active = false;
  screenElements[elementCount].action = callback;
  screenElements[elementCount].id = elementCount;

  clickableElements[clickableCount].type = BUTTON;
  clickableElements[clickableCount].data.buttonParams = btn;
  clickableElements[clickableCount].active = false;
  clickableElements[clickableCount].action = callback;
  clickableElements[clickableCount].id = elementCount;

  clickableCount++;
  elementCount++;
}

void DisplayLib::addCheckbox(int xPos, int yPos, int size, boolean clicked, void (*callback)()) {
  if (elementCount >= MAX_ELEMENTS) return;
  CheckParams checkbox = {xPos, yPos, size, clicked};

  screenElements[elementCount].type = CHECKBOX;
  screenElements[elementCount].data.checkParams = checkbox;
  screenElements[elementCount].active = false;
  screenElements[elementCount].action = callback;
  screenElements[elementCount].id = elementCount;

  clickableElements[clickableCount].type = CHECKBOX;
  clickableElements[clickableCount].data.checkParams = checkbox;
  clickableElements[clickableCount].active = false;
  clickableElements[clickableCount].action = callback;
  clickableElements[clickableCount].id = elementCount;


  checkboxStates[elementCount] = clicked; 
  
  
  clickableCount++;
  elementCount++;
}

void DisplayLib::addList(int xPos, int yPos, int itemSpacing, bool downwards, std::string textList[], int listSize) {
  if (downwards) {
    int yyPos = yPos;
    for (int i = 0; i < listSize; i++) {
      addText(textList[i].c_str(), xPos, yyPos);
      yyPos += itemSpacing;
    }
  } else {
    int xxPos = xPos;
    for (int i = 0; i < listSize; i++) {
      addText(textList[i].c_str(), xxPos, yPos);
      xxPos += itemSpacing;
      xxPos += (textList[i].length() * 6);
    }
  }
}

void DisplayLib::addBtnList(int xPos, int yPos, int itemSpacing, boolean downwards, ButtonDef btnList[], int listSize) {
  if (downwards) {
    for (int i = 0; i < listSize; i++) {
      addButton(btnList[i].label, xPos, yPos + i * itemSpacing, btnList[i].callback);

    }
  } else {
    int xxPos = xPos;
    for (int i = 0; i < listSize; i++) {
      addButton(btnList[i].label, xxPos, yPos, btnList[i].callback);
      xxPos += itemSpacing;
      xxPos += (strlen(btnList[i].label) * 6) + 4 ;

    }
  }
}

void DisplayLib::begin() {
  _display->begin(0x3c, true);
  _display->clearDisplay();
  _display->setTextSize(1);
  _display->setTextColor(SH110X_WHITE);
}

void DisplayLib::cycle(boolean forward) {
  if (clickableCount == 1) { return;} 
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
      Serial.println("Clicked> " + id);
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
  if ((sizeof(history) / sizeof(history[0])) < 2) {
    return;
  }

  historyPop();
  const std::string &name = historyBack();

  this->loadScreen(name.c_str());
}

void DisplayLib::loadScreen(const char *name, int startPos, boolean historyFlag) {
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
        if(j == startPos){
          Serial.println("Thing> " + startPos);

          screenElements[screens[i].clickableElements[startPos].id].active = true;
        }
        clickableElements[j] = screens[i].clickableElements[j];
      
        if (clickableElements[j].type == CHECKBOX) {
          int id = clickableElements[j].id;
          clickableElements[j].data.checkParams.clicked = checkboxStates[id];
        }
      }
      
      if(historyFlag){
        
        addHistory(name);
      }

      updateScreen();
      return;
    }
  }

  // fallback
  updateScreen();
}

std::string DisplayLib::screenName() {
  return historyBack().c_str();
}



//
// Private functions
//



void DisplayLib::addHistory(std::string name){
  historyIndex++;
  if(historyIndex > MAX_HISTORY){
    historyIndex = 0;
  } 
  history[historyIndex] = name;
}

std::string DisplayLib::historyBack(){
  return history[historyIndex];
}
void DisplayLib::historyPop(){
  historyIndex--;
  if(historyIndex < 0){
    historyIndex = MAX_ELEMENTS;
  }
}
void DisplayLib::showText(const char *text, int xPos, int yPos) {
  Serial.println(text);
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
  _display->drawRect(xPos - 2, yPos - 2, (strlen(text) * 6) + 4, 12,SH110X_WHITE);
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

